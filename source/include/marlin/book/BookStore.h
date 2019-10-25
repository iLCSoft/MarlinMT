#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

#include "marlin/book/Flags.h"
#include "marlin/book/Entry.h"

namespace marlin::book {

  class MemLayout;
  template<typename, typename ...>
  class SingleMemLayout;
  template<typename T,
    void(const std::shared_ptr<T>&,
      const std::shared_ptr<T>&),
    typename ...>
  class SharedMemLayout;

  class BookStore {

    void addEntry(
      const std::shared_ptr<EntryBase>& entry,
      const EntryKey& key) {
        bool newEntry = _entries.insert(
          std::make_pair(
            key.hash,
            Entry(entry, key)
          )
        ).second;

        if(!newEntry) {
          throw "no double booking !";
        }
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
      key.hash = 1;

      auto entry = std::make_shared<EntrySingle<T>>(
          Context(
            std::make_shared<SingleMemLayout<T, Args_t ...>>(
              ctor_p ...
            )
          )
        );

      addEntry(entry, key);

      return *std::static_pointer_cast<const EntrySingle<T>>(
        entry
      );
    }
    
    template<class T, typename ... Args_t>
    EntryMultiCopy<T>
    bookMultiCopy(
      std::size_t n,
      const std::string_view& path,
      const std::string_view& name,
      Args_t ... ctor_p
    ) {
      EntryKey key{};
      key.name = name;
      key.path = path;
      key.amt = n;
      key.flags = Flags::Book::MultiCopy;
      key.hash = 2;

      auto entry = std::make_shared<EntryMultiCopy<T>>(
        Context(
          std::make_shared<SharedMemLayout<T, trait<T>::Merge, Args_t ...>>(
            n, ctor_p ...
          )
        )
      );

      addEntry(entry, key);

      return *std::static_pointer_cast<const EntryMultiCopy<T>>( entry );
    }

  private:
    std::unordered_map<std::size_t, Entry> _entries{};
  };

}
