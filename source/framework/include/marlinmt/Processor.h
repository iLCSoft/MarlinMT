#ifndef MARLINMT_PROCESSOR_h
#define MARLINMT_PROCESSOR_h 1

// -- marlinmt headers
#include <marlinmt/Component.h>
#include <marlinmt/EventStore.h>
#include <marlinmt/RunHeader.h>
#include <marlinmt/MarlinConfig.h>  // for Marlin version macros

// -- std headers
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>

namespace marlinmt {

  class Application ;
  class ProcessorApi ;

  /**
   *  @brief  Processor class
   *
   *  Base class for Marlin processors.
   *  Users can optionaly overwrite the following methods: <br>
   *    init, processRun, processEvent and end.<br>
   *  Use registerProcessorParameter to define all parameters that the module uses.
   *  Registered parameters are filled automatically before init() is called.
   *  With MyAplication -l you can print a list of available processors including
   *  the steering parameters they use/need.<br>
   *  With MyAplication -x you can print an example XML steering file for all known
   *  processors.
   *
   *  @see init
   *  @see processRun
   *  @see processEvent
   *  @see end
   *
   *  @author F. Gaede, DESY
   *  @version $Id: Processor.h,v 1.38 2008-06-26 10:25:36 gaede Exp $
   */
  class Processor : public Component {
    friend class ProcessorApi ;

  private:
    // prevent users from making (default) copies of processors
    Processor() = delete ;
    Processor(const Processor&) = delete ;
    Processor& operator=(const Processor&) = delete ;

  public:
    virtual ~Processor() = default ;

    /**
     *  @brief  ERuntimeOption enumerator
     */
    enum class ERuntimeOption {
      eCritical,     /// Whether the processor has to be executed in a critical section
      eClone         /// Whether the processor must be cloned in each thread worker
    };

    using RuntimeOptions = std::map<ERuntimeOption, bool> ;

  public:
    /**
     *  @brief  Constructor. Subclasses need to call this in their default constructor.
     *
     *  @param  t the processor type
     */
    Processor( const std::string& t ) ;

    /**
     *  @brief  Initialize the processor.
     *  Called at the begin of the job before anything is read.
     *  Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() { /* nop */ }

    /**
     *  @brief  Call at start of run
     */
    virtual void processRunHeader( RunHeader * ) { /* nop */ }

    /**
     *  @brief  Process an input event.
     *  Called for every event - the working horse.
     */
    virtual void processEvent( EventStore * ) { /* nop */ }

    /**
     *  @brief  Terminate the processor.
     *  Called after data processing for clean up in the inverse order of the init()
     *  method so that resources allocated in the first processor also will be available
     *  for all following processors.
     */
    virtual void end() { /* nop */ }

    /**
     *  @brief  Get the forced runtime option settings.
     *  The return value is set if the forced option was specified by the user.
     *
     *  @param  option the runtime option to get
     */
    std::optional<bool> runtimeOption( ERuntimeOption option ) const ;

  protected:
    /**
     *  @brief  Force the runtime option to a given boolean value.
     *
     *  Depending on the implementation of your processor, setting
     *  one of the runtime option might be a necessity. For example, you
     *  handle a lot of data in your processor members that you don't want
     *  to duplicate. In this case you should call in the constructor
     *  @code{cpp}
     *  forceRuntimeOption( Processor::RuntimeOption::Clone, false ) ;
     *  @endcode
     *  The code contained in processEvent() might also not be thread safe.
     *  In this case you can ask the Marlin framework to call the processEvent()
     *  method in a critical section (using std::mutex). Do to so, call:
     *  @code{cpp}
     *  forceRuntimeOption( RuntimeOption::Critical, true ) ;
     *  @endcode
     *  If a runtime option is forced in the code and the steering file tries
     *  to overwrite it, an exception will be raised.
     *
     *  Note that this method must be called in user processor constructor
     *  to ensure it is correctly handled by the framework at configuration time.
     *
     *
     *  @param  option the runtime option to force
     *  @param  value the boolean value to set
     */
    void setRuntimeOption( ERuntimeOption option, bool value ) ;
    
  private:
    /// From Component class
    void initialize() override ;

  private:
    /// The user forced runtime options for parallel processing
    RuntimeOptions                     _forcedRuntimeOptions {} ;
  };

} // end namespace marlinmt

#endif
