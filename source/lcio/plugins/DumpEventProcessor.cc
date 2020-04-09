
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/PluginManager.h>

// -- lcio headers
#include <UTIL/LCTOOLS.h>

namespace marlinmt {

  /** DumpEventProcessor simply dumps an event in the console
   *  <h4>Input - Prerequisites</h4>
   *  No input needed for this processor.
   *
   *  <h4>Output</h4>
   *  none
   *
   * @param DumpDetailed  make a detailed dump of the event
   *
   * @author R. Ete, DESY
   */
  class DumpEventProcessor : public Processor {
  public:
  	/**
     *  @brief Constructor
     */
  	DumpEventProcessor() ;

    // from Processor
  	void init() ;
  	void processEvent( EventStore * evt ) ;

  protected:
    BoolParameter _dumpDetailed {*this, "DumpDetailed",
             "Whether to make a detailed dump of the event", true } ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  DumpEventProcessor::DumpEventProcessor() :
    Processor("DumpEvent") {
  	// modify processor description
  	setDescription( "Simple processor to dump an event" ) ;
    // duplicate and don't lock. Anyway, this processor in MT mode doesn't make sense...
    setRuntimeOption( Processor::ERuntimeOption::eCritical, false ) ;
    setRuntimeOption( Processor::ERuntimeOption::eClone, true ) ;
  }

  //--------------------------------------------------------------------------

  void DumpEventProcessor::init() {
  	// Print the initial parameters
  	printParameters() ;
  }

  //--------------------------------------------------------------------------

  void DumpEventProcessor::processEvent( EventStore * evt ) {
    auto lcevent = evt->event<EVENT::LCEvent>() ;
	   if( _dumpDetailed ) {
       UTIL::LCTOOLS::dumpEventDetailed( lcevent.get() ) ;
     }
     else {
       UTIL::LCTOOLS::dumpEvent( lcevent.get() ) ;
     }
  }

  // plugin declaration
  MARLINMT_DECLARE_PROCESSOR( DumpEventProcessor )
}
