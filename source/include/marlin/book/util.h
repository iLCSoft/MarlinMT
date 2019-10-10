#pragma once

#include <unordered_map>
#include <string>
#include <utility>

class BookStore;

namespace marlin {
  using Key_t = std::string;
  using void_ptr = std::shared_ptr<void>;
  class MergeMgr {  
    friend BookStore;
    struct MergeObj {
      std::function<void(void_ptr&, void_ptr&)> merge;
      void_ptr src, dst;
    };
    using map_t = std::unordered_multimap<Key_t, MergeObj>;
    map_t _merges;
  public:
    void Merge(const Key_t& key) {
      std::pair<map_t::iterator, map_t::iterator>
        range = _merges.equal_range(key);
      for(auto itr = range.first;
        itr != range.second; ++itr)
      {
        MergeObj& obj = itr->second;
        obj.merge(obj.src, obj.dst);
      }
    } 
    void insert(std::pair<const Key_t, MergeObj>&& k_v) {
      _merges.insert(k_v);
    }
  };
}
