#pragma once

#include "marlin/book/bookStore.h"
#include "ROOT/RHist.hxx"

namespace marlin::book {
#define book_trait_hist(TYPE) \
  template<>  \
    struct book_trait<TYPE> { \
      using Type = TYPE; \
      using Filler = FillerHist<Type>; \
      using FillMgr = FillMgrHist<Type>; \
      static void mearge(const std::shared_ptr<Type>& p1, const std::shared_ptr<Type>& p2) { \
        ROOT::Experimental::Add(*p1, *p2); \
      } \
    }
  book_trait_hist(ROOT::Experimental::RH1D);
  book_trait_hist(ROOT::Experimental::RH1F);
}
