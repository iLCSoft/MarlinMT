
// -- marlin headers
#include <marlin/BookStoreManager.h>
#include <marlin/Application.h>
#include <marlin/Exceptions.h>

namespace marlin {

  void BookStoreManager::init( const Application *app ) {
    if( isInitialized() ) {
      MARLIN_THROW( "BookStoreManager::init: Already initialized !" ) ;
    }
    _application = app ;
    _logger = _application->createLogger( "BookStoreManager" ) ;
  }
  
  //--------------------------------------------------------------------------
  
  bool BookStoreManager::isInitialized() const {
    return ( nullptr != _application ) ;
  }
  
}
