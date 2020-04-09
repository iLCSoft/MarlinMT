// -- marlinmt headers
#include <marlinmt/Parameter.h>
#include <marlinmt/Configuration.h>

namespace marlinmt {
  
  EParameterType ParameterImpl::type() const {
    return _type ;
  }
  
  //--------------------------------------------------------------------------

  const std::string& ParameterImpl::name() const {
    return _name ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::string& ParameterImpl::description() const {
    return _description ;
  }

  //--------------------------------------------------------------------------

  bool ParameterImpl::isSet() const {
    return _isSet ;
  }
  
  //--------------------------------------------------------------------------
  
  bool ParameterImpl::hasDefault() const {
    return (nullptr != _defaultValue) ;
  }
  
  //--------------------------------------------------------------------------
  
  std::string ParameterImpl::str() const {
    return isSet() ? _strFunction( _value ) : "" ;
  }
  
  //--------------------------------------------------------------------------
  
  std::string ParameterImpl::defaultStr() const {
    return hasDefault() ? _strFunction( _defaultValue ) : "" ;
  }
  
  //--------------------------------------------------------------------------
  
  void ParameterImpl::str( const std::string &value ) {
    _fromStrFunction( _value, value ) ;
    _isSet = true ;
  }
  
  //--------------------------------------------------------------------------
  
  std::string ParameterImpl::typeStr() const {
    return _typeFunction() ;
  }
  
  //--------------------------------------------------------------------------
  
  const std::type_index &ParameterImpl::typeIndex() const {
    return _typeIndex ;
  }
  
  //--------------------------------------------------------------------------
  
  void ParameterImpl::reset() {
    _resetFunction() ;
    _isSet = false ;
  }
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  
  void Configurable::checkParameter( const std::string &name ) const {
    if( exists( name ) ) {
      MARLINMT_THROW( "Parameter '" + name +  "' already present" ) ;
    }
  }
  
  //--------------------------------------------------------------------------

  bool Configurable::exists( const std::string &name ) const {
    return _parameters.find( name ) != _parameters.end() ;
  }
  
  //--------------------------------------------------------------------------
  
  bool Configurable::isSet( const std::string &name ) const {
    auto iter = _parameters.find( name ) ;
    if( iter == _parameters.end() ) {
      return false ;
    }
    return iter->second->isSet() ;
  }
  
  //--------------------------------------------------------------------------

  void Configurable::clear() {
    _parameters.clear() ;
  }
  
  //--------------------------------------------------------------------------
  
  void Configurable::unset() {
    std::for_each( begin(), end(), []( auto &p ){ p.second->reset(); } ) ;
  }
  
  //--------------------------------------------------------------------------
  
  Configurable::iterator Configurable::begin() { 
    return _parameters.begin() ; 
  }
  
  //--------------------------------------------------------------------------
  
  Configurable::const_iterator Configurable::begin() const { 
    return _parameters.begin() ; 
  }    
  
  //--------------------------------------------------------------------------
  
  Configurable::iterator Configurable::end() { 
    return _parameters.end() ; 
  }
  
  //--------------------------------------------------------------------------
  
  Configurable::const_iterator Configurable::end() const { 
    return _parameters.end() ; 
  }
  
}