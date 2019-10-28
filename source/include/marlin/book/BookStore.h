#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

#include "marlin/book/Entry.h"

namespace ROOT::Experimental {
  class RAxisConfig;
}

namespace marlin::book {
  class Flag_t;

  class MemLayout;
  template<typename, typename ...>
  class SingleMemLayout;
  template<typename T,
    void(const std::shared_ptr<T>&,
      const std::shared_ptr<T>&),
    typename ...>
  class SharedMemLayout;
  class Condition;
  class Selection;

  class BookStore {

    void addEntry(
      const std::shared_ptr<EntryBase>& entry,
      const EntryKey& key) {
        EntryKey k = key;
        k.hash = _entries.size();
        _entries.push_back(
          Entry(entry, k)
        );
    }
  public:
    using AxisConfig = ROOT::Experimental::RAxisConfig;

    template<class T, typename ... Args_t>
    EntrySingle<T>
    book(
      const std::string_view& path,
      const std::string_view& name,
      Args_t ... ctor_p );


    template<typename T>
    const auto bookH1(
      const std::string_view& path,
      const std::string_view& name,
      AxisConfig axis
    ) {
      return book<RH<T, 1>, AxisConfig>(path, name, axis);
    }
    
    template<class T, typename ... Args_t>
    EntryMultiCopy<T>
    bookMultiCopy(
      std::size_t n,
      const std::string_view& path,
      const std::string_view& name,
      Args_t ... ctor_p);

    Selection find(const Condition& cond);

  private:
    // std::unordered_map<std::size_t, Entry> _entries{};
    std::vector<Entry> _entries{};
  };

}
