#pragma once

// -- std header
#include <memory>
#include <functional>

// -- marlin header
#include "marlin/book/Flags.h"
#include "marlin/book/Modifier.h"
#include "marlin/book/MemLayout.h"

// -- root header
#include "ROOT/RHistConcurrentFill.hxx"

namespace marlin {
  namespace book {

    class BookStore;
    template<class T>
    struct book_trait;
    
    /**
     *  @brief Creates and manage Modifier.
     */
    class ModifierManager {
      friend BookStore;
      static constexpr std::size_t BufferSize = 1024;

    public:
      /**
       *  @brief create a new Modifer for the managet Object.
       *  @param idx id of TODO 
       */
      virtual std::shared_ptr< Modifier > createModifier( std::size_t idx )
        = 0 ;

      /**
       *  @brief flush and process every modifier.
       *  call before mearging
       */
      virtual void finalize() = 0 ;

      virtual ~ModifierManager() = default ;
    };

    template<class T>
    class ModifierManagerHist : public ModifierManager {

    public:
    
      ModifierManagerHist( const Flag_t& flags ) ;
      
      /**
       *  @brief constructs Memory layout for Manager.
       *  @attention only use exactly once
       *  @note used for easyer construction
       *  @param amt amount of instances
       *  @param ctor_p arguments to construct object
       */
      template<typename ... Args_t>
      std::shared_ptr<MemLayout>
      constructLayer(std::size_t amt, Args_t ... ctor_p) ;

      /**
       *  @brief creates a Filler for the histogramm.
       *  @param idx id of TODO
       *  @return shared_ptr to Filler.
       */
      std::shared_ptr<Modifier> 
      createModifier( std::size_t idx ) override final ;

      /**
       *  @brief flush buffer when concurrent filling is used.
       */
      void finalize() override final ;

    private:
      /// Flags how to handle the object
      Flag_t                                                _flags ;
      /// FillerBase for creating Filler. 
      std::shared_ptr<void>                                 _fillerBase ;
      /// List of produced filler
      std::vector<std::shared_ptr<marlin::book::Modifier>>  _producedFiller ;
    };

  } // end namespace book
} // end namespace marlin

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/// @private
template<class T>
marlin::book::ModifierManagerHist<T>::ModifierManagerHist(
  const marlin::book::Flag_t& flags
) : _flags {flags}, _fillerBase {nullptr}, _producedFiller (0) {

}
/// @private
template<class T>
template<typename ... Args_t>
std::shared_ptr<marlin::book::MemLayout>
marlin::book::ModifierManagerHist<T>::constructLayer(
  std::size_t amt,
  Args_t ... ctor_p) {

  std::shared_ptr<MemLayout> mem{nullptr};
  if(!_flags.Contains(Flags::Book::MultiInstance)) {
    mem
      = std::make_shared<
        SharedMemLayout<
          typename book_trait< T >::Type,
          book_trait< T >::merge,
          Args_t ...
        >
      >( amt, ctor_p ... ) ;
  } else {
    mem
      = std::make_shared<
        SingleMemLayout<
          typename book_trait< T >::Type,
          Args_t ...
        >
      >( ctor_p ... ) ;
  }
  
  if ( _flags.Contains( Flags::Book::MultiInstance ) ) {

    _fillerBase = mem;

  } else {

    _fillerBase
      = std::make_shared<
          ROOT::Experimental::RHistConcurrentFillManager< T, BufferSize >
        > (
      *(mem->template at<T>(0)) // FIXME: depends on implimentation
    ) ;
  }

  return mem;
}

/// @private
template<class T>
std::shared_ptr<marlin::book::Modifier> 
marlin::book::ModifierManagerHist<T>::createModifier( std::size_t idx ) {
  if( !_flags.Contains( Flags::Book::MultiInstance ) ) {
    auto filler = std::make_shared<
        *std::static_pointer_cast
          <ROOT::Experimental::RHistConcurrentFillManager
            <T, BufferSize
        >>( _fillerBase ) ) ;

    auto fill = [ filler = filler ]
      ( const typename T::CoordArray_t& x, const typename T::Weight_t& w ) {
        filler->Fill(x, w) ;
      } ;

    auto fillN = [ filler = filler ] (
      const std::span< typename T::CoordArray_t >& x,
      const std::span< typename T::Weight_t >& w) {
        filler->FillN(x, w) ;
      } ;

    auto flush = [ filler = filler ]
      () -> void{
        filler->Flush() ;
    } 
    ;
    _producedFiller.push_back(
      std::make_shared< ModifierHist< T > >( fill, fillN, flush )
    ) ;

    return _producedFiller.back() ;

  } else {
    auto filler
      = std::static_pointer_cast< MemLayout >( _fillerBase )
        ->template at< T >( idx ) ;

    auto fill = [ filler = filler ] 
      (const typename T::CoordArray_t& x, const typename T::Weight_t& w) {
        filler->Fill( x, w ) ;
      } ;

    auto fillN = [ filler = filler ] (
      const std::span< typename T::CoordArray_t >& x,
      const std::span< typename T::Weight_t >& w) {
        filler->FillN( x, w ) ;
      } ;

    auto flush = [ filler = filler ] () {} ; 

    return  std::make_shared< ModifierHist< T > >( fill, fillN, flush ) ;
  }
>>>>>>> moved modifier handling in ModifierManager
}

/// @private
template<class T>
void marlin::book::ModifierManagerHist<T>::finalize() {
  for(auto& filler : _producedFiller) {
    if ( filler->isModified() ) {
      filler->flush();
    }
  }
}
