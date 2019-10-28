#pragma once

#include <functional>
#include <typeindex>
#include <string_view>
#include <optional>

#include "marlin/book/EntryData.h"

namespace marlin::book {
  class EntryKey;

  class Condition {

  public: 
    using FilterFn_t = 
      std::function<
        bool (const EntryKey&)
      >;

    Condition();
    Condition(const FilterFn_t& filterFn);

    // Condition(Condition &&) = default;
    Condition(const Condition&) = default;

    bool operator()(const EntryKey& key) const  {
      return _fiterFn(key);
    }
  
    // FIXME: 'and' and 'or' are keywords in c++(and = &&, or = ||)
    Condition And(const Condition& rhs) const {
      return Condition(
        [lh = _fiterFn, rh = rhs](const EntryKey& key) {
          return lh(key) && rh(key);
        }
      );
    }

    Condition Or(const Condition& rhs) const {
      return Condition(
        [lh = _fiterFn, rh = rhs](const EntryKey& key) {
          return lh(key) || rh(key);
        }
      );
    }

    Condition Not() const {
      return Condition (
        [fn = _fiterFn](const EntryKey& key) -> bool{
          return !fn(key);
        }
      );
    }

  private:
    FilterFn_t _fiterFn;
  };

  class ConditionBuilder {
  public:

    operator Condition() const;

    Condition condition() const;
    
    ConditionBuilder& setName(const std::string_view& rgx);

    ConditionBuilder& setPath(const std::string_view& rgx);

    ConditionBuilder& setType(const std::type_index& type);
    
    template<typename T>
    ConditionBuilder& setType() {
      return setType(std::type_index(typeid(T)));
    }

  private:

    std::optional<std::string> _name{};
    std::optional<std::string> _path{};
    std::optional<std::type_index> _type{};
  };

}
