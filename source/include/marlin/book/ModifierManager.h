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

      virtual ~ModifierManager() = default ;
    };

    template<class T>
    class ModifierManagerHist : public ModifierManager {

    public:
    
      ModifierManagerHist(
        const std::shared_ptr<MemLayout>& layout,
        const Flag_t& flags
      ) ;

      /**
       *  @brief creates a Filler for the histogramm.
       *  @param idx id of TODO
       *  @return shared_ptr to Filler.
       */
      std::shared_ptr<Modifier> 
      createModifier( std::size_t idx ) override final ;

    private:
      /// Flags how to handle the object
      Flag_t                _flags  ;
      /// FillerBase for creating Filler. 
      std::shared_ptr<void> _fillerBase ;
    };

  } // end namespace book
} // end namespace marlin

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/// @private
template<class T>
marlin::book::ModifierManagerHist<T>::ModifierManagerHist(
  const std::shared_ptr<marlin::book::MemLayout>& layout,
  const marlin::book::Flag_t& flags
) : _flags{flags}, _fillerBase{nullptr} {

  if ( flags.Contains( Flags::Book::MultiInstance ) ) {
    _fillerBase = layout;
  } else {
    _fillerBase
      = std::make_shared<
          ROOT::Experimental::RHistConcurrentFillManager< T, BufferSize >
        > (
      *(layout->template at<T>(0)) // FIXME: depends on implimentation
    ) ;
  }
}

/// @private
template<class T>
std::shared_ptr<marlin::book::Modifier> 
marlin::book::ModifierManagerHist<T>::createModifier( std::size_t idx ) {
  if( !_flags.Contains( Flags::Book::MultiInstance ) ) {
    auto filler = std::make_shared<
      ROOT::Experimental::RHistConcurrentFiller<T, BufferSize>>(
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
    return std::make_shared< ModifierHist< T > >( fill, fillN, flush ) ;

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
}
