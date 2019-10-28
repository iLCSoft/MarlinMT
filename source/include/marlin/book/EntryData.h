#pragma once

#include <string>
#include <memory>
#include <typeindex>

#include "marlin/book/Flags.h"

namespace marlin::book {
  class MemLayout;

struct EntryKey {
<<<<<<< HEAD
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
||||||| merged common ancestors
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
=======
    // TODO: clean solution
    EntryKey(const std::type_index& t)
    : type{t}{}

    std::string path{""}, name{""};
    std::size_t amt{0};
    std::type_index type;
    Flag_t flags{0};
    std::size_t hash{0};
  };
  struct Context {
    Context(const std::shared_ptr<MemLayout>& memLayout) 
      : mem{memLayout}
    {}
    std::shared_ptr<MemLayout> mem;
  };
>>>>>>> Added Conditions and selections
class EntryBase {};


}
