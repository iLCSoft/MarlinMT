#include <marlinmt/concurrency/PEPScheduler.h>

// -- marlinmt headers
#include <marlinmt/Application.h>
#include <marlinmt/Utils.h>
#include <marlinmt/Sequence.h>
#include <marlinmt/Processor.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/EventStore.h>
#include <marlinmt/RunHeader.h>

// -- std headers
#include <exception>
#include <algorithm>
#include <iomanip>
#include <set>

namespace marlinmt {

  namespace concurrency {

    /**
     *  @brief  ProcessorSequenceWorker class
     */
    class ProcessorSequenceWorker : public WorkerBase<PEPScheduler::InputType,PEPScheduler::OutputType> {
    public:
      using Base = WorkerBase<PEPScheduler::InputType,PEPScheduler::OutputType>;
      using Input = Base::Input ;
      using Output = Base::Output ;

    public:
      ~ProcessorSequenceWorker() = default ;

    public:
      /**
       *  @brief  Constructor
       *
       *  @param  sequence the processor sequence to execute
       */
      ProcessorSequenceWorker( std::shared_ptr<Sequence> sequence ) ;

    private:
      // from WorkerBase<IN,OUT>
      Output process( Input && event ) override ;

    private:
      ///< The processor sequence to run in the worker thread
      std::shared_ptr<Sequence>     _sequence {nullptr} ;
    };

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    ProcessorSequenceWorker::ProcessorSequenceWorker( std::shared_ptr<Sequence> sequence ) :
      _sequence(sequence) {
      /* nop */
    }

    //--------------------------------------------------------------------------

    ProcessorSequenceWorker::Output ProcessorSequenceWorker::process( Input && event ) {
      Output output {} ;
      output._event = event ;
      try {
        _sequence->processEvent( event ) ;
      }
      catch(...) {
        output._exception = std::current_exception() ;
      }
      return output ;
    }

    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------

    PEPScheduler::PEPScheduler() :
      IScheduler() {
      setName( "PEPScheduler" ) ;
    }
    
    //--------------------------------------------------------------------------

