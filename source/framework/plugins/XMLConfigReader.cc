
// -- marlinmt headers
#include <marlinmt/Configuration.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Utils.h>

// -- std headers
#include <algorithm>
#include <locale>
#include <sstream>
#include <set>
#include <memory>
#include <thread>
#include <filesystem>

// -- tinyxml headers
#include <tinyxml.h>

namespace marlinmt {
  
  /**
   *  @brief  XMLConfigReader plugin
   *  Read an XML file and populate the configuration with sections and parameters
   */
  class XMLConfigReader : public ConfigReader {
  public:
    // from ConfigReader
    void init( const std::string &desc ) override ;
    void read( Configuration &cfg ) override ;
    
  private:
    /**
     *  @brief  Parse a configuration section from the parent XML element
     *  
     *  @param  parentElement the parent XML element of the config
     *  @param  name the XML element tag to read 
     *  @param  cfg the configuration object to populate
     *  @param  addAttributes whether to add the XML element attributes in the parameters
     *  @param  throwIfNotFound whether to throw if the section if not found
     */
    void parseSection( TiXmlElement *parentElement, const std::string &name, Configuration &cfg, bool addAttributes, bool throwIfNotFound ) const ;
    
    /**
     *  @brief  Parse the <execute> section and populate the configuration object accordingly
     *  
     *  @param  parentElement the parent element of the <execute> section 
     *  @param  cfg the configuration object to populate
     */
    void parseExecuteSection( TiXmlElement *parentElement, Configuration &cfg ) const ;
    
    /**
     *  @brief  Resolve the <group> sections. 
     *  Add common group parameter to every processor in the group
     *  
     *  @param  rootElement the XML parent element (the <marlinmt> root element)
     */
    void resolveGroupSections( TiXmlElement *rootElement ) const ;
    
    /**
     *  @brief  Parse all the processor parameter sections in the parent XML element.
     *  Create a global config section 'processors' and add every processor parameters
     *  in a separate sub section. 
     * 
     *  @param  parent the parent XML element of the <processor> tags
     *  @param  cfg the configuration object to populate
     */
    void parseProcessorParameters( TiXmlElement* parent, Configuration &cfg ) const ;

    /**
     *  @brief  Helper method to get a XML element attribute as string.
     *  Throw an exception if not found
     * 
     *  @param  element the XML element
     *  @param  name the attribute name
     */
    std::string getAttribute( const TiXmlElement* element, const std::string& name ) const ;
    
    /**
     *  @brief  Parse the <constants> section. Populate the configuration object with constants.
     *  Also parse <include> elements contained in the <constants> element (only).
     *  
     *  @param  idoc the input XML document
     *  @param  cfg the configuration object to populate
     */
    void parseConstants( TiXmlDocument *idoc, Configuration &cfg ) const ;
    
    /**
     *  @brief  Parse a single <constant> element. Add a constant to the configuration object
     * 
     *  @param  element the constant XML element
     *  @param  cfg the configuration object to populate
     */
    void parseConstant( TiXmlElement *element, Configuration &cfg ) const ;
    
    /**
     *  @brief  Process all <include> XML elements recursively. 
     *  Import the XML tree of the include reference in-place in the current XML tree.
     *   
     *  @param  element the current XML element to parse
     *  @param  cfg the configuration object for constants replacement
     */
    void processIncludeElements( TiXmlElement* element, const Configuration &cfg ) const ;
    
    /**
     *  @brief  Process a single <include> XML element.
     *  Resolve the 'ref' location and load the XML document.
     *  Check for nested include XML element which not allowed for the current implementation. 
     *  
     *  @param  element the <include> XML element to process
     *  @param  cfg the configuration for constant resolution
     *  @param  document the include document object to receive
     */
    void processIncludeElement( TiXmlElement* element , const Configuration &cfg , TiXmlDocument &document ) const ;
    
    /**
     *  @brief  Check recursively for <include> XML element in the node.
     *  Throw if an include element is found.
     *   
     *  @param  node the parent element to check
     */
    void checkForNestedIncludes( const TiXmlNode *node ) const ;
    
