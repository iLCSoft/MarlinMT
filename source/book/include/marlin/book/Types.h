#pragma once

// -- std includes
#include <type_traits>

// -- ROOT includes
#include "RVersion.h"

#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"
#include "ROOT/RHistData.hxx"
#include "ROOT/RSpan.hxx"
#include "ROOT/RFile.hxx"
#include "ROOT/RDirectory.hxx"

namespace marlin {
  namespace book {
    /// Alias for Types used by MarlinBook
    namespace types {

      enum struct Categories { None, Hist};

      template<typename T>
      struct TypeInfo {
        static constexpr Categories category = Categories::None;
      };

      template< int D, typename T >
      struct HistType {
        static_assert(D != D, "No histogram type for this argument combination.");
      };

      template< int D, typename T >
      using HistType_t = typename HistType<D,T>::type;

      template<typename T>
      constexpr Categories category_of = TypeInfo<T>::category;




      /// Type for Describing a Histogram Axis.
      using RAxisConfig = ROOT::Experimental::RAxisConfig ;


#define AddHistType(Alias, Type, Dim) \
      using Alias = Type; \
      template<> struct TypeInfo<Alias> { \
        static constexpr Categories category = Categories::Hist;\
        static constexpr int dimension = Dim;\
      };\
      template<> struct HistType<Dim, typename Type::Weight_t> {\
        using type = Type;\
      }

      /// often used Histogram Instance
      AddHistType(RH1F, ROOT::Experimental::RH1F, 1);
      AddHistType(RH1D, ROOT::Experimental::RH1D, 1);
      AddHistType(RH1I, ROOT::Experimental::RH1I, 1);
      AddHistType(RH2F, ROOT::Experimental::RH2F, 2);
      AddHistType(RH2D, ROOT::Experimental::RH2D, 2);
      AddHistType(RH2I, ROOT::Experimental::RH2I, 2);
      AddHistType(RH3F, ROOT::Experimental::RH3F, 3);
      AddHistType(RH3D, ROOT::Experimental::RH3D, 3);
      AddHistType(RH3I, ROOT::Experimental::RH3I, 3);


      /// Histogram merge function.
      template < typename TO, typename FROM >
      void addHists( const TO &to, const FROM &from ) {
        ROOT::Experimental::Add( to, from ) ;
      }

      template < typename TO, typename FROM >
      void addHists( const std::shared_ptr< TO > &  to,
                     const std::shared_ptr< FROM > &from ) {
        ROOT::Experimental::Add( *to, *from ) ;
      }

#if ROOT_VERSION_CODE > ROOT_VERSION( 6, 18, 4 )
      template < typename T >
      using CoordArraySpan_t = std::span< typename T::CoordArray_t const > ;

      template < typename T >
      using WeightSpan_t = std::span< typename T::Weight_t const > ;
#else
      template < typename T >
      using CoordArraySpan_t = std::span< typename T::CoordArray_t > ;

      template < typename T >
      using WeightSpan_t = std::span< typename T::Weight_t > ;
#endif

#ifndef MARLIN_HIST_FILLER_BUFFER_SIZE
# define MARLIN_HIST_FILLER_BUFFER_SIZE 1024
#endif
      /**
       *  @brief Buffer size for Histograms used in Shared mode.
       *  - larger →  less synchronisation points
       *  - larger →  more memory consumption.
       *  @note can set with the CMAKE  Variable \cod
       * {MARLIN_HIST_FILLER_BUFFER_SIZE}
       */
      constexpr std::size_t HistogramFillerBufferSize
        = MARLIN_HIST_FILLER_BUFFER_SIZE ;

      /**
       *  @brief produce concurrent filler for one Histogram.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFillManager = ROOT::Experimental::
        RHistConcurrentFillManager< T, MARLIN_HIST_FILLER_BUFFER_SIZE > ;

      /**
       *  @brief to fill a Histogram concurrent.
       *  @tparam T Histogram type
       */
      template < class T >
      using RHistConcurrentFiller = ROOT::Experimental::
        RHistConcurrentFiller< T, MARLIN_HIST_FILLER_BUFFER_SIZE > ;


      using RFile = ROOT::Experimental::RFile;
      using RFilePtr = ROOT::Experimental::RFilePtr;
      using RDirectory = ROOT::Experimental::RDirectory;
/*      namespace RFile {
        using Options_t = ROOT::Experimental::RFile::Options_t;
        constexpr auto Create =
          static_cast<RFilePtr (*)(std::string_view, const Options_t&)>(
            ROOT::Experimental::RFile::Create
          );
      } // end namespace RFile */

    } // end namespace types
    
    // -- MarlinBook forward declarations
    class BookStore ;
    template < typename T, types::Categories = types::category_of<T>>
    class Handle ;
    template < typename T >
    class BaseHandle ;

  } // end namespace book
} // end namespace marlin
