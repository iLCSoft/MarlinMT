
// -- marlin headers
#include <marlin/Processor.h>
#include <marlin/PluginManager.h>
#include <marlin/Logging.h>

// -- std headers
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <atomic>

namespace marlin {

    /** Simple processor for writing out a status message every n-th event.
     *
     *  <h4>Input - Prerequisites</h4>
     *  none
     *  <h4>Output</h4>
     *  none
     * @parameter HowOften  print run and event number for every HowOften-th event
     *
     * @author A.Sailer CERN
     * @version $Id:$
     */
  class Statusmonitor : public Processor {

   public:
    Statusmonitor() ;

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

  private:
    UIntParameter _howOften {*this, "howOften",
              "Print event number every N events", 1 } ;

    // runtime members
    int                 _nRun {0} ;
    std::atomic<int>    _nEvt {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  Statusmonitor::Statusmonitor() : Processor("Statusmonitor") {
    // modify processor description
    setDescription( "Statusmonitor prints out information on running Marlin Job: Prints number of runs run and current number of the event. Counting is sequential and not the run or event ID." ) ;
    // no need to lock, this processor is thread safe
    setRuntimeOption( Processor::ERuntimeOption::eCritical, false ) ;
    // don't duplicate since it is thread safe and hold no big data
    setRuntimeOption( Processor::ERuntimeOption::eClone, false ) ;
  }

  //--------------------------------------------------------------------------

  void Statusmonitor::init() {
    log<DEBUG>() << "INIT CALLED  " << std::endl ;
    // usually a good idea to
    printParameters() ;
  }

  //--------------------------------------------------------------------------

  void Statusmonitor::processRunHeader( RunHeader* ) {
    _nRun++ ;
  }

  //--------------------------------------------------------------------------

  void Statusmonitor::processEvent( EventStore *  ) {
    auto eventid = _nEvt.fetch_add(1) ;
    if (eventid % _howOften == 0) {
      log<MESSAGE>()
        << " ===== Run  : " << std::setw(7) << _nRun
        << "  Event: " << std::setw(7) << eventid << std::endl;
    }
  }

  //--------------------------------------------------------------------------

  void Statusmonitor::end() {
    log<MESSAGE>() << "Statusmonitor::end()  " << name()  << " processed " << _nEvt << " events in " << _nRun << " runs"	<< std::endl ;
  }

  // processor declaration
  MARLIN_DECLARE_PROCESSOR( Statusmonitor )
}
