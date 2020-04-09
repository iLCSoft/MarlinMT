// ROOT7 -> ROOT6 histogram converter (minimal declaration)
//
// You can use this simplified header to speed up your builds if you only call
// into_root_6_hist on RHist<DIMS, T> histograms without custom statistics.

#pragma once

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include <type_traits>


// Forward declarations for ROOT 7 types
namespace ROOT { namespace Experimental {
  template <int DIMS,
            class PRECISION,
            template <int D_, class P_> class... STAT>
  class RHist;
  //
  template <int D_, class P_>
  class RHistStatContent;
} }


// Evil machinery turning ROOT 7 histograms into ROOT 6 histograms
namespace marlinmt 
{
  namespace book {
    // Typing this gets old quickly
    namespace RExp = ROOT::Experimental;

    // Trick for static_asserts that only fail when a template is instantiated
    //
    // When you write a struct template that must always be specialized, you may
    // want to print a compiler error when the non-specialized struct is
    // instantiated, by adding a failing static_assert to it.
    //
    // However, you must then prevent the compiler from firing the static_assert
    // even when the non-specialized version of the template struct is never
    // instantiated, by making its evaluation "depend on" the template
    // parameters of the struct. This variable template seems to do the job.
    //
    template <typename T> constexpr bool always_false = false;


    // === TOP-LEVEL ENTRY POINT FOR INTO_ROOT6_HIST ===

    // ROOT 7 -> ROOT 6 histogram converter
    //
    // Must be specialized for every supported ROOT 7 histogram type. Falling
    // back on the base case means that a histogram conversion is not supported,
    // and will be reported as such.
    //
    // Every specialization will provide a "convert()" static function that
    // performs the conversion. That function takes the following parameters:
    //
    // - The ROOT 7 histogram that must be converted into a ROOT 6 one.
    // - A ROOT 6 histogram name (used for ROOT I/O, ROOT 7 doesn't have this)
    //
    template <typename Input, typename Enable = void>
    struct HistConverter
    {
      // Tell the user that we haven't implemented this conversion (yet?)
      static_assert(always_false<Input>, "Unsupported histogram conversion");

      // Dummy conversion function to keep compiler errors bounded
      static auto convert(const Input& src, const char* name);
    };


    // === CHECK THAT REQUIRED RHIST STATS ARE PRESENT ===

    // For a ROOT 7 histogram to be convertible to the ROOT 6 format, it must
    // collect the RHistStatContent statistic. Let's check for this.
    template <template <int D_, class P_> class... STAT>
    constexpr bool stats_ok = true;

    // If the user declares an RHist with an empty stats list, ROOT silently
    // adds RHistStatContent. So we must special-case this empty list.
    template <>
    constexpr bool stats_ok<> = true;

    // If there is only one stat in the list, then the assertion will succeed or
    // fail depending on if this stat is RHistStatContent.
    template <template <int D_, class P_> class SINGLE_STAT>
    constexpr bool stats_ok<SINGLE_STAT> = false;
    template <>
    constexpr bool stats_ok<RExp::RHistStatContent> = true;

    // If there are 2+ stats in the list, then we iterate through recursion.
    // This case won't catch the 1-stat scenario due to above specializations.
    template <template <int D_, class P_> class STAT_HEAD,
              template <int D_, class P_> class... STAT_TAIL>
    constexpr bool stats_ok<STAT_HEAD, STAT_TAIL...> =
      stats_ok<STAT_HEAD> || stats_ok<STAT_TAIL...>;

    // We'll also want a nice compiler error message in the failing case
    template <template <int D_, class P_> class... STAT>
    struct CheckStats : public std::bool_constant<stats_ok<STAT...>> {
      static_assert(stats_ok<STAT...>,
                    "Only ROOT 7 histograms that record RHistStatContent "
                    "statistics may be converted into ROOT 6 histograms");
    };

    // ...and then we can add a nicer user interface on top of that check
    template <template <int D_, class P_> class... STAT>
    static constexpr bool CheckStats_v = CheckStats<STAT...>::value;


    // === LOOK UP THE ROOT 6 EQUIVALENT OF OUR RHIST (IF ANY) ===

