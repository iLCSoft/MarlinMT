
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/ProcessorApi.h>
#include <marlinmt/Logging.h>
#include <marlinmt/PluginManager.h>

// -- lcio headers
#include <lcio.h>

// -- std headers
#include <cmath>
#include <ctime>
#include <random>

namespace marlinmt {

  /** Simple processor that triggers the LCEvent data unpacking.
   *
   * @author R.Ete, DESY
   */
  class LCIOEventUnpackingProcessor : public Processor {

   public:
    LCIOEventUnpackingProcessor() ;
    void init() ;
    void processEvent( EventStore * evt ) ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOEventUnpackingProcessor::LCIOEventUnpackingProcessor() :
    Processor("LCIOEventUnpacking") {
    // modify processor description
    setDescription( "LCIOEventUnpackingProcessor unpacks LCIO event" ) ;
  }

  //--------------------------------------------------------------------------

  void LCIOEventUnpackingProcessor::init() {
    log<DEBUG>() << "LCIOEventUnpackingProcessor::init() called" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void LCIOEventUnpackingProcessor::processEvent( EventStore *event ) {
    auto lcevent = event->event<EVENT::LCEvent>() ;
    auto colnames = lcevent->getCollectionNames() ;
    if( not colnames->empty() ) {
      auto colname = colnames->at(0) ;
      lcevent->getCollection( colname ) ;
    }
  }

  // processor declaration
  MARLINMT_DECLARE_PROCESSOR( LCIOEventUnpackingProcessor )
}
