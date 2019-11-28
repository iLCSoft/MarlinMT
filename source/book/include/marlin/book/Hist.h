#pragma once

// -- Hist includes
#include "marlin/book/HistEntryData.h"
#include "marlin/book/HistEntry.h"

namespace marlin {
	namespace book {

		//--------------------------------------------------------------------------
		
		template < int D, typename T, template < int, class > class... STAT >
		EntryDataBase< types::RHist< D, T, STAT... > >::EntryDataBase(
			const std::string_view &title )
			: _title{title} {}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::Single) >
		EntryDataBase< types::RHist< D, T, STAT... > >::single() const {
			return EntryData< types::RHist< D, T, STAT... >,
												Flags::value(Flags::Book::Single) >( *this ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiCopy) >
		EntryDataBase< types::RHist< D, T, STAT... > >::multiCopy( std::size_t n ) const {
			return EntryData< types::RHist< D, T, STAT... >,
												Flags::value(Flags::Book::MultiCopy) >( *this, n ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiShared) >
		EntryDataBase< types::RHist< D, T, STAT... > >::multiShared() const {
			return EntryData< types::RHist< D, T, STAT... >,
												Flags::value(Flags::Book::MultiShared) >( *this ) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 1, T, STAT... >, 0 >::EntryData( const types::RAxisConfig &axis )
			: EntryDataBase< types::RHist< 1, T, STAT... > >() {
			this->axis(0) = std::make_unique<types::RAxisConfig>(axis) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 1, T, STAT... >, 0 >::EntryData(
			const std::string_view &title, const types::RAxisConfig &axis )
			: EntryDataBase< types::RHist< 1, T, STAT... > >( title ) {
			this->axis(0) = std::make_unique<types::RAxisConfig>(axis) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 2, T, STAT... >, 0 >::EntryData( const types::RAxisConfig &x_axis,
																										const types::RAxisConfig &y_axis )
			: EntryDataBase< types::RHist< 2, T, STAT... > >() {
			this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
			this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 2, T, STAT... >, 0 >::EntryData(
			const std::string_view &title,
			const types::RAxisConfig &     x_axis,
			const types::RAxisConfig &     y_axis )
			: EntryDataBase< types::RHist< 2, T, STAT... > >( title ) {
			this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
			this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 3, T, STAT... >, 0 >::EntryData( const types::RAxisConfig &x_axis,
																										const types::RAxisConfig &y_axis,
																										const types::RAxisConfig &z_axis )
			: EntryDataBase< types::RHist< 3, T, STAT... > >() {
			this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
			this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
			this->axis(2) = std::make_unique<types::RAxisConfig>(z_axis) ;
		}

		//--------------------------------------------------------------------------

		template < typename T, template < int, class > class... STAT >
		EntryData< types::RHist< 3, T, STAT... >, 0 >::EntryData(
			const std::string_view &title,
			const types::RAxisConfig &     x_axis,
			const types::RAxisConfig &     y_axis,
			const types::RAxisConfig &     z_axis )
			: EntryDataBase< types::RHist< 3, T, STAT... > >( title ) {
			this->axis(0) = std::make_unique<types::RAxisConfig>(x_axis) ;
			this->axis(1) = std::make_unique<types::RAxisConfig>(y_axis) ;
			this->axis(2) = std::make_unique<types::RAxisConfig>(z_axis) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		Handle< types::RHist< D, T, STAT... > >::Handle(
			const std::shared_ptr< MemLayout > &                        mem,
			const std::shared_ptr< types::RHist< D, T, STAT... > > &              obj,
			typename Handle< types::RHist< D, T, STAT... > >::FillFn_t     fillFn,
			typename Handle< types::RHist< D, T, STAT... > >::FillNFn_t    fillNFn,
			typename Handle< types::RHist< D, T, STAT... > >::FinalizeFn_t finalFn )
			: BaseHandle< types::RHist< D, T, STAT... > >{mem, obj}, _fillFn{std::move(fillFn)},
				_fillNFn{std::move(fillNFn)}, _finalFn{std::move(finalFn)} {}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		void Handle< types::RHist< D, T, STAT... > >::fill(
			const typename Handle< types::RHist< D, T, STAT... > >::CoordArray_t &x,
			const typename Handle< types::RHist< D, T, STAT... > >::Weight_t &    w ) {
			_fillFn( x, w ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		void Handle< types::RHist< D, T, STAT... > >::fillN(
			const types::CoordArraySpan_t<types::RHist< D, T, STAT... > >  &x,
			const types::WeightSpan_t< types::RHist< D, T, STAT... > > &w ) {
			_fillNFn( x, w ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		const types::RHist< D, T, STAT... > &Handle< types::RHist< D, T, STAT... > >::merged() {
			_finalFn() ;
			return BaseHandle< types::RHist< D, T, STAT... > >::merged() ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntrySingle< types::RHist< D, T, STAT... > >::EntrySingle( Context context )
			: _context{std::move(context)} {}

		template < int D, typename T, template < int, class > class... STAT >
		Handle< types::RHist< D, T, STAT... > > EntrySingle< types::RHist< D, T, STAT... > >::handle() {
			using Hnd_t = types::RHist< D, T, STAT... > ;
			auto hist   = _context.mem->at< Type >( 0 ) ;
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

		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiCopy< types::RHist< D, T, STAT... > >::EntryMultiCopy(
			Context context )
			: _context{std::move(context)} {}

		template < int D, typename T, template < int, class > class... STAT >
		Handle< types::RHist< D, T, STAT... > >
		EntryMultiCopy< types::RHist< D, T, STAT... > >::handle( std::size_t idx ) {
			using Hnd_t = types::RHist< D, T, STAT... > ;
			auto pHist  = _context.mem->at< Type >( idx ) ;
			return Handle< Type >(
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
		
		template < int D, typename T, template < int, class > class... STAT >
		void EntryMultiShared< types::RHist< D, T, STAT... > >::flush() {
			for ( auto &filler : _fillers ) {
				if ( auto ptr = filler.lock() ) {
					ptr->Flush() ;
				}
			}
		}


		//--------------------------------------------------------------------------
		
		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiShared< types::RHist< D, T, STAT... > >::EntryMultiShared(
			Context context )
			: _context{std::move(context)},
				_fillMgr{
					std::make_shared< types::RHistConcurrentFillManager< types::RHist< D, T, STAT... > > >(
						*_context.mem->at< types::RHist< D, T, STAT... > >( 0 ) )},
				_fillers( 0 ) {}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiShared< types::RHist< D, T, STAT... > >::~EntryMultiShared() {
			flush() ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		Handle< types::RHist< D, T, STAT... > >
		EntryMultiShared< types::RHist< D, T, STAT... > >::handle() {
			using Hnd_t = types::RHist< D, T, STAT... > ;
			auto pFiller
				= std::make_shared< types::RHistConcurrentFiller< types::RHist< D, T, STAT... > > >(
					*_fillMgr ) ;
			_fillers.push_back( pFiller ) ;
			return Handle< Type >(
				_context.mem,
				_context.mem->at< Type >( 0 ),
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

		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 1, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::Single)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookSingle< types::RHist< 1, T, STAT... >,
															 const std::string_view &,
															 const types::RAxisConfig & >(
				args..., _data.title(), *_data.axis(0) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 2, std::shared_ptr<Entry>>
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::Single)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookSingle< types::RHist< 2, T, STAT... >,
															 const std::string_view &,
															 const types::RAxisConfig &,
															 const types::RAxisConfig & >(
				args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 3, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::Single)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookSingle< types::RHist< 3, T, STAT... >,
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
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 1, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiCopy)>
			::book( BookStore &store, const Args_t &... args ) const {
			using Type = types::RHist< 1, T, STAT... > ;
			return store.bookMultiCopy< Type,
																	&merge,
																	const std::string_view &,
																	const types::RAxisConfig & >(
				_n, args..., _data.title(), *_data.axis(0) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 2, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiCopy)>
			::book( BookStore &store, const Args_t &... args ) const {
			using Type = types::RHist< 2, T, STAT... > ;
			return store.bookMultiCopy<Type,
																	&merge,
																	const std::string_view &,
																	const types::RAxisConfig &,
																	const types::RAxisConfig & >(
				_n, args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 3, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiCopy)>
			::book( BookStore &store, const Args_t &... args ) const {
			using Type = types::RHist< 3, T, STAT... > ;
			return store.bookMultiCopy< Type,
																	&merge,
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
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 1, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiShared)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookMultiShared< types::RHist< 1, T, STAT... >,
																		const std::string_view &,
																		const types::RAxisConfig & >(
				args..., _data.title(), *_data.axis(0) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 2, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiShared)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookMultiShared< types::RHist< 2, T, STAT... >,
																		const std::string_view &,
																		const types::RAxisConfig &,
																		const types::RAxisConfig & >(
				args..., _data.title, *_data.axis(0), *_data.axis(1) ) ;
		}

		//--------------------------------------------------------------------------
		
		template<int D, typename T, template < int, class > class... STAT>
		template < typename... Args_t, int d >
		std::enable_if_t< d == 3, std::shared_ptr<Entry> >
		EntryData<types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiShared)>
			::book( BookStore &store, const Args_t &... args ) const {
			return store.bookMultiShared< types::RHist< 3, T, STAT... >,
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
