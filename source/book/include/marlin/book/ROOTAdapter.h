#pragma once

// -- std includes
#include <type_traits>

// -- ROOT includes
#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"
#include "ROOT/RHistData.hxx"
#include "ROOT/RSpan.hxx"
#include "RVersion.h"

namespace marlin {
  namespace book {
    /// Alias for Types used by MarlinBook
    namespace types {

      /// Type for Describing a Histogram Axis.
      using RAxisConfig = ROOT::Experimental::RAxisConfig ;

      /// Generic Histogram.
      template < int D, typename T, template < int, class > class... STAT >
      using RHist = ROOT::Experimental::RHist< D, T, STAT... > ;

      /// Histogram merge function.
      template < typename TO, typename FROM >
      void addHists( const TO &to, const FROM &from ) {
        ROOT::Experimental::Add( to, from ) ;
      }

      template <typename TO, typename FROM>
      void addHists( const std::shared_ptr<TO> &to,
                     const std::shared_ptr<FROM> &from ) {
        ROOT::Experimental::Add(*to, *from);
      }

      /// often used Histogram Instance
      using RH1D = ROOT::Experimental::RH1D ;
      /// often used Histogram Instance
      using RH1F = ROOT::Experimental::RH1F ;
      /// often used Histogram Instance
      using RH1I = ROOT::Experimental::RH1I ;
      /// often used Histograms Instance
      using RH2D = ROOT::Experimental::RH2D ;
      /// often used Histograms Instance.
      using RH2F = ROOT::Experimental::RH2F ;
      /// often used Histograms Instance.
      using RH2I = ROOT::Experimental::RH2I ;
      /// often used Histograms Instance.
      using RH3D = ROOT::Experimental::RH3D ;
      /// often used Histograms Instance.
      using RH3F = ROOT::Experimental::RH3F ;
      /// often used Histograms Instance.
      using RH3I = ROOT::Experimental::RH3I ;

#if ROOT_VERSION_CODE > ROOT_VERSION(16,18,4)
      template<typename T>
      using CoordArraySpan_t = std::span<typename T::CoordArray_t const>;

      template<typename T>
      using WeightSpan_t = std::span<typename T::Weight_t const>;
#else
      template<typename T>
      using CoordArraySpan_t = std::span<typename T::CoordArray_t>;

      template<typename T>
      using WeightSpan_t = std::span<typename T::Weight_t>;
#endif

#ifndef MARLIN_HIST_FILLER_BUFFER_SIZE
#define MARLIN_HIST_FILLER_BUFFER_SIZE 1024
#endif
      /**
       *  @brief Buffer size for Histograms used in Shared mode.
       *  - larger →  less synchronisation points
       *  - larger →  more memory consumption.
       *  @note can set with the CMAKE  Variable \cod {MARLIN_HIST_FILLER_BUFFER_SIZE}
       */
      constexpr std::size_t HistogramFillerBufferSize = MARLIN_HIST_FILLER_BUFFER_SIZE;

      /**
       *  @brief produce concurrent filler for one Histogram.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFillManager
        = ROOT::Experimental::RHistConcurrentFillManager< T, MARLIN_HIST_FILLER_BUFFER_SIZE > ;

      /**
       *  @brief to fill a Histogram concurrent.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFiller
        = ROOT::Experimental::RHistConcurrentFiller< T, MARLIN_HIST_FILLER_BUFFER_SIZE > ;

    } // end namespace types
  } // end namespace book
} // end namespace marlin
