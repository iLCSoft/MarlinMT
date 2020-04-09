#ifndef MARLINMT_PLUGINMANAGER_h
#define MARLINMT_PLUGINMANAGER_h 1

// -- std headers
#include <map>
#include <string>
#include <mutex>
#include <any>
#include <memory>
#include <vector>
#include <functional>
#include <filesystem>

// -- marlinmt headers
#include "marlinmt/Exceptions.h"
#include "marlinmt/Logging.h"

// Helper macros to declare plugins
// R. Ete: Impossible to make a function outside of main.
// The workaround is to create a structure on-the-fly that make the function call in the constructor
#define MARLINMT_DECLARE_PLUGIN( Base, Class ) MARLINMT_DECLARE_PLUGIN_NAME( Base, Class, #Class )

#define MARLINMT_DECLARE_PLUGIN_NAME( Base, Class, NameStr ) \
  namespace marlinmt_plugins { \
    struct PluginDeclaration_##Class { \
      PluginDeclaration_##Class() { \
        marlinmt::PluginManager::instance().registerPlugin<Base, Class>( NameStr, false ) ; \
      } \
    }; \
    static PluginDeclaration_##Class __instance_##Class ; \
  }

// processor plugin declaration
#define MARLINMT_DECLARE_PROCESSOR( Class ) namespace { \
  static const auto __processortype__ = Class().type() ; \
  MARLINMT_DECLARE_PLUGIN_NAME( marlinmt::Processor, Class, __processortype__ ) \
}

// geometry plugin declaration
#define MARLINMT_DECLARE_GEOMETRY( Class ) MARLINMT_DECLARE_PLUGIN( marlinmt::GeometryPlugin, Class )
#define MARLINMT_DECLARE_GEOMETRY_NAME( Class, NameStr ) MARLINMT_DECLARE_PLUGIN_NAME( marlinmt::GeometryPlugin, Class, NameStr )

// data source plugin declaration
#define MARLINMT_DECLARE_DATASOURCE( Class ) MARLINMT_DECLARE_PLUGIN( marlinmt::DataSourcePlugin, Class )
#define MARLINMT_DECLARE_DATASOURCE_NAME( Class, NameStr ) MARLINMT_DECLARE_PLUGIN_NAME( marlinmt::DataSourcePlugin, Class, NameStr )

// config reader plugin declaration
#define MARLINMT_DECLARE_CONFIG_READER( Class ) MARLINMT_DECLARE_PLUGIN( marlinmt::ConfigReader, Class )
#define MARLINMT_DECLARE_CONFIG_READER_NAME( Class, NameStr ) MARLINMT_DECLARE_PLUGIN_NAME( marlinmt::ConfigReader, Class, NameStr )

// config writer plugin declaration
#define MARLINMT_DECLARE_CONFIG_WRITER( Class ) MARLINMT_DECLARE_PLUGIN( marlinmt::ConfigWriter, Class )
#define MARLINMT_DECLARE_CONFIG_WRITER_NAME( Class, NameStr ) MARLINMT_DECLARE_PLUGIN_NAME( marlinmt::ConfigWriter, Class, NameStr )

namespace marlinmt {

  /**
   *  @brief  PluginManager singleton class
   *  Responsible for loading shared libraries and collecting
   *  processor factory instances. Processor instances can be
   *  created from factories using the PluginManager::create()
   *  method on query.
   */
  class PluginManager {
  public:
    // typedefs
    using FactoryFunction = std::any ;
    template <typename B>
    using FactoryFunctionT = std::function<std::unique_ptr<B>()> ;
    // typedef void*                                       PluginPtr ;
    // typedef std::function<PluginPtr()>                  FactoryFunction ;
    
    struct FactoryData {
      /// The name of the library of the plugin 
      std::string           _libraryName {} ;
      /// The plugin factory
      FactoryFunction       _factory {} ;
    };
    
    typedef std::map<std::string, FactoryData>          PluginFactoryMap ;
    typedef std::map<std::filesystem::path, void*>      LibraryList ;
    typedef Logging::Logger                             Logger ;
    typedef std::recursive_mutex                        mutex_type ;
    typedef std::lock_guard<mutex_type>                 lock_type ;

  private:
    PluginManager(const PluginManager &) = delete ;
    PluginManager& operator=(const PluginManager &) = delete ;
    ~PluginManager() = default ;

    /**
     *  @brief  Constructor
     */
    PluginManager() ;

