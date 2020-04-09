
// --  marlinmt headers
#include "marlinmt/Processor.h"
#include "marlinmt/Logging.h"
#include "marlinmt/PluginManager.h"

// -- lcio headers
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"

using namespace marlinmt ;
using namespace EVENT ;
using namespace IMPL ;

/**
 * Example processor to event / run modification
 *
 * @author F. Gaede, DESY
 * @version $Id: TestEventModifier.h,v 1.4 2005-10-11 12:57:39 gaede Exp $
 */
class TestEventModifier : public Processor {
 public:
  TestEventModifier() ;

  /** Called for every run.
   */
  void processRunHeader( RunHeader* run ) override ;

  /** Called for every event - the working horse.
   */
  void processEvent( EventStore * evt ) override ;

  /** Called after data processing for clean up.
   */
  void end() ;

 protected:
  int _nRun = {0} ;
  int _nEvt = {0} ;
} ;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

TestEventModifier::TestEventModifier() :
  Processor("TestEventModifier") {
  // modify processor description
  _description = "TestEventModifier make changes to the run header and the event data for testing" ;
}

//--------------------------------------------------------------------------

void TestEventModifier::processRunHeader( RunHeader * ) {
  _nRun ++ ;
}

//--------------------------------------------------------------------------

void TestEventModifier::processEvent( EventStore * e ) {

  IMPL::LCEventImpl* evt = dynamic_cast<LCEventImpl*>( e->event<EVENT::LCEvent>().get() ) ;

  try {

    if( evt != 0 ){
      evt->setEventNumber(  _nEvt + 42 ) ;
      streamlog_out(MESSAGE4)  << " processEvent modified event number to be : "
            << _nEvt << " + 42 = " <<  evt->getEventNumber() << std::endl ;
      _nEvt++ ;
    }
  }
  catch( EVENT::Exception& ) {
    streamlog_out(ERROR)  << " processEvent LCEvent::eventNumber not modified ! "  << std::endl;
  }
}

//--------------------------------------------------------------------------

void TestEventModifier::end() {
  streamlog_out(MESSAGE4) << name()
        << " modified " << _nEvt << " events in " << _nRun << " run "
        << std::endl ;
}

MARLINMT_DECLARE_PROCESSOR( TestEventModifier )
