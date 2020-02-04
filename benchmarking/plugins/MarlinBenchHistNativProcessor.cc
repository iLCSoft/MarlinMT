#include <vector>
#include <list>
#include <random>

// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/ProcessorApi.h>
#include <marlin/PluginManager.h>

// -- root headers
#include <ROOT/RHist.hxx>
#include <ROOT/RHistConcurrentFill.hxx>

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
  std::vector<RH1F> _histograms ;
  std::list<RHistConcurrentFillManager<RH1F, 1024>> _managers;  
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
      _managers.emplace_back(_histograms.back());
    }
  }
}

void MarlinBenchHistNativeProcessor::processEvent(EventStore * evt) {
  auto seed = ProcessorApi::getRandomSeed( this, evt ) ;
  std::mt19937 generator{ seed };
  std::normal_distribution<float> distributionV{0, 30.f};
  std::list<RHistConcurrentFiller<RH1F, 1024>> fillers{}; 

  for(auto & manager : _managers) {
    fillers.emplace_back(manager);
  }
  for(marlin::size_t i = 0; i < _nFills; ++i) {
    for ( auto& filler : fillers) {
      filler.Fill({distributionV(generator)}, 1.);
    }
  }
}

void MarlinBenchHistNativeProcessor::end() {}

MARLIN_DECLARE_PROCESSOR( MarlinBenchHistNativeProcessor )
