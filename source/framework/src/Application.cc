
// -- marlinmt headers
#include <marlinmt/Application.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Utils.h>
#include <marlinmt/DataSourcePlugin.h>
#include <marlinmt/Processor.h>
#include <marlinmt/MarlinMTConfig.h>
#include <marlinmt/EventExtensions.h>
#include <marlinmt/IScheduler.h>
#include <marlinmt/SimpleScheduler.h>
#include <marlinmt/concurrency/PEPScheduler.h>
#include <marlinmt/EventStore.h>
#include <marlinmt/RunHeader.h>

// -- std headers
#include <cstring>
#include <filesystem>

using namespace std::placeholders ;

namespace marlinmt {
  
  int Application::main( int argc, char**argv ) {
    // configure and run application
    Application application ;
    auto logger = application.logger() ;
    try {    
      application.init( argc, argv ) ;
      application.run() ;
    }
    catch ( marlinmt::Exception &e ) {
      logger->log<ERROR>() << "MarlinMT main, caught MarlinMT exception " << e.what() << std::endl ;
      logger->log<ERROR>() << "Exiting with status 1" << std::endl ;
      return 1 ;
    }
    catch ( std::exception &e ) {
      logger->log<ERROR>() << "MarlinMT main, caught std::exception " << e.what() << std::endl ;
      logger->log<ERROR>() << "Exiting with status 1" << std::endl ;
      return 1 ;
    }
    catch ( ... ) {
      logger->log<ERROR>() << "MarlinMT main, caught unknown exception" << std::endl ;
      logger->log<ERROR>() << "Exiting with status 2" << std::endl ;
      return 2 ;
    }
    return 0 ;
  }
  
  //--------------------------------------------------------------------------

  void Application::init( int argc, char **argv ) {
    // Geeky MarlinMT logo 
    details::print_banner( std::cout ) ;
    
    // Parse command line first
    CmdLineParser parser ;
    _parseResult = parser.parse( argc, argv ) ;
    
    // Populate the configuration with command line parameters
    auto fullCmdLine = details::convert<std::vector<std::string>>::to_string( _parseResult._arguments ) ;
    fullCmdLine = std::string(argv[0]) + " " + fullCmdLine ;
    auto &cmdLineSection = _configuration.createSection( "CmdLine" ) ;
    cmdLineSection.setParameter( "Program", _parseResult._programName ) ;
    cmdLineSection.setParameter( "Arguments", fullCmdLine ) ;
    cmdLineSection.setParameter( "Config", _parseResult._config.value() ) ;
    cmdLineSection.setParameter( "NThreads", _parseResult._nthreads ) ;
    auto &argsSection = cmdLineSection.addSection( "AdditionalArgs" ) ;
    for( auto &arg : _parseResult._additionalArgs ) {
      argsSection.setParameter( arg.first, arg.second ) ;
    }
    
    // set the main logger name a bit early for more sexy logging
    std::filesystem::path progName = _parseResult._programName ;
    _parseResult._programName = progName.filename().string() ;
    
    // load plugins
    auto &pluginMgr = PluginManager::instance() ;
    pluginMgr.logger()->setLevel<MESSAGE>() ;
    auto libraries = details::split_string<std::string>( details::getenv<std::string>( "MARLINMT_DLL", "" ), ":" ) ;
    pluginMgr.loadLibraries( libraries ) ;
    
    if( _parseResult._dumpExample ) {
      _initialized = true ;
      return ;
    }
    
    // parse the config
    ConfigHelper::readConfig( _parseResult._config.value(), _configuration ) ;
    
    // initialize logging
    _loggerMgr.setup( this ) ;
    logger()->log<MESSAGE0>() << "============ Configuration ============" << std::endl ;
    logger()->log<MESSAGE0>() << _configuration << std::endl ;
    logger()->log<MESSAGE0>() << "=======================================" << std::endl ;
    
    // initialize BookStore
    _bookStoreManager.setup( this ) ;
    
    // initialize geometry
    _geometryMgr.setup( this ) ;
    
    // setup scheduler
    if( 0 == _parseResult._nthreads ) {
      MARLINMT_THROW( "Number of threads can't be 0 !" ) ;
    }
    if( 1 == _parseResult._nthreads ) {
      logger()->log<MESSAGE>() << "Running in single-thread mode" << std::endl ;
      _scheduler = std::make_shared<SimpleScheduler>() ;
    }
    else {
      logger()->log<MESSAGE>() << "Running in multi-thread mode (nthreads=" << _parseResult._nthreads << ")" << std::endl ;
      _scheduler = std::make_shared<concurrency::PEPScheduler>() ;
    }
    _scheduler->setup( this ) ;

    // initialize data source
    auto dstype = configuration().section("datasource").parameter<std::string>( "DatasourceType" ) ;
    _dataSource = pluginMgr.create<DataSourcePlugin>( dstype ) ;
    if( nullptr == _dataSource ) {
      MARLINMT_THROW( "Data source of type '" + dstype + "' not found in plugins" ) ;
    }
    _dataSource->setup( this ) ;
    _dataSource->onEventRead( std::bind( &Application::onEventRead, this, _1 ) ) ;
    _dataSource->onRunHeaderRead( std::bind( &Application::onRunHeaderRead, this, _1 ) ) ;
    
    // store processor conditions
    auto &execSection = _configuration.section("execute") ;
    auto procs = execSection.parameterNames() ;
    for( auto &proc : procs ) {
      _conditions[ proc ] = execSection.parameter<std::string>( proc ) ;
    }
    _initialized = true ;
  }

