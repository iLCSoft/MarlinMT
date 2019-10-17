#pragma once

// -- std header
#include <memory>

// -- root header
#include "ROOT/RHist.hxx"

namespace marlin {
  namespace book {

    template<class T>
    class ModifierHist;

    template<class T>
    class ModifierManagerHist;

#   define book_trait_hist(TYPE)                            \
    template<>                                              \
      struct book_trait<TYPE> {                             \
        using Type = TYPE;                                  \
        using Modifier = ModifierHist<Type>;                \
        using ModifierManager = ModifierManagerHist<Type>;  \
        static void merge(                                  \
            const std::shared_ptr<Type>& p1,                \
            const std::shared_ptr<Type>& p2) {              \
          ROOT::Experimental::Add(*p1, *p2);                \
        }                                                   \
      }

    /// definiton of book_traits for RHist
    book_trait_hist(ROOT::Experimental::RH1D);
    book_trait_hist(ROOT::Experimental::RH1F);
    // TODO: add for other classes :)
  }
}