    // We also need a machinery that, given a ROOT 7 histogram type, can give us
    // the corresponding ROOT 6 histogram type, if any.
    //
    // This must be done via specialization, so let's define the failing case...
    //
    template <int DIMENSIONS, class PRECISION>
    struct CheckRoot6Type : public std::false_type {
      static_assert(always_false<PRECISION>,
                    "No known ROOT 6 histogram type matches the input "
                    "histogram's dimensionality and precision");
    };

    // ...then we can define the successful cases...
    template <>
    struct CheckRoot6Type<1, Char_t> : public std::true_type {
      using type = TH1C;
    };
    template <>
    struct CheckRoot6Type<1, Short_t> : public std::true_type {
      using type = TH1S;
    };
    template <>
    struct CheckRoot6Type<1, Int_t> : public std::true_type {
      using type = TH1I;
    };
    template <>
    struct CheckRoot6Type<1, Float_t> : public std::true_type {
      using type = TH1F;
    };
    template <>
    struct CheckRoot6Type<1, Double_t> : public std::true_type {
      using type = TH1D;
    };

    template <>
    struct CheckRoot6Type<2, Char_t> : public std::true_type {
      using type = TH2C;
    };
    template <>
    struct CheckRoot6Type<2, Short_t> : public std::true_type {
      using type = TH2S;
    };
    template <>
    struct CheckRoot6Type<2, Int_t> : public std::true_type {
      using type = TH2I;
    };
    template <>
    struct CheckRoot6Type<2, Float_t> : public std::true_type {
      using type = TH2F;
    };
    template <>
    struct CheckRoot6Type<2, Double_t> : public std::true_type {
      using type = TH2D;
    };

    template <>
    struct CheckRoot6Type<3, Char_t> : public std::true_type {
      using type = TH3C;
    };
    template <>
    struct CheckRoot6Type<3, Short_t> : public std::true_type {
      using type = TH3S;
    };
    template <>
    struct CheckRoot6Type<3, Int_t> : public std::true_type {
      using type = TH3I;
    };
    template <>
    struct CheckRoot6Type<3, Float_t> : public std::true_type {
      using type = TH3F;
    };
    template <>
    struct CheckRoot6Type<3, Double_t> : public std::true_type {
      using type = TH3D;
    };

    // ...and then we can add a nice user interface to get the ROOT6 hist type...
    template <int DIMENSIONS, class PRECISION>
    using CheckRoot6Type_t =
      typename CheckRoot6Type<DIMENSIONS, PRECISION>::type;

    // ...and the truth that a suitable ROOT6 hist type exists.
    template <int DIMENSIONS, class PRECISION>
    static constexpr bool CheckRoot6Type_v =
      CheckRoot6Type<DIMENSIONS, PRECISION>::value;


    // === UNCHECKED HISTOGRAM CONVERTER ===

    // Convert a ROOT 7 histogram into a ROOT 6 one
    //
    // This template does not validate its input type arguments. You'll get
    // horrible C++ template error messages if you get them wrong. Use the
    // type-checked into_root6_hist API instead to avoid this.
    //
    template <class Output, class Input>
    Output convert_hist(const Input& src, const char* name);

    // Explicit instantiations are provided for all basic histogram types
    extern template TH1C convert_hist(const RExp::RHist<1, Char_t>&, const char*);
    extern template TH1S convert_hist(const RExp::RHist<1, Short_t>&, const char*);
    extern template TH1I convert_hist(const RExp::RHist<1, Int_t>&, const char*);
    extern template TH1F convert_hist(const RExp::RHist<1, Float_t>&, const char*);
    extern template TH1D convert_hist(const RExp::RHist<1, Double_t>&, const char*);
    //
    extern template TH2C convert_hist(const RExp::RHist<2, Char_t>&, const char*);
    extern template TH2S convert_hist(const RExp::RHist<2, Short_t>&, const char*);
    extern template TH2I convert_hist(const RExp::RHist<2, Int_t>&, const char*);
    extern template TH2F convert_hist(const RExp::RHist<2, Float_t>&, const char*);
    extern template TH2D convert_hist(const RExp::RHist<2, Double_t>&, const char*);
    //
    extern template TH3C convert_hist(const RExp::RHist<3, Char_t>&, const char*);
    extern template TH3S convert_hist(const RExp::RHist<3, Short_t>&, const char*);
    extern template TH3I convert_hist(const RExp::RHist<3, Int_t>&, const char*);
    extern template TH3F convert_hist(const RExp::RHist<3, Float_t>&, const char*);
    extern template TH3D convert_hist(const RExp::RHist<3, Double_t>&, const char*);


