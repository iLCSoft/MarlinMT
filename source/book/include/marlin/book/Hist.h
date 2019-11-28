#pragma once

// -- std includes
#include <array>
#include <functional>
#include <iostream>
#include <type_traits>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/EntryData.h"
#include "marlin/book/Entry.h"
#include "marlin/book/BookStore.h"
#include "marlin/book/ROOTAdapter.h"

namespace marlin {
	namespace book {

		// -- MarlinBook forward declaration
		class BookStore ;
		template < typename T >
		class BaseHandle ;
		template < typename T >
		class Handle ;
		template < typename T >
		class EntrySingle ;
		template < typename T >
		class EntryMultiCopy ;
		template < typename T >
		class EntryMultiShared ;
		template < typename >
		class EntryDataBase ;
		template < typename, unsigned long long >
		class EntryData ;


		template< int D, typename T, template < int, class > class... STAT >
		void merge(
				const std::shared_ptr<types::RHist< D, T, STAT... >>& dst,
				const std::shared_ptr<types::RHist< D, T, STAT... >>& src) {
				types::addHists(dst, src);
		}


		/**
		 *  @brief Base for Histogram EntryData.
		 */
		template < int D, typename T, template < int, class > class... STAT >
		class EntryDataBase< types::RHist< D, T, STAT... > >
			: public EntryDataBase< void > {
			template < typename, unsigned long long >
			friend class EntryData ;
			static constexpr std::string_view empty{""} ;

		public:
			explicit EntryDataBase( const std::string_view &title = empty ) ;

			/**
			 *  @brief construct EntryData for single booking.
			 */
			EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::Single) >
			single() const ;

			/**
			 *  @brief construct EntryData for multi copy booking.
			 *  @param n number of memory instances which should be constructed
			 */
			[[nodiscard]]
			EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiCopy) >
			multiCopy( std::size_t n ) const ;

			/**
			 *  @brief construct EntryData for multi shared booking.
			 */
			[[nodiscard]]
			EntryData< types::RHist< D, T, STAT... >, Flags::value(Flags::Book::MultiShared) >
			multiShared() const ;

