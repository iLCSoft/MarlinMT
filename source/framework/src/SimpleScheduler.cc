#include <marlinmt/SimpleScheduler.h>

// -- marlinmt headers
#include <marlinmt/Application.h>
#include <marlinmt/Utils.h>
#include <marlinmt/Sequence.h>
#include <marlinmt/Processor.h>

// -- std headers
#include <algorithm>
#include <set>

namespace marlinmt {
  
  SimpleScheduler::SimpleScheduler() :
    IScheduler() {
    setName( "SimpleScheduler" ) ;
  }
  
  //--------------------------------------------------------------------------

  void SimpleScheduler::initialize() {
    // base init
    IScheduler::initialize() ;
    auto &config = application().configuration() ;
    auto &execSection = config.section("execute") ;
    auto &procsSection = config.section("processors") ;
    auto activeProcessors = execSection.parameterNames() ;
    // auto activeProcessors = app->activeProcessors() ;
    // create super sequence with only only sequence and fill it
    _superSequence = std::make_shared<SuperSequence>(1) ;
    log<DEBUG5>() << "Creating processors ..." << std::endl ;
    if ( activeProcessors.empty() ) {
      MARLINMT_THROW( "Active processor list is empty !" ) ;
    }
    // populate processor sequences
    for ( size_t i=0 ; i<activeProcessors.size() ; ++i ) {
      auto procName = activeProcessors[ i ] ;
      log<DEBUG5>() << "Active processor " << procName << std::endl ;
      auto &procSection = procsSection.section( procName ) ;
      _superSequence->addProcessor( procSection ) ;
    }
    _superSequence->init( &application() ) ;
    log<DEBUG5>() << "Creating processors ... OK" << std::endl ;
  }

  //--------------------------------------------------------------------------

  void SimpleScheduler::end() {
    log<MESSAGE>() << "Terminating application" << std::endl ;
    _superSequence->end() ;
    // print some statistics
    _superSequence->printStatistics( _logger ) ;
  }

  //--------------------------------------------------------------------------

  void SimpleScheduler::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    _superSequence->processRunHeader( rhdr ) ;
  }

  //--------------------------------------------------------------------------

  void SimpleScheduler::pushEvent( std::shared_ptr<EventStore> event ) {
    _currentEvent = event ;
    auto sequence = _superSequence->sequence(0) ;
    sequence->processEvent( _currentEvent ) ;
  }

  //--------------------------------------------------------------------------

  void SimpleScheduler::popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) {
    if( nullptr != _currentEvent ) {
      events.push_back( _currentEvent ) ;
      _currentEvent = nullptr ;
    }
  }

  //--------------------------------------------------------------------------

  std::size_t SimpleScheduler::freeSlots() const {
    return ( _currentEvent != nullptr ) ? 0 : 1 ;
  }

}