    // === CHECKED HISTOGRAM CONVERTER ===

    // This specialization of HistConverter uses SFINAE to assert that the input
    // ROOT7 histogram can be converted into a ROOT6 histogram, then calls the
    // unchecked convert_hist converter defined above.
    template <int DIMS,
              class PRECISION,
              template <int D_, class P_> class... STAT>
    struct HistConverter<RExp::RHist<DIMS, PRECISION, STAT...>,
                         std::enable_if_t<CheckRoot6Type_v<DIMS, PRECISION>
                                          && CheckStats_v<STAT...>>>
    {
    private:
      using Input = RExp::RHist<DIMS, PRECISION, STAT...>;
      using Output = CheckRoot6Type_t<DIMS, PRECISION>;

    public:
      static Output convert(const Input& src, const char* name) {
        return convert_hist<Output>(src, name);
      }
    };

    // TODO: Support THn someday, if someone asks for it
  } // end namespace book
} // end namespace marlinmt


// High-level interface to the above conversion machinery
//
// "src" is the ROOT 7 histogram to be converted, and "name" is a ROOT 6
// histogram name (used for ROOT I/O, should be unique).
//
template <typename Root7Hist>
auto into_root6_hist(const Root7Hist& src, const char* name) {
  return marlinmt::book::HistConverter<Root7Hist>::convert(src, name);
}
// ROOT7 -> ROOT6 histogram converter (full header)
//
// You only need to use this header if you want to instantiate into_root_6_hist
// for a ROOT7 histogram with a custom statistics configuration (that is, a
// RHist<DIMS, T, STAT...> histogram with a non-empty STAT... list).
//
// See histConv.hpp.dcl for the basic declarations, which may be all you need.


#include "ROOT/RAxis.hxx"
#include "ROOT/RHist.hxx"
#include "ROOT/RHistImpl.hxx"
#include "TAxis.h"

#include <cxxabi.h>
#include <exception>
#include <sstream>
#include <string>
#include <tuple>
#include <typeinfo>
#include <utility>


namespace marlinmt
{
  namespace book {
    // === BUILD A THx, FAILING AT RUNTIME IF NO CONSTRUCTOR EXISTS ===

    // TH1 and TH2 constructors enable building histograms with all supported
    // axes configurations. TH3, however, only provide constructors for
    // all-equidistant and all-irregular axis configurations.
    //
    // We do not know the axis configuration of a ROOT 7 histogram until runtime,
    // therefore we must fail at runtime when a ROOT 7 histogram with incompatible
    // axis configuration is requested.
    //
    // So, in the general case, used by TH1 and TH2, we just build a ROOT 6
    // histogram with the specified constructor parameters...
    //
    template <int DIMS>
    struct MakeRoot6Hist
    {
      template <typename Output, typename... BuildParams>
      static Output make(std::tuple<BuildParams...>&& build_params) {
        return std::make_from_tuple<Output>(std::move(build_params));
      }
    };

    // ...while in the TH3 case, we detect incompatible axis configurations and
    // fail at runtime upon encountering them.
    template <>
    struct MakeRoot6Hist<3>
    {
      // Generally speaking, we fail at runtime...
      template <typename Output, typename... BuildParams>
      static Output make(std::tuple<BuildParams...>&& th3_params) {
        std::ostringstream s;
        char * args_type_name;
        int status;
        args_type_name = abi::__cxa_demangle(typeid(th3_params).name(),
                                             0,
                                             0,
                                             &status);
        s << "Unsupported TH3 axis configuration"
          << " (no constructor from argument-tuple " << args_type_name
          << ')';
        free(args_type_name);
        throw std::runtime_error(s.str());
      }

