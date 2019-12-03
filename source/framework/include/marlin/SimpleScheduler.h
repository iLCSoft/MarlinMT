#ifndef MARLIN_SIMPLESCHEDULER_h
#define MARLIN_SIMPLESCHEDULER_h 1

// -- marlin headers
#include <marlin/IScheduler.h>
#include <marlin/Logging.h>

namespace marlin {

  class SuperSequence ;
  class Sequence ;

  /**
   *  @brief  SimpleScheduler class
   */
  class SimpleScheduler : public IScheduler {
  public:
    using Logger = Logging::Logger ;
    using ProcessorSequence = std::shared_ptr<SuperSequence> ;

  public:
    SimpleScheduler() = default ;

    // from IScheduler interface
    void init( Application *app ) override ;
    void end() override ;
    void processRunHeader( std::shared_ptr<RunHeader> rhdr ) override ;
    void pushEvent( std::shared_ptr<EventStore> event ) override ;
    void popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) override ;
    std::size_t freeSlots() const override ;

  private:
    ///< The logger instance
    Logger                           _logger {nullptr} ;
    ///< The processor super sequence
    ProcessorSequence                _superSequence {nullptr} ;
    ///< The current event being processed
    std::shared_ptr<EventStore>      _currentEvent {nullptr} ;
  };

} // end namespace marlin

#endif
