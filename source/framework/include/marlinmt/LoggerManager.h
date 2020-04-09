#ifndef MARLINMT_LOGGERMANAGER_h
#define MARLINMT_LOGGERMANAGER_h 1

// -- std headers
#include <string>

// -- marlinmt headers
#include <marlinmt/Exceptions.h>
#include <marlinmt/Logging.h>
#include <marlinmt/Component.h>

namespace marlinmt {

  class Application ;

  /**
   *  @brief  LoggerManager class
   *  Responsible for configuring logger for a given application.
   *  Can possibly configure the global logger instance.
   */
  class LoggerManager : public Component {
  public:
    using Logger = Logging::Logger ;

  public:
    LoggerManager(const LoggerManager &) = delete ;
    LoggerManager& operator=(const LoggerManager &) = delete ;
    ~LoggerManager() = default ;

    /**
     *  @brief  Constructor
     */
    LoggerManager() ;

    /**
     *  @brief  Initialize the logger manager
     */
    void initialize() override ;

    /**
     *  @brief  Get the main logger instance (not the global one)
     */
    Logger mainLogger() const ;

    /**
     *  @brief  Create a new logger instance.
     *  The logger shares the same sinks as the main logger
     *
     *  @param  name the logger name
     */
    Logger createLogger( const std::string &name ) const ;
    
  private:
    /// The name of the log file (optional)
    StringParameter         _logfile {*this, "Logfile", "The name of the log file", "" } ;
    /// Whether to use a colored console printout
    BoolParameter           _coloredConsole {*this, "ColoredConsole", "Whether to use a colored console printout", false } ;
  };

} // end namespace marlinmt

#endif
