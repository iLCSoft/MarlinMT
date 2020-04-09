#pragma once

// -- std headers
#include <exception>
#include <string>

// -- high level exception macros
#define MARLINMT_THROW( message ) throw marlinmt::Exception( __LINE__, __PRETTY_FUNCTION__, __FILE__, message )
#define MARLINMT_THROW_T( ExceptionType, message ) throw ExceptionType( __LINE__, __PRETTY_FUNCTION__, __FILE__, message )
#define MARLINMT_RETHROW( orig, message ) throw marlinmt::Exception( orig, __LINE__, __PRETTY_FUNCTION__, __FILE__, message )
#define MARLINMT_RETHROW_T( ExceptionType, orig, message ) throw ExceptionType( orig, __LINE__, __PRETTY_FUNCTION__, __FILE__, message )

// -- specific marlinmt exception
#define MARLINMT_SKIP_EVENT( proc ) MARLINMT_THROW_T( marlinmt::SkipEventException, proc->name() )
#define MARLINMT_STOP_PROCESSING( proc ) MARLINMT_THROW_T( marlinmt::StopProcessingException, proc->name() )

// -- custom exception class definition macro
#define MARLINMT_DEFINE_EXCEPTION( ClassName ) \
  class ClassName : public marlinmt::Exception { \
  public: \
    ClassName() = delete ; \
    ClassName( const ClassName & ) = default ; \
    ~ClassName() = default ; \
    inline ClassName( const std::string &message ) : \
      marlinmt::Exception( std::string( #ClassName ) + " - " + message ) {} \
    inline ClassName( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) : \
      marlinmt::Exception( line, func, fname, std::string( #ClassName ) + " - " + message ) {} \
    template <typename T> \
    inline ClassName( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) : \
      marlinmt::Exception( rhs, line, func, fname, std::string( #ClassName ) + " - " + message ) {} \
  }

namespace marlinmt {
  
  /**
   *  @brief  Exception class.
   *
   *  Smart exception class. Tracking of:
   *  - file name
   *  - line number
   *  - function name
   *  of the exception location. The exception message can be stack 
   *  with any other previously thrown exception.
   *  Use the pre-processor macro to throw an exception:
   *  @code{cpp}
   *  MARLINMT_THROW( "A problem occured" );
   *  @endcode
   *  An exception can rethrown:
   *  @code{cpp}
   *  try {
   *    // ... some code
   *  }
   *  catch ( const marlinmt::Exception &e ) {
   *    MARLINMT_RETHROW( e, "A problem occured" );
   *  }
   *  @endcode
   *  In this case, the message will appended to the previous exception message
   *  
   */
  class Exception : public std::exception {
  public:
    Exception() = delete ;
    Exception( const Exception & ) = default ;
    virtual ~Exception() = default ;
    
    /**
     *  @brief  Constructor
     * 
     *  @param  message the exception message
     */
    Exception( const std::string &message ) ;
    
    /**
     *  @brief  Constructor
     *
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    Exception( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) ;
    
    /**
     *  @brief  Constructor
     *
     *  @param  rhs any possible class having a method what() returning a string
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    template <typename T>
    Exception( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) ;
    
    /**
     *  @brief  Get the full exception message
     */
    const char* what() const noexcept override ;

  protected:
    /**
     *  @brief  Helper function creating the full exception message
     *  
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    std::string createMessage( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const ;
    
    /**
     *  @brief  Helper function creating the full exception message
     *
     *  @param  a previous message to prepend in the message (plus a line break)
     *  @param  line the exception line number
     *  @param  func the function name in which the exception has been thrown
     *  @param  fname the file in which the exception has been thrown
     *  @param  message the exception message
     */
    std::string createMessage( const std::string &previous, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const ;
    
  protected:
    ///< The full exception message
    const std::string       _message {} ;
  };
  
  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------
  
  template <typename T>
  inline Exception::Exception( const T &rhs, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) :
    _message( createMessage(rhs.what(), line, func, fname, message) ) {
    /* nop */
  }
  
  /// Definition of Marlin exceptions
  MARLINMT_DEFINE_EXCEPTION( SkipEventException ) ;
  MARLINMT_DEFINE_EXCEPTION( StopProcessingException ) ;
  MARLINMT_DEFINE_EXCEPTION( ParseException ) ;
  MARLINMT_DEFINE_EXCEPTION( BookStoreException );

} // end namespace
