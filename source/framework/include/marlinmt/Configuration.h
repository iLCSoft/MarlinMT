#pragma once

// -- std headers
#include <string>
#include <map>
#include <vector>

// -- marlinmt headers
#include <marlinmt/Utils.h>
#include <marlinmt/Exceptions.h>

namespace marlinmt {
  
  class Configurable ;
  
  /**
   *  @brief  ConfigSection class
   *  Holds a set of parameters and subsection
   */
  class ConfigSection {
  public:
    using ConfigSectionMap = std::map<std::string, ConfigSection> ;
    using ParameterMap = std::map<std::string, std::string> ;
    using Metadata = std::map<std::string, std::string> ;
        
  public:
    // default copy and move
    ConfigSection( const ConfigSection& ) = default ;
    ConfigSection& operator=( const ConfigSection& ) = default ;
    ConfigSection( ConfigSection&& ) = default ;
    ConfigSection& operator=( ConfigSection&& ) = default ;
    
    /**
     *  @brief  Constructor with name
     * 
     *  @param  name the section name
     */
    ConfigSection( const std::string &name ) ;
    
    /**
     *  @brief  Get the section name
     */
    const std::string &name() const ;
    
    /**
     *  @brief  Whether the section is empty (no parameter, no subsection)
     */
    bool empty() const ;
    
    /** @name Sections
     */
    ///@{ 
    /**
     *  @brief  Get a subsection by name. Throws if not found
     * 
     *  @param  n the subsection name
     */
    ConfigSection &section( const std::string &n ) ;
    
    /**
     *  @brief  Get a subsection by name (const version). Throws if not found
     * 
     *  @param  n the subsection name
     */
    const ConfigSection &section( const std::string &n ) const ;
    
    /**
     *  @brief  Create a new subsection. Throws if already exists
     * 
     *  @param  n the subsection name
     */
    ConfigSection &addSection( const std::string &n ) ;
    
    /**
     *  @brief  Whether the subsection exists
     * 
     *  @param  n the subsection name
     */
    bool hasSection( const std::string &n ) const ;
    
    /**
     *  @brief  Get the list of subsection names
     */
    std::vector<std::string> subsectionNames() const ;
    ///@}
    
    /** @name Parameters
     */
    ///@{
    /**
     *  @brief  Set a parameter value. If the parameter exists it is replaced
     * 
     *  @param  n the parameter name
     *  @param  val the parameter value
     */
    template <typename T>
    ConfigSection &setParameter( const std::string &n, const T &val ) {
      _parameters.insert_or_assign( n, details::convert<T>::to_string(val) ) ;
      return *this ;
    }
    
    /**
     *  @brief  Get a parameter value as type T. 
     *  Throw if the parameter doesn't exist
     * 
     *  @param  n the parameter name
     */
    template <typename T>
    inline T parameter( const std::string &n ) const {
      auto iter = _parameters.find( n ) ;
      if( _parameters.end() == iter  ) {
        MARLINMT_THROW( "No parameter '" + n + "' in section '" + name() + "'" ) ;
      }
      return details::convert<T>::from_string( iter->second ) ;
    }
    
    /**
     *  @brief  Get a parameter value as type T. 
     *  Return the default value if the parameter doesn't exist
     *  
     *  @param  n the parameter name
     *  @param  defVal the defult value
     */
    template <typename T>
    inline T parameter( const std::string &n, const T &defVal ) const {
      auto iter = _parameters.find( n ) ;
      if( _parameters.end() == iter  ) {
        return defVal ;
      }
      return details::convert<T>::from_string( iter->second ) ;
    }
    
    /**
     *  @brief  Whether the parameter exists
     * 
     *  @param  n the parameter name
     */
    bool hasParameter( const std::string &n ) const ;
    
    /**
     *  @brief  Get the list of parameter names
     */
    std::vector<std::string> parameterNames() const ;
    
    /**
     *  @brief  Get the section metadata
     */
    const Metadata &metadata() const ;
    
    /**
     *  @brief  Get the section metadata
     */
    Metadata &metadata() ;
    ///@}
    
    /**
     *  @brief  Load the parameters of the current section in the configurable object
     * 
     *  @param  cfg the configurable object
     */
    void loadParameters( Configurable &cfg ) const ;
    
    /**
     *  @brief  Get the raw parameter storage
     */
    const ParameterMap &parameters() const ;
    
  private:
    /// The subsection map
    ConfigSectionMap           _subsections {} ;
    /// The parameter map
    ParameterMap               _parameters {} ;
    /// The section metdata map
    Metadata                   _metadata {} ;
    /// The section name
    const std::string          _name ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  Configuration class.
   *  Top level configuration class holding config sections and constant.
   *  It is the main object to populate when reading the configuration from config files
   */
  class Configuration {
  public:
    using ConfigSectionMap = ConfigSection::ConfigSectionMap ;
    using ConstantsMap = std::map<std::string, std::string> ;
    
  public:
    // Default constructors, destructors, copy, move
    Configuration() = default ;
    ~Configuration() = default ;
    Configuration( const Configuration & ) = default ;
    Configuration( Configuration && ) = default ;
    Configuration& operator=( const Configuration & ) = default ;
    Configuration& operator=( Configuration && ) = default ;
    
