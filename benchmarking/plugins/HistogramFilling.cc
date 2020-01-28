#include <utility>
#include <vector>
#include <random>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/ProcessorApi.h"
#include "marlin/PluginManager.h"

using namespace marlin ;

class HistogramFilling : public Processor {
public:
  HistogramFilling() ;
  void init() final override;
  void processEvent( EventStore * evt) final override;
  void end() final override;

private:
  Property<int> _nBins {this, "NBins",  "number of bins per histogram.", 1000} ;
  Property<int> _nHist {this, "NHists", "number of histograms to create.", 5} ;
  Property<int> _nFills {this, "NFills", "number of fill operations per event", 1000};
  Property<int> _msCrunch {this, "CrunchTime", "time crunch per event (unit ms)", 200};
  Property<float> _sigmaCrunch {this, "CrunchSigma", "Smearing factor on crunching time using a gaussian generator (unit ms)", 0};
  std::vector<H1FEntry> _histograms;
  std::mt19937 _generator{ 0x1bff1822 };
  std::normal_distribution<clock::duration_rep> _distributionT{0, _sigmaCrunch};
  std::normal_distribution<float> _distributionV{0, 30.f};
};

HistogramFilling::HistogramFilling() :
  Processor("HistogramFilling") {}

void HistogramFilling::init() {
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

void HistogramFilling::processEvent(EventStore * evt) {
  clock::crunchFor<clock::milliseconds>(
    static_cast<clock::duration_rep>(_msCrunch)
    + _distributionT(_generator));
  std::vector<H1FHandle> hists{}; 
  for(auto itr = _histograms.begin(); itr != _histograms.end(); ++itr) {
    hists.emplace_back(itr->handle());
  }
  for(int i = 0; i < _nFills; ++i) {
    for ( auto& hist : hists) {
      hist.fill({_distributionV(_generator)}, 1.);
    }
  }
}

void HistogramFilling::end() {
  streamlog_out(MESSAGE) << "success\n";  
}

MARLIN_DECLARE_PROCESSOR( HistogramFilling )
