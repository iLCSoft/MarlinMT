#ifndef MARLINMT_CONCURRENCY_PEPSCHEDULER_h
#define MARLINMT_CONCURRENCY_PEPSCHEDULER_h 1

// -- marlinmt headers
#include <marlinmt/IScheduler.h>
#include <marlinmt/Logging.h>
#include <marlinmt/Utils.h>
#include <marlinmt/concurrency/ThreadPool.h>

// -- std headers
#include <unordered_set>

namespace marlinmt {

  class SuperSequence ;
  class Sequence ;

  namespace concurrency {

    /**
     *  @brief  WorkerOutput struct
     *  Stores the output of a processor sequence call
     */
    struct WorkerOutput {
      ///< The input event
      std::shared_ptr<EventStore>         _event {nullptr} ;
      ///< An exception potential throw in the worker thread
      std::exception_ptr                  _exception {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    /**
     *  @brief  PEPScheduler class
     *  Parallel Event Processing Scheduler.
     *  Implements the scheduling of parallel inter-event processing.
     *
     *  A set of N worker threads are allocated at startup within a thread pool.
     *  Every time a new event is pushed in the scheduler, the event is queued
     *  in the thread pool for further processing. Note that this operation can
     *  fail if the thread pool queue is full. Use freeSlots() to know how many
     *  slots are free in the thread pool queue and avoid unexpected exceptions.
     */
    class PEPScheduler : public IScheduler {
    public:
      using ConditionsMap = std::map<std::string, std::string> ;
      using InputType = std::shared_ptr<EventStore> ;
      using OutputType = WorkerOutput ;
      using WorkerPool = ThreadPool<InputType,OutputType> ;
      using ProcessorSequence = std::shared_ptr<SuperSequence> ;
      using PushResultList = std::vector<WorkerPool::PushResult> ;
      using EventList = std::vector<std::shared_ptr<EventStore>> ;
      using Clock = std::chrono::steady_clock ;
      using TimePoint = std::chrono::steady_clock::time_point ;

    public:
      /// Constructor
      PEPScheduler() ;

      // from IScheduler interface
      void initialize() override ;
      void end() override ;
      void processRunHeader( std::shared_ptr<RunHeader> rhdr ) override ;
      void pushEvent( std::shared_ptr<EventStore> event ) override ;
      void popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) override ;
      std::size_t freeSlots() const override ;

    private:
      void preConfigure() ;
      void configureProcessors() ;
      void configurePool() ;

    private:
      ///< The worker thread pool
      WorkerPool                       _pool {} ;
      ///< The processor super sequence
      ProcessorSequence                _superSequence {nullptr} ;
      ///< The list of worker output promises
      PushResultList                   _pushResults {} ;
      ///< The start time
      clock::time_point                _startTime {} ;
      ///< The end time
      clock::time_point                _endTime {} ;
      ///< The total time spent on processing run headers
      clock::duration_rep              _runHeaderTime {0} ;
      ///< The total time spent on locking on thread pool queue access
      clock::duration_rep              _lockingTime {0} ;
      ///< The total time spent on popping events from the output event pool
      clock::duration_rep              _popTime {0} ;
      /// The scheduler event queue size
      UIntParameter                    _queueSize {*this, "EventQueueSize", "The input event queue size (default 2*nthreads)"} ;
    };

  }

} // end namespace marlinmt

#endif
