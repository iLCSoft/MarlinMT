#pragma once

// -- std includes
#include <array>
#include <functional>

// -- MarlinBook includes
#include "marlin/book/Types.h"
#include "marlin/book/EntryData.h"

// -- Hist includes
#include "marlin/book/HistEntryDataBase.h"

namespace marlin {
  namespace book {

    /**
     *  @brief EntryData for 1 dimensional Histograms.
     */
    template < typename T>
    class EntryData< T, types::Categories::Hist, 0 >
      : public EntryDataBase<T, types::Categories::Hist> {
      static constexpr int D = types::TypeInfo<T>::dimension;
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
      EntryData( const std::string_view&  title,
                 const types::RAxisConfig &axis ) ;

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
    template < typename T >
    class EntryData< T,
                      types::Categories::Hist,
                     Flags::value( Flags::Book::Single ) > {
      friend EntryDataBase< T > ;
      friend BookStore ;
      static constexpr int D = types::TypeInfo<T>::dimension;

      explicit EntryData(
        const EntryDataBase< T, types::Categories::Hist > &data )
        : _data{data} {}

      /**
       *  @brief book Histogram in Single Mode. Only available for 1D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 1, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in Single Mode. Only available for 2D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 2, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in Single Mode. Only available for 3D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 3, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      const EntryDataBase< T, types::Categories::Hist > &_data ;
      using Object_t = T ;
    } ;

    /**
     *  @brief EntryData for objects in MultiCopy Mode
     */
    template < typename T>
    class EntryData< T,
                     types::Categories::Hist,
                     Flags::value( Flags::Book::MultiCopy ) > {
      friend EntryDataBase< T > ;
      friend BookStore ;
      static constexpr int D = types::TypeInfo<T>::dimension;

      EntryData( const EntryDataBase< T, types::Categories::Hist> &data,
                 std::size_t                                           n )
        : _data{data}, _n{n} {}

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 1D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 1, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 2D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 2, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 3D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 3, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      const EntryDataBase< T, types::Categories::Hist> &_data ;
      const std::size_t                                     _n ;
      using Object_t = T ;
    } ;

    /**
     *  @brief  EntryData for objects in MultiShared mode
     */
    template < typename T>
    class EntryData< T,
                     types::Categories::Hist,
                     Flags::value( Flags::Book::MultiShared ) > {
      friend EntryDataBase< T, types::Categories::Hist > ;
      friend BookStore ;
      static constexpr int D = types::TypeInfo<T>::dimension;

      explicit EntryData(
        const EntryDataBase< T, types::Categories::Hist> &data )
        : _data{data} {}

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 1D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 1, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 2D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 2, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      /**
       *  @brief book Histogram in MultiCopy Mode. Only available for 3D Hist.
       *  @param store to where book Histogram.
       */
      template < typename... Args_t, int d = D >
      std::enable_if_t< d == 3, std::shared_ptr< Entry > >
      book( BookStore &store, const Args_t &... args ) const ;

      const EntryDataBase< T, types::Categories::Hist> &_data ;
      using Object_t = T;
    } ;

  } // end namespace book
} // end namespace marlin
