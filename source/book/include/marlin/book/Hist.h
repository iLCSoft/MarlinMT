#pragma once

// -- MarlinBook includes
#include "marlin/book/Types.h"
#include "marlin/book/BookStore.h"

// -- Hist includes
#include "marlin/book/HistEntry.h"
#include "marlin/book/HistEntryData.h"

namespace marlin {
  namespace book {

    //--------------------------------------------------------------------------
    
    template < typename T>
    EntryDataBase< T, types::Categories::Hist >::EntryDataBase(
      const std::string_view &title )
      : _title{title} {}

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, Flags::value(Flags::Book::Single) >
    EntryDataBase< T, types::Categories::Hist >::single() const {
      return EntryData< T,
                        types::Categories::Hist,
                        Flags::value(Flags::Book::Single) >( *this ) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, Flags::value(Flags::Book::MultiCopy) >
    EntryDataBase< T, types::Categories::Hist >::multiCopy( std::size_t n ) const {
      return EntryData< T,
                        types::Categories::Hist,
                        Flags::value(Flags::Book::MultiCopy) >( *this, n ) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, Flags::value(Flags::Book::MultiShared) >
    EntryDataBase< T, types::Categories::Hist >::multiShared() const {
      return EntryData< T,
                        types::Categories::Hist,
                        Flags::value(Flags::Book::MultiShared) >( *this ) ;
    }

    //--------------------------------------------------------------------------

    template<typename  T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData( 
        const types::RAxisConfig &axis )
      : EntryDataBase< T, types::Categories::Hist >() {
      static_assert(D == 1, "This is no 1D Hist, therefor it can't be constructed with 1 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(axis) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData(
        const std::string_view &title, const types::RAxisConfig &axis )
      : EntryDataBase< T >( title ) {
      static_assert(D == 1, "This is no 1D Hist, therefor it can't be constructed with 1 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(axis) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData(
        const types::RAxisConfig &     x_axis,
      const types::RAxisConfig &     y_axis )
      : EntryDataBase< T >( ) {
      static_assert(D == 2, "This is no 2D Hist, therefor it can't be constructed with 2 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
      this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData(
        const std::string_view &title,
      const types::RAxisConfig &     x_axis,
      const types::RAxisConfig &     y_axis )
      : EntryDataBase< T >( title ) {
      static_assert(D == 2, "This is no 2D Hist, therefor it can't be constructed with 2 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
      this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData( 
        const types::RAxisConfig &x_axis,
      const types::RAxisConfig &y_axis,
      const types::RAxisConfig &z_axis )
      : EntryDataBase< T >() {
      static_assert(D == 3, "This is no 3D Hist, therefor it can't be constructed with 3 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
      this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
      this->axis(2) = std::make_unique<types::RAxisConfig>(z_axis) ;
    }

    //--------------------------------------------------------------------------

    template < typename T>
    EntryData< T, types::Categories::Hist, 0 >::EntryData(
      const std::string_view &title,
      const types::RAxisConfig &     x_axis,
      const types::RAxisConfig &     y_axis,
      const types::RAxisConfig &     z_axis )
      : EntryDataBase< T >( title ) {
      static_assert(D == 3, "This is no 3D Hist, therefor it can't be constructed with 3 axis.");
      this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
      this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
      this->axis(2) = std::make_unique<types::RAxisConfig>(z_axis) ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    Handle< T, types::Categories::Hist >::Handle(
      const std::shared_ptr< MemLayout > &                        mem,
      const std::shared_ptr< T > &              obj,
      typename Handle< T, types::Categories::Hist >::FillFn_t     fillFn,
      typename Handle< T, types::Categories::Hist >::FillNFn_t    fillNFn,
      typename Handle< T, types::Categories::Hist >::FinalizeFn_t finalFn )
      : BaseHandle< T >{mem, obj}, _fillFn{std::move(fillFn)},
        _fillNFn{std::move(fillNFn)}, _finalFn{std::move(finalFn)} {}

    //--------------------------------------------------------------------------

    template <  typename T>
    void Handle< T, types::Categories::Hist >::fill(
      const typename Handle< T, types::Categories::Hist >::CoordArray_t &x,
      const typename Handle< T, types::Categories::Hist >::Weight_t &    w ) {
      _fillFn( x, w ) ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    void Handle< T, types::Categories::Hist >::fillN(
      const types::CoordArraySpan_t<T >  &x,
      const types::WeightSpan_t< T > &w ) {
      _fillNFn( x, w ) ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    const T &Handle< T, types::Categories::Hist >::merged() {
      _finalFn() ;
      return BaseHandle< T >::merged() ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    EntrySingle< T, types::Categories::Hist >::EntrySingle( Context context )
      : _context{std::move(context)} {}

    template <  typename T>
    Handle< T, types::Categories::Hist > EntrySingle< T, types::Categories::Hist >::handle() {
      using Hnd_t = Handle<T, types::Categories::Hist > ;
      auto hist   = _context.mem->at< T >( 0 ) ;
      return Handle< Type >(
        _context.mem,
        hist,
        [hist]( const typename Hnd_t::CoordArray_t &x,
                const typename Hnd_t::Weight_t &    w ) { hist->Fill( x, w ); },
        [hist]( const types::CoordArraySpan_t< Hnd_t > &x,
                const types::WeightSpan_t< Hnd_t >     &w ) {
          hist->FillN( x, w ) ;
        },
        []() {} ) ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    EntryMultiCopy< T, types::Categories::Hist >::EntryMultiCopy(
      Context context )
      : _context{std::move(context)} {}

    template <  typename T>
    Handle< T, types::Categories::Hist >
    EntryMultiCopy< T, types::Categories::Hist >::handle( std::size_t idx ) {
      using Hnd_t = Handle<T, types::Categories::Hist > ;
      auto pHist  = _context.mem->at< T >( idx ) ;
      return Handle< T >(
        _context.mem,
        pHist,
        [pHist]( const typename Hnd_t::CoordArray_t &x,
                 const typename Hnd_t::Weight_t &w ) { pHist->Fill( x, w ); },
        [pHist]( const types::CoordArraySpan_t< Hnd_t > &x,
                 const types::WeightSpan_t< Hnd_t >     &w ) {
          pHist->FillN( x, w ) ;
        },
        []() {} ) ;
    }

    //--------------------------------------------------------------------------
    
    template <  typename T>
    void EntryMultiShared< T, types::Categories::Hist >::flush() {
      for ( auto &filler : _fillers ) {
        if ( auto ptr = filler.lock() ) {
          ptr->Flush() ;
        }
      }
    }


    //--------------------------------------------------------------------------
    
    template <  typename T>
    EntryMultiShared< T, types::Categories::Hist >::EntryMultiShared(
      Context context )
      : _context{std::move(context)},
        _fillMgr{
          std::make_shared< types::RHistConcurrentFillManager< T  > >(
            *_context.mem->at< T >( 0 ) )},
        _fillers( 0 ) {}

    //--------------------------------------------------------------------------

    template <  typename T>
    EntryMultiShared< T, types::Categories::Hist >::~EntryMultiShared() {
      flush() ;
    }

    //--------------------------------------------------------------------------

    template <  typename T>
    Handle< T, types::Categories::Hist >
    EntryMultiShared< T, types::Categories::Hist >::handle() {
      using Hnd_t = T ;
      auto pFiller
        = std::make_shared< types::RHistConcurrentFiller< T  > >(
          *_fillMgr ) ;
      _fillers.push_back( pFiller ) ;
      return Handle< T, types::Categories::Hist >(
        _context.mem,
        _context.mem->at< T >( 0 ),
        [pFiller = pFiller]( const typename Hnd_t::CoordArray_t &x,
                             const typename Hnd_t::Weight_t &    w ) {
          pFiller->Fill( x, w ) ;
        },
        [pFiller = pFiller]( const types::CoordArraySpan_t< Hnd_t > &x,
                             const types::WeightSpan_t< Hnd_t >     &w ) {
          pFiller->FillN( x, w ) ;
        },
        [this]() { this->flush(); } ) ;
    }

    //--------------------------------------------------------------------------

    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 1, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::Single)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookSingle< T,
                               const std::string_view &,
                               const types::RAxisConfig & >(
        args..., _data.title(), *_data.axis(0) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 2, std::shared_ptr<Entry>>
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::Single)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookSingle< T,
                               const std::string_view &,
                               const types::RAxisConfig &,
                               const types::RAxisConfig & >(
        args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 3, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::Single)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookSingle< T,
                               const std::string_view &,
                               const types::RAxisConfig &,
                               const types::RAxisConfig &,
                               const types::RAxisConfig & >(
        args...,
        _data.title(),
        *_data.axis(0),
        *_data.axis(1),
        *_data.axis(2)) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 1, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiCopy)>
      ::book( BookStore &store, const Args_t &... args ) const {
      using Type = T ;
      return store.bookMultiCopy< Type,
                                  &types::addHists,
                                  const std::string_view &,
                                  const types::RAxisConfig & >(
        _n, args..., _data.title(), *_data.axis(0) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 2, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiCopy)>
      ::book( BookStore &store, const Args_t &... args ) const {
      using Type = T ;
      return store.bookMultiCopy<Type,
                                  &types::addHists,
                                  const std::string_view &,
                                  const types::RAxisConfig &,
                                  const types::RAxisConfig & >(
        _n, args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 3, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiCopy)>
      ::book( BookStore &store, const Args_t &... args ) const {
      using Type = T ;
      return store.bookMultiCopy< Type,
                                  &types::addHists,
                                  const std::string_view &,
                                  const types::RAxisConfig &,
                                  const types::RAxisConfig &,
                                  const types::RAxisConfig & >(
        _n,
        args...,
        _data.title(),
        *_data.axis(0),
        *_data.axis(1),
        *_data.axis(2) ) ;
    }
    
    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 1, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiShared)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookMultiShared< T,
                                    const std::string_view &,
                                    const types::RAxisConfig & >(
        args..., _data.title(), *_data.axis(0) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 2, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiShared)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookMultiShared< T,
                                    const std::string_view &,
                                    const types::RAxisConfig &,
                                    const types::RAxisConfig & >(
        args..., _data.title, *_data.axis(0), *_data.axis(1) ) ;
    }

    //--------------------------------------------------------------------------
    
    template< typename T>
    template < typename... Args_t, int d >
    std::enable_if_t< d == 3, std::shared_ptr<Entry> >
    EntryData<T, types::Categories::Hist, Flags::value(Flags::Book::MultiShared)>
      ::book( BookStore &store, const Args_t &... args ) const {
      return store.bookMultiShared< T,
                                    const std::string_view &,
                                    const types::RAxisConfig &,
                                    const types::RAxisConfig &,
                                    const types::RAxisConfig & >(
        args...,
        _data.title(),
        *_data.axis(0),
        *_data.axis(1),
        *_data.axis(2) ) ;
    }

  } // end namespace book
} // end namespace marlin
