#pragma once

#include <variant>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

#include "marlin/book/Flags.h"
#include "marlin/book/Entry.h"

namespace marlin::book {

  class BookStore {

    void addEntry(const Entry& entry) {
    }
  public:
    template<class T, typename ... Args_t>
    EntrySingle<T>
    book(
      const std::string_view& path,
      const std::string_view& name,
      Args_t ... ctor_p ) {
      EntryKey key{};
      key.name = name;
      key.path = path;
      key.amt = 1;
      key.flags = Flags::Book::Single;

      Entry entry{key};
      entry.setEntry(
        std::make_shared<EntrySingle<T>>(
          Context(
            std::make_shared<SingleMemLayout<T, Args_t ...>>(
              ctor_p ...
            )
          )
        )
      );

      addEntry(entry);

      return *std::static_pointer_cast<const EntrySingle<T>>(
        entry.entry() 
      );
    }

  private:
    std::unordered_map<std::size_t, Entry> _entries{};
  };

}
