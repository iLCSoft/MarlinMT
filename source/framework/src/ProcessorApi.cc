#include <marlin/ProcessorApi.h>

// -- std headers
#include <utility>

// -- marlin headers
#include <marlin/Exceptions.h>
#include <marlin/EventExtensions.h>

namespace marlin {

  std::unique_ptr<book::BookStore> ProcessorApi::Store::_store{nullptr};
  void ProcessorApi::Store::registerStore(std::unique_ptr<book::BookStore>&& store) {
    std::swap(_store, store); 
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::registerForRandomSeeds( Processor *const proc ) {
    proc->app().randomSeedManager().addEntry( proc ) ;
  }

  //--------------------------------------------------------------------------

  unsigned int ProcessorApi::getRandomSeed( const Processor *const proc, EventStore *event ) {
    auto randomSeeds = event->extensions().get<extensions::RandomSeed, RandomSeedExtension>() ;
    if( nullptr == randomSeeds ) {
      MARLIN_THROW( "No random seed extension in event" ) ;
    }
    return randomSeeds->randomSeed( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EventStore *event, bool value ) {
    auto procConds = event->extensions().get<extensions::ProcessorConditions, ProcessorConditionsExtension>() ;
    if( nullptr == procConds ) {
      MARLIN_THROW( "No processor conditions extension in event" ) ;
    }
    return procConds->set( proc, value ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::setReturnValue( const Processor *const proc, EventStore *event, const std::string &name, bool value ) {
    auto procConds = event->extensions().get<extensions::ProcessorConditions, ProcessorConditionsExtension>() ;
    if( nullptr == procConds ) {
      MARLIN_THROW( "No processor conditions extension in event" ) ;
    }
    return procConds->set( proc, name, value ) ;
  }

  //--------------------------------------------------------------------------

  bool ProcessorApi::isFirstEvent( EventStore *event ) {
    return *event->extensions().get<extensions::IsFirstEvent, bool>() ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::skipCurrentEvent( const Processor *const proc ) {
    proc->log<WARNING>() << "Skipping current event !" << std::endl ;
    MARLIN_SKIP_EVENT( proc ) ;
  }

  //--------------------------------------------------------------------------

  void ProcessorApi::abort( const Processor *const proc, const std::string &reason ) {
    proc->log<WARNING>() << "Stopping application: " << reason << std::endl ;
    MARLIN_STOP_PROCESSING( proc ) ;
  }

}
