#include "marlin/book/Hist.h"

// -- MarlinBook includes
#include "marlin/book/Handle.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/ROOTAdapter.h"

namespace marlin {
	namespace book {
		using namespace types ;

		template < int D, typename T, template < int, class > class... STAT >
		Handle< RH< D, T, STAT... > >::Handle(
		  const std::shared_ptr< MemLayout > &                        mem,
		  const std::shared_ptr< RH< D, T, STAT... > > &              obj,
		  const typename Handle< RH< D, T, STAT... > >::FillFn_t &    fillFn,
		  const typename Handle< RH< D, T, STAT... > >::FillNFn_t &   fillNFn,
		  const typename Handle< RH< D, T, STAT... > >::FinalizeFn_t &finalFn )
		  : BaseHandle< RH< D, T, STAT... > >{mem, obj}, _fillFn{fillFn},
		    _fillNFn{fillNFn}, _finalFn{finalFn} {}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		void Handle< RH< D, T, STAT... > >::fill(
		  const typename Handle< RH< D, T, STAT... > >::CoordArray_t &x,
		  const typename Handle< RH< D, T, STAT... > >::Weight_t &    w ) {
			_fillFn( x, w ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		void Handle< RH< D, T, STAT... > >::fillN(
		  const std::span< typename Handle< RH< D, T, STAT... > >::CoordArray_t >
		    &                                                                  x,
		  const std::span< typename Handle< RH< D, T, STAT... > >::Weight_t > &w ) {
			_fillNFn( x, w ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		const RH< D, T, STAT... > &Handle< RH< D, T, STAT... > >::merged() {
			_finalFn() ;
			return BaseHandle< RH< D, T, STAT... > >::merged() ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntrySingle< RH< D, T, STAT... > >::EntrySingle( const Context &context )
		  : _context{context} {}

		template < int D, typename T, template < int, class > class... STAT >
		Handle< RH< D, T, STAT... > > EntrySingle< RH< D, T, STAT... > >::handle() {
			using Hnd_t = RH< D, T, STAT... > ;
			auto hist   = _context.mem->at< Type >( 0 ) ;
			return Handle< Type >(
			  _context.mem,
			  hist,
			  [hist]( const typename Hnd_t::CoordArray_t &x,
			          const typename Hnd_t::Weight_t &    w ) { hist->Fill( x, w ); },
			  [hist]( const std::span< typename Hnd_t::CoordArray_t > &x,
			          const std::span< typename Hnd_t::Weight_t > &    w ) {
				  hist->FillN( x, w ) ;
			  },
			  []() {} ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiCopy< RH< D, T, STAT... > >::EntryMultiCopy(
		  const Context &context )
		  : _context{context} {}

		template < int D, typename T, template < int, class > class... STAT >
		Handle< RH< D, T, STAT... > >
		EntryMultiCopy< RH< D, T, STAT... > >::handle( std::size_t idx ) {
			using Hnd_t = RH< D, T, STAT... > ;
			auto pHist  = _context.mem->at< Type >( idx ) ;
			return Handle< Type >(
			  _context.mem,
			  pHist,
			  [pHist]( const typename Hnd_t::CoordArray_t &x,
			           const typename Hnd_t::Weight_t &w ) { pHist->Fill( x, w ); },
			  [pHist]( const std::span< typename Hnd_t::CoordArray_t > &x,
			           const std::span< typename Hnd_t::Weight_t > &    w ) {
				  pHist->FillN( x, w ) ;
			  },
			  []() {} ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiShared< RH< D, T, STAT... > >::EntryMultiShared(
		  const Context &context )
		  : _context{context},
		    _fillMgr{
		      std::make_shared< RHistConcurrentFillManager< RH< D, T, STAT... > > >(
		        *context.mem->at< RH< D, T, STAT... > >( 0 ) )},
		    _fillers( 0 ) {}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		EntryMultiShared< RH< D, T, STAT... > >::~EntryMultiShared() {
			flush() ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		Handle< RH< D, T, STAT... > >
		EntryMultiShared< RH< D, T, STAT... > >::handle() {
			using Hnd_t = RH< D, T, STAT... > ;
			auto pFiller
			  = std::make_shared< RHistConcurrentFiller< RH< D, T, STAT... > > >(
			    *_fillMgr ) ;
			_fillers.push_back( pFiller ) ;
			return Handle< Type >(
			  _context.mem,
			  _context.mem->at< Type >( 0 ),
			  [pFiller = pFiller]( const typename Hnd_t::CoordArray_t &x,
			                       const typename Hnd_t::Weight_t &    w ) {
				  pFiller->Fill( x, w ) ;
			  },
			  [pFiller = pFiller]( const std::span< typename Hnd_t::CoordArray_t > &x,
			                       const std::span< typename Hnd_t::Weight_t > &w ) {
				  pFiller->FillN( x, w ) ;
			  },
			  [this]() { this->flush(); } ) ;
		}

		//--------------------------------------------------------------------------

		template < int D, typename T, template < int, class > class... STAT >
		void EntryMultiShared< RH< D, T, STAT... > >::flush() {
			for ( auto &filler : _fillers ) {
				if ( auto ptr = filler.lock() ) {
					ptr->Flush() ;
				}
			}
		}

		

#define LinkType( TYPE )                                                       \
	template class Handle< TYPE >;                                               \
	template class EntrySingle< TYPE >;                                          \
	template class EntryMultiCopy< TYPE >;                                       \
	template class EntryMultiShared< TYPE >

		LinkType( RH1D ) ;
		LinkType( RH1F ) ;
		LinkType( RH1I ) ;

	} // end namespace book
} // end namespace marlin