    /**
     *  @brief  Read the <parameter> XML elements form the parent XML element.
     *  Add a new or overwrite an existing parameter in the input config section.
     *  The flag addAttributes specifies whether the XML element attributes should be 
     *  imported as parameters. For example:
     *  @code{.xml}
     *  <superHero name="Batman">
     *    ...
     *  </superHero>
     *  @endcode
     *  will add a parameter 'SuperHeroName' with the value 'Batman' to the list of parameters
     * 
     *  @param  element the parent XML element containing parameter tags
     *  @param  cfg the configuration object for constants resolution
     *  @param  section the section to populate with parameters
     *  @param  addAttributes whether to add the XML element attributes in the list of parameters
     *  @param  replacePrefix the section prefix for replacement parameters
     */
    void parametersFromXMLElement( const TiXmlElement *element, const Configuration &cfg, ConfigSection &section, bool addAttributes, const std::string &replacePrefix = "" ) const ;
    
    /**
     *  @brief  Treat the conditions for a XML node
     *   
     *  @param  current the input XML node
     *  @param  aCondition the condition string to parse
     */
    void processConditions( TiXmlNode* current, const std::string &aCondition ) const ;
    
    /**
     *  @brief  Replace the <group> tags in the execute section by the corresponding processors
     * 
     *  @param  processorsParent the XML element containing the list of groups
     *  @param  section the execute XML element section
     */
    void replaceGroups( TiXmlNode* processorsParent, TiXmlNode* section ) const ;
    
    /**
     *  @brief  Find a particular XML node by matching tag, and a specific attribute value.
     *  For example: 
     *  @code{.xml}
     *  <parameter name="Value">
     *  @endcode
     *  corresponds to:
     *  @code
     *  findElement( parent, "parameter", "name", "Value" )
     *  @endcode
     *  
     * @param  node the parent node
     * @param  tag the child XML element tag
     * @param  attribute the attribute key to match
     * @param  value the attribute value to match
     */
    TiXmlNode *findElement( TiXmlNode* node, const std::string& tag, const std::string& attribute, const std::string& value ) const ;
    
    std::optional<std::string> getReplacementParameter( const std::string &arg, const Configuration &config ) const ; 
    
