#pragma once

# include "marlin/book/configs/Base.h"

// -- std includes
#include <exception>
#include <string>
#include <string_view>

#define MARLIN_BOOK_THROW( message ) \
  throw marlin::book::exceptions::BookStoreException( \
      __LINE__, __FUNCTION__, __FILE__, message );

namespace marlin {
  namespace book {
    namespace exceptions {
      class BookStoreException : public std::exception {
      public:
        BookStoreException() = delete;
        BookStoreException(const BookStoreException&) = default;

        BookStoreException(unsigned int line, const char * func, const char * fname,
            const std::string_view& message) ;

        const char* what() const noexcept override ;
      private:
        const std::string _message; 
      };


      inline BookStoreException::BookStoreException(unsigned int line,
          const char * func,
          const char * fname,
          const std::string_view& message) 
      : _message{
        std::string(fname) 
          + " (l." + std::to_string(line) + ") in " 
          + func + ": " + std::string(message)
      } {}

      inline const char* BookStoreException::what() const noexcept {
        return _message.c_str();
      }
    } // end namespace exceptions
  } // end namespace book
} // end namespace marlin


