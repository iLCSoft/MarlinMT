#pragma once

// -- std header
#include <memory>

// -- root header
#include "ROOT/RHist.hxx"

namespace marlin {
  namespace book {
    namespace Types {
#     define book_type_alias_hist(TYPE) using TYPE = ROOT::Experimental::TYPE
      book_type_alias_hist(RH1I);
      book_type_alias_hist(RH1F);
      book_type_alias_hist(RH1D);
      book_type_alias_hist(RH2I);
      book_type_alias_hist(RH2F);
      book_type_alias_hist(RH2D);
      book_type_alias_hist(RH3I);
      book_type_alias_hist(RH3F);
      book_type_alias_hist(RH3D);

      using RAxisConfig = ROOT::Experimental::RAxisConfig;
    } // end namespace Types

    template<class T>
    class ModifierHist;

    template<class T>
    class ModifierManagerHist;

#   define book_trait_hist(TYPE)                            \
    template<>                                              \
      struct book_trait<Types::TYPE> {                      \
        using Type =  Types::TYPE;                          \
        using Modifier = ModifierHist<Type>;                \
        using ModifierManager = ModifierManagerHist<Type>;  \
        static void merge(                                  \
            const std::shared_ptr<Type>& p1,                \
            const std::shared_ptr<Type>& p2) {              \
          ROOT::Experimental::Add(*p1, *p2);                \
        }                                                   \
      }

    /// definiton of book_traits for RHist
      book_trait_hist(RH1I);
      book_trait_hist(RH1F);
      book_trait_hist(RH1D);
      book_trait_hist(RH2I);
      book_trait_hist(RH2F);
      book_trait_hist(RH2D);
      book_trait_hist(RH3I);
      book_trait_hist(RH3F);
      book_trait_hist(RH3D);
    // TODO: add for other classes :)

  } // end namespace book
} // end namespace marlin
