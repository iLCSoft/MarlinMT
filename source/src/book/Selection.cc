#include "marlin/book/MemLayout.h"

#include "marlin/book/Selection.h"
#include "marlin/book/Condition.h"

#include <algorithm>
#include <iterator>
#include <iostream>

namespace marlin::book {

	Selection::Selection(
		const Selection& sel,
		const Condition& cond,
		ComposeStrategie strategie) 
		: Selection{find(sel.begin(), sel.end(), cond)}{
		switch (strategie) {
			case ComposeStrategie::AND: 
					_condition = sel.condition().And(cond);
				break;
			case ComposeStrategie::ONLY_CHILD:
					_condition = cond;
				break;
			case ComposeStrategie::ONLY_PARENT:
					_condition = sel.condition();
				break;
			default:
				throw "Strategie not definet!";
		}
	}

	template<typename T>
	Selection Selection::find(T begin, T end, const Condition& cond){
		Selection res{};
		res._condition = cond;

		std::copy_if(begin, end,
			std::back_inserter(res._entries),
			[&c = cond](const Entry& itr) -> bool{ 
					return c(itr.key());
			}
		);
		return res;
	}

	Selection Selection::find(const Condition& cond, ComposeStrategie strategie) {
		return Selection(*this, cond, strategie);
	}

	void Selection::remove(std::size_t id) {
		_entries.erase(_entries.cbegin() + id);
	}

	void Selection::remove(std::size_t id, std::size_t n) {
		auto beg = _entries.cbegin() + id;
		_entries.erase(beg, beg + n);
	}

	void Selection::remove(iterator itr) {
		_entries.erase(itr);
	}

	void Selection::remove(iterator begin, iterator end) {
		_entries.erase(begin, end);
	}

	template Selection Selection::find<Selection::iterator>(
		Selection::iterator begin,
		Selection::iterator end,
		const Condition& cond
	);

}