    void PEPScheduler::initialize() {
      // base init
      IScheduler::initialize() ;
      preConfigure() ;
      configureProcessors() ;
      configurePool() ;
      _startTime = clock::now() ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::end() {
      _pool.stop(false) ;
      EventList events ;
      popFinishedEvents( events ) ;
      if( not _pushResults.empty() ) {
        error() << "This should never happen !!" << std::endl ;
      }
      message() << "Terminating application" << std::endl ;
      _endTime = clock::now() ;
      _superSequence->end() ;
      // print some statistics
      _superSequence->printStatistics( _logger ) ;
      // print additional threading summary
      const auto parallelTime = clock::time_difference( _startTime, _endTime ) - _runHeaderTime ;
      double totalProcessorClock {0.0} ;
      double totalApplicationClock {0.0} ;
      for ( unsigned int i=0 ; i<_superSequence->size() ; ++i ) {
        auto summary = _superSequence->sequence(i)->clockMeasureSummary() ;
        totalProcessorClock += summary._procClock ;
        totalApplicationClock += summary._appClock ;
      }
      const double speedup = totalProcessorClock / parallelTime ;
      const double lockTimeFraction = ((totalApplicationClock - totalProcessorClock) / totalApplicationClock) * 100. ;
      message() << "---------------------------------------------------" << std::endl ;
      message() << "-- Threading summary" << std::endl ;
      message() << "--   N threads:                      " << _superSequence->size() << std::endl ;
      message() << "--   Speedup (serial/parallel):      " << totalProcessorClock << " / " << parallelTime << " = " << speedup << std::endl ;
      if( _superSequence->size() > 1 ) {
        double speedupPercent = (speedup - 1) * 100 / static_cast<double>( _superSequence->size() - 1 ) ;
        if( speedupPercent < 0 ) {
          speedupPercent = 0. ;
        }
        message() << "--   Speedup percentage:             " << speedupPercent << " " << '%' << std::endl ;
      }
      message() << "--   Queue lock time:                " << _lockingTime << " ms" << std::endl ;
      message() << "--   Pop event time:                 " << _popTime << " ms" << std::endl ;
      message() << "--   Lock time fraction:             " << lockTimeFraction << " %" << std::endl ;
      message() << "---------------------------------------------------" << std::endl ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::preConfigure() {
      // create processor super sequence
      unsigned int nthreads = application().cmdLineParseResult()._nthreads ;
      _superSequence = std::make_shared<SuperSequence>(nthreads) ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::configureProcessors() {
      log<DEBUG5>() << "PEPScheduler configureProcessors ..." << std::endl ;
      auto &execSection = application().configuration().section("execute") ;
      auto &procsSection = application().configuration().section("processors") ;
      // create list of active processors
      auto activeProcessors = execSection.parameterNames() ;
      // auto activeProcessors = app->activeProcessors() ;
      if ( activeProcessors.empty() ) {
        MARLINMT_THROW( "Active processor list is empty !" ) ;
      }
      // populate processor sequences
      for ( size_t i=0 ; i<activeProcessors.size() ; ++i ) {
        auto procName = activeProcessors[ i ] ;
        log<DEBUG5>() << "Active processor " << procName << std::endl ;
        auto &procSection = procsSection.section( procName ) ;
        // if ( nullptr == processorParameters ) {
        //   throw Exception( "PEPScheduler::configureProcessors: undefined processor '" + procName + "'" ) ;
        // }
        _superSequence->addProcessor( procSection ) ;
      }
      _superSequence->init( &application() ) ;
      log<DEBUG5>() << "configureProcessors ... DONE" << std::endl ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::configurePool() {
      // create N workers for N processor sequences
      log<DEBUG5>() << "configurePool ..." << std::endl ;
      log<DEBUG5>() << "Number of workers: " << _superSequence->size() << std::endl ;
      for( unsigned int i=0 ; i<_superSequence->size() ; ++i ) {
        log<DEBUG>() << "Adding worker ..." << std::endl ;
        _pool.addWorker<ProcessorSequenceWorker>( _superSequence->sequence(i) ) ;
      }
      log<DEBUG5>() << "starting thread pool" << std::endl ;
      unsigned int queueSize = _queueSize.isSet() ? 
        _queueSize.get() : 
        static_cast<unsigned int>(2 * _superSequence->size()) ;
      _pool.setMaxQueueSize( queueSize ) ;
      _pool.start() ;
      _pool.setAcceptPush( true ) ;
      log<DEBUG5>() << "configurePool ... DONE" << std::endl ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
      // Current way to process run header:
      //  - Stop accepting event in thread pool
      //  - Wait for current events processing to finish
      //  - Process run header
      //  - Resume pool access for new event push
      _pool.setAcceptPush( false ) ;
      // need to wait for all current tasks to finish
      // and then process run header
      while( _pool.active() ) {
        std::this_thread::sleep_for( std::chrono::microseconds(10) ) ;
      }
      auto rhdrStart = clock::now() ;
      _superSequence->processRunHeader( rhdr ) ;
      auto rhdrEnd = clock::now() ;
      _runHeaderTime += clock::time_difference<clock::seconds>( rhdrStart, rhdrEnd ) ;
      _pool.setAcceptPush( true ) ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::pushEvent( std::shared_ptr<EventStore> event ) {
      // push event to thread pool queue. It might throw !
      auto start = clock::now() ;
      _pushResults.push_back( _pool.push( WorkerPool::PushPolicy::ThrowIfFull, std::move(event) ) ) ;
      _lockingTime += clock::elapsed_since<clock::milliseconds>( start ) ;
    }

    //--------------------------------------------------------------------------

    void PEPScheduler::popFinishedEvents( std::vector<std::shared_ptr<EventStore>> &events ) {
      auto start = clock::now() ;
      auto iter = _pushResults.begin() ;
      while( iter != _pushResults.end() ) {
        const bool finished = (iter->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) ;
        if( finished ) {
          auto output = iter->second.get() ;
          // if an exception was raised during processing rethrow it there !
          if( nullptr != output._exception ) {
            std::rethrow_exception( output._exception ) ;
          }
          message() << "Finished event uid " << output._event->uid() << std::endl ;
          events.push_back( output._event ) ;
          iter = _pushResults.erase( iter ) ;
          continue;
        }
        ++iter ;
      }
      _popTime += clock::elapsed_since<clock::milliseconds>( start ) ;
    }

    //--------------------------------------------------------------------------

    std::size_t PEPScheduler::freeSlots() const {
      return _pool.freeSlots() ;
    }

  }

} // namespace marlinmt
