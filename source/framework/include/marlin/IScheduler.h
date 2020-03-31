#ifndef MARLIN_ISCHEDULER_h
#define MARLIN_ISCHEDULER_h 1

// -- std headers
#include <memory>
#include <vector>

// -- marlin headers
#include <marlin/Component.h>

namespace marlin {

  class Application ;
  class EventStore ;
  class RunHeader ;

  /**
   *  @brief  IScheduler interface
   *  Interface for implementing a scheduling algorithm for event processing.
   *  The scheduling can be sequential or parallel (inter-event / intra-event / both).
   *  See implementation classes for more details.
   */
  class IScheduler :public Component {
  public:
    /// Constructor
    IScheduler() ;
    
    virtual ~IScheduler() = default ;
    
    /**
     *  @brief  Initialize the scheduler. 
     *  Read the config section from the configuration 
     */
    virtual void initComponent() override ;

    /**
     *  @brief  Terminate the scheduler activites
     *  Cleanup memory, etc ...
     */
    virtual void end() = 0 ;

    /**
     *  @brief  Process a run header
     *
     *  @param  rhdr the run header to process
     */
    virtual void processRunHeader( std::shared_ptr<RunHeader> rhdr ) = 0 ;

    /**
     *  @brief  Push a new event to the scheduler for processing
     *
     *  @param  event the event to push
     */
    virtual void pushEvent( std::shared_ptr<EventStore> event ) = 0 ;

    /**
     *  @brief  Retrieve finished events from the scheduler
     *
     *  @param  events the list of event to retrieve
     */
    virtual void popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) = 0 ;

    /**
     *  @brief  Get the number of free event slots
     */
    virtual std::size_t freeSlots() const = 0 ;
  };

} // end namespace marlin

#endif
