#pragma once


// -- std headers
#include <string>
#include <tuple>
#include <array>
#include <bitset>
#include <unordered_map>
#include <memory>
#include <utility>
#include <typeinfo>
#include <vector>
#include <functional>
#include <iostream>

// -- marlin headers
#include "marlin/book/Flags.h"
#include "marlin/book/Filler.h"
#include "marlin/book/FillMgr.h"
#include "marlin/book/Handle.h"


// -- root headers
#include "ROOT/RHist.hxx"

namespace marlin {
  namespace book {

    /**
     *  @brief TypeTrait for bookable Objects.
     *  Maps bookable Type to manager and modifier type.
     */
    template<class T>
    struct book_trait {
      static_assert( true, "Not Implimented trait" ) ;
      //! @typedef Type type of Object 
      //! @typedef Filler type of Object Modifier
      //! @typedef FillMgr type of Object Modifier Factory/Manager

      /**
       *  @fn static void merge(
                const std::shared_ptr<Type>& dst,
                const std::shared_ptr<Type>& src)
       *  @brief function for mearging towe objects.
       *  The source object will copyed to the other and than cleared.
       *  @param dst object in wich the other will merged.
       *  @param src object wich will be copyed to the first one and cleared
       */
    };

    /**
     *  @brief Booking managment class.
     *  Used to create and get handle for Bookable objects.
     */
    class BookStore {
      
      /**
       *  @brief struct to store data for a bookentry.
       */
      struct BookEntry {
        BookEntry(
          const std::shared_ptr<FillMgr>& mgr,
          const std::shared_ptr<MemLayout>& mem
        ) : fillMgr {mgr}, memory {mem}
        {}      
        
        /// An Manager which creates Modifier for the Object.
        std::shared_ptr<FillMgr>    fillMgr {nullptr} ;
        /// Memory where the Object is stored.
        std::shared_ptr<MemLayout>  memory  {nullptr} ;

        /**
         *  @brief creates a Modifier for the Object.
         *  Probergate call to modifie manager.
         *  @param idx is the id of the TODO
         *  @return shared_ptr to Modifier.
         */
        std::shared_ptr<Filler> createFiller( std::size_t idx ) {
          return fillMgr->createFiller( idx ) ;
        }
      };

    public:
      BookStore() : _objs{}, _fillers{}
      {}

      /** 
       *  @brief return finalized object with hash.
       *  @attention only for internal usage
       *  @param hash of the entry in internal database
       */ 
      template<typename T>
      const T& Read(std::size_t hash) ;

      /** 
       * @book Object and create Handle.
       * @tparam T object Type to be booked
       * @param name name of object
       * @param path to objcet
       * @param idx instance id
       * @param amt amount of instances
       * @param flags booking configuration Flags
       * @param ctor_p arguments to construct Object
       * @return handle for booked object
       */
      template<class T, typename ... Args_t>
        const std::string& name,
        const std::string& path,
        std::size_t idx,
        std::size_t amt,
        Flag_t flags,
        Args_t... ctor_p
      );  

    private:
      using EntryMap = std::unordered_map< std::size_t, BookEntry >;
      using ModifierMulMap 
        = std::unordered_multimap<std::size_t, std::shared_ptr<Filler>>;
      /// Maps id to an BookEntry. 
      EntryMap        _objs     {} ;
      /// Maps id to all created Filler for this Entry.
      ModifierMulMap  _fillers  {} ;
    
    };
  } // end namespace book
} // end namespace marlin

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

template<typename T>
const T&
marlin::book::BookStore::Read( std::size_t hash ) {
  bool modified = false ;
  auto beItr = _fillers.equal_range( hash ) ;
  for ( auto itr = beItr.first; itr != beItr.second; ++itr ) {
    if(itr->second->IsModified()) {
      itr->second->Flush() ;
      modified = true;
    }
  }

  // TODO: mod flag for MemLayer
  return *_objs.find( hash )->second.memory->merged< T >() ;
}

template<class T, typename ... Args_t>
marlin::book::Handle< marlin::book::book_trait< T > >
  marlin::book::BookStore::book (
  const std::string& name,
  const std::string& path,
  std::size_t idx,
  std::size_t amt,
  Flag_t flags,
  Args_t ... ctor_p
) { 
  using FillMgr_t = typename book_trait< T >::FillMgr ;
  using Value_t = typename book_trait< T >::Type ;

  std::size_t hash = std::hash< std::string >{}( name + path ) ;
  
  auto itrO = _objs.find( hash ) ;
  if ( itrO == _objs.end() ) {
    std::shared_ptr< FillMgr > fillMgr( nullptr ) ;
    std::shared_ptr< MemLayout > memLayout( nullptr );
    
    // TODO: shift this to FillMgr
    if ( flags.Contains( Flags::Book::MultiInstance ) ) {
      memLayout
        = std::make_shared<
            SingleMemLayout<
              Value_t,
              Args_t ...
            >
          >
          ( ctor_p ... ) ;
    } else {
      memLayout 
        = std::make_shared<
            SharedMemLayout<
              Value_t,
              book_trait<T>::mearge,
              Args_t ...
            >
          >
        ( amt, ctor_p ... ) ;
    }
    
    fillMgr = std::make_shared< FillMgr_t >(
      memLayout,
      flags
    ) ;

    itrO = _objs.insert( std::make_pair(
      hash,
      BookEntry(
        fillMgr,
        memLayout
      )
    )).first ;
  }

  BookEntry& obj = itrO->second ;
  std::shared_ptr< Filler > filler = obj.createFiller( idx ) ;
  _fillers.insert( std::make_pair( hash, filler ) ) ;
  
  auto finalFn = [ store = this ]( std::size_t idHash ) -> const T& {
    return store->template Read<T>( idHash ) ;
  } ;

  return Handle< book_trait< T > >( filler, hash, finalFn );
>>>>>>> polsish bookStore.h
}
