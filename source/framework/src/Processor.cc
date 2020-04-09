
// -- marlinmt headers
#include <marlinmt/Processor.h>
#include <marlinmt/Application.h>

namespace marlinmt {

  Processor::Processor( const std::string& typeName ) :
    Component( typeName ) {
    /* nop */
  }
    
  //--------------------------------------------------------------------------
  
  void Processor::initialize() {
    auto &config = application().configuration() ;
    auto &section = config.section("processors") ;
    if( section.hasSection( name() ) ) {
      setParameters( section.section( name() ) ) ;
    }
    init() ;
  }
  
  //--------------------------------------------------------------------------

  std::optional<bool> Processor::runtimeOption( ERuntimeOption option ) const {
    std::optional<bool> result {} ;
    auto iter = _forcedRuntimeOptions.find( option ) ;
    if( _forcedRuntimeOptions.end() != iter ) {
      result = iter->second ;
    }
    return result ;
  }

  //--------------------------------------------------------------------------

  void Processor::setRuntimeOption( ERuntimeOption option, bool value ) {
    _forcedRuntimeOptions[option] = value ;
  }

} // namespace marlinmt
