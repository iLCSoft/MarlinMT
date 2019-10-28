#include <vector>
#include <unordered_map>

#include "marlin/book/Condition.h"
#include "marlin/book/Entry.h"

namespace marlin::book {
  
  class BookStore;
  
class Selection {
  friend BookStore;

  template<typename T>
  static 
  Selection find(T begin, T end, const Condition& cond);
  
public:
  using iterator = typename std::vector<Entry>::const_iterator;
  enum struct ComposeStrategie {
    AND,
    ONLY_CHILD,
    ONLY_PARENT
  };

  Selection() = default;
  Selection(const Selection&) = delete;
  Selection(Selection&&) = default;

  Selection(
    const Selection& sel,
    const Condition& cond,
    ComposeStrategie strategie = ComposeStrategie::AND);

  const Condition& condition() const { return _condition; }

  iterator begin() const { return _entries.cbegin(); }
  iterator end() const { return _entries.cend(); }
  std::size_t size() const { return _entries.size(); }

  Selection find(const Condition& cond, ComposeStrategie strategie = ComposeStrategie::AND);

private:
  std::vector<Entry> _entries{};
  Condition _condition{};
};

  
}