      // ...except in the two cases where it actually works!
      template <typename Output>
      static Output make(std::tuple<const char*, const char*,
                                    Int_t, Double_t, Double_t,
                                    Int_t, Double_t, Double_t,
                                    Int_t, Double_t, Double_t>&& th3_params) {
        return std::make_from_tuple<Output>(std::move(th3_params));
      }
      template <typename Output>
      static Output make(std::tuple<const char*, const char*,
                                    Int_t, const Double_t*,
                                    Int_t, const Double_t*,
                                    Int_t, const Double_t*>&& th3_params) {
        return std::make_from_tuple<Output>(std::move(th3_params));
      }
    };


    // === MISC UTILITIES TO EASE THE ROOT7-ROOT6 IMPEDANCE MISMATCH ===

    // Convert a ROOT 7 histogram title into a ROOT 6 histogram title
    std::string convert_hist_title(const std::string& title);

    // Map from ROOT 7 axis index to ROOT 6 histogram axes
    TAxis& get_root6_axis(TH1& hist, size_t idx);
    TAxis& get_root6_axis(TH2& hist, size_t idx);
    TAxis& get_root6_axis(TH3& hist, size_t idx);

    // ROOT 7-like GetBinFrom for ROOT 6
    Double_t get_bin_from_root6(const TAxis& axis, Int_t bin);
    std::array<Double_t, 1> get_bin_from_root6(const TH1& hist, Int_t bin);
    std::array<Double_t, 2> get_bin_from_root6(const TH2& hist, Int_t bin);
    std::array<Double_t, 3> get_bin_from_root6(const TH3& hist, Int_t bin);

    // Transfer histogram axis settings which are common to all axis
    // configurations (currently equidistant, growable, irregular and labels)
    void setup_axis_base(TAxis& dest, const RExp::RAxisBase& src);


    // === MAIN CONVERSION FUNCTIONS ===

    // Shorthand for an excessively long name
    template <int DIMS>
    using RHistImplBase = RExp::Detail::RHistImplPrecisionAgnosticBase<DIMS>;

