#pragma once

// -- std includes
#include <array>
#include <functional>
#include <iostream>
#include <type_traits>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/BookStore.h"
#include "marlin/book/EntryData.h"
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
    template < typename T >
    struct trait ;
    template < typename >
    class EntryDataBase;
    template <typename, unsigned long long>
    class EntryData;
    
    /// trait specialisation for Histograms.
    template < int D, typename T, template < int, class > class... STAT >
    struct trait< types::RH< D, T, STAT... > > {
      /**
       *  @brief merging two histograms using Add.
       *  @note \code {RHist::GetEntries()} is broken after that.
       */
      static void
      Merge( const std::shared_ptr< types::RH< D, T, STAT... > > &dst,
             const std::shared_ptr< types::RH< D, T, STAT... > > &src ) {
        Add( *dst, *src ) ;
      }
    } ;
    
    /**
     *  @brief Base for Histogram EntryData.
     */
    template<int D, typename T, template < int, class > class... STAT >
    class EntryDataBase< types::RH< D, T, STAT... >> : public EntryDataBase<void> {
      template<typename, unsigned long long>
      friend class EntryData;
      static constexpr std::string_view empty{""};
    public:
      EntryDataBase(const std::string_view& title = empty); 
      
      /**
       *  @brief construct EntryData for single booking.
       */
      EntryData< types::RH< D, T, STAT... >, Flags::Book::Single.VAL_INIT >
        single() const;

      /**
       *  @brief construct EntryData for multi copy booking.
       */
      EntryData<types::RH< D, T, STAT... >, Flags::Book::MultiCopy.VAL_INIT >
        multiCopy(std::size_t n) const;

      /**
       *  @brief construct EntryData for multi shared booking.
       */
      EntryData< types::RH< D, T, STAT... >, Flags::Book::MultiShared.VAL_INIT >
        multiShared() const;

    protected:
      const std::string_view& _title;
      std::array<const types::RAxisConfig*, D> _axis{};
    };

    template<typename T, template<int,class>class ...STAT>
    class EntryData<types::RH< 1, T, STAT...>, 0> : public EntryDataBase<types::RH< 1, T, STAT... >>{
    public:
      EntryData(
          const types::RAxisConfig& axis);

      EntryData(
          const std::string_view& title,
          const types::RAxisConfig& axis);
    };

    template<typename T, template<int,class>class ...STAT>
    class EntryData<types::RH< 2, T, STAT...>, 0> : public EntryDataBase<types::RH< 2, T, STAT... >>{
    public:
      EntryData(
          const types::RAxisConfig& x_axis,
          const types::RAxisConfig& y_axis);

      EntryData(
          const std::string_view& title,
          const types::RAxisConfig& x_axis,
          const types::RAxisConfig& y_axis);
    };
    
    template<typename T, template<int,class>class ...STAT>
    class EntryData<types::RH< 3, T, STAT...>, 0> : public EntryDataBase<types::RH< 3, T, STAT... >>{
    public:
      EntryData(
          const types::RAxisConfig& x_axis,
          const types::RAxisConfig& y_axis,
          const types::RAxisConfig& z_axis);
      
      EntryData(
          const std::string_view& title,
          const types::RAxisConfig& x_axis,
          const types::RAxisConfig& y_axis,
          const types::RAxisConfig& z_axis);
    };



    template<int D, typename T, template<int,class>class...STAT>
    class EntryData<types::RH<D,T,STAT...>, Flags::Book::Single.VAL_INIT>{
      friend EntryDataBase<types::RH< D, T, STAT... >>;
      friend BookStore;
      EntryData(const EntryDataBase<types::RH< D, T, STAT... >>& data)
        : _data{data}{}

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==1, EntrySingle<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookSingle<types::RH< 1, T, STAT... >, const types::RAxisConfig&>(
              args...,
              *_data._axis[0]
            );
      }

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==2, EntrySingle<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookSingle<types::RH< 2, T, STAT... >, const types::RAxisConfig&, const types::RAxisConfig&>(
          args...,
          *_data._axis[0],
          *_data._axis[1]
        );
      }

      template<typename ...Args_t, int d = D>
        std::enable_if_t<d==3,EntrySingle<types::RH< D, T, STAT... >>>
        book(BookStore& store, const Args_t& ... args)const {
          return store.bookSingle<types::RH< 3, T, STAT... >, const types::RAxisConfig&, const types::RAxisConfig&, const types::RAxisConfig&>(
            args...,
            *_data._axis[0],
            *_data._axis[1],
            *_data._axis[2]
            );
        }
      const EntryDataBase<types::RH< D, T, STAT... >>& _data;
    } ;

    template<int D, typename T, template<int,class>class...STAT>
    class EntryData<types::RH<D,T,STAT...>, Flags::Book::MultiCopy.VAL_INIT>{
      friend EntryDataBase<types::RH< D, T, STAT... >>;
      friend BookStore;
      EntryData(const EntryDataBase<types::RH< D, T, STAT... >>& data, std::size_t n)
        : _data{data}, _n{n}{}

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==1, EntryMultiCopy<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookMultiCopy<types::RH< 1, T, STAT... >, const types::RAxisConfig&>(
              _n,
              args...,
              *_data._axis[0]
            );
      }

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==2, EntryMultiCopy<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookMultiCopy<types::RH< 2, T, STAT... >,  const types::RAxisConfig&, const types::RAxisConfig&>(
          _n,
          args...,
          *_data._axis[0],
          *_data._axis[1]
        );
      }

      template<typename ...Args_t, int d = D>
        std::enable_if_t<d==3,EntryMultiCopy<types::RH< D, T, STAT... >>>
        book(BookStore& store, const Args_t& ... args)const {
          return store.bookMultiCopy<types::RH< 3, T, STAT... >, const types::RAxisConfig&, const types::RAxisConfig&, const types::RAxisConfig&>(
            _n,
            args...,
            *_data._axis[0],
            *_data._axis[1],
            *_data._axis[2]
            );
        }
      const EntryDataBase<types::RH< D, T, STAT... >>& _data;
      const std::size_t _n;
    } ;

  
    template<int D, typename T, template<int,class>class...STAT>
    class EntryData<types::RH<D,T,STAT...>, Flags::Book::MultiShared.VAL_INIT>{
      friend EntryDataBase<types::RH< D, T, STAT... >>;
      friend BookStore;
      EntryData(const EntryDataBase<types::RH< D, T, STAT... >>& data)
        : _data{data}{}

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==1, EntryMultiShared<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookMultiShared<types::RH< 1, T, STAT... >, const types::RAxisConfig&>(
              args...,
              *_data._axis[0]
            );
      }

      template<typename ...Args_t, int d = D>
      std::enable_if_t<d==2, EntryMultiShared<types::RH< D, T, STAT... >>>
      book(BookStore& store, const Args_t& ... args)const {
        return store.bookMultiShared<types::RH< 2, T, STAT... >,  const types::RAxisConfig&, const types::RAxisConfig&>(
          args...,
          *_data._axis[0],
          *_data._axis[1]
        );
      }

      template<typename ...Args_t, int d = D>
        std::enable_if_t<d==3,EntryMultiShared<types::RH< D, T, STAT... >>>
        book(BookStore& store, const Args_t& ... args)const {
          return store.bookMultiShared<types::RH< 3, T, STAT... >, const types::RAxisConfig&, const types::RAxisConfig&, const types::RAxisConfig&>(
            args...,
            *_data._axis[0],
            *_data._axis[1],
            *_data._axis[2]
            );
        }
      const EntryDataBase<types::RH< D, T, STAT... >>& _data;
    } ;

    /// Handle specialisation for Histograms.
    template < int D, typename T, template < int, class > class... STAT >
    class Handle< types::RH< D, T, STAT... > >
      : private BaseHandle< types::RH< D, T, STAT... > > {
      friend BookStore ;

    public:
      /// Histogram Type which is Handled
      using Type = types::RH< D, T, STAT... > ;
      /// CoordArray_t from managed Histogram
      using CoordArray_t = typename Type::CoordArray_t ;
      /// Weigh_t from managed Histogram
      using Weight_t = typename Type::Weight_t ;

      /// type of the RHist::Fill function.
      using FillFn_t
        = std::function< void( const CoordArray_t &, const Weight_t & ) > ;
      /// type of the RHist::FillN function
      using FillNFn_t = std::function< void( const std::span< CoordArray_t > &,
                                             const std::span< Weight_t > & ) > ;
      using FinalizeFn_t = std::function< void() > ;

    public:
      /// construct a Handle.
      Handle( const std::shared_ptr< MemLayout > &mem,
              const std::shared_ptr< Type > &     obj,
              const FillFn_t &                    fillFn,
              const FillNFn_t &                   fillNFn,
              const FinalizeFn_t &                finalFn ) ;

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
      void fillN( const std::span< CoordArray_t > &x,
                  const std::span< Weight_t > &    w ) ;

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
    class EntrySingle< types::RH< D, T, STAT... > > : public EntryBase {

      friend BookStore ;

    public:
      /// Type of contained Histogram.
      using Type = types::RH< D, T, STAT... > ;

      /// constructor
      EntrySingle( const Context &context ) ;

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
    class EntryMultiCopy< types::RH< D, T, STAT... > > : public EntryBase {

      friend BookStore ;

    public:
      /// Type of contained Histogram.
      using Type = types::RH< D, T, STAT... > ;

      /// constructor
      EntryMultiCopy( const Context &context ) ;

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
    class EntryMultiShared< types::RH< D, T, STAT... > > : public EntryBase {
      friend BookStore ;

    public:
      /// Type of contained Histogram.
      using Type = types::RH< D, T, STAT... > ;

      /// constructor
      EntryMultiShared( const Context &context ) ;

      // EntryMultiShared() = default ;
      /// destructor. Flush filler.
      ~EntryMultiShared() ;

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

  } // end namespace book
} // end namespace marlin
