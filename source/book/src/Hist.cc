#include "marlin/book/Hist.h"

// -- MarlinBook includes
#include "marlin/book/Handle.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/ROOTAdapter.h"

namespace marlin {
	namespace book {
		using namespace types ;
		
		template < int D, typename T, template < int, class > class... STAT >
		EntryDataBase< RH< D, T, STAT... > >::EntryDataBase(
			const std::string_view& title		
		) : _title{title}{}


		//--------------------------------------------------------------------------
		
		template < int D, typename T, template < int, class > class... STAT >
		EntryData<RH< D, T, STAT... >, Flags::Book::Single.VAL_INIT>
		EntryDataBase< RH< D, T, STAT... > >::single() const {
			return EntryData<types::RH< D, T, STAT...> , Flags::Book::Single.VAL_INIT>(
				*this
			);
		}

		//--------------------------------------------------------------------------
	
		template < int D, typename T, template < int, class > class... STAT >
		EntryData<RH< D, T, STAT... >, Flags::Book::MultiCopy.VAL_INIT>
		EntryDataBase<RH< D, T, STAT... > >::multiCopy(std::size_t n) const {
			return EntryData< types::RH< D, T, STAT...>, Flags::Book::MultiCopy.VAL_INIT >(
				*this, n
			);
		}

		//--------------------------------------------------------------------------
		
		template < int D, typename T, template < int, class > class... STAT >
		EntryData< RH< D, T, STAT... >, Flags::Book::MultiShared.VAL_INIT >
		EntryDataBase< RH< D, T, STAT... > >::multiShared() const {
			return EntryData<types::RH< D, T, STAT...>, Flags::Book::MultiShared.VAL_INIT>(
				*this
			);
		}

		//--------------------------------------------------------------------------
		
		template < typename T, template < int, class > class... STAT >
		EntryData<RH< 1, T, STAT... >, 0 >::EntryData(
			const RAxisConfig& axis	
		) : EntryDataBase<RH< 1, T, STAT... > >(){
			this->_axis[0] = &axis;
		}

		//--------------------------------------------------------------------------
		
		template < typename T, template < int, class > class... STAT >
		EntryData<RH< 1, T, STAT... >,0 >::EntryData(
				const std::string_view& title,
				const RAxisConfig& axis
		) : EntryDataBase<RH< 1, T, STAT... > >(title){
			this->_axis[0] = &axis;
		}

		//--------------------------------------------------------------------------
		
	template < typename T, template < int, class > class... STAT >
		EntryData<RH< 2, T, STAT... >, 0 >::EntryData(
			const RAxisConfig& x_axis,
			const RAxisConfig& y_axis
		) : EntryDataBase<RH< 2, T, STAT... > >(){
			this->_axis[0] = &x_axis;
			this->_axis[1] = &y_axis;
		}

		//--------------------------------------------------------------------------
		
		template < typename T, template < int, class > class... STAT >
		EntryData<RH< 2, T, STAT... >,0 >::EntryData(
				const std::string_view& title,
				const RAxisConfig& x_axis,
				const RAxisConfig& y_axis
		) : EntryDataBase<RH< 2, T, STAT... > >(title){
			this->_axis[0] = &x_axis;
			this->_axis[1] = &y_axis;
		}

		//--------------------------------------------------------------------------
		
		template < typename T, template < int, class > class... STAT >
		EntryData<RH< 3, T, STAT... >, 0 >::EntryData(
			const RAxisConfig& x_axis,	
			const RAxisConfig& y_axis,
			const RAxisConfig& z_axis
		) : EntryDataBase<RH< 3, T, STAT... > >(){
			this->_axis[0] = &x_axis;
			this->_axis[1] = &y_axis;
			this->_axis[2] = &z_axis;
		}

		//--------------------------------------------------------------------------
		
		template < typename T, template < int, class > class... STAT >
		EntryData<RH< 3, T, STAT... >,0 >::EntryData(
				const std::string_view& title,
				const RAxisConfig& x_axis,
				const RAxisConfig& y_axis,
				const RAxisConfig& z_axis
		) : EntryDataBase<RH< 3, T, STAT... > >(title){
			this->_axis[0] = &x_axis;
			this->_axis[1] = &y_axis;
			this->_axis[2] = &z_axis;
		}

		//--------------------------------------------------------------------------

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
	template class EntryData< TYPE, 0>;\
	template EntryData<TYPE, Flags::Book::Single.VAL_INIT>\
		EntryDataBase< TYPE>::single()const;\
	template EntryData<TYPE, Flags::Book::MultiShared.VAL_INIT>\
		EntryDataBase<TYPE>::multiShared()const;\
	template EntryData<TYPE, Flags::Book::MultiCopy.VAL_INIT>\
		EntryDataBase<TYPE>::multiCopy(std::size_t)const;\
	template class EntryData< TYPE, 1>;\
	template class EntryData< TYPE, 2>;\
	template class EntryData< TYPE, 4>;\
	template class EntryMultiShared< TYPE >

		LinkType( RH1D ) ;
		LinkType( RH1F ) ;
		LinkType( RH1I ) ;
		LinkType( RH2D ) ;
		LinkType( RH2F ) ;
		LinkType( RH2I ) ;
		LinkType( RH3D ) ;
		LinkType( RH3F ) ;
		LinkType( RH3I ) ;

	} // end namespace book
} // end namespace marlin
