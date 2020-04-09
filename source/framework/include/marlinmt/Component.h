#pragma once

// -- std headers
#include <typeindex>

// -- marlinmt headers
#include <marlinmt/Logging.h>
#include <marlinmt/Parameter.h>

namespace marlinmt {
  
  class Application ;
  
  /**
   *  @brief  Component class.
   *  Base class for all application components. Groups a few feature common
   *  too all application components:
   *   - configuration via parameters
   *   - logger access
   *   - access to application
   */
  class Component : public Configurable {
  public:    
    friend class Application ; 
    using LoggerPtr = Logging::Logger ;
    
  public:
    /// No default constructor
    Component() = delete ;
    /// No copy or assignement
    Component(const Component &) = delete ;
    Component &operator=(const Component &) = delete ;

    /// Default destructor
    virtual ~Component() = default ;
    
    /// Constructor with component type
    Component( const std::string &type ) ;
    
    /**
     *  @brief  Get the component name
     */
    const std::string &type() const ;
    
    /**
     *  @brief  Get the component name
     */
    const std::string &name() const ;
    
    /**
     *  @brief  Set the component name
     *  
     *  @param  n the component name
     */
    void setName( const std::string &n ) ;
    
    /**
     *  @brief  Get the component description
     */
    const std::string &description() const ;
    
    /**
     *  @brief  Set the component description
     *
     *  @param  desc the component description
     */
    void setDescription( const std::string &desc ) ;
    
    /**
     *  @brief  Initialize the component
     */
    virtual void initialize() { /*nop*/ }
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    const Application &application() const ;
    
    /**
     *  @brief  Get the application in which the component is registered
     */
    Application &application() ;
    
    /**
     *  @brief  Log a message with specific log level.
     *  Returns a stream object in which you can log.
     *  Usage:
     *  @code{.cpp}
     *  log<MESSAGE>() << "This is a message" << std::endl ;
     *  @endcode
     */
    template <class T>
    inline Logging::StreamType log() const {
      return _logger->log<T>() ;
    }
    
    /// Shortcut for log<DEBUG>()
    Logging::StreamType debug() const ;
    
    /// Shortcut for log<MESSAGE>()
    Logging::StreamType message() const ;
    
    /// Shortcut for log<WARNING>()
    Logging::StreamType warning() const ;
    
    /// Shortcut for log<ERROR>()
    Logging::StreamType error() const ;
    
    /**
     *  @brief  Set the verbosity level
     * 
     *  @param  level the verbosity level to set
     */
    void setVerbosity( const std::string &level ) ;
    
    /**
     *  @brief  Get the verbosity level
     */
    const std::string &verbosity() const ;
    
    /**
     *  @brief  Whether the component has been initialized
     */
    bool isInitialized() const ;
    
    /**
     *  @brief  Setup the component. 
     *  Called by the application before initialization
     * 
     *  @param  app the application owning the component
     */
    void setup( Application *app ) ;
    
    /**
     *  @brief  Print the component parameters
     */
    void printParameters() const ;
    
    /**
     *  @brief  Print the component parameters at specific verbosity
     */
    template <class T>
    void printParameters() const ;
    
    /**
     *  @brief  Set the parameters from the configuration section
     * 
     *  @param  section the input parameter section
     *  @param  throwIfNotFound whether to throw if a parameter of the input
     *          section can not be mapped to a parameters of this component
     */
    void setParameters( const ConfigSection &section, bool throwIfNotFound = false ) ;
    
    /**
     *  @brief  Get the parameters from configurable object and 
     *          populate the config section with 
     * 
     *  @param  section the config section to populate
     *  @param  exclude a list of parameter keys to exclude
     */
    void getParameters( ConfigSection &section, const std::set<std::string> &exclude = {} ) const ;
        
  protected:
    /// The component type
    std::string              _type {} ;
    /// The component name
    std::string              _name {} ;
    /// The component description
    std::string              _description {"No description"} ;
    /// The application in which the component has been registered
    Application             *_application {nullptr} ;
    /// The logger instance
    LoggerPtr                _logger {nullptr} ;
    /// The verbosity level of the logger (parameter)
    StringParameter          _verbosity { *this, "Verbosity", "The component verbosity level", "MESSAGE" } ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  template <class T>
  void Component::printParameters() const {
    log<T>() << name() << " (" << type() << ") parameters:" << std::endl ;
    for( auto iter : _parameters ) {
      auto paramStr = iter.second->isSet() ? iter.second->str() : ( iter.second->hasDefault() ? iter.second->defaultStr() :"[undefined]" ) ;
      log<T>() << iter.first << " (" << iter.second->typeStr() << "): " << paramStr << std::endl ;
    }
  }
  
}