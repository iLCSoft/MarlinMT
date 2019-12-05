#pragma once

// -- std includes
#include <functional>
#include <type_traits>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/Types.h"
#include "marlin/book/Entry.h"

namespace marlin {
  namespace book {  

    /// Handle specialisation for Histograms.
    template < typename T>
    class Handle< T, types::Categories::Hist>
      : private BaseHandle< T > {
      friend BookStore ;
      static constexpr int D = types::TypeInfo<T>::dimension;

    public:
      /// Histogram Type which is Handled
      using Type = T ;
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
    template < typename T>
    class EntrySingle< T, types::Categories::Hist > : public EntryBase {
      friend BookStore ;
      static constexpr int D = types::TypeInfo<T>::dimension;

    public:
      /// Type of contained Histogram.
      using Type = T ;

      /// constructor
      explicit EntrySingle( Context context ) ;

      /// default constructor. Constructs invalid Entry.
      EntrySingle() = default ;

      /**
       *  @brief creates new Handle for Contained Histogram.
       *  @note not thread save.
       */
      Handle< Type, types::Categories::Hist > handle() ;

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;

    /// specialisation of EntryMultiCopy for Histograms
    template < typename Type>
    class EntryMultiCopy< Type, types::Categories::Hist > : public EntryBase {

      friend BookStore ;

    public:

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
      Handle< Type, types::Categories::Hist > handle( std::size_t idx ) ;

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;
    /// specialisation of EntryMultiShared for Histograms
    template < typename Type>
    class EntryMultiShared< Type, types::Categories::Hist> : public EntryBase {
      friend BookStore ;

    public:

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
      Handle< Type, types::Categories::Hist > handle() ;

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
