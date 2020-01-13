#include <utility>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/ProcessorApi.h"

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

// -- book headers
#include "marlin/book/BookStore.h"
#include "marlin/book/Hist.h"
#include "marlin/book/configs/ROOTv7.h"

using namespace marlin ;
using namespace EVENT ;
using namespace IMPL ;

class TestHistogram : public Processor {
public:
  TestHistogram() ;
  void processRunHeader( RunHeader* run ) final;
  void processEvent( EventStore * evt) final;
  void end();

private:
  book::Handle<book::Manager<book::types::H1F>> _histogram;
};

void TestHistogram::processRunHeader( RunHeader* run) {
  _histogram = ProcessorApi::Store::createdHistogram<book::types::H1F>(
    this,
    "/someWhere/hit",
    "test histogram",
    {
      book::types::AxisConfig<double>(3, 0., 3.)
    }
  );
}
