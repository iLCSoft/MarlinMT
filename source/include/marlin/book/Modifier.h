#pragma once

// -- std header
#include <functional>

// -- root header
#include "ROOT/RSpan.hxx"

namespace marlin {
  namespace book {

    /**
     *  @brief BaseClass for modifier.
     *  nedded to clear Moidifer befor merging results.
     */
    class Modifier {
    public:
      /**
       *  @brief writes every modifications to the memory object.
       */
      virtual void flush() = 0;

      /**
       * @brief check if the modifer used.
       * @return true if modifier contains no commitet changes.
       */
      virtual bool isModified() const = 0;

      virtual ~Modifier() = default;
    };

    template<class T>
    class ModifierHist : public Modifier {
      
      using FillFn_t 
        = std::function<
            void(const typename T::CoordArray_t&, const typename T::Weight_t&)
        > ;

      using FillNFn_t 
        = std::function<
          void(
            const std::span<typename T::CoordArray_t>&,
            const std::span<typename T::Weight_t>&
        )>;

        using FlushFn_t = std::function<void(void)>;

    public:
      ~ModifierHist() override = default;
      
      /**
       *  @brief Connstructor.
       *  @param fillFn functor for fill one object in Hist.
       *  @param fillNFn functor for fill N objects in Hist.
       *  @param flushFn functior for flushing filler if exist.
       */
      ModifierHist(
        FillFn_t fillFn,
        FillNFn_t fillNFn,
        FlushFn_t flushFn
      ) : _fillFn {fillFn},
          _fillNFn {fillNFn},
          _flushFn {flushFn}
      {}  

      /**
       *   @brief Flushs filler queue if exist.
       */
      void flush() override final {
        _modified = false ;
        _flushFn() ;
      }
      
      /**
       *  @brief check if the Filler has unmerged changes.
       *  @return true if Filler was used since last Flush.
       *  @attention only for internal usage.
       */
      bool isModified() const override final {
        return _modified ;
      }

      /**
       * @brief fill one object in hist.
       * @param x value to add.
       * @param w value's weight
       */
      void fill( 
        const typename T::CoordArray_t& x,
        const typename T::Weight_t& w) {

        _modified = true ;
        _fillFn(x, w) ;
      }

      /**
       * @brief fills N objects in hist.
       * For mapping value weight the index is used.
       * @param x values to add.
       * @param w value's weights.
       */
      void fillN (
        const std::span<typename T::CoordArray_t>& x,
        const typename std::span<typename T::Weight_t>& w ) {
        _modified = true ;
        _fillNFn( x, w ) ;
      }

    private:
      /// Functor for fill
      FillFn_t  _fillFn   {}      ;
      /// Functor for fillN
      FillNFn_t _fillNFn  {}      ;
      /// Functor for flush
      FlushFn_t _flushFn  {}      ;
      /// flag to track if modified.
      bool      _modified {false} ;
    };

  } // end namespace book
} // end namespace marlin


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

