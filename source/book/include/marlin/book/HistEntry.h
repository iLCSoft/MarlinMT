#pragma once


// -- std includes
#include <functional>
#include <mutex>
#include <type_traits>
#include <vector>

// -- MarlinBook includes
#include "marlin/book/Entry.h"
#include "marlin/book/Flags.h"
#include "marlin/book/Types.h"


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
      /// Function signature called when close Handle.
      using FinalizeFn_t = std::function< void() > ;

    public:
      /// construct a Handle.
      Handle( const std::shared_ptr< MemLayout > &mem,
              const std::shared_ptr< Type > &     obj,
              std::shared_ptr<void> data,
              Flag_t type,  
              FinalizeFn_t                 finalFn ) ;


      /**
       *  @brief Adds one datum to the Histogram.
       *  @param x point to add.
       *  @param w weight of point.
       */
      void fill( const Point_t &x, const Weight_t &w ) ;


      /**
       *  @brief add N entries. 
       *  @param points container containing N points 
       *  @param weights container containing N weights
       *  @tparam PointContainer container of Point_t with linear memory, begin() and end()
       *  @tparam WeightContainer container of Weight_t with linear memory, begin() and end()
       *  @attention objects must be stored in linear memory
       */
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
      /**
       *  @brief call implementation of fill depend of _type 
       *  @param x
       *  @param w
       */
      template<std::size_t I = 0>
      void fillImp(const Point_t& x, const Weight_t& w);



      /**
       *  @brief call implementation of fillN depend of _type. 
       *  @param pFirst address of first Point 
       *  @param pLast address of last Point (not included)
       *  @param wFirst address of first Weight
       *  @param wLast address of last Weight (not included)
       *  @note only support linear memory 
       */
      template<std::size_t I = 0>
      void fillNImp(
        const Point_t* pFirst, const Point_t* pLast,
        const Weight_t* wFirst, const Weight_t* wLast); 



      /// functor called when merging.
      FinalizeFn_t _finalFn ;
      /// pointer to data defined from entry.
      std::shared_ptr<void> _data;
      /// entry type decoded as flag.
      const Flag_t _type;
      
    } ;

    /// specialisation of EntrySingle for Histograms
    template < typename Config>
    class EntrySingle< types::HistT<Config> > : public EntryBase {

      friend BookStore ;
      friend Handle<types::HistT<Config>> ;

    public:
      /// Type of contained Histogram.
      using Type = types::HistT<Config> ;
      /// Point type for Hist
      using Point_t = typename Type::Point_t ;
      /// Weight type for Hist
      using Weight_t = typename Type::Weight_t ;

      /// Type Flag. Inherited from default EntrySingle.
      static constexpr Flag_t Flag = EntrySingle<void>::Flag ;

    private:
      static void fill(const std::shared_ptr<void>& data,
          Point_t const& x, Weight_t const& w);

      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast) ;

    public:
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
      friend Handle<types::HistT<Config>> ;

    public:
      /// Type of contained Histogram.
      using Type = types::HistT<Config> ;
      /// Point type for Hist
      using Point_t = typename Type::Point_t ;
      /// Weight type for Hist
      using Weight_t = typename Type::Weight_t ;

      /// Type Flag. Inherited from default EntrySingle.
      static constexpr Flag_t Flag = EntryMultiCopy<void>::Flag;

    private:
      // TODO: Fix Doxygen links
      /// add one entry. {\ref EntrySingle<types::HistT<Config>>::fill} for more information
      static void fill(const std::shared_ptr<void>& data,
          Point_t const& x,
          Weight_t const& w);

      /// add N entry. /ref EntrySingle<types::HistT<Config>>::fillN for more information
      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast);

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
      Handle< Type  > handle( std::size_t idx ) ;

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;
    /// specialisation of EntryMultiShared for Histograms
    template < typename Config>
    class EntryMultiShared< types::HistT<Config>> : public EntryBase {

      friend BookStore ;
      friend Handle<types::HistT<Config>> ;
      
    public:
      /// Type of contained Histogram.
      using Type = types::HistT<Config> ;
      /// Point type for Hist
      using Point_t = typename Type::Point_t ;
      /// Weight type for Hist
      using Weight_t = typename Type::Weight_t ;

      /// Type Flag. Inherited from default EntrySingle.
      static constexpr Flag_t Flag = EntryMultiShared<void>::Flag;

    private:
      /// add one entry. /ref EntrySingle<types::HistT<Config>>::fill for more information
      static void fill(const std::shared_ptr<void>& data,
          Point_t const& x,
          Weight_t const& w);

      /// add N entry. /ref EntrySingle<types::HistT<Config>>::fill for more information
      static void fillN(const std::shared_ptr<void>& data,
          Point_t const* pFirst, Point_t const* pLast,
          Weight_t const* wFirst, Weight_t const* wLast);

    public:
      /// constructor
      explicit EntryMultiShared( Context context ) ;

      // EntryMultiShared() = default ;
      /// destructor. Flush filler.
      ~EntryMultiShared() ;

      EntryMultiShared(const EntryMultiShared&) = default;
      EntryMultiShared& operator=(const EntryMultiShared&) = default ;
      EntryMultiShared(EntryMultiShared&&)  = default;
      EntryMultiShared& operator=(EntryMultiShared && ) = default;

      /**
       *  @brief creates a new Handle.
       *  @note each Handle contains a buffer to reduce synchronisation.
       *  This memory will be freed when the handle is destructed.
       */
      Handle< Type > handle() ;

      /**
       *  @brief creates a new Handle with existing buffer.
       */
      Handle< Type > handle( std::size_t idx ) ;

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
      /// list of permanent Filler
      std::vector< 
        std::shared_ptr<types::HistConcurrentFiller< Config > > > _staticFiller {};
      /// lock _fillers when extend memory 
      std::mutex _fillersExtend {};
    } ;
  } // end namespace book
} // end namespace marlin
