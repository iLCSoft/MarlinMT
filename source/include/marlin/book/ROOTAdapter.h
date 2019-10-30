#pragma once

#include "ROOT/RHistData.hxx"
#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"
#include "ROOT/span.hxx"

#include <type_traits>

namespace marlin::book::types {

  template<int D, typename T, template<int, class> class ... STAT>
  using RH = ROOT::Experimental::RHist<D, T, STAT ... >;

  template<typename TO, typename FROM>
  void ADD(const TO& to, const FROM& from) {
    ROOT::Experimental::Add(to, from);
  }


  using RH1D = ROOT::Experimental::RH1D;
  using RH1F = ROOT::Experimental::RH1F;
  using RH1I = ROOT::Experimental::RH1I;


  // TODO: select prober number :)
  template<class T>
  using RHistConcurrentFillManager 
    = ROOT::Experimental::RHistConcurrentFillManager<T, 1024>;

  template<class T>
  using RHistConcurrentFiller
    = ROOT::Experimental::RHistConcurrentFiller<T, 1024>;

}
