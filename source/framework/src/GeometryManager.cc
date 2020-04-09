#include "marlinmt/GeometryManager.h"

// -- marlinmt headers
#include "marlinmt/PluginManager.h"
#include "marlinmt/Application.h"

namespace marlinmt {

  GeometryManager::GeometryManager() :
    Component( "GeometryManager" ) {
    setName( "Geometry" ) ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::initialize() {
    auto &config = application().configuration() ;
    if( config.hasSection("geometry") ) {
      const auto &section = config.section("geometry") ;
      setParameters( section ) ;
    }
    auto geomType = _geometryType.get() ;
    message() << "Loading geometry plugin: " << geomType << std::endl ;
    auto &mgr = PluginManager::instance() ;
    _plugin = mgr.create<GeometryPlugin>( geomType ) ;
    if ( nullptr == _plugin ) {
      MARLINMT_THROW( "Couldn't find geometry plugin '" + geomType + "'" ) ;
    }
    if( config.hasSection("geometry") ) {
      const auto &section = config.section("geometry") ;
      _plugin->setParameters( section ) ;
    }
    _plugin->setup( &application() ) ;
    _plugin->print() ;
  }

  //--------------------------------------------------------------------------

  std::type_index GeometryManager::typeIndex() const {
    if ( nullptr == _plugin ) {
      MARLINMT_THROW( "Geometry not initialized !" ) ;
    }
    return _plugin->typeIndex() ;
  }

  //--------------------------------------------------------------------------

  void GeometryManager::clear() {
    if ( nullptr != _plugin ) {
      _plugin->destroy() ;
      _plugin = nullptr ;
    }
  }

} // namespace marlinmt
