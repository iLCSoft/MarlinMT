#include <marlinmt/PluginManager.h>

// Just for testing compilation and linking against MarlinMT libraries ...

int main() {
  auto &mgr = marlinmt::PluginManager::instance() ;
  mgr.dump() ;
  return 0;
}