    // Create a ROOT 6 histogram whose global and per-axis configuration matches
    // that of an input ROOT 7 histogram as closely as possible.
    template <class Output, int AXIS, int DIMS, class... BuildParams>
    Output convert_hist_loop(const RHistImplBase<DIMS>& src_impl,
                             std::tuple<BuildParams...>&& build_params) {
      // This function is actually a kind of recursive loop for AXIS ranging
      // from 0 to the dimension of the histogram, inclusive.
      if constexpr (AXIS < DIMS) {
        // The first iterations query the input histogram axes one by one
        const auto axis_view = src_impl.GetAxis(AXIS);

        // Is this an equidistant axis?
        const auto* eq_axis_ptr = axis_view.GetAsEquidistant();
        if (eq_axis_ptr != nullptr) {
          const auto& eq_axis = *eq_axis_ptr;

          // Append equidistant axis constructor parameters to the list of
          // ROOT 6 histogram constructor parameters
          auto new_build_params =
            std::tuple_cat(
              std::move(build_params),
              std::make_tuple((Int_t)(eq_axis.GetNBinsNoOver()),
                              (Double_t)(eq_axis.GetMinimum()),
                              (Double_t)(eq_axis.GetMaximum()))
            );

          // Process other axes and construct the histogram
          auto dest =
            convert_hist_loop<Output,
                              AXIS+1>(src_impl,
                                      std::move(new_build_params));

          // Propagate basic axis properties
          setup_axis_base(get_root6_axis(dest, AXIS), eq_axis);

          // If the axis is labeled, propagate labels
          //
          // FIXME: There does not seem to be a way to go from RAxisView to axis
          //        labels at the moment. Even dynamic_cast will fail because
          //        RAxisXyz do not contain a single virtual method, and therefore
          //        do not have the required infrastructure for dcasting.
          //
          /* const auto* lbl_axis_ptr =
            dynamic_cast<const RExp::RAxisLabels*>(&eq_axis);
          if (lbl_axis_ptr) {
            auto labels = lbl_axis_ptr->GetBinLabels();
            for (size_t bin = 0; bin < labels.size(); ++bin) {
                std::string label{labels[bin]};
                dest.SetBinLabel(bin, label.c_str());
            }
          } */

          // Send back the histogram to caller
          return dest;
        }

        // Is this an irregular axis?
        const auto* irr_axis_ptr = axis_view.GetAsIrregular();
        if (irr_axis_ptr != nullptr) {
          const auto& irr_axis = *irr_axis_ptr;

          // Append irregular axis constructor parameters to the list of
          // ROOT 6 histogram constructor parameters
          auto new_build_params =
            std::tuple_cat(
              std::move(build_params),
              std::make_tuple(
                (Int_t)(irr_axis.GetNBinsNoOver()),
                (const Double_t*)(irr_axis.GetBinBorders().data())
              )
            );

          // Process other axes and construct the histogram
          auto dest =
            convert_hist_loop<Output,
                              AXIS+1>(src_impl,
                                      std::move(new_build_params));

          // Propagate basic axis properties
          // There aren't any other properties for irregular axes.
          setup_axis_base(get_root6_axis(dest, AXIS), irr_axis);

          // Send back the histogram to caller
          return dest;
        }

        // As of ROOT 6.18.0, there should be no other axis kind, so
        // reaching this point indicates a bug in the code.
        throw std::runtime_error("Unsupported histogram axis type");
      } else if constexpr (AXIS == DIMS) {
        // We've reached the bottom of the histogram construction recursion.
        // All histogram constructor parameters have been collected in the
        // build_params tuple, so we can now construct the ROOT 6 histogram.
        return MakeRoot6Hist<DIMS>::template make<Output>(
          std::move(build_params)
        );
      } else {
        // The loop shouldn't reach this point, there's a bug in the code
        static_assert(always_false<Output>,
                      "Invalid loop iteration in build_hist_loop");
      }
    }


    // Check that the input and output histogram use the same binning
    // conventions (starting index, N-d array serialization order) since we
    // currently rely on that assumption for fast histogram bin data transfer.
    template <class THx, int DIMS>
    void check_binning(const THx& dest, const RHistImplBase<DIMS>& src_impl)
    {
      // Check that bins from ROOT 7 are "close enough" to those from ROOT 7
      auto bins_similar = [](auto src_bins, auto dest_bins) -> bool {
        static constexpr double TOLERANCE = 1e-6;
        if (src_bins.size() != dest_bins.size()) return false;
        for (size_t i = 0; i < src_bins.size(); ++i) {
          double diff = std::abs(src_bins[i] - dest_bins[i]);
          if (diff >= TOLERANCE * std::abs(src_bins[i])) { return false; }
        }
        return true;
      };

      // Display a bunch of bins into a stringstream
      auto print_bins = [](std::ostringstream& s, auto local_bin_indices) {
        s << "{ ";
        for (size_t i = 0; i < local_bin_indices.size()-1; ++i) {
          s << local_bin_indices[i] << ", ";
        }
        s << local_bin_indices[local_bin_indices.size()-1] << " }";
      };

      // If any of these test fails, ROOT 7 binning went out of sync with
      // ROOT 6 binning, and thusly broke our simple data transfer strategy :(
      if (!bins_similar(src_impl.GetBinFrom(0), get_bin_from_root6(dest, 0))) {
        std::ostringstream s;
        s << "Binning origin doesn't match"
          << " (source histogram's first bin is at ";
        print_bins(s, src_impl.GetBinFrom(0));
        s << ", target histogram's first bin is at ";
        print_bins(s, get_bin_from_root6(dest, 0));
        s << ')';
        throw std::runtime_error(s.str());
      }
      if ((src_impl.GetNBins() >=2)
          && (!bins_similar(src_impl.GetBinFrom(1), get_bin_from_root6(dest, 1)))) {
        std::ostringstream s;
        s << "Binning order doesn't match"
          << " (source histogram's second bin is at ";
        print_bins(s, src_impl.GetBinFrom(1));
        s << ", target histogram's second bin is at ";
        print_bins(s, get_bin_from_root6(dest, 1));
        s << ')';
        throw std::runtime_error(s.str());
      }
      if (src_impl.GetNBins() != dest.GetNcells()) {
        std::ostringstream s;
        s << "Bin count doesn't match"
          << " (source histogram has " << src_impl.GetNBins() << " bins"
          << ", target histogram has " << dest.GetNcells() << " bins)";
        throw std::runtime_error(s.str());
      }
    }


