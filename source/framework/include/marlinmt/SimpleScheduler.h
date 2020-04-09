#ifndef MARLINMT_SIMPLESCHEDULER_h
#define MARLINMT_SIMPLESCHEDULER_h 1

// -- marlinmt headers
#include <marlinmt/IScheduler.h>
#include <marlinmt/Logging.h>

namespace marlinmt {

  class SuperSequence ;
  class Sequence ;

  /**
   *  @brief  SimpleScheduler class
   */
  class SimpleScheduler : public IScheduler {
  public:
    using ProcessorSequence = std::shared_ptr<SuperSequence> ;

  public:
    SimpleScheduler() ;

    // from IScheduler interface
    void initialize() override ;
    void end() override ;
    void processRunHeader( std::shared_ptr<RunHeader> rhdr ) override ;
    void pushEvent( std::shared_ptr<EventStore> event ) override ;
    void popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) override ;
    std::size_t freeSlots() const override ;

  private:
    ///< The processor super sequence
    ProcessorSequence                _superSequence {nullptr} ;
    ///< The current event being processed
    std::shared_ptr<EventStore>      _currentEvent {nullptr} ;
  };

} // end namespace marlinmt

#endif