  //--------------------------------------------------------------------------

  void Application::run() {
    if( _parseResult._dumpExample ) {
      logger()->log<MESSAGE>() << "Dry run. Output configuration in: " << _parseResult._config.value() << std::endl ;
      dumpExampleConfig() ;
      return ;
    }
    try {
      _dataSource->readAll() ;
    }
    catch( StopProcessingException &e ) {
      logger()->log<ERROR>() << std::endl
          << " **********************************************************" << std::endl
          << " *                                                        *" << std::endl
          << " *   Stop of event processing requested by processor :    *" << std::endl
          << " *                  "  << e.what()                           << std::endl
          << " *     will call end() method of all processors !         *" << std::endl
          << " *                                                        *" << std::endl
          << " **********************************************************" << std::endl
          << std::endl ;
    }
    _geometryMgr.clear() ;
    _scheduler->end() ;
    _bookStoreManager.writeToDisk();
  }
  
  //--------------------------------------------------------------------------
  
  const Application::CmdLineParseResult &Application::cmdLineParseResult() const {
    return _parseResult ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Application::programName() const { 
    return cmdLineParseResult()._programName ; 
  }

  //--------------------------------------------------------------------------

  bool Application::isInitialized() const {
    return _initialized ;
  }

  //--------------------------------------------------------------------------

  Application::Logger Application::logger() const {
    return _loggerMgr.mainLogger() ;
  }

  //--------------------------------------------------------------------------

  Application::Logger Application::createLogger( const std::string &name ) const {
    if ( isInitialized() ) {
      return _loggerMgr.createLogger( name ) ;
    }
    else {
      return Logging::createLogger( name ) ;
    }
  }

  //--------------------------------------------------------------------------

  void Application::onEventRead( std::shared_ptr<EventStore> event ) {
    EventList events ;
    while( _scheduler->freeSlots() == 0 ) {
      _scheduler->popFinishedEvents( events ) ;
      if( not events.empty() ) {
        processFinishedEvents( events ) ;
        events.clear() ;
        break;
      }
      std::this_thread::sleep_for( std::chrono::milliseconds(1) ) ;
    }
    // prepare event extensions for users
    // random seeds extension
    auto seeds = _randomSeedMgr.generateRandomSeeds( event.get() ) ;
    auto randomSeedExtension = new RandomSeedExtension( std::move(seeds) ) ;
    event->extensions().add<extensions::RandomSeed>( randomSeedExtension ) ;
    // runtime conditions extension
    // TODO replace this
    auto procCondExtension = new ProcessorConditionsExtension( _conditions ) ;
    event->extensions().add<extensions::ProcessorConditions>( procCondExtension )  ;
    _scheduler->pushEvent( event ) ;
    // check a second time
    _scheduler->popFinishedEvents( events ) ;
    if( not events.empty() ) {
      processFinishedEvents( events ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Application::onRunHeaderRead( std::shared_ptr<RunHeader> rhdr ) {
    logger()->log<MESSAGE9>() << "New run header no " << rhdr->runNumber() << std::endl ;
    _scheduler->processRunHeader( rhdr ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const GeometryManager &Application::geometryManager() const {
    return _geometryMgr ;
  }
  
  //--------------------------------------------------------------------------
  
  const RandomSeedManager &Application::randomSeedManager() const {
    return _randomSeedMgr ;
  }
  
  //--------------------------------------------------------------------------
  
  RandomSeedManager &Application::randomSeedManager() {
    return _randomSeedMgr ;
  }
  
  //--------------------------------------------------------------------------
  
  void Application::processFinishedEvents( const EventList &events ) const {
    // simple printout for the time being
    for( auto event : events ) {
      logger()->log<MESSAGE9>()
        << "Event uid " << event->uid()
        << " finished" << std::endl ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  BookStoreManager &Application::bookStoreManager() {
    return _bookStoreManager ;
  }

  //--------------------------------------------------------------------------
  
  const BookStoreManager &Application::bookStoreManager() const {
    return _bookStoreManager ;
  }
  
  //--------------------------------------------------------------------------
  
  const Configuration &Application::configuration() const {
    return _configuration ;
  }
  
  //--------------------------------------------------------------------------
  
  void Application::dumpExampleConfig() {
    Configuration &config = _configuration ;
    auto &pluginMgr = PluginManager::instance() ;
    
    auto &execSection = config.createSection("execute") ;
    execSection.setParameter( "MyTestProcessor", true ) ;
    
    auto &loggingSection = config.createSection("logging") ;
    _loggerMgr.getParameters( loggingSection ) ;
    
    auto &geometrySection = config.createSection("geometry") ;
    geometrySection.setParameter<std::string>( "GeometryType", "EmptyGeometry" ) ;
    
    auto &datasourceSection = config.createSection("datasource") ;
    auto lcioReader = pluginMgr.create<DataSourcePlugin>( "LCIOReader" ) ;
    if( lcioReader ) {
      lcioReader->getParameters( datasourceSection ) ;
      datasourceSection.setParameter<std::string>( "DatasourceType", "LCIOReader" ) ;
    }
    else {
      datasourceSection.setParameter<std::string>( "DatasourceType", "CustomSource" ) ;
      datasourceSection.setParameter<std::string>( "InputFile", "yourinput.dat" ) ;
    }

    auto &bookstoreSection = config.createSection("bookstore") ;
    _bookStoreManager.getParameters( bookstoreSection ) ;
    
    auto &globalSection = config.createSection("global") ;
    globalSection.setParameter( "RandomSeed", 1234567890 ) ;
    
    auto &procsSection = config.createSection("processors") ;
    auto pluginNames = pluginMgr.pluginNames<Processor>() ;
    for( auto pluginName : pluginNames ) {
      auto processor = pluginMgr.create<Processor>( pluginName ) ;
      processor->setName( "My" + processor->type() ) ;
      auto &procSection = procsSection.addSection( processor->name() ) ;
      procSection.setParameter( "ProcessorName", processor->name() ) ;
      procSection.setParameter( "ProcessorType", processor->type() ) ;
      auto criticalOpt = processor->runtimeOption( Processor::ERuntimeOption::eCritical ) ;
      auto cloneOpt = processor->runtimeOption( Processor::ERuntimeOption::eClone ) ;
      procSection.setParameter( "ProcessorCritical", criticalOpt.has_value() ? criticalOpt.value() : false ) ;
      procSection.setParameter( "ProcessorClone", cloneOpt.has_value() ? cloneOpt.value() : false ) ;
      processor->getParameters( procSection ) ;
    }
    ConfigHelper::writeConfig( _parseResult._config.value(), config ) ;
  }

} // namespace marlinmt
