#pragma once

// -- std includes
#include <type_traits>


// -- ROOT includes
#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"
#include "ROOT/RHistData.hxx"
#include "ROOT/span.hxx"

namespace marlin {
  namespace book {
    /// Alias for Types used by MarlinBook
    namespace types {

      /// Type for Describing a Histogram Axis.
      using RAxisConfig = ROOT::Experimental::RAxisConfig ;

      /// Generic Histogram.
      template < int D, typename T, template < int, class > class... STAT >
      using RH = ROOT::Experimental::RHist< D, T, STAT... > ;

      /// Histogram merge function.
      template < typename TO, typename FROM >
      void ADD( const TO &to, const FROM &from ) {
        ROOT::Experimental::Add( to, from ) ;
      }

      /// often used Histogram Instance
      using RH1D = ROOT::Experimental::RH1D ;
      /// often used Histogram Instance
      using RH1F = ROOT::Experimental::RH1F ;
      /// often used Histogram Instance
      using RH1I = ROOT::Experimental::RH1I ;

      /**
       *  @brief Buffer size for Histograms used in Shared mode.
       *  - larger →  less synchronisation points
       *  - larger →  more memory consumption.
       *  @note can set with the CMAKE  Variable \cod {NAME}
       */
      constexpr std::size_t HistogramFillerBufferSize
#       ifdef HISTOGRAM_FILLER_BUFFER_SIZE
          = HISTOGRAM_FILLER_BUFFER_SIZE;
#       else
          = 1024;
#       endif

      /**
       *  @brief produce concurrent filler for one Histogram.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFillManager
        = ROOT::Experimental::RHistConcurrentFillManager< T, 1024 > ;

      /**
       *  @brief to fill a Histogram concurrent.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFiller
        = ROOT::Experimental::RHistConcurrentFiller< T, 1024 > ;

    } // end namespace types
  } // end namespace book
} // end namespace marlin
