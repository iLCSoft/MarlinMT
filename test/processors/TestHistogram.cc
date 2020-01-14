#include <utility>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/ProcessorApi.h"
#include "marlin/PluginManager.h"

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

#include <thread>

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

// -- book headers
#include "marlin/book/BookStore.h"
#include "marlin/book/Hist.h"
#include "marlin/book/configs/ROOTv7.h"

using namespace marlin ;

class TestHistogram : public Processor {
public:
  TestHistogram() ;
  void init() final;
  void processEvent( EventStore * evt) final;
  void end() final;

private:
  book::Handle<book::Entry<book::types::H1F>> _histogram;
  std::thread::id _tid{};
};

TestHistogram::TestHistogram() :
  Processor("TestHistogram") {}

void TestHistogram::init() {
  _histogram = ProcessorApi::Book::create<book::types::H1F>(
    this,
    "/someWhere/hit",
    "test histogram",
    {
      book::types::AxisConfig<double>(3, 0., 3.)
    }
  );
}

void TestHistogram::processEvent(EventStore * evt) {
  IMPL::LCEventImpl* event 
    = dynamic_cast<IMPL::LCEventImpl*>(evt->event<EVENT::LCEvent>().get());
  book::Handle<book::types::H1F> hnd = _histogram.handle(_tid);
  try {
    EVENT::LCCollection * coll 
      =  event->getCollection("MCParticle");
    int nHits = coll->getNumberOfElements();
    for(int i = 0; i < nHits; ++i) {
      EVENT::MCParticle* par =
        dynamic_cast<EVENT::MCParticle*>(coll->getElementAt(i));
      hnd.fill({par->getEnergy()}, 1.);
    }
  } catch ( EVENT::Exception& ) {
    streamlog_out(ERROR) << " failed to process event, LC error\n";
  }
}

void TestHistogram::end() {
  streamlog_out(ERROR) << "success\n";  
}

MARLIN_DECLARE_PROCESSOR( TestHistogram )
