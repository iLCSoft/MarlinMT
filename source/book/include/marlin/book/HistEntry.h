#pragma once

// -- std includes
#include <functional>
#include <type_traits>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/Types.h"
#include "marlin/book/Entry.h"
#include "marlin/book/Flags.h"


namespace marlin {
  namespace book {  

    /// Handle specialisation for Histograms.
    template < typename Config>
    class Handle< types::HistT< Config > >
      : private BaseHandle< types::HistT<Config> > {
      friend BookStore ;
      static constexpr int D = types::HistT<Config>::Dimension;

    public:
      /// Histogram Type which is Handled
      using Type = types::HistT<Config> ;
      /// CoordArray_t from managed Histogram
      using Point_t = typename Type::Point_t ;
      /// Weigh_t from managed Histogram
      using Weight_t = typename Type::Weight_t ;

      using FinalizeFn_t = std::function< void() > ;

    public:
      /// construct a Handle.
      Handle( const std::shared_ptr< MemLayout > &mem,
              const std::shared_ptr< Type > &     obj,
              const std::shared_ptr<void>& data,
              Flag_t type,  
              FinalizeFn_t                 finalFn ) ;


      /**
       *  @brief Adds one datum to the Histogram.
       *  @param x point to add.
       *  @param w weight of point.
       */
      inline void fill( const Point_t &x, const Weight_t &w ) ;

/*     
       *  @brief Adds N data to the Histogram.
      template<typename Point_Itr, typename Weight_Itr>
      void fillN(
          Point_Itr pFirst, Point_Itr pLast,
          Weight_Itr wFirst, Weight_Itr wLast ) ;*/

      template<typename PointContainer, typename WeightContainer>
      void fillN(
          const PointContainer& points,
          const WeightContainer& weights);

      /**
       *  @brief get completed Object.
       *  @return Object which all data from every handle.
       *  @note for MultyCopy objects expansive.
       *  Creates every time a new merged Histogram.
       */
      const Type &merged() ;

    private:
      template<std::size_t I = 0>
      inline void fillImp(
          const Point_t& x,
          const Weight_t& w) {
        using EntryType = std::tuple_element_t<I, EntryTypes<Type>>;
        if(_type == EntryType::Flag) {
          EntryType::fill(_data, x, w);
        }
        else if constexpr (I + 1 < (std::tuple_size_v<EntryTypes<Type>>)) {
          fillImp<I + 1>(x, w);
        }
      }
      template<std::size_t I = 0>
      inline void fillNImp(
          const Point_t* pFirst, const Point_t* pLast,
          const Weight_t* wFirst, const Weight_t* wLast) {
        using EntryType = std::tuple_element_t<I, EntryTypes<Type>>;
        if(_type == EntryType::Flag) {
          EntryType::fillN(_data, pFirst, pLast, wFirst, wLast);
        }
        else if constexpr (I + 1 < (std::tuple_size_v<EntryTypes<Type>>)) {
          fillNImp<I + 1>(pFirst, pLast, wFirst, wLast);
        }
      }

      FinalizeFn_t _finalFn ;
      std::shared_ptr<void> _data;
      Flag_t _type;
      
    } ;

    /// specialisation of EntrySingle for Histograms
    template < typename Config>
    class EntrySingle< types::HistT<Config> > : public EntryBase {
      friend BookStore ;
      static constexpr int D = types::HistT<Config>::Dimension;

    public:
      /// Type of contained Histogram.
      using Type = types::HistT<Config> ;
      using Point_t = typename Type::Point_t;
      using Weight_t = typename Type::Weight_t;

      static constexpr Flag_t Flag = EntrySingle<void>::Flag;
      static void fill(const std::shared_ptr<void>& data,
          typename Type::Point_t const& x,
          typename Type::Weight_t const& w) {
        static_cast<Type*>(data.get())->Fill(x,w);
      }

      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast) {
        static_cast<Type*>(data.get())->FillN(
            pFirst, pLast, wFirst, wLast);
      }

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
    template < typename Config>
    class EntryMultiCopy< types::HistT<Config>> : public EntryBase {

      friend BookStore ;

    public:
      using Type = types::HistT<Config>;
      using Point_t = typename Type::Point_t;
      using Weight_t = typename Type::Weight_t;

      static constexpr Flag_t Flag = EntryMultiCopy<void>::Flag;
      static void fill(const std::shared_ptr<void>& data,
          Point_t const& x,
          Weight_t const& w) {
        static_cast<Type*>(data.get())->Fill(x,w);
      }

      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast) {
        static_cast<Type*>(data.get())->FillN(
            pFirst, pLast, wFirst, wLast);
      }


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
      Handle< Type  > handle( std::size_t idx ) ;

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;
    /// specialisation of EntryMultiShared for Histograms
    template < typename Config>
    class EntryMultiShared< types::HistT<Config>> : public EntryBase {
      friend BookStore ;

    public:
      static constexpr Flag_t Flag = EntryMultiShared<void>::Flag;
      using Type = types::HistT<Config>;
      using Point_t = typename Type::Point_t;
      using Weight_t = typename Type::Weight_t;

      static void fill(const std::shared_ptr<void>& data,
          typename Type::Point_t const& x,
          typename Type::Weight_t const& w) {
        static_cast<types::HistConcurrentFiller<Config>*>(data.get())->Fill(x,w);
      }
      
      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast) {
        static_cast<types::HistConcurrentFiller<Config>*>(data.get())->FillN(
            pFirst, pLast, wFirst, wLast);
      }

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
      std::shared_ptr< types::HistConcurrentFillManager< Config > > _fillMgr ;
      /// list of produced Filler to flush them when needed.
      std::vector< std::weak_ptr< types::HistConcurrentFiller< Config > > >
        _fillers ;
    } ;
  } // end namespace book
} // end namespace marlin