		protected:
			[[nodiscard]]
			const std::string_view& title() const { return _title; }
			[[nodiscard]]
			std::unique_ptr<types::RAxisConfig>& axis(const std::size_t id) { return _axis.at(id); }
			[[nodiscard]]
			const std::unique_ptr<types::RAxisConfig>& axis(const std::size_t id ) const { return _axis.at(id); }
		private:
			const std::string_view &                    _title ;
			std::array< std::unique_ptr<types::RAxisConfig>, D > _axis{} ;
		} ;

		/**
		 *  @brief EntryData for 1 dimensional Histograms.
		 */
		template < typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< 1, T, STAT... >, 0 >
			: public EntryDataBase< types::RHist< 1, T, STAT... > > {
		public:
			/**
			 *  @brief Constructor without Title.
			 *  @param axis configuration.
			 */
			explicit EntryData( const types::RAxisConfig &axis ) ;

			/**
			 *  @brief Constructor.
			 *  @param title of the Histogram.
			 *  @param axis configuration.
			 */
			EntryData( const std::string_view &  title,
								 const types::RAxisConfig &axis ) ;
		} ;

		/**
		 *  @brief EntryData for 2 dimensional Histograms.
		 */
		template < typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< 2, T, STAT... >, 0 >
			: public EntryDataBase< types::RHist< 2, T, STAT... > > {
		public:
			/**
			 *  @brief Constructor without title.
			 *  @param x_axis configuration of first axis.
			 *  @param y_axis configuration of second axis.
			 */
			EntryData( const types::RAxisConfig &x_axis,
								 const types::RAxisConfig &y_axis ) ;

			/**
			 *  @brief Constructor.
			 *  @param title of the Histogram.
			 *  @param x_axis configuration of first axis.
			 *  @param y_axis configuration of second axis.
			 */
			EntryData( const std::string_view &  title,
								 const types::RAxisConfig &x_axis,
								 const types::RAxisConfig &y_axis ) ;
		} ;

		/**
		 *  @brief EntryData for 3 dimensional Histograms.
		 */
		template < typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< 3, T, STAT... >, 0 >
			: public EntryDataBase< types::RHist< 3, T, STAT... > > {
		public:
			/**
			 *  @brief Constructor without title.
			 *  @param x_axis configuration of first axis.
			 *  @param y_axis configuration of second axis.
			 *  @param z_axis configuration of third axis.
			 */
			EntryData( const types::RAxisConfig &x_axis,
								 const types::RAxisConfig &y_axis,
								 const types::RAxisConfig &z_axis ) ;

			/**
			 *  @brief Constructor.
			 *  @param title of Histogram
			 *  @param x_axis configuration of first axis.
			 *  @param y_axis configuration of second axis.
			 *  @param z_axis configuration of third axis.
			 */
			EntryData( const std::string_view &  title,
								 const types::RAxisConfig &x_axis,
								 const types::RAxisConfig &y_axis,
								 const types::RAxisConfig &z_axis ) ;
		} ;

		/**
		 *  @brief EntryData for objects in Single mode.
		 */
		template < int D, typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< D, T, STAT... >,
										 Flags::value(Flags::Book::Single) > {
			friend EntryDataBase< types::RHist< D, T, STAT... > > ;
			friend BookStore ;

			explicit EntryData( const EntryDataBase< types::RHist< D, T, STAT... > > &data )
				: _data{data} {}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
				return store.bookSingle< types::RHist< 1, T, STAT... >,
																 const std::string_view &,
																 const types::RAxisConfig & >(
					args..., _data.title(), *_data.axis(0) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry>>
			book( BookStore &store, const Args_t &... args ) const {
				return store.bookSingle< types::RHist< 2, T, STAT... >,
																 const std::string_view &,
																 const types::RAxisConfig &,
																 const types::RAxisConfig & >(
					args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
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
			const EntryDataBase< types::RHist< D, T, STAT... > > &_data ;
			using Object_t = types::RHist< D, T, STAT...>;
		} ;

		/**
		 *  @brief EntryData for objects in MultiCopy Mode
		 */
		template < int D, typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< D, T, STAT... >,
										 Flags::value(Flags::Book::MultiCopy) > {
			friend EntryDataBase< types::RHist< D, T, STAT... > > ;
			friend BookStore ;
			EntryData( const EntryDataBase< types::RHist< D, T, STAT... > > &data,
								 std::size_t                                        n )
				: _data{data}, _n{n} {}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
				using Type = types::RHist< 1, T, STAT... > ;
				return store.bookMultiCopy< Type,
																		&merge,
																		const std::string_view &,
																		const types::RAxisConfig & >(
					_n, args..., _data.title(), *_data.axis(0) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
				using Type = types::RHist< 2, T, STAT... > ;
				return store.bookMultiCopy<Type,
																		&merge,
																		const std::string_view &,
																		const types::RAxisConfig &,
																		const types::RAxisConfig & >(
					_n, args..., _data.title(), *_data.axis(0), *_data.axis(1) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
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
			const EntryDataBase< types::RHist< D, T, STAT... > > &_data ;
			const std::size_t                                  _n ;
			using Object_t = types::RHist< D, T, STAT...>;
		} ;

		/**
		 *  @brief  EntryData for objects in MultiShared mode
		 */
		template < int D, typename T, template < int, class > class... STAT >
		class EntryData< types::RHist< D, T, STAT... >,
										 Flags::value(Flags::Book::MultiShared)> {
			friend EntryDataBase< types::RHist< D, T, STAT... > > ;
			friend BookStore ;
			explicit EntryData( const EntryDataBase< types::RHist< D, T, STAT... > > &data )
				: _data{data} {}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
				return store.bookMultiShared< types::RHist< 1, T, STAT... >,
																			const std::string_view &,
																			const types::RAxisConfig & >(
					args..., _data.title(), *_data.axis(0) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
				return store.bookMultiShared< types::RHist< 2, T, STAT... >,
																			const std::string_view &,
																			const types::RAxisConfig &,
																			const types::RAxisConfig & >(
					args..., _data.title, *_data.axis(0), *_data.axis(1) ) ;
			}

			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const {
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
			const EntryDataBase< types::RHist< D, T, STAT... > > &_data ;
			using Object_t = types::RHist< D, T, STAT...>;
		} ;

		/// Handle specialisation for Histograms.
		template < int D, typename T, template < int, class > class... STAT >
		class Handle< types::RHist< D, T, STAT... > >
			: private BaseHandle< types::RHist< D, T, STAT... > > {
			friend BookStore ;

		public:
			/// Histogram Type which is Handled
			using Type = types::RHist< D, T, STAT... > ;
			/// CoordArray_t from managed Histogram
			using CoordArray_t = typename Type::CoordArray_t ;
			/// Weigh_t from managed Histogram
			using Weight_t = typename Type::Weight_t ;

			/// type of the RHist::Fill function.
			using FillFn_t
				= std::function< void( const CoordArray_t &, const Weight_t & ) > ;
			/// type of the RHist::FillN function
			using FillNFn_t = std::function< void( 
					const types::CoordArraySpan_t<Type>&,
					const types::WeightSpan_t<Type>& ) > ;
			using FinalizeFn_t = std::function< void() > ;

		public:
			/// construct a Handle.
			Handle( const std::shared_ptr< MemLayout > &mem,
							const std::shared_ptr< Type > &     obj,
							FillFn_t                     fillFn,
							FillNFn_t                    fillNFn,
							FinalizeFn_t                 finalFn ) ;

			/**
			 *  @brief Adds one datum to the Histogram.
			 *  @param x point to add.
			 *  @param w weight of point.
			 */
			void fill( const CoordArray_t &x, const Weight_t &w ) ;

			/**
			 *  @brief Adds N data to the Histogram.
			 *  @param x span of points to add.
			 *  @param w span of weights to add.
			 */
			void fillN(
					const types::CoordArraySpan_t<Type> &x,
					const types::WeightSpan_t<Type>  &w ) ;

			/**
			 *  @brief get completed Object.
			 *  @return Object which all data from every handle.
			 *  @note for MultyCopy objects expansive.
			 *  Creates every time a new merged Histogram.
			 */
			const Type &merged() ;

		private:
			/// Function to call for fill one object.
			FillFn_t _fillFn ;
			/// Function to call for fill N objects.
			FillNFn_t _fillNFn ;
			/// Function to call to flush queues which may exists
			FinalizeFn_t _finalFn ;
		} ;

		/// specialisation of EntrySingle for Histograms
		template < int D, typename T, template < int, class > class... STAT >
		class EntrySingle< types::RHist< D, T, STAT... > > : public EntryBase {

			friend BookStore ;

		public:
			/// Type of contained Histogram.
			using Type = types::RHist< D, T, STAT... > ;

			/// constructor
			explicit EntrySingle( Context context ) ;

			/// default constructor. Constructs invalid Entry.
			EntrySingle() = default ;

			/**
			 *  @brief creates new Handle for Contained Histogram.
			 *  @note not thread save.
			 */
			Handle< Type > handle() ;

		private:
			/// \see {EntrySingle::_context}
			Context _context ;
		} ;

		/// specialisation of EntryMultiCopy for Histograms
		template < int D, typename T, template < int, class > class... STAT >
		class EntryMultiCopy< types::RHist< D, T, STAT... > > : public EntryBase {

			friend BookStore ;

		public:
			/// Type of contained Histogram.
			using Type = types::RHist< D, T, STAT... > ;

			/// constructor
			explicit EntryMultiCopy( Context context ) ;

			/// default constructor. Constructs invalid Entry.
			EntryMultiCopy() = default ;

			/**
			 *  @brief creates a new Handle for one instance.
			 *  @param idx id of instance which should be accessed.
			 *  @note handles to the same instance should be only use in sequential
			 *code.
			 */
			Handle< Type > handle( std::size_t idx ) ;

		private:
			/// \see {EntrySingle::_context}
			Context _context ;
		} ;
		/// specialisation of EntryMultiShared for Histograms
		template < int D, typename T, template < int, class > class... STAT >
		class EntryMultiShared< types::RHist< D, T, STAT... > > : public EntryBase {
			friend BookStore ;

		public:
			/// Type of contained Histogram.
			using Type = types::RHist< D, T, STAT... > ;

			/// constructor
			explicit EntryMultiShared( Context context ) ;

			// EntryMultiShared() = default ;
			/// destructor. Flush filler.
			~EntryMultiShared() ;

			EntryMultiShared(const EntryMultiShared&) = default;
			EntryMultiShared& operator=(const EntryMultiShared&) = default ;
			EntryMultiShared(EntryMultiShared&&) noexcept = default;
			EntryMultiShared& operator=(EntryMultiShared && ) noexcept     = default;

			/**
			 *  @brief creates a new Handle.
			 *  @note each Handle contains a buffer to reduce synchronisation.
			 *  This memory will be freed when the handle is destructed.
			 */
			Handle< Type > handle() ;

			/// flush every Buffer from each Handle.
			void flush() ;

		private:
			/// \see {EntrySingle::_context}
			Context _context ;
			/// Manager to construct Filler.
			std::shared_ptr< types::RHistConcurrentFillManager< Type > > _fillMgr ;
			/// list of produced Filler to flush them when needed.
			std::vector< std::weak_ptr< types::RHistConcurrentFiller< Type > > >
				_fillers ;
		} ;

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

	} // end namespace book
} // end namespace marlin
