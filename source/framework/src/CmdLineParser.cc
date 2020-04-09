
// -- marlinmt headers
#include <marlinmt/CmdLineParser.h>
#include <marlinmt/MarlinMTConfig.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Logging.h>
#include <marlinmt/Utils.h>

// -- TCLAP headers
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>

namespace marlinmt {
  
  class Output : public TCLAP::StdOutput {
  	public:
  		void failure(TCLAP::CmdLineInterface& c, TCLAP::ArgException& e) override { 
        std::string progName = c.getProgramName();
      	std::cerr << "PARSE ERROR: " << e.argId() << std::endl
      		      << "             " << e.error() << std::endl << std::endl;
      	if ( c.hasHelpAndVersion() ) {
    			std::cerr << "Brief USAGE: " << std::endl;
      		_shortUsage( c, std::cerr );	              
    			std::cerr << std::endl << "For complete USAGE and HELP type: " 
    					  << std::endl << "   " << progName << " "
    					  << TCLAP::Arg::nameStartString() << "help"
    					  << std::endl << std::endl;
    		}
      	else {
      		usage(c) ;
        }
        MARLINMT_RETHROW( e, "Command line parsing failed" ) ;
  		}

  		void usage(TCLAP::CmdLineInterface& c) override {
        version(c);
        std::cout << std::endl << "USAGE: " << std::endl << std::endl; 
        _shortUsage( c, std::cout );
      	std::cout << std::endl << std::endl << "Where: " << std::endl << std::endl;
      	_longUsage( c, std::cout );
        std::cout << std::endl; 
        std::cout << "Dynamic command line options:" << std::endl;
        std::cout << std::endl ;
        std::cout << "   --{section}.parameter=value" << std::endl ;
        std::cout << "   --constant.name=value" << std::endl ;
        std::cout << "   --{processor}.parameter=value" << std::endl ;
        std::cout << std::endl ;
        spacePrint( std::cout, "with {section} = datasource, logging, scheduler, geometry, bookstore, global", 75, 5, 17 ); 
        spacePrint( std::cout, "with {processor} the name of a processor configuration section", 75, 5, 0 ); 
        std::cout << std::endl ;
        std::stringstream ss ;
        ss << "Dynamic command line options may be specified in order to overwrite individual "
        "configuration parameters, e.g:" ;
        spacePrint( std::cout, ss.str(), 75, 5, 0 ); 
        ss.str("");
        std::cout << std::endl;
        spacePrint( std::cout, c.getProgramName() + " -c config.xml \\", 75, 5, 0 ); 
        spacePrint( std::cout,      "--logging.Verbosity=ERROR \\", 75, 8, 0 ); 
        spacePrint( std::cout,      "--constant.Calibration=0.0087 \\", 75, 8, 0 ); 
        spacePrint( std::cout,      "--MyProcessor.EnergyCut=0.42", 75, 8, 0 ); 
      	std::cout << std::endl; 
  		}

  		void version(TCLAP::CmdLineInterface& c) override {
        std::string progName = c.getProgramName();
        std::string xversion = c.getVersion();
        std::cout << std::endl << progName << "  version: " 
              << xversion << std::endl << std::endl;
  		}
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  void CmdLineParser::setOptionalArgs( bool opt ) {
    _optionalArgs = opt ;
  }
  
  //--------------------------------------------------------------------------
  
  std::vector<std::string> CmdLineParser::getStandardOptions() const {
    return { "-c", "--config", 
      "-x", "--example", 
      "-j", "--nthreads",
      "--help", "--version" };
  }
  
  //--------------------------------------------------------------------------
  
  CmdLineParser::ParseResult CmdLineParser::parse( int argc, char**argv ) {
    ParseResult parseResult {} ;
    parseResult._arguments.reserve(argc) ;
    // Create command line object
    TCLAP::CmdLine cmdLine( "MarlinMT", ' ', MARLINMT_RELEASE , true ) ;
    Output output ;
    cmdLine.setOutput( &output ) ;
    // Config argument
    TCLAP::ValueArg<std::string> configArg( "c", "config", "The input configuration settings (plugin:descriptor)", (!_optionalArgs), "", "string" ) ;
    // Dump example config argument
    TCLAP::ValueArg<std::string> exampleArg( "x", "example", "Dump an example configuration (plugin:descriptor)", false, "", "string" ) ;
    cmdLine.xorAdd( configArg, exampleArg ) ;
    // number of cores
    TCLAP::ValueArg<std::string> nthreadsArg( "j", "nthreads", "The number of worker threads (only in multi-threaded mode). 'auto' will use all available cores", false, "1", "unsigned int / string" ) ;
    cmdLine.add( nthreadsArg ) ;
    // parse additional args
    std::vector<std::string> parserArgs{} ;
    parserArgs.reserve(argc) ; 
    parserArgs.push_back( argv[0] ) ; // for the program name
    for(int i=1 ; i<argc ; i++) {
      parseResult._arguments.push_back( argv[i] ) ;
    }
    auto &args = cmdLine.getArgList() ;
    // analyze the arguments.
    for( auto &arg : parseResult._arguments ) {
      if( arg.substr( 0, 2 ) == "--" ) {
        auto argName = arg.substr( 2 ) ;
        auto iter = std::find_if( args.begin(), args.end(), [&](auto val){
          return (val->getName() == argName) ;
        }) ;
        // Not an additional arg. Add it for later parsing
        if( args.end() != iter ) {
          parserArgs.push_back( arg ) ;
          continue ;
        }
        // Parse the additional argument
        auto tokens = details::split_string<std::string>( argName, "=" ) ;
        if( 2 != tokens.size() ) {
          MARLINMT_THROW( "Additional argument '" + arg + "' with wrong syntax. Expected '--name=value' format" ) ;
        }
        auto res = parseResult._additionalArgs.insert( { tokens[0], tokens[1] } ) ;
        if( not res.second ) {
          MARLINMT_THROW( "Additional argument '" + arg + "' set twice on command line" ) ;
        }
      }
      else {
        parserArgs.push_back( arg ) ;
      }
    }
    // parse the command line
    cmdLine.parse( parserArgs ) ;
    // extract command line values
    parseResult._programName = cmdLine.getProgramName() ;
    if( configArg.isSet() ) {
      parseResult._config = configArg.getValue() ;
      parseResult._dumpExample = false ;
    }
    else if( exampleArg.isSet() ) {
      parseResult._config = exampleArg.getValue() ;
      parseResult._dumpExample = true ;
    }
    parseResult._nthreads = details::nthreads( nthreadsArg.getValue() ) ;
    return parseResult ;
  }
  
}

