#pragma  once

// -- std includes
#include <array>
#include <functional>

// -- MarlinBook includes
#include "marlin/book/EntryData.h"
#include "marlin/book/ROOTAdapter.h"
#include "marlin/book/BookStore.h"

namespace marlin {
	namespace book {		

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
			/**
			 *	@brief read access for passed title. 
			 */
			[[nodiscard]]
			const std::string_view& title() const { return _title; }

			/**
			 *	@brief access Histogram axis configuration.
			 *	@param id of axis, start by 0.
			 */
			[[nodiscard]]
			std::unique_ptr<types::RAxisConfig>& axis(const std::size_t id) { return _axis.at(id); }

			/**
			 *	@brief access Histogram axis configuration. Can't Modified.
			 *	@param id of axis, start by 0. 
			 */
			[[nodiscard]]
			const std::unique_ptr<types::RAxisConfig>& axis(const std::size_t id ) const { return _axis.at(id); }

		private:
			/// Histogram custom title, length = 0 when nothing setted.
			const std::string_view &                    _title ;
			/// Histogram axis configuration. 
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

			/**
			 *	@brief book Histogram in Single Mode. Only available for 1D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in Single Mode. Only available for 2D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry>>
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in Single Mode. Only available for 3D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;
			
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

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 1D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 2D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 3D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

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

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 1D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 1, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 2D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 2, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			/**
			 *	@brief book Histogram in MultiCopy Mode. Only available for 3D Hist.
			 *	@param store to where book Histogram.
			 */
			template < typename... Args_t, int d = D >
			std::enable_if_t< d == 3, std::shared_ptr<Entry> >
			book( BookStore &store, const Args_t &... args ) const;

			const EntryDataBase< types::RHist< D, T, STAT... > > &_data ;
			using Object_t = types::RHist< D, T, STAT...>;
		} ;

	} // end namespace book
} // end namespace marlin
