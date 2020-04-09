
// -- marlin headers
#include <marlin/Configuration.h>
#include <marlin/CmdLineParser.h>
#include <marlin/PluginManager.h>
#include <marlin/Logging.h>
#include <marlin/Utils.h>

// -- std headers
#include <iostream>
#include <set>
#include <vector>
#include <string>

using namespace marlin ;

void globParameters( const ConfigSection &section, std::vector<std::string> &parameters, const std::set<std::string> &skip ) ;

/**
 *  Completion helper program. For internal use only
 *  Parse a Marlin steering file and printout of all possible 
 *  options that can be modified in the steering file by using 
 *  command line. The output is used by the bash completion program
 *  for the Marlin/MarlinMT program
 */
int main(int argc, char** argv ) {

  try {
    // shut down cout and cerr
    std::cout.setstate( std::ios_base::failbit ) ;
    std::cerr.setstate( std::ios_base::failbit ) ;
    
    // load plugins first
    auto &mgr = PluginManager::instance() ;
    mgr.logger()->setLevel<SILENT>() ;
    auto libraries = details::split_string<std::string>(
      details::getenv<std::string>( "MARLIN_DLL", "" ), ":" 
    );
    mgr.loadLibraries( libraries ) ;
        
    // parse the command line in relax mode
    CmdLineParser parser ;
    parser.setOptionalArgs( true ) ;
    CmdLineParser::ParseResult parseResult {} ;
        
    // 1 - Get command line options
    auto cmdLineOpts = parser.getStandardOptions() ;

    try {
      parseResult = parser.parse( argc, argv ) ;      
    }
    catch(...) {
      std::cout.clear() ;
      std::cerr.clear() ;
      for( auto &opt : cmdLineOpts ) {
        std::cout << opt << std::endl ;
      }
      return 0 ;      
    }

    std::cout.clear() ;
    std::cerr.clear() ;
    
    // no config file on the command line yet
    // print standard arguments and exit
    if( not parseResult._config.has_value() or parseResult._dumpExample) {
      for( auto &opt : cmdLineOpts ) {
        std::cout << opt << std::endl ;
      }
      return 0 ;
    }
    // try to parse the configuration
    Configuration configuration {} ;
    ConfigHelper::readConfig( parseResult._config.value(), configuration ) ;

    // 2 - Get all section parameters
    std::vector<std::string> parameters ;
    auto sections = configuration.sections() ;
    for( auto sec : sections ) {
      auto &section = configuration.section( sec ) ;
      if( section.name() == "processors" ) {
        auto procSections = section.subsectionNames() ;
        for( auto proc : procSections ) {
          auto &procSection = section.section( proc ) ;
          globParameters( procSection, parameters, {"ProcessorName", "ProcessorType"} ) ;
        }
      }
      else if( section.name() == "geometry" ) {
        globParameters( section, parameters, {"GeometryType"} ) ;
      }
      else if( section.name() == "datasource" ) {
        globParameters( section, parameters, {"DatasourceType"} ) ;
      }
      else {
        globParameters( section, parameters, {} ) ;
      }
    }
    
    // 3 - Get all constants
    auto constants = details::keys( configuration.constants() ) ;
    
    // 4 - Combine and print
    std::vector<std::string> allOptions ;
    allOptions.insert( allOptions.end(), cmdLineOpts.begin(), cmdLineOpts.end() ) ;
    allOptions.insert( allOptions.end(), parameters.begin(), parameters.end() ) ;
    allOptions.insert( allOptions.end(), constants.begin(), constants.end() ) ;

    for( auto &opt : allOptions ) {
      std::cout << opt << std::endl ;
    }
  }
  catch(std::exception &e) {
    return 1 ;
  }  
}

void globParameters( const ConfigSection &section, std::vector<std::string> &parameters, const std::set<std::string> &skip ) {
  auto names = section.parameterNames() ;
  for( auto name : names ) {
    if( skip.end() != skip.find( name ) ) {
      continue ;
    }
    parameters.push_back( "--" + section.name() + "." + name + "=" ) ;
    // TODO keep this ?
    parameters.push_back( "--" + section.name() + "." + name + "=" + section.parameter<std::string>( name ) ) ;
  }
}
