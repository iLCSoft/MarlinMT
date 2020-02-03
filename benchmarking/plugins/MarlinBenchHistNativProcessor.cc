#include <vector>
#include <list>
#include <random>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/PluginManager.h"

// -- root headers
#include "ROOT/RHist.hxx"
#include "ROOT/RHistConcurrentFill.hxx"

using namespace marlin ;
using namespace ROOT::Experimental ;

namespace marlin{
  using size_t = unsigned long long int;
}

class MarlinBenchHistNativeProcessor : public Processor {
public:
  MarlinBenchHistNativeProcessor() ;
  void init() final override;
  void processEvent( EventStore * evt) final override;
  void end() final override;

private:
  Property<int> _nBins {this, "NBins",  "number of bins per histogram.", 1000} ;
  Property<int> _nHist {this, "NHists", "number of histograms to create.", 5} ;
  Property<marlin::size_t> _nFills {this, "NFills", "number of fill operations per event", 1000} ;
  Property<bool> _shared {this, "Shared", "true if used shared access, false for copy", false} ;
  std::vector<RH1F> _histograms ;
  std::list<RHistConcurrentFillManager<RH1F, 1024>> _managers;  
  std::mt19937 _generator{ 0x1bff1822 };
  std::normal_distribution<float> _distributionV{0, 30.f};
};

MarlinBenchHistNativeProcessor::MarlinBenchHistNativeProcessor() :
  Processor("MarlinBenchHistNativeProcessor") {}

void MarlinBenchHistNativeProcessor::init() {
  if (_histograms.empty()) {
    for (int i = 0; i < _nHist; ++i) {
      _histograms.emplace_back(
          "dummy histogram", 
          RAxisConfig(  
            "normal distributed values", 
            static_cast<int>(_nBins), 
            -20.f, 20.f));
      if (_shared) {
        _managers.emplace_back(_histograms.back());
      }
    }
  }
}

void MarlinBenchHistNativeProcessor::processEvent(EventStore * evt) {
  if (_shared) {
    std::list<RHistConcurrentFiller<RH1F, 1024>> fillers{}; 
    for(auto & manager : _managers) {
      fillers.emplace_back(manager);
    }
    for(marlin::size_t i = 0; i < _nFills; ++i) {
      for ( auto& filler : fillers) {
        filler.Fill({_distributionV(_generator)}, 1.);
      }
    }
  } else {
    for(marlin::size_t i = 0; i < _nFills; ++i) {
      for (auto& hist : _histograms) {
        hist.Fill({_distributionV(_generator)}, 1.);
      }
    }
  }
}

void MarlinBenchHistNativeProcessor::end() {}

MARLIN_DECLARE_PROCESSOR( MarlinBenchHistNativeProcessor )