  public:
    /**
     *  @brief  Get the plugin manager instance
     */
    static PluginManager &instance() ;

    /**
     *  @brief  Register a new plugin to the manager.
     *  A new factory function creating an object of type T
     *  is inserted into the registry. The type T must be
     *  default constructible. If you want to provide a custom
     *  factory function, use the corresponding overloaded function.
     *  If the flag ignoreDuplicate is set to true, no exception is
     *  thrown in case a duplicate is found in the registry. In this
     *  case, the registry is not modified.
     *
     *  @param  name the plugin name
     *  @param  ignoreDuplicate whether to avoid exception throw in case of duplicate entry
     */
    template <typename B, typename T>
    void registerPlugin( const std::string &name, bool ignoreDuplicate = false ) ;

    /**
     *  @brief  Register a new plugin to the manager.
     *  See overloaded function description for more details
     *
     *  @param  name the plugin name
     *  @param  factoryFunction the factory function responsible for the plugin creation
     *  @param  ignoreDuplicate whether to avoid exception throw in case of duplicate entry
     */
    void registerPlugin( const std::string &name, FactoryFunction factoryFunction, bool ignoreDuplicate = false ) ;

    /**
     *  @brief  Load a shared library to populate the list of plugins
     *
     *  @param  library the library to load
     */
    void loadLibrary( const std::string &library ) ;
    
    /**
     *  @brief  Load shared libraries to populate the list of plugins
     *
     *  @param  libraries the list of libraries to load
     */
    void loadLibraries( const std::vector<std::string> &libraries ) ;

    /**
     *  @brief  Get all registered plugin name
     */
    std::vector<std::string> pluginNames() const ;
    
    /**
     *  @brief  Get all registered plugin name with the Base type
     */
    template <typename Base>
    std::vector<std::string> pluginNames() const ;

    /**
     *  @brief  Whether the plugin with of a given name is registered
     *
     *  @param  name the plugin name to check
     */
    bool pluginRegistered( const std::string &name ) const ;
    
    /**
    *  @brief  Create a new plugin instance.
    *  A factory function must have been registered before hand.
    *  The template parameter T is the final plugin type requested
    *  by the caller.
     *
     *  @param  name the plugin name
     */
    template <typename T>
    std::unique_ptr<T> create( const std::string &name ) const ;

    /**
     *  @brief  Dump plugin manager content in console
     */
    void dump() const ;

    /**
     *  @brief  Get the plugin manager logger
     */
    Logger logger() const ;
    
  private:
    /// the workhorse !
    void doLoadLibrary( const std::string &library ) ;

  private:
    /// The map of plugin factories
    PluginFactoryMap           _pluginFactories {} ;
    /// The list of loaded libraries
    LibraryList                _libraries {} ;
    /// The plugin manager logger
    mutable Logger             _logger {nullptr} ;
    /// The synchronization mutex
    mutable mutex_type         _mutex {} ;
    /// The current library being loaded
    std::string                _currentLibrary {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename B, typename T>
  inline void PluginManager::registerPlugin( const std::string &name, bool ignoreDuplicate ) {
    FactoryFunctionT<B> factoryFunction = []() -> std::unique_ptr<B> {
      return std::make_unique<T>() ;
    };
    std::any anyFactory = factoryFunction ;
    registerPlugin( name, anyFactory, ignoreDuplicate ) ;
  }

  //--------------------------------------------------------------------------

  template <typename Base>
  inline std::vector<std::string> PluginManager::pluginNames() const {
    std::vector<std::string> names {} ;
    for( auto &p : _pluginFactories ) {
      try {
        auto factory = std::any_cast<FactoryFunctionT<Base>>( p.second._factory ) ;
        names.push_back( p.first ) ;
      }
      catch(const std::bad_any_cast& e) {
        continue ;
      }
    }
    return names ;
  }
  
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline std::unique_ptr<T> PluginManager::create( const std::string &name ) const {
    lock_type lock( _mutex ) ;
    auto factoryIter = _pluginFactories.find( name ) ;
    // plugin not found ?
    if ( _pluginFactories.end() == factoryIter ) {
      return nullptr ;
    }
    try {
      auto factory = std::any_cast<FactoryFunctionT<T>>( factoryIter->second._factory ) ;
      return factory() ;
    }
    catch(const std::bad_any_cast& e) {
      return nullptr ;
    }
  }

} // end namespace marlinmt

#endif
