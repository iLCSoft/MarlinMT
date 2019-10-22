#pragma once

// -- std headers
#include <string>
#include <unordered_map>
#include <memory>

// -- marlin headers
#include "marlin/book/Flags.h"
#include "marlin/book/Modifier.h"
#include "marlin/book/ModifierManager.h"
#include "marlin/book/Handle.h"


// -- root headers
#include "ROOT/RHist.hxx"

namespace marlin {
  //! Namespace for the booking submodule
  namespace book {

    /**
     *  @brief TypeTrait for bookable Objects.
     *  Maps bookable Type to manager and modifier type.
     */
    template<class T>
    struct book_trait {
      static_assert( true, "Not Implimented trait" ) ;
      //! @typedef Type type of Object 
      //! @typedef Modifier type of Object Modifier
      //! @typedef ModifierManager type of Object Modifier Factory/Manager

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
      struct BookEntry;

      using EntryMap = std::unordered_map< std::size_t, BookEntry >;
      using ModifierMulMap 
        = std::unordered_multimap< std::size_t, std::shared_ptr<Modifier > >;
      
      /**
       *  @brief struct to store data for a bookentry.
       */
      struct BookEntry {
        BookEntry(
          const std::shared_ptr<ModifierManager>& mgr,
          const std::shared_ptr<MemLayout>& mem
        ) : memory {mem}, modMgr {mgr}
        {}      
        
        /// Memory where the Object is stored.
        std::shared_ptr< MemLayout >          memory {nullptr} ;
        /// An Manager which creates Modifier for the Object.
        std::shared_ptr< ModifierManager >    modMgr {nullptr} ;

        /**
         *  @brief creates a Modifier for the Object.
         *  Probergate call to modifie manager.
         *  @param idx is the id of the TODO
         *  @return shared_ptr to Modifier.
         */
        std::shared_ptr< Modifier > createModifier( std::size_t idx ) {
          return modMgr->createModifier( idx ) ;
        }
      };

    public:
      BookStore() : _objs {}
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
      Handle<book_trait<T>> book(
        const std::string& name,
        const std::string& path,
        std::size_t idx,
        std::size_t amt,
        Flag_t flags,
        Args_t... ctor_p
      );  

    private:
      /// Maps id to an BookEntry. 
      EntryMap        _objs       {} ;
    };
  } // end namespace book
} // end namespace marlin

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/// @private
template<typename T>
const T&
marlin::book::BookStore::Read( std::size_t hash ) {
  bool modified = false ;
  
  auto obj = _objs.find( hash )->second;
  obj.modMgr->finalize();

  // TODO: mod flag for MemLayer
  return *obj.memory->merged< T >() ;
}

/// @private
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
  using ModMgr_t = typename book_trait< T >::ModifierManager ;
  using Value_t = typename book_trait< T >::Type ;

  std::size_t hash = std::hash< std::string >{}( name + path ) ;
  
  auto itrO = _objs.find( hash ) ;
  if ( itrO == _objs.end() ) {
    std::shared_ptr< ModMgr_t > modMgr( nullptr ) ;
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
              book_trait<T>::merge,
              Args_t ...
            >
          >
        ( amt, ctor_p ... ) ;
    }
    
    modMgr = std::make_shared< ModMgr_t >(
      memLayout,
      flags
    ) ;

    itrO = _objs.insert( std::make_pair(
      hash,
      BookEntry(
        modMgr,
        memLayout
      )
    )).first ;
  }

  BookEntry& obj = itrO->second ;
  std::shared_ptr< Modifier > modifier = obj.createModifier( idx ) ;
  
  auto finalFn = [ store = this ]( std::size_t idHash ) -> const T& {
    return store->template Read<T>( idHash ) ;
  } ;

  return Handle< book_trait< T > >( modifier, hash, finalFn );
}

