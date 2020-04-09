
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/ProcessorApi.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Logging.h>
#include <marlinmt/EventExtensions.h>

// -- std headers
#include <iostream>
#include <sstream>
#include <atomic>

namespace marlinmt {

  /** Simple processor for testing.
   *  Writes something to stdout for every callbackmethod.
   *
   *  <h4>Input - Prerequisites</h4>
   *  none
   *
   *  <h4>Output</h4>
   *  none
   *
   * @author F. Gaede, DESY
   * @version $Id: TestProcessor.h,v 1.7 2007-05-23 13:12:21 gaede Exp $
   */

  class TestProcessor : public Processor {
  public:
    TestProcessor() ;

    /** Called at the begin of the job before anything is read.
     * Use to initialize the processor, e.g. book histograms.
     */
    void init() override ;

    /** Called for every run.
     */
    void processRunHeader( RunHeader* run ) override ;

    /** Called for every event - the working horse.
     */
    void processEvent( EventStore * evt ) override ;

    /** Called after data processing for clean up.
     */
    void end() override ;

  protected:

    /**Test method for const.*/
    void printEndMessage() const ;

    int                   _nRun {0} ;
    std::atomic_int       _nEvt {0} ;
    bool                  _doCalibration {false} ;
  } ;

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  TestProcessor::TestProcessor() :
    Processor("TestProcessor") {
    setDescription( "Simple processor to test the marlinmt application."
      " Prints run and event number." ) ;
  }

  //--------------------------------------------------------------------------

  void TestProcessor::init() {
    log<MESSAGE>() << "TestProcessor::init()  " << name() << std::endl ;
		printParameters() ;
  }

  //--------------------------------------------------------------------------

  void TestProcessor::processRunHeader( RunHeader* run) {
   log<MESSAGE>() << " processRun() "
			    << run->runNumber()
			    << std::endl ;
    _nRun++ ;
  }

  //--------------------------------------------------------------------------

  void TestProcessor::processEvent( EventStore * evt ) {
    // event counter: add +1 and get the old value.
    // For example, if _nEvt is 0, it adds +1 and returns 0
    auto eventCounter = _nEvt.fetch_add(1) ;
    // This how we know if we are currently processing the first event
    const bool firstEvent = ProcessorApi::isFirstEvent( evt ) ;
    // Get the processor runtime conditions (not conditions data)
    // You can use them to set boolean values that next processors
    // in the chain can use as conditions
    // An example usage in this processor ...
    const bool calibrate = (eventCounter % 3 ) == 0 ;

    if( firstEvent ) {
      log<DEBUG>() << " This is the first event ! uid = " << evt->uid() << std::endl ;
    }
    ProcessorApi::setReturnValue( this, evt, "Calibrating", calibrate ) ;

    if( calibrate ) {
      log<MESSAGE>() << "processEvent()  ---CALIBRATING ------ "
			      << " in event uid " << evt->uid()
			      << std::endl ;
    }

    log<MESSAGE>() << " processing event uid " << evt->uid() << std::endl ;

    log<MESSAGE>() << "(MESSAGE) local verbosity level: " << verbosity() << std::endl ;

    // always return true for this processor
    ProcessorApi::setReturnValue( this, evt, true ) ;
    // set ProcessorName.EvenNumberOfEvents == true if this processor has been called 2n (n=0,1,2,...) times
    ProcessorApi::setReturnValue( this , evt, "EvenNumberOfEvents", !( eventCounter % 2 ) ) ;
  }

  //--------------------------------------------------------------------------

  void TestProcessor::end(){
    printEndMessage() ;
  }

  //--------------------------------------------------------------------------

  void TestProcessor::printEndMessage() const {
    log<MESSAGE>() << " end() "
			   << " processed "     << _nEvt << " events in "
			   << _nRun << " runs " << std::endl
			   << std::endl ;
  }

  // processor declaration
  MARLINMT_DECLARE_PROCESSOR( TestProcessor )
}
