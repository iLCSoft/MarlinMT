#include <marlinmt/DataSourcePlugin.h>

// -- marlinmt headers
#include <marlinmt/Application.h>
#include <marlinmt/EventStore.h>
#include <marlinmt/RunHeader.h>

namespace marlinmt {

  DataSourcePlugin::DataSourcePlugin( const std::string &dstype ) :
    Component(dstype) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::readAll() {
    while( readOne() ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onEventRead( EventFunction func ) {
    _onEventRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::onRunHeaderRead( RunHeaderFunction func ) {
    _onRunHeaderRead = func ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processRunHeader( std::shared_ptr<RunHeader> rhdr ) {
    if( nullptr == _onRunHeaderRead ) {
      throw Exception( "DataSourcePlugin::processRunHeader: no callback function available" ) ;
    }
    _onRunHeaderRead( rhdr ) ;
  }

  //--------------------------------------------------------------------------

  void DataSourcePlugin::processEvent( std::shared_ptr<EventStore> event ) {
    if( nullptr == _onEventRead ) {
      throw Exception( "DataSourcePlugin::processEvent: no callback function available" ) ;
    }
    _onEventRead( event ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void DataSourcePlugin::initialize() {
    auto &config = application().configuration() ;
    if( config.hasSection("datasource") ) {
      setParameters( config.section("datasource") ) ;
    }
  }

}
