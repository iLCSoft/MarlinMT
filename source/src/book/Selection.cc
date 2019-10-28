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

	template Selection Selection::find<Selection::iterator>(
		Selection::iterator begin,
		Selection::iterator end,
		const Condition& cond
	);

	Selection Selection::find(const Condition& cond, ComposeStrategie strategie) {
		return Selection(*this, cond, strategie);
	}

}
