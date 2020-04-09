
// -- marlinmt headers
#include "marlinmt/Processor.h"
#include "marlinmt/Logging.h"
#include "marlinmt/ProcessorApi.h"
#include "marlinmt/PluginManager.h"

using namespace marlinmt ;


/**  test processor for testing the uniquie event seeding functionality of the ProcessorEventSeeder service in MarlinMT.
 *
 *
 * @author S. J. Aplin, DESY
 */

class TestProcessorEventSeeder : public Processor {
 public:

  TestProcessorEventSeeder() ;

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
  std::map< unsigned int, unsigned int>  _seeds {} ;
  int _nRun = {0} ;
  int _nEvt = {0} ;
} ;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

TestProcessorEventSeeder::TestProcessorEventSeeder() : Processor("TestProcessorEventSeeder") {

  // modify processor description
  _description = "TestProcessorEventSeeder test the uniquie event seeding functionality of the ProcessorEventSeeder service in MarlinMT" ;

  _nRun = -1 ;
  _nEvt = -1 ;

}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::init() {
  ProcessorApi::registerForRandomSeeds( this ) ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::processRunHeader( RunHeader* ) {
  ++_nRun ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::processEvent( EventStore * evt ) {

  streamlog_out(DEBUG) << "   processing event: " << evt->uid()
		       << std::endl ;

  unsigned int seed = ProcessorApi::getRandomSeed( this, evt ) ;

  streamlog_out( DEBUG ) << "seed set to "
			    << seed
			    << " for event "
			    << evt->uid()
			    << std::endl;

  try {
    ProcessorApi::registerForRandomSeeds( this ) ;
  }
  catch( marlinmt::Exception& ) {
    log<ERROR>() << name() << " failed to register processor to event seed generator (TEST is OK)" << std::endl ;
  }

  // unsigned long long runnum_and_eventnum = evt->getRunNumber() ;
  // runnum_and_eventnum = runnum_and_eventnum << 32  ;
  //
  // runnum_and_eventnum += evt->getEventNumber() ;

  // std::map< unsigned long long, unsigned int>::iterator it ;

  auto it = _seeds.find( evt->uid() );

  if( it == _seeds.end() ){
    _seeds[evt->uid()] = seed ;
  }
  else {

    if( seed != _seeds[evt->uid()] ) {
      streamlog_out(ERROR) << " Seeds don't match for"
			   << " uid " <<   evt->uid()
			   << " old seed = " << _seeds[evt->uid()]
			   << " new seed = " << seed
			   << std::endl ;
    }
    else{
      streamlog_out(DEBUG) << " Seeds match for"
			   << " uid " << evt->uid()
			   << " old seed = " << _seeds[evt->uid()]
			   << " new seed = " << seed
			   << std::endl ;

    }
  }
  ++_nEvt ;
}

//--------------------------------------------------------------------------

void TestProcessorEventSeeder::end(){

  streamlog_out(MESSAGE4) << name()
			  << " random seeds received for " << _nEvt << " events in " << _nRun << " run "
			  << std::endl ;

}

MARLINMT_DECLARE_PROCESSOR( TestProcessorEventSeeder )
