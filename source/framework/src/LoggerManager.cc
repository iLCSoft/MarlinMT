#include <marlinmt/LoggerManager.h>

// -- marlinmt headers
#include <marlinmt/Application.h>

namespace marlinmt {

  LoggerManager::LoggerManager() : 
    Component("LoggerManager") {
    setDescription( "The logger manager manages all the logging facility whithin the application" ) ;
    // temporary name before initialization
    mainLogger()->setName( "main" ) ;
  }

  //--------------------------------------------------------------------------

  void LoggerManager::initialize() {
    auto config = application().configuration() ;
    if( config.hasSection("logging") ) {
      setParameters( config.section("logging") ) ;
    }
    streamlog::logsink_list sinks {} ;
    if ( _coloredConsole.get() ) {
      sinks.push_back( streamlog::logstream::coloredConsole<Logging::mutex_type>() ) ;
    }
    else {
      sinks.push_back( streamlog::logstream::console<Logging::mutex_type>() ) ;
    }
    if ( not _logfile.get().empty() ) {
      auto logfilename = _logfile.get() ;
      sinks.push_back( streamlog::logstream::simpleFile<Logging::mutex_type>( logfilename ) ) ;
    }
    mainLogger()->setName( application().programName() ) ;
    mainLogger()->setSinks( sinks ) ;
    streamlog::logstream::global().setName( application().programName() ) ;
    streamlog::logstream::global().setSinks( sinks ) ;
  }

  //--------------------------------------------------------------------------

  LoggerManager::Logger LoggerManager::mainLogger() const {
    return _logger ;
  }

  //--------------------------------------------------------------------------

  LoggerManager::Logger LoggerManager::createLogger( const std::string &name ) const {
    auto logger = Logging::createLogger( name ) ;
    logger->setSinks( mainLogger()->sinks() ) ;
    logger->setLevel( mainLogger()->levelName() ) ;
    return logger ;
  }

} // namespace marlinmt
