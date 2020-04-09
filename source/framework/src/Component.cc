#include <marlinmt/Component.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Application.h>
#include <marlinmt/Utils.h>

namespace marlinmt {
  
  Component::Component( const std::string &type ) :
    _type(type),
    _name(details::convert<void*>::to_string(this)) {
    _logger = Logging::createLogger( this->type() + "_" + this->name() ) ;
    _logger->setLevel( "MESSAGE" ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::type() const {
    return _type ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::name() const {
    return _name ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setName( const std::string &n ) {
    _name = n ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::description() const {
    return _description ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setDescription( const std::string &desc ) {
    _description = desc ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::debug() const {
    return log<loglevel::DEBUG>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::message() const {
    return log<loglevel::MESSAGE>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::warning() const {
    return log<loglevel::WARNING>() ;
  }
  
  //--------------------------------------------------------------------------
  
  Logging::StreamType Component::error() const {
    return log<loglevel::ERROR>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setVerbosity( const std::string &level ) {
    _logger->setLevel( level ) ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string &Component::verbosity() const {
    return _logger->levelName() ;
  }
  
  //--------------------------------------------------------------------------
  
  const Application &Component::application() const {
    if( nullptr == _application ) {
      MARLINMT_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  //--------------------------------------------------------------------------
  
  Application &Component::application() {
    if( nullptr == _application ) {
      MARLINMT_THROW( "Application not set" ) ;
    }
    return *_application ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setup( Application *app ) {
    _application = app ;
    _logger = application().createLogger( _name ) ;
    if( _verbosity.isSet() ) {
      _logger->setLevel( _verbosity.get() ) ;
    }
    initialize() ;
  }
  
  //--------------------------------------------------------------------------
  
  bool Component::isInitialized() const {
    return ( nullptr == _application ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::printParameters() const {
    printParameters<MESSAGE>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Component::setParameters( const ConfigSection &section, bool throwIfNotFound ) {
    auto names = section.parameterNames() ;
    for( auto n : names ) {
      auto iter = _parameters.find( n ) ;
      if( _parameters.end() != iter ) {
        iter->second->str( section.parameter<std::string>( n ) ) ;
      }
      else if( throwIfNotFound ) {
        MARLINMT_THROW( "Input parameter '" + n + "' from section '" + section.name() + "' can't be set (not found)" ) ;
      }
    }
  }
  
  //--------------------------------------------------------------------------
  
  void Component::getParameters( ConfigSection &section, const std::set<std::string> &exclude ) const {
    auto &metadata = section.metadata() ;
    metadata["description"] = description() ;
    metadata["name"] = name() ;
    metadata["type"] = type() ;
    for( auto iter : _parameters ) {
      if( exclude.end() != exclude.find( iter.first ) ) {
        continue ;
      }
      auto value = iter.second->hasDefault() ? iter.second->defaultStr() : "" ;
      section.setParameter( iter.first, value ) ;
      metadata[ iter.first + ".description" ] = iter.second->description() ;
      metadata[ iter.first + ".optional" ] = iter.second->hasDefault() ? "true" : "false" ;
    }
  }
  
}