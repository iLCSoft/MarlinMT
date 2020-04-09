// -- std headers
#include <memory>

// -- marlinmt headers
#include <marlinmt/Logging.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Utils.h>

using namespace marlinmt ;

int main() {
  // load plugins first
  auto &mgr = PluginManager::instance() ;
  mgr.logger()->setLevel<MESSAGE>() ;
  auto libraries = details::split_string<std::string>( 
    details::getenv<std::string>( "MARLINMT_DLL", "" ), 
    ":" 
  );
  mgr.loadLibraries( libraries ) ;
  mgr.dump() ;
  return 0 ;
}
