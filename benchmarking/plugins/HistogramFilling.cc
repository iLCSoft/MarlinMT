#include <utility>

// -- marlin headers
#include "marlin/Processor.h"
#include "marlin/ProcessorApi.h"
#include "marlin/PluginManager.h"

using namespace marlin ;

class HistogramFilling : public Processor {
public:
  HistogramFilling() ;
  void init() final;
  void processEvent( EventStore * evt) final;
  void end() final;

private:
  H1FEntry _histogram;
};

HistogramFilling::HistogramFilling() :
  Processor("HistogramFilling") {}

void HistogramFilling::init() {
  _histogram = ProcessorApi::Book::bookHist1F(
    this,
    "/someWhere/",
    "hit",
    "test histogram",
      AxisConfigD(10, -1., 10.)
  );
}

void HistogramFilling::processEvent(EventStore * evt) {
}

void HistogramFilling::end() {
  streamlog_out(MESSAGE) << "success\n";  
}

MARLIN_DECLARE_PROCESSOR( HistogramFilling )
