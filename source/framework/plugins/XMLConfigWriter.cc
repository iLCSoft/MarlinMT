
// -- marlinmt headers
#include <marlinmt/Configuration.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Utils.h>

// -- std headers
#include <locale>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <ctime>

// -- tinyxml headers
#include <tinyxml.h>

namespace marlinmt {
  
  /**
   *  @brief  XMLConfigWriter plugin
   *  Write an XML file from the configuration sections and parameters
   */
  class XMLConfigWriter : public ConfigWriter {
  public:
    using StringMap = std::vector<std::pair<std::string, std::string>> ;
    
    // from ConfigWriter
    void init( const std::string &desc ) override ;
    void write( const Configuration &cfg ) override ;
    
    /**
     *  @brief  Create an XML element containing all parameters from a section.
     *  The list of attributes in third argument is a mapping of parameters to write as
     *  XML element attribute and not as parameter.
     *  
     *  @param  name the XML element name
     *  @param  section the config section to dump
     *  @param  attrMapping a list of parameters to add as element attribute, not in parameter list
     */
    TiXmlElement createParameterSection( const std::string &name, const ConfigSection &section, const StringMap &attrMapping = {} ) ;

    /**
     *  @brief  Append the config section description in the element tree as comment
     *  
     *  @param  element the parent element of the XML comment
     *  @param  section the config section to get the description from
     */
    void appendDescription( TiXmlElement &element, const ConfigSection &section ) const ;
    
    // factory helper functions
    TiXmlElement createElement( const std::string &name, const StringMap &attrs = {} ) const ;
    TiXmlComment createComment( const std::string &comment ) const ;    
    TiXmlText    createText( const std::string &text ) const ;
    TiXmlElement createParameter( const std::string &name, const std::string &value ) const ;
    
