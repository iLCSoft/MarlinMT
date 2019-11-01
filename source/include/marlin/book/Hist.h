#pragma once

// -- std includes
#include <functional>
#include <iostream>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/EntryData.h"
#include "marlin/book/ROOTAdapter.h"
#include "marlin/book/BookStore.h"

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
    template < typename,unsigned long long>
    class BookHelper ;

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

    template < typename T, template < int, class > class... STAT >
    class BookHelper<types::RH< 1,T,STAT...>, Flags::Book::Single.VAL_INIT> {
      friend BookStore;
      BookHelper(
        BookStore &store,
        const std::string_view &path,
        const std::string_view &name
      ) : _name {name},
          _path {path},
          _store {store} {}
      void setN(std::size_t n) {
        _amt = n;
      }

    public:
      EntrySingle<types::RH<1, T, STAT ...>>
      operator()(const types::RAxisConfig& axis) {
        return _store.book<
          types::RH<1,T,STAT...>,
          types::RAxisConfig
        >(_path, _name, axis);
      }
      BookHelper<types::RH<1,T,STAT...>,Flags::Book::Single.VAL_INIT>
      single() { return *this; }
      BookHelper<types::RH<1,T,STAT...>,Flags::Book::MultiCopy.VAL_INIT>
      multiCopy(std::size_t n) {
        BookHelper<types::RH<1, T, STAT...>, Flags::Book::MultiCopy.VAL_INIT> res(
          _store,
          _path,
          _name);
        res.setN(n);
        return res;
      }

    private:
      std::size_t _amt = 0;
      const std::string_view &_name;
      const std::string_view &_path;
      BookStore &_store;
    } ;
    
    template < typename T, template < int, class > class... STAT >
    class BookHelper<types::RH< 1,T,STAT...>, Flags::Book::MultiCopy.VAL_INIT> {
      friend BookHelper<types::RH<1,T,STAT...>,Flags::Book::Single.VAL_INIT>;

      BookHelper(
        BookStore &store,
        const std::string_view &path,
        const std::string_view &name
      ) : _name {name},
          _path {path},
          _store {store} {}
      void setN(std::size_t n) {
        _amt = n;
      }

    public:
      EntryMultiCopy<types::RH<1, T, STAT ...>>
      operator()(const types::RAxisConfig& axis) {
        return _store.bookMultiCopy<
          types::RH<1,T,STAT...>,
          types::RAxisConfig
        >(_amt, _path, _name, axis);

      }

    private:
      std::size_t _amt = 0;
      const std::string_view &_name;
      const std::string_view &_path;
      BookStore &_store;
    } ;

    /// Handle specialisation for Histograms.
    template < int D, typename T, template < int, class > class... STAT >
    class Handle< types::RH< D, T, STAT... > >
      : private BaseHandle< types::RH< D, T, STAT... > > {
      friend BookStore ;

    public:
      /// Histogram Type which is Handled
      using Type         = types::RH< D, T, STAT... > ;
      /// CoordArray_t from managed Histogram
      using CoordArray_t = typename Type::CoordArray_t ;
      /// Weigh_t from managed Histogram
      using Weight_t     = typename Type::Weight_t ;

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
      void        fill( const CoordArray_t &x, const Weight_t &w ) ;

      /**
       *  @brief Adds N data to the Histogram.
       *  @param x span of points to add.
       *  @param w span of weights to add.
       */
      void        fillN( const std::span< CoordArray_t > &x,
                         const std::span< Weight_t >     &w ) ;

      /**
       *  @brief get completed Object.
       *  @return Object which all data from every handle.
       *  @note for MultyCopy objects expansive.
       *  Creates every time a new merged Histogram. 
       */
      const Type &merged() ;

    private:
      /// Function to call for fill one object.
      FillFn_t     _fillFn ;
      /// Function to call for fill N objects.
      FillNFn_t    _fillNFn ;
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
       *  @note handles to the same instance should be only use in sequential code. 
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
      Context                                                      _context ;
      /// Manager to construct Filler.
      std::shared_ptr< types::RHistConcurrentFillManager< Type > > _fillMgr ;
      /// list of produced Filler to flush them when needed.
      std::vector< std::weak_ptr< types::RHistConcurrentFiller< Type > > >
        _fillers ;
    } ;
    
  } // end namespace book
} // end namespace marlin
