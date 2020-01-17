#pragma once

// -- marlin headers
#include <marlin/MarlinBookConfig.h>
#include <marlin/Logging.h>

namespace marlin {
  
  // forward declaration
  class Application ;
  class Processor ;

  /**
   *  @brief  BookStoreManager class
   */
  class BookStoreManager {
  public:
    using Logger = Logging::Logger ;
    
  public:
    BookStoreManager() = default ;
    BookStoreManager( const BookStoreManager & ) = delete ;
    BookStoreManager &operator=( const BookStoreManager & ) = delete ;
    BookStoreManager( BookStoreManager && ) = delete ;
    BookStoreManager &operator=( BookStoreManager && ) = delete ;
    ~BookStoreManager() = default ;
    
    /// Whether the book store has been initialized
    [[nodiscard]] bool isInitialized() const ;
    
    /// Initialize the book store manager
    void init( const Application *app ) ;
    
    /**
     *  @brief  Book  a histogram 1D, float type
     *  
     *  @param  proc       the processor booking the histogram
     *  @param  path       the histogram entry path
     *  @param  name       the histogram name
     *  @param  title      the histogram title
     *  @param  axisconfig the histogram X axis config
     *  @param  flags      the book flag policy
     */
    [[nodiscard]] H1FEntry bookHist1F (
      const Processor *proc, 
      const std::filesystem::path &path, 
      const std::string_view &name,
      const std::string_view &title,
      const AxisConfigF &axisconfig,
      const BookFlag &flags ) ;
    
  private:
    /// The application in which the geometry manager has been initialized
    const Application                   *_application {nullptr} ;
    /// The book store
    book::BookStore                      _bookStore {} ;
    /// The logger instance
    Logger                               _logger {nullptr} ;
  };

  
}