  private:
    /// The input/output file name
    std::string           _fname {} ;  
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::init( const std::string &desc ) {
    _fname = desc ;
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::read( Configuration &cfg ) {
    // create and parse the XML document
    auto document = std::make_unique<TiXmlDocument>() ;
    bool loadOkay = document->LoadFile( _fname ) ;
    if( !loadOkay ) {
      std::stringstream str ;
      str  << "Parse error in file [" << _fname
          << ", row: " << document->ErrorRow() << ", col: " << document->ErrorCol() << "] : "
          << document->ErrorDesc() ;
      
      MARLINMT_THROW_T( ParseException, str.str() ) ;
    }
    auto rootElement = document->RootElement() ;
    if( nullptr == rootElement || rootElement->ValueStr() != "marlinmt" ) {
      MARLINMT_THROW_T( ParseException, "No root tag <marlinmt>...</marlinmt> found in " + _fname ) ;
    }
    // parse constants
    parseConstants( document.get(), cfg ) ;
    // resolve <include> elements
    processIncludeElements( rootElement, cfg ) ;

    // parseSection( parentElement, section, cfg, addAttributes, throwIfNotFound ) ;
    // parse geometry section
    parseSection(rootElement, "geometry", cfg, true, true) ;
    // parse bookstore section
    parseSection(rootElement, "bookstore", cfg, true, false) ;
    // parse scheduler section
    parseSection(rootElement, "scheduler", cfg, true, false) ;
    // parse data source section
    parseSection(rootElement, "datasource", cfg, true, true) ;
    // parse logging section
    parseSection(rootElement, "logging", cfg, true, false) ;
    // parse global section
    parseSection(rootElement, "global", cfg, true, false) ;
    
    // parse the execute section: groups, conditions and active processors
    parseExecuteSection( rootElement, cfg ) ;
    // resolve groups sections in the root XML element
    // simply copy all group parameters to the processors
    // and then copy the processors to the root node <marlinmt>
    // 'section' comes from above as first execute child, get the next section so we
    // do not cleanup the execute section in loop body below, only the groups
    resolveGroupSections( rootElement ) ;
    
    // parse all <processor> elements
    parseProcessorParameters( rootElement, cfg ) ;
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parseSection( TiXmlElement *parentElement, const std::string &name, Configuration &cfg, bool addAttributes, bool throwIfNotFound ) const {
    auto sectionElement = parentElement->FirstChildElement( name ) ;
    if( nullptr == sectionElement ) {
      if( throwIfNotFound ) {
        MARLINMT_THROW_T( ParseException, "Missing <" + name + "> section in " + _fname ) ;
      }
    }
    else {
      auto &section = cfg.createSection( name ) ;
      parametersFromXMLElement( sectionElement, cfg, section, addAttributes, name ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parseExecuteSection( TiXmlElement *parentElement, Configuration &cfg ) const {
    try {
      // parse execute section
      auto executeElement = parentElement->FirstChildElement("execute") ;
      if( nullptr == executeElement ) {
        MARLINMT_THROW_T( ParseException, "Missing <execute> section in " + _fname ) ;
      }
      replaceGroups( parentElement, executeElement ) ;
      processConditions( executeElement, "" ) ;
      TiXmlNode* proc = 0 ;
      std::set<std::string> processorDuplicates {} ;
      std::vector<std::string> processorConditions {}, processorNames {} ;
      // create a section and subsections for storing the scheduling conditions 
      auto &executeSection = cfg.createSection("execute") ;
      while( ( proc = executeElement->IterateChildren( "processor", proc ) )  != 0  ) {
        std::string processorName( getAttribute( proc->ToElement(), "name") ) ;
        cfg.replaceConstants( processorName ) ;
        auto inserted = processorDuplicates.insert( processorName ) ;
        if( 1 != inserted.second ) {
          MARLINMT_THROW_T( ParseException, "Processor " + processorName + " defined more than once in <execute> section" ) ;
        }
        processorNames.push_back( processorName ) ;
        std::string condition ;
        auto conditionReplace = getReplacementParameter( "execute." + processorName, cfg ) ;
        if( conditionReplace.has_value() ) {
          condition = conditionReplace.value() ;
        }
        else {
          condition = getAttribute( proc->ToElement(), "condition") ;
        }
        cfg.replaceConstants( condition ) ;
        if( condition.empty() ) {
          condition = "true" ;
        }
        processorConditions.push_back( condition ) ;
      }
      for( std::size_t i=0 ; i<processorNames.size() ; i++ ) {
        executeSection.setParameter( processorNames[i], processorConditions[i] ) ;
      }      
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't parse execute section" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::resolveGroupSections( TiXmlElement *rootElement ) const {
    TiXmlNode *section = nullptr ;
    TiXmlNode* nextSection = rootElement->IterateChildren("group", section) ;
    while((section = nextSection) != 0) {
      nextSection = rootElement->IterateChildren("group", section);
      std::vector<TiXmlNode*> groupParams ;
      TiXmlNode* param = 0 ;
      while( ( param = section->IterateChildren( "parameter" , param ) )  != 0  ) {
        groupParams.push_back( param->Clone() ) ;
      }
      TiXmlNode* proc = 0 ;
      while( ( proc = section->IterateChildren( "processor" , proc ) )  != 0  ) {
        for( std::vector<TiXmlNode*>::iterator it = groupParams.begin() ; it != groupParams.end() ; it++){
          proc->InsertEndChild( **it ) ;
        }
        std::shared_ptr<TiXmlNode> clone( proc->Clone() ) ;
        rootElement->InsertBeforeChild( section , *clone  ) ;   // FIXME: memory mngmt. ?
      }
      rootElement->RemoveChild( section ) ;
      for( std::vector<TiXmlNode*>::iterator it = groupParams.begin() ; it != groupParams.end() ; it++) {
        delete *it ;
      }
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parseProcessorParameters( TiXmlElement* parent, Configuration &cfg ) const {
    std::vector<std::string> availableProcs ;
    TiXmlNode *section = nullptr ;
    auto &parentSection = cfg.createSection( "processors" ) ;
    while( (section = parent->IterateChildren("processor",  section ) )  != 0  ) {
      // get processor name and create a new config section
      std::string name = getAttribute( section->ToElement(), "name") ;
      try {
        // create the section in the configuration
        auto &procSection = parentSection.addSection( name ) ;
        // parse parameters from XML element. Also copy attributes 
        parametersFromXMLElement( section->ToElement(), cfg, procSection, true, name ) ;
        availableProcs.push_back( name ) ;
      }
      catch( marlinmt::Exception &e ) {
        MARLINMT_RETHROW_T( ParseException, e, "Duplicated processor section: " + name ) ;
      }
    }
    if( availableProcs.empty() ) {
      MARLINMT_THROW_T( ParseException, "No <processor> section found in root section <marlinmt>" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  std::string XMLConfigReader::getAttribute( const TiXmlElement* element , const std::string& name ) const {
    const char* attr = element->Attribute( name.c_str() )  ;
    if( nullptr == attr ) {
      MARLINMT_THROW_T( ParseException, "Missing attribute '" + name + "' in element <" + element->ValueStr() + "> in file " + _fname ) ;
    }
    return attr ;
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parseConstants( TiXmlDocument *idoc, Configuration &cfg ) const {
    TiXmlElement *constantsElement = idoc->RootElement()->FirstChildElement("constants") ;
    if( nullptr == constantsElement ) {
      return ;
    }
    TiXmlElement *previous(nullptr), *child(nullptr) ;
    while(1) {
      if( nullptr == child ) {
        child = constantsElement->FirstChildElement() ;
      }
      else {
        child = child->NextSiblingElement() ;
      }
      if( nullptr == child ) {
        break ;
      }
      if( child->ValueStr() == "constant" ) {
        // process single constant
        parseConstant( child , cfg ) ;
      }
      // need to process includes in constants section since some
      // constants may be defined in includes and could then be
      // used in next constant values
      else if ( child->ValueStr() == "include" ) {
        // process the include and returns the first and last elements found in the include
        TiXmlDocument document ;
        processIncludeElement( child , cfg , document ) ;
        // add them to the xml tree
        TiXmlNode *includeAfter( child ) ;
        for( TiXmlElement *elt = document.FirstChildElement() ; elt ; elt =  elt->NextSiblingElement() ) {
          if ( elt->ValueStr() == "constant" ) {
            includeAfter = constantsElement->InsertAfterChild( includeAfter, *elt ) ;
          }
        }
        constantsElement->RemoveChild(child) ;
        child = previous ;
      }
      previous = child ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parseConstant( TiXmlElement* element, Configuration &cfg ) const {
    try {
      // parse constant name
      std::string name = getAttribute( element, "name" ) ;
      // parse constant value
      std::string value ;
      if( element->Attribute("value") ) {
        value = element->Attribute("value") ;
      }
      else {
        if( element->FirstChild() ) {
          value = element->FirstChild()->ValueStr() ;
        }
      }
      // check if we have a replacement constant
      auto replValue = getReplacementParameter( "constant." + name, cfg ) ;
      if( replValue.has_value() ) {
        // TODO missing handling of non-existing cmd line parameters
        value = replValue.value() ;
      }
      cfg.replaceConstants( value ) ;
      cfg.addConstant( name , value ) ;      
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't parse constant XML element" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::processIncludeElements( TiXmlElement* element, const Configuration &cfg ) const {
    TiXmlElement* child = element->FirstChildElement() ;
    while( 1 ) {
      if( nullptr == child ) {
        break ;
      }
      // constants already processed a step before
      if( child->ValueStr() == "constants" ) {
        child = child->NextSiblingElement() ;
        continue ;
      }
      // go recursively !
      if( child->ValueStr() != "include" ) {
        processIncludeElements( child , cfg ) ;
        child = child->NextSiblingElement() ;
        continue ;
      }
      // process the include and returns the first and last elements found in the include
      TiXmlDocument document ;
      processIncludeElement( child , cfg , document ) ;
      // add them to the xml tree
      TiXmlNode *includeAfter( child ) ;
      for( TiXmlElement *includeElement = document.FirstChildElement() ; includeElement ; includeElement =  includeElement->NextSiblingElement() ) {
        includeAfter = element->InsertAfterChild( includeAfter, *includeElement ) ;
      }
      // tricky lines :
      // 1) Remove the include element
      element->RemoveChild(child) ;
      // 2) Go to the next sibling element of the last included element to skip nested <include> elements
      child = includeAfter->NextSiblingElement() ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::processIncludeElement( TiXmlElement* element , const Configuration &cfg , TiXmlDocument &document ) const {
    try {
      std::string ref = getAttribute( element, "ref" ) ;
      cfg.replaceConstants( ref ) ;
      std::filesystem::path filepath = ref ;
      if( not filepath.has_stem() || filepath.extension() != ".xml" ) {
        MARLINMT_THROW_T( ParseException, "Invalid ref file name '" + ref + "' in element <" + element->ValueStr() + "/> in file " + _fname ) ;
      }
      // if the include ref is not absolute, then
      // it is relative to the input file name
      if( not filepath.is_absolute() ) {
        auto idocdir = std::filesystem::path(_fname).parent_path() ;
        filepath = idocdir / filepath ;
      }
      // load the include document and check for nested includes (not allowed for the time being)
      bool loadOkay = document.LoadFile( filepath.string() ) ;
      if( not loadOkay ) {      
        std::stringstream str ;
        str << "Couldn't load include document. Error in file [" << filepath
            << ", row: " << document.ErrorRow() << ", col: " << document.ErrorCol() << "] : "
            << document.ErrorDesc() ;
        MARLINMT_THROW_T( ParseException, str.str() ) ;
      }
      checkForNestedIncludes( &document ) ;
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't process include element" ) ;
    }
  }
    
  //--------------------------------------------------------------------------

  void XMLConfigReader::checkForNestedIncludes( const TiXmlNode *node ) const {
    for(const TiXmlElement *child = node->FirstChildElement() ; child ; child = child->NextSiblingElement()) {
      if( child->ValueStr() == "include" ) {
        std::stringstream ss;
        ss << "Nested includes are not allowed [in file: " << node->GetDocument()->ValueStr() << ", line: " << child->Row() << "] !" ;
        MARLINMT_THROW_T( ParseException, ss.str() ) ;
      }
      checkForNestedIncludes( child ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::parametersFromXMLElement( const TiXmlElement *element, const Configuration &cfg, ConfigSection &section, bool addAttributes, const std::string &replacePrefix ) const {
    try {
      auto elementValue = element->ValueStr() ;
      // Copy all XML element attributes to the parameter list
      // e.g <element name="toto" type="tutu"/>
      // two parameters called ElementName and ElementType will be added in the parameter list 
      if( addAttributes ) {
        elementValue[0] = std::toupper( elementValue[0], std::locale() ) ;
        for(const TiXmlAttribute *attr = element->FirstAttribute() ; attr != nullptr ; attr = attr->Next()) {
          std::string attrName = attr->Name() ;
          attrName[0] = std::toupper( attrName[0], std::locale() ) ;
          std::string attrValue = attr->ValueStr() ;
          if( not replacePrefix.empty() ) {
            attrValue = getReplacementParameter( replacePrefix + "." + attrName, cfg ).value_or( attrValue ) ;
          }
          cfg.replaceConstants( attrValue ) ;
          section.setParameter( elementValue + attrName, attrValue ) ;
        }
      }
      for(const TiXmlElement *child = element->FirstChildElement("parameter") ; child ; child = child->NextSiblingElement("parameter")) {
        auto parameterName = getAttribute( child, "name" ) ;
        std::string parameterValue ;
        // get the value from the attribute value or from the XML element content
        try {
          parameterValue = getAttribute( child->ToElement() , "value" ) ;
        }
        catch( ParseException& ) {
          if( child->FirstChild() ) {
            parameterValue = child->FirstChild()->ValueStr() ;          
          }
        }
        if( not replacePrefix.empty() ) {
          parameterValue = getReplacementParameter( replacePrefix + "." + parameterName, cfg ).value_or( parameterValue ) ;
        }
        cfg.replaceConstants( parameterValue ) ;
        section.setParameter( parameterName, parameterValue ) ;
        // TODO: treatment of in/out types ? drop it?
      }      
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't parse parameters from XML section" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::processConditions( TiXmlNode* current, const std::string &aCondition ) const {
    try {
      std::string condition ;
      // put parentheses around compound expressions
      if( aCondition.find('&') != std::string::npos  || aCondition.find('|') != std::string::npos ) {
        condition = "(" + aCondition + ")" ;
      }
      else {
        condition = aCondition ;
      }
      // Do it recursively
      TiXmlNode* child = 0 ;
      while( ( child = current->IterateChildren( "if" , child )  ) != 0 ) {
        processConditions( child->ToElement() , getAttribute( child->ToElement(), "condition") ) ;
      }
      while( ( child = current->IterateChildren( "processor" , child )  ) != 0 ) {
        if(  child->ToElement()->Attribute( "condition" ) == 0 ) {
          child->ToElement()->SetAttribute( "condition", condition ) ;
        } 
        else {
          std::string cond( child->ToElement()->Attribute("condition") ) ;
          if( cond.size() > 0 && not condition.empty() ) {
            cond += " && " ;
          }
          cond += condition ;
          child->ToElement()->SetAttribute( "condition", cond ) ;
        }
        if( current->ValueStr() != "execute" ) {
          // unless we are already in the top node (<execute/>) we have to move all processors up
          TiXmlNode* parent = current->Parent() ;
          std::shared_ptr<TiXmlNode> clone( child->Clone() ) ;
          parent->InsertBeforeChild(  current , *clone ) ;
        }
      }
      // remove the current <if/> node
      if( current->ValueStr() != "execute" ) {
        TiXmlNode* parent = current->Parent() ;
        parent->RemoveChild( current ) ;
      }      
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't process conditions in execute section" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  void XMLConfigReader::replaceGroups( TiXmlNode* processorsParent, TiXmlNode* section ) const {
    try {
      TiXmlNode* child = 0 ;
      TiXmlNode* nextChild = section->IterateChildren( child ) ;
      while((child = nextChild) != 0) {
        nextChild = section->IterateChildren(child) ;
        if( child->ValueStr()  == "group" ) {
          // find group definition in root node
          auto groupName = getAttribute( child->ToElement(), "name") ;
          TiXmlNode* group = findElement( processorsParent, "group", "name" , groupName ) ;
          if( nullptr == group ) {
            MARLINMT_THROW_T( ParseException, "Group not found : " +  groupName ) ;
          }
          TiXmlNode* sub = 0 ;
          while( ( sub = group->IterateChildren( "processor" , sub ) )  != 0  ){
            // insert <processor/> tag
            TiXmlElement item( "processor" ) ;
            item.SetAttribute( "name",  getAttribute( sub->ToElement(), "name") ) ;
            section->InsertBeforeChild( child , item ) ;
          }
          section->RemoveChild( child ) ;
        } 
        else if( child->ValueStr() == "if" ) {  // other element, e.g. <if></if>
          replaceGroups( processorsParent, child ) ;
        }
      }      
    }
    catch(Exception &e) {
      MARLINMT_RETHROW( e, "Couldn't replace groups in XML file" ) ;
    }
  }
  
  //--------------------------------------------------------------------------
  
  TiXmlNode* XMLConfigReader::findElement( TiXmlNode* node, const std::string& type, const std::string& attribute, const std::string& value ) const {
    TiXmlNode* child = 0 ;
    while( (child = node->IterateChildren( type , child ) )  != 0  ) {
      if( std::string( *child->ToElement()->Attribute( attribute ) ) == value ) {
        return child ;
      }
    }
    return nullptr ;
  }
  
  //--------------------------------------------------------------------------
  
  std::optional<std::string> XMLConfigReader::getReplacementParameter( const std::string &arg, const Configuration &config ) const {
    if( config.hasSection("CmdLine") ) {
      auto &cmdline = config.section("CmdLine") ;
      if( cmdline.hasSection("AdditionalArgs") ) {
        auto &addArgs = cmdline.section("AdditionalArgs") ;
        if( addArgs.hasParameter( arg ) ) {
          return addArgs.parameter<std::string>( arg ) ;
        }
      }
    }
    return std::nullopt ;
  }
  
  // plugin registration
  MARLINMT_DECLARE_CONFIG_READER( XMLConfigReader )

}  // namespace marlinmt
