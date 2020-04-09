#include <utility>
#include <array>

// -- marlinmt headers
#include "marlinmt/Processor.h"
#include "marlinmt/ProcessorApi.h"
#include "marlinmt/PluginManager.h"

#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace marlinmt ;

class TestHistogram : public Processor {
public:
  TestHistogram() ;
  void init() final;
  void processEvent( EventStore * evt) final;
  void end() final;

private:
  std::array<H1FEntry,2> _histogram;
};

TestHistogram::TestHistogram() :
  Processor("TestHistogram") {}

void TestHistogram::init() {
  _histogram[0] = ProcessorApi::Book::bookHist1F(
    this,
    "/someWhere/",
    "share",
    "test histogram shared",
      AxisConfigD(10, -1., 10.),
      BookFlags::MultiShared | BookFlags::Store
  );
  _histogram[1] = ProcessorApi::Book::bookHist1F(
    this,
    "/someWhere/",
    "copy",
    "test histogram copied",
      AxisConfigD(10, -1., 10.),
      BookFlags::MultiCopy | BookFlags::Store
  );
}

void TestHistogram::processEvent(EventStore * evt) {
  IMPL::LCEventImpl* event 
    = dynamic_cast<IMPL::LCEventImpl*>(evt->event<EVENT::LCEvent>().get());
  std::array<H1FHandle,2> 
    hnd = {_histogram[0].handle(), _histogram[1].handle()};
  try {
    EVENT::LCCollection * coll 
      =  event->getCollection("MCParticle");

    int nHits = coll->getNumberOfElements();

    for(int i = 0; i < nHits; ++i) {
      EVENT::MCParticle* par =
        dynamic_cast<EVENT::MCParticle*>(coll->getElementAt(i));

      hnd[0].fill({par->getEnergy()}, 1.);
      hnd[1].fill({par->getEnergy()}, 1.);
    }
  } catch ( EVENT::Exception& ) {
    streamlog_out(ERROR) << " failed to process event, LC error\n";
  }
}

void TestHistogram::end() {
  streamlog_out(MESSAGE) << "success\n";  
}

MARLINMT_DECLARE_PROCESSOR( TestHistogram )