  private:
    /// The output file name
    std::string           _fname {} ;  
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  void XMLConfigWriter::init( const std::string &desc ) {
    _fname = desc ;
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigWriter::write( const Configuration &cfg ) {
    auto root = createElement( "marlinmt" ) ;
    // write all standard sections (only)
    auto procsConds = cfg.section("execute").parameters() ;
    auto executeElement = createElement( "execute" ) ;
    for( auto &pc : procsConds ) {
      if( pc.second == "true" ) {
        executeElement.InsertEndChild( createElement( "processor", {{"name", pc.first}} ) ) ;
      }
      else {
        executeElement.InsertEndChild( createElement( "processor", {{"name", pc.first}, {"condition", pc.second}} ) ) ;        
      }
    }
    root.InsertEndChild( executeElement ) ;
    if( cfg.hasSection( "global" ) ) {
      appendDescription( root, cfg.section("global") ) ;
      root.InsertEndChild( createParameterSection( "global", cfg.section("global") ) ) ;
    }
    if( cfg.hasSection( "logging" ) ) {
      appendDescription( root, cfg.section("logging") ) ;
      root.InsertEndChild( createParameterSection( "logging", cfg.section("logging") ) ) ;
    }
    if( cfg.hasSection( "scheduler" ) ) {
      appendDescription( root, cfg.section("scheduler") ) ;
      root.InsertEndChild( createParameterSection( "scheduler", cfg.section("scheduler") ) ) ;
    }
    if( cfg.hasSection( "datasource" ) ) {
      appendDescription( root, cfg.section("datasource") ) ;
      root.InsertEndChild( createParameterSection( "datasource", cfg.section("datasource"), {
        {"DatasourceType", "type"}
      } ) ) ;
    }
    if( cfg.hasSection( "geometry" ) ) {
      appendDescription( root, cfg.section("geometry") ) ;
      root.InsertEndChild( createParameterSection( "geometry", cfg.section("geometry"), {
        {"GeometryType", "type"}
      } ) ) ;
    }
    if( cfg.hasSection( "bookstore" ) ) {
      appendDescription( root, cfg.section("bookstore") ) ;
      root.InsertEndChild( createParameterSection( "bookstore", cfg.section("bookstore") ) ) ;
    }
    auto &procs = cfg.section( "processors" ) ;
    auto processorNames = procs.subsectionNames() ;
    for( auto &proc : processorNames ) {
      auto &procSection = procs.section( proc ) ;
      appendDescription( root, procSection ) ;
      root.InsertEndChild( createParameterSection( "processor", procSection, {
        {"ProcessorName", "name"},
        {"ProcessorType", "type"},
        {"ProcessorCritical", "critical"},
        {"ProcessorClone", "clone"}
      } ) ) ;
    }
    
    // write out document
    TiXmlDocument document ;
    // XML declaration
    TiXmlDeclaration declaration( "1.0", "utf8", "" ) ;
    document.InsertEndChild( declaration ) ;
    // A bit of info is nice
    auto t = std::time(nullptr) ;
    auto tm = *std::localtime(&t) ;
    std::string indent = "  " ;
    std::stringstream dateStr ; dateStr << std::put_time(&tm, "Date: %d-%m-%Y %T") ;
    std::stringstream header ;
    header << "\n" <<
    indent << "This an example steering file generated by MarlinMT\n" <<
    indent << dateStr.str() << "\n" <<
    indent << "MarlinMT version: " << MARLINMT_RELEASE << "\n" ;
    std::cout << cfg << std::endl ;
    if( cfg.hasSection( "CmdLine" ) ) {
      auto &cmdline = cfg.section( "CmdLine" ) ;
      header << indent << "Full command line:   " << cmdline.parameter<std::string>("Arguments") << " \n" ;
    }
    document.InsertEndChild( createComment( header.str() ) ) ;
    // Add root element and write
    document.InsertEndChild( root ) ;
    document.SaveFile( _fname ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigWriter::appendDescription( TiXmlElement &element, const ConfigSection &section ) const {
    auto iter = section.metadata().find( "description" ) ;
    if( section.metadata().end() != iter ) {
      element.InsertEndChild( createComment( iter->second ) ) ;        
    }
  }
  
  //--------------------------------------------------------------------------
  
  TiXmlElement XMLConfigWriter::createParameterSection( const std::string &name, const ConfigSection &section, const StringMap &attrMapping ) {
    StringMap attributes {} ;
    auto excludeParameters = details::keys( attrMapping ) ;
    for( auto &attr : attrMapping ) {
      attributes.push_back( { attr.second, section.parameter<std::string>( attr.first ) } ) ;
    }
    auto element = createElement( name, attributes ) ;
    auto parameters = section.parameters() ;
    for( auto &p : parameters ) {
      if( excludeParameters.end() != std::find( excludeParameters.begin(), excludeParameters.end(), p.first ) ) {
        continue ;
      }
      const auto &metadata = section.metadata(  ) ;
      auto desciter = metadata.find( p.first + ".description" ) ;
      auto optiter = metadata.find( p.first + ".optional" ) ;
      if( (metadata.end() != desciter) && (metadata.end() != optiter) ) {
        std::stringstream comment ;
        if( optiter->second == "true" ) {
          comment << "[Optional] " ;  
        }
        comment << desciter->second ;
        element.InsertEndChild( createComment( comment.str() ) ) ;
      }
      element.InsertEndChild( createParameter( p.first, p.second ) ) ;
    }
    return element ;
  }
  
  //--------------------------------------------------------------------------
  
  TiXmlElement XMLConfigWriter::createElement( const std::string &name, const StringMap &attrs ) const {
    TiXmlElement element( name ) ;
    for( auto &attr : attrs ) {
      element.SetAttribute( attr.first, attr.second ) ;
    }
    return element ;
  }
  
  //--------------------------------------------------------------------------

  TiXmlComment XMLConfigWriter::createComment( const std::string &comment ) const {
    return TiXmlComment( comment.c_str() ) ;
  }
  
  //--------------------------------------------------------------------------

  TiXmlText XMLConfigWriter::createText( const std::string &text ) const {
    return TiXmlText( text ) ;
  }
  
  //--------------------------------------------------------------------------
  
  TiXmlElement XMLConfigWriter::createParameter( const std::string &name, const std::string &value ) const {
    auto parameter = createElement( "parameter", { {"name", name} } ) ;
    parameter.InsertEndChild( createText( value ) ) ;
    return parameter ;
  }
  
  // plugin registration
  MARLINMT_DECLARE_CONFIG_WRITER( XMLConfigWriter )

}  // namespace marlinmt
