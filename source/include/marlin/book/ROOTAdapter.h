#pragma once

#include "ROOT/RHistData.hxx"
#include "ROOT/RHist.hxx"

template<int D, typename T, template<int, class> class ... STAT>
using RH = ROOT::Experimental::RHist<D, T, STAT ... >;

template<typename TO, typename FROM>
void ADD(const TO& to, const FROM& from) {
  ROOT::Experimental::Add(to, from);
}


using RH1D = ROOT::Experimental::RH1D;
using RH1F = ROOT::Experimental::RH1F;
using RH1I = ROOT::Experimental::RH1I;

