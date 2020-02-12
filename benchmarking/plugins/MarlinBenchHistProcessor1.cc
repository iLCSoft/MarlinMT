#include <algorithm>
#include <utility>
#include <vector>
#include <random>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/ProcessorApi.h"
#include "marlin/PluginManager.h"

using namespace marlin ;

namespace marlin{
  using size_t = unsigned long long int;
}

class MarlinBenchHistProcessor1 : public Processor {
public:
  MarlinBenchHistProcessor1() ;
  void init() final override;
  void processEvent( EventStore * evt) final override;
  void end() final override;

private:
  Property<int> _nBins {this, "NBins",  "number of bins per histogram.", 1000} ;
  Property<int> _nHist {this, "NHists", "number of histograms to create.", 5} ;
  Property<marlin::size_t> _nFills {this, "NFills", "number of fill operations per event", 1000};
  std::vector<H1FEntry> _histograms;
};

MarlinBenchHistProcessor1::MarlinBenchHistProcessor1() :
  Processor("MarlinBenchHistProcessor1") {}

void MarlinBenchHistProcessor1::init() {
  ProcessorApi::registerForRandomSeeds( this ) ;
  if (_histograms.empty()) {
    for (int i = 0; i < _nHist; ++i) {
      _histograms.push_back(
        ProcessorApi::Book::bookHist1F(
          this, "/", std::to_string(i), "dummy histogram", 
          {"normal distributed values", 
            book::details::safe_cast<int, std::size_t>(_nBins),
            -50.f, 50.f}));
    }
  }
}

void MarlinBenchHistProcessor1::processEvent(EventStore * evt) {
  auto seed = ProcessorApi::getRandomSeed( this, evt ) ;
  std::mt19937 generator{ seed };
  std::normal_distribution<float> distributionV{0, 10.f};
  std::vector<H1FHandle> hists{}; 

  auto itr = _histograms.begin();
  for(marlin::size_t i = 0; i < _nFills;) {
    H1FHandle hist = itr->handle();
    marlin::size_t end = std::min<marlin::size_t>(_nFills, i + (_nFills / _histograms.size()));
    for(;i < end; ++i) {
      hist.fill({distributionV(generator)}, 1.);  
    }
  }
}

void MarlinBenchHistProcessor1::end() {}

MARLIN_DECLARE_PROCESSOR( MarlinBenchHistProcessor1 )
