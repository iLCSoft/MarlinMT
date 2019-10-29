#pragma once

#include <variant>
#include <memory>
#include <iostream>
#include <typeinfo>

#include "marlin/book/EntryData.h"

namespace marlin::book {
  class BookStore;
  class MemLayout;
  template<typename T>
  class Handle;

  
  template<typename T>
  struct trait {
  static void Merge(const std::shared_ptr<T>&, const std::shared_ptr<T>&) { std::cout <<  typeid(T).name() << " hallo:(\n";}

  };
  template<typename T>
  class EntrySingle : public EntryBase {
    friend BookStore;
    
    EntrySingle(const Context& context)
    : _context {context} {}

  public:
    EntrySingle() = default;
    Handle<T> handle() {
      return Handle(_context.mem, _context.mem->at<T>(0));
    }

  private:
    Context _context;
  };

  template<typename T>
  class EntryMultiCopy : public EntryBase {
    friend BookStore;

    EntryMultiCopy(const Context& context)
    : _context {context} {}

  public:
    EntryMultiCopy() = default;
    Handle<T> handle(std::size_t idx) {
      return Handle(_context.mem, _context.mem->at<T>(idx));
    }

  private:
    Context _context;
  };

  template<typename T>
  class EntryMultiShared : public EntryBase {
    friend BookStore;

    EntryMultiShared(const Context& context)
    : _context{context}{}
  
  public:
    EntryMultiShared() = default;
    Handle<T> handle() {
      return Handle(_context.mem, _context.mem->at<T>(0));
    }
  private:
    Context _context;

  };




  class Entry {
    friend BookStore;


    Entry(
      const std::shared_ptr<EntryBase>& entry,
      const EntryKey& key)
      : _key {key}, _entry{entry}
      {}

    void clear() {
      _key = EntryKey{};
      _entry.reset();
    }

  public:
    Entry() = default;

    std::shared_ptr<const EntryBase> entry() const {
      return _entry;
    }

    const EntryKey& key() const {
      return _key;
    }


  private:
    EntryKey _key {std::type_index(typeid(void))};
    std::shared_ptr<EntryBase> _entry {nullptr};
  };

}
