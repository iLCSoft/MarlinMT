// -- std headers
#include <memory>

// -- marlin headers
#include <marlin/Logging.h>
#include <marlin/PluginManager.h>
#include <marlin/Exceptions.h>
#include <marlin/Utils.h>

using namespace marlin ;

int main() {
  // load plugins first
  auto &mgr = PluginManager::instance() ;
  mgr.logger()->setLevel<MESSAGE>() ;
  auto libraries = details::split_string<std::string>( 
    details::getenv<std::string>( "MARLIN_DLL", "" ), 
    ":" 
  );
  mgr.loadLibraries( libraries ) ;
  mgr.dump() ;
  return 0 ;
}