    /** @name Constants
     */
    ///@{
    /**
     *  @brief  Get a constant value as type T. Throw if not found
     * 
     *  @param  cn the constant name
     */
    template <typename T>
    T constantAs( const std::string &cn ) const {
      auto iter = _constants.find( cn ) ;
      if( iter == _constants.end() ) {
        MARLINMT_THROW( "No constant '" + cn +  "' in configuration" ) ;
      }
      return details::convert<T>::from_string( iter->second ) ;
    }
    
    /**
     *  @brief  Get a constant value as type T. 
     *  Return the default value if not found
     * 
     *  @param  cn the constant name
     *  @param  defVal the default value
     */
    template <typename T>
    T constantAs( const std::string &cn, const T &defVal ) const {
      auto iter = _constants.find( cn ) ;
      if( iter == _constants.end() ) {
        return defVal ;
      }
      return details::convert<T>::from_string( iter->second ) ;
    }
    
    /**
     *  @brief  Add a constant. The value must be convertible to/from string.
     *  Throw if already the constant already exists
     *   
     *  @param  cn the constant name
     *  @param  val the constant value
     */
    template <typename T>
    Configuration &addConstant( const std::string &cn, const T &val ) {
      auto iter = _constants.find( cn ) ;
      if( iter != _constants.end() ) {
        MARLINMT_THROW( "Constant '" + cn +  "' already present in configuration" ) ;
      }
      _constants.emplace( cn, details::convert<T>::to_string( val ) ) ;
      return *this ;
    }
    
    /**
     *  @brief  Whetehr the constant is registered in the configuration
     */
    bool hasConstant( const std::string &cn ) const ;
    
    /**
     *  @brief  Get the constants map
     */
    const ConstantsMap &constants() const ;
    ///@}
    
    /** @name Sections
     */
    ///@{
    /**
     *  @brief  Create a new section by name. Throw if already exists 
     *  
     *  @param  sn the new section name
     */
    ConfigSection &createSection( const std::string &sn ) ;
    
    /**
     *  @brief  Get a section by name. Throw if doesn't exists 
     *  
     *  @param  sn the section name
     */
    ConfigSection &section( const std::string &sn ) ;
    
    /**
     *  @brief  Get a section by name. Throw if doesn't exists 
     *  
     *  @param  sn the section name
     */
    const ConfigSection &section( const std::string &sn ) const ;
    
    /**
     *  @brief  Get the list of subsections
     */
    std::vector<std::string> sections() const ;
    
    /**
     *  @brief  Whether the section exists
     * 
     *  @param  n the section name
     */
    bool hasSection( const std::string &n ) const ;
    ///@}    

    /** @name Helper methods
     */
    ///@{
    /**
     *  @brief  Replace all occurences of ${constant_name} in the input string
     *  where "constant_name" must match a registered constant name
     *  
     *  @param  str the input string to modify
     */
    void replaceConstants( std::string& str ) const ;
    ///@}
    
  private:
    /// The top level configuration sections
    ConfigSectionMap           _sections {} ;
    /// The constants map
    ConstantsMap               _constants {} ;
  };
  
  /// Stream operator
  std::ostream &operator<<( std::ostream &stream, const Configuration &cfg ) ;
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ConfigReader base class
   *  Interface for reading configuration
   */
  class ConfigReader {
  public:
    /// Default destructor
    virtual ~ConfigReader() = default ;
    
    /**
     *  @brief  Initialize the parser. The descriptor string can be e.g:
     *     - file name
     *     - database description
     *     
     *  @param desc a descriptor string
     */
    virtual void init( const std::string &desc ) = 0 ;

    /**
     *  @brief  Read the configuration and populate the configuration object.
     *  
     *  @param  cfg the configuration object to populate
     */
    virtual void read( Configuration &cfg ) = 0 ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ConfigWriter base class
   *  Interface for writing configuration
   */
  class ConfigWriter {
  public:    
    /// Default destructor
    virtual ~ConfigWriter() = default ;
    
    /**
     *  @brief  Initialize the parser. The descriptor string can be e.g:
     *     - file name
     *     - database description
     *     
     *  @param desc a descriptor string
     */
    virtual void init( const std::string &desc ) = 0 ;

    /**
     *  @brief  Write the configuration object
     * 
     *  @param  cfg the configuration object to write
     */
    virtual void write( const Configuration &cfg ) = 0 ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  /**
   *  @brief  ConfigHelper class
   *  A simple class with helper methods for configuration
   */
  class ConfigHelper {
    // Static API only
    ConfigHelper() = delete ;
    ~ConfigHelper() = delete ;
    
  public:
    /**
     *  @brief  Split the string to a pair "plugin, input".
     *  The separator is a colon ':' character.
     *  For example:
     *    - "XMLConfigReader:/path/to/myfile.xml"
     *    - "XMLConfigWriter:/path/to/myfile.xml"
     * 
     *  @param  str the input string to split
     */
    static std::pair<std::string, std::string> splitPluginInput( const std::string &str ) ;
    
    /**
     *  @brief  Read the configuration using a ConfigReader plugin.
     *  
     *  @param  str the reader descriptor (file name, database config, ...)
     *  @param  cfg the configuration object to populate
     */
    static void readConfig( const std::string &str, Configuration &cfg ) ;
    
    /**
     *  @brief  Write the configuration using a ConfigWriter plugin.
     *  
     *  @param  str the writer descriptor (file name, database config, ...)
     *  @param  cfg the configuration object to write
     */
    static void writeConfig( const std::string &str, Configuration &cfg ) ;
  };
    
}
