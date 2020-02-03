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

class MarlinBenchHistProcessor : public Processor {
public:
  MarlinBenchHistProcessor() ;
  void init() final override;
  void processEvent( EventStore * evt) final override;
  void end() final override;

private:
  Property<int> _nBins {this, "NBins",  "number of bins per histogram.", 1000} ;
  Property<int> _nHist {this, "NHists", "number of histograms to create.", 5} ;
  Property<marlin::size_t> _nFills {this, "NFills", "number of fill operations per event", 1000};
  std::vector<H1FEntry> _histograms;
  std::mt19937 _generator{ 0x1bff1822 };
  std::normal_distribution<float> _distributionV{0, 10.f};
};

MarlinBenchHistProcessor::MarlinBenchHistProcessor() :
  Processor("MarlinBenchHistProcessor") {}

void MarlinBenchHistProcessor::init() {
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

void MarlinBenchHistProcessor::processEvent(EventStore * evt) {
  std::vector<H1FHandle> hists{}; 
  for(auto itr = _histograms.begin(); itr != _histograms.end(); ++itr) {
    hists.emplace_back(itr->handle());
  }
  
  for(marlin::size_t i = 0; i < _nFills; ++i) {
    for ( auto& hist : hists) {
      hist.fill({_distributionV(_generator)}, 1.);
    }
  }
}

void MarlinBenchHistProcessor::end() {}

MARLIN_DECLARE_PROCESSOR( MarlinBenchHistProcessor )