    // Convert a ROOT 7 histogram into a ROOT 6 one
    template <class Output, class Input>
    Output convert_hist(const Input& src, const char* name) {
      // Make sure that the input histogram's impl-pointer is set
      const auto* impl_ptr = src.GetImpl();
      if (impl_ptr == nullptr) {
        throw std::runtime_error("Input histogram has a null impl pointer");
      }
      const auto& impl = *impl_ptr;

      // Compute the first ROOT 6 histogram constructor parameters
      //
      // Beware that "title" must remain a separate variable, otherwise
      // the title string will be deallocated before use...
      //
      auto title = convert_hist_title(impl.GetTitle());
      auto first_build_params = std::make_tuple(name, title.c_str());

      // Build the ROOT 6 histogram, copying src's axis configuration
      auto dest = convert_hist_loop<Output, 0>(impl,
                                               std::move(first_build_params));

      // Make sure that under- and overflow bins are included in the
      // statistics, to match the ROOT 7 behavior (as of ROOT v6.18.0).
      dest.SetStatOverflows(TH1::EStatOverflows::kConsider);

      // Use normal statistics for bin errors, since ROOT7 doesn't seem to support
      // Poisson bin error computation yet.
      dest.SetBinErrorOption(TH1::EBinErrorOpt::kNormal);

      // Set norm factor to zero (disable), since ROOT 7 doesn't seem to have this
      dest.SetNormFactor(0);

      // Now we're ready to transfer histogram data. First of all, let's
      // assert that ROOT 6 and ROOT 7 use the same binning convention. This
      // seems true as of ROOT 6.18.0, but may change in the future...
      check_binning(dest, *src.GetImpl());

      // Propagate bin uncertainties, if present.
      //
      // This must be done before inserting any other data in the TH1,
      // otherwise Sumw2() will perform undesirable black magic...
      //
      const auto& stat = src.GetImpl()->GetStat();
      if (stat.HasBinUncertainty()) {
        dest.Sumw2();
        auto& sumw2 = *dest.GetSumw2();
        for (size_t bin = 0; bin < stat.size(); ++bin) {
          sumw2[bin] = stat.GetBinUncertainty(bin);
        }
      }

      // Propagate basic histogram statistics
      dest.SetEntries(stat.GetEntries());
      for (size_t bin = 0; bin < stat.size(); ++bin) {
        dest.AddBinContent(bin, stat.GetBinContent(bin));
      }

      // Compute remaining statistics
      //
      // FIXME: If the input RHist computes all of...
      //        - fTsumw (total sum of weights)
      //        - fTsumw2 (total sum of square of weights)
      //        - fTsumwx (total sum of weight*x)
      //        - fTsumwx2 (total sum of weight*x*x)
      //
      //        ...then we should propagate those statistics to the TH1. The
      //        same applies for the higher-order statistics computed by TH2+.
      //
      //        But as of ROOT 6.18.0, we can never do this, because the
      //        RHistDataMomentUncert stats associated with fTsumwx and
      //        fTsumwx2 do not expose their contents publicly.
      //
      //        Therefore, we must always ask TH1 to do the computation
      //        for us. It's better to do so using a GetStats/PutStats
      //        pair, as ResetStats alters more than those stats...
      //
      //        The same problem occurs with the higher-order statistics
      //        computed by TH2+, but this approach is dimension-agnostic.
      //
      std::array<Double_t, TH1::kNstat> stats;
      dest.GetStats(stats.data());
      dest.PutStats(stats.data());

      // Return the ROOT 6 histogram to the caller
      return dest;
    }
  } // end namespace book
} // end namespace marlinmt
