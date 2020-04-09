#include <marlinmt/Exceptions.h>

namespace marlinmt {
  
  Exception::Exception( const std::string &message ) : 
    _message(message) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  Exception::Exception( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) :
    _message(createMessage(line, func, fname, message)) {
    /* nop */
  }
  
  //--------------------------------------------------------------------------
  
  const char* Exception::what() const noexcept {
    return _message.c_str() ;
  }
  
  //--------------------------------------------------------------------------
  
  std::string Exception::createMessage( unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const {
    return "in file: " + fname + " (l." + std::to_string(line) + ")\nin function: " + func + "\nmessage: " + message ;
  }
  
  //--------------------------------------------------------------------------
  
  std::string Exception::createMessage( const std::string &previous, unsigned int line, const std::string &func, const std::string &fname, const std::string &message ) const {
    if( previous.empty() ) {
      return createMessage(line, func, fname, message) ;
    }
    else {
      return previous + "\n" + createMessage(line, func, fname, message) ;
    }
  }

}
