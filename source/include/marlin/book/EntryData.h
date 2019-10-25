#pragma once

#include <string>
#include <memory>

#include "marlin/book/Flags.h"

namespace marlin::book {
  class MemLayout;

struct EntryKey {
    std::string path, name;
    std::size_t amt;
    Flag_t flags;
    std::size_t hash;
  };
  struct Context {
    Context(const std::shared_ptr<MemLayout>& memLayout) 
      : mem{memLayout}
    {}
    std::shared_ptr<MemLayout> mem;
  };
class EntryBase {};


}
