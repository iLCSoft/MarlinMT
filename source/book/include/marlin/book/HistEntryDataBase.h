#pragma  once

// -- std includes
#include <array>

// -- MarlinBook includes
#include  "marlin/book/EntryData.h"
#include  "marlin/book/ROOTAdapter.h"

namespace marlin {
  namespace book {
    
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
      EntryData< types::RHist< D, T, STAT... >,
                 Flags::value( Flags::Book::Single ) >
      single() const ;

      /**
       *  @brief construct EntryData for multi copy booking.
       *  @param n number of memory instances which should be constructed
       */
      [[nodiscard]] EntryData< types::RHist< D, T, STAT... >,
                               Flags::value( Flags::Book::MultiCopy ) >
      multiCopy( std::size_t n ) const ;

      /**
       *  @brief construct EntryData for multi shared booking.
       */
      [[nodiscard]] EntryData< types::RHist< D, T, STAT... >,
                               Flags::value( Flags::Book::MultiShared ) >
      multiShared() const ;

    protected:
      /**
       *  @brief read access for passed title.
       */
      [[nodiscard]] const std::string_view &title() const { return _title; }

      /**
       *  @brief access Histogram axis configuration.
       *  @param id of axis, start by 0.
       */
      [[nodiscard]] std::unique_ptr< types::RAxisConfig > &
      axis( const std::size_t id ) {
        return _axis.at( id ) ;
      }

      /**
       *  @brief access Histogram axis configuration. Can't Modified.
       *  @param id of axis, start by 0.
       */
      [[nodiscard]] const std::unique_ptr< types::RAxisConfig > &
      axis( const std::size_t id ) const {
        return _axis.at( id ) ;
      }

    private:
      /// Histogram custom title, length = 0 when nothing setted.
      const std::string_view &_title ;
      /// Histogram axis configuration.
      std::array< std::unique_ptr< types::RAxisConfig >, D > _axis{} ;
    } ;
  } // end namespace book
} // end namespace marlin
