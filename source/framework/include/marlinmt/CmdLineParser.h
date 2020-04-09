#pragma once

// -- std headers
#include <map>
#include <string>
#include <vector>

namespace marlinmtmt {

  /**
   *  @brief  CmdLineParser class
   *  Does the main command line parsing for MarlinMT
   */
  class CmdLineParser {
  public:
    using AdditionalArgs = std::map<std::string, std::string> ;
    
    /// ParseResult struct. The result of the command line parsing. 
    struct ParseResult {
      /// The program name
      std::string                             _programName {} ;
      /// All command line arguments except the program name (argv[0])
      std::vector<std::string>                _arguments {} ;
      /// Additional command line arguments
      AdditionalArgs                          _additionalArgs {} ;
      /// The config argument
      std::optional<std::string>              _config {} ;
      /// The number of threads
      unsigned int                            _nthreads {1} ;
      /// Whether to dump an example configuration and exit
      bool                                    _dumpExample {false} ;
    };
    
  public:
    /// Default constructor
    CmdLineParser() = default ;
    /// Default destructor
    ~CmdLineParser() = default ;
    
    /**
     *  @brief  Set all cmd line arguments to optional during parsing
     * 
     *  @param  opt input flag
     */
    void setOptionalArgs( bool opt = true ) ;
    
    /**
     *  @brief  Get all possible command options (short and long options)
     */
    std::vector<std::string> getStandardOptions() const ;
    
    /**
     *  @brief  Parse the command line
     * 
     *  @param  argc the number of arguments
     *  @param  argv the full argument list
     */
    [[nodiscard]] ParseResult parse( int argc, char**argv ) ;
    
  private:
    /// Whether all arguments are optional
    bool                    _optionalArgs {false} ;
  };
  
}