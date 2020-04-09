#include "marlinmt/PluginManager.h"

// -- std headers
#include <dlfcn.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>

// -- marlinmt headers
#include <marlinmt/Utils.h>

namespace marlinmt {

  PluginManager::PluginManager() {
    _logger = Logging::createLogger( "PluginManager" ) ;
    _logger->setLevel<MESSAGE>() ;
  }

  //--------------------------------------------------------------------------

  void PluginManager::registerPlugin( const std::string &name, FactoryFunction factoryFunction, bool ignoreDuplicate ) {
    lock_type lock( _mutex ) ;
    auto factoryIter = _pluginFactories.find( name ) ;
    if ( _pluginFactories.end() != factoryIter ) {
      if ( not ignoreDuplicate ) {
        MARLINMT_THROW( "Plugin '" + name + "' already registered" ) ;
      }
      _logger->log<DEBUG2>() << "Plugin '" << name << "' already registered. Skipping ..." << std::endl ;
    }
    else {
      FactoryData fdata {} ;
      fdata._libraryName = _currentLibrary ;
      fdata._factory = factoryFunction ;
      _pluginFactories.insert( PluginFactoryMap::value_type( name, fdata ) ) ;
      _logger->log<DEBUG5>() << "New plugin registered: '" << name << "'" <<std::endl ;
    }
  }

  //--------------------------------------------------------------------------

  std::vector<std::string> PluginManager::pluginNames() const {
    lock_type lock( _mutex ) ;
    return details::keys( _pluginFactories ) ; 
  }

  //--------------------------------------------------------------------------

  bool PluginManager::pluginRegistered( const std::string &name ) const {
    lock_type lock( _mutex ) ;
    return ( _pluginFactories.find( name ) != _pluginFactories.end() ) ;
  }

  //--------------------------------------------------------------------------

  PluginManager &PluginManager::instance() {
    static PluginManager mgr;
    return mgr;
  }
  
  //--------------------------------------------------------------------------
  
  void PluginManager::loadLibrary( const std::string &library ) {
    lock_type lock( _mutex ) ;
    doLoadLibrary( library ) ;
  }

  //--------------------------------------------------------------------------

  void PluginManager::loadLibraries( const std::vector<std::string> &libraries ) {
    lock_type lock( _mutex ) ;
    for ( const auto &library : libraries ) {
      doLoadLibrary( library ) ;
    }
  }

  //--------------------------------------------------------------------------

  void PluginManager::dump() const {
    lock_type lock( _mutex ) ;
    std::map<std::string, std::vector<std::string>> pluginMap ;
    _logger->log<MESSAGE>() << "------------------------------------" << std::endl ;
    _logger->log<MESSAGE>() << " ** MarlinMT plugin manager dump ** " << std::endl ;
    if( _pluginFactories.empty() ) {
      _logger->log<MESSAGE>() << " No plugin entry !" << std::endl ;
    }
    else {
      for ( auto iter : _pluginFactories ) {
        pluginMap[iter.second._libraryName].push_back( iter.first ) ;
      }
      for ( auto iter : pluginMap ) {
        _logger->log<MESSAGE>() << "Library: " << iter.first << std::endl ;
        for( auto p : iter.second ) {
          _logger->log<MESSAGE>() << " - " << p << std::endl ;        
        }
      }
    }
    _logger->log<MESSAGE>() << "----------------------------------" << std::endl ;
  }

  //--------------------------------------------------------------------------

  PluginManager::Logger PluginManager::logger() const {
    return _logger ;
  }
  
  //--------------------------------------------------------------------------
  
  void PluginManager::doLoadLibrary( const std::string &library ) {
    auto libraryPath = std::filesystem::absolute(library) ;
    auto libIter = _libraries.find( libraryPath ) ;
    auto libraryName = libraryPath.filename() ;
    auto libraryStr = libraryPath.string() ;
    _logger->log<MESSAGE>() << "Loading shared library : " << libraryStr << std::endl ;
    if( _libraries.end() != libIter ) {
      MARLINMT_THROW( "ERROR loading shared library '" + libraryStr + "': duplicated library" ) ;
    }
    _currentLibrary = libraryStr ;
    void* libPointer = ::dlopen( _currentLibrary.c_str() , RTLD_LAZY | RTLD_GLOBAL) ;
    _currentLibrary.clear() ;
    if( nullptr == libPointer ) {
      MARLINMT_THROW( "ERROR loading shared library '" + libraryStr + "': " + std::string(dlerror()) ) ;
    }
    _libraries.insert( {libraryStr, libPointer} ) ;
  }

} // namespace marlinmt
