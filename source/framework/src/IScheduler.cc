
// -- marlinmt headers
#include <marlinmt/IScheduler.h>
#include <marlinmt/Application.h>

namespace marlinmt {
  
  IScheduler::IScheduler() : 
    Component("Scheduler") {
    /* nop */
  }
  
  //--------------------------------------------------------------------------

  void IScheduler::initialize() {
    auto &config = application().configuration() ;
    if( config.hasSection("scheduler") ) {
      setParameters( config.section("scheduler") ) ;
    }
  }
  
}


