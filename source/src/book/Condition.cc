#include "marlin/book/Condition.h"
#include "marlin/book/EntryData.h"

#include <string>

namespace marlin::book {

bool rgxEvaluation(
	const std::string_view& rgx,
	const std::string_view& str) {
	return rgx == str; // TODO: proper evaluation	
}

Condition::Condition()
	: _fiterFn{[](const EntryKey&){ return true; }}
{}

Condition::Condition(const FilterFn_t& filterFn) 
	: _fiterFn{filterFn}
{}

ConditionBuilder::operator Condition() const {
	return condition();
}

// FIXME: performance 
Condition ConditionBuilder::condition() const {
	typename Condition::FilterFn_t fn
		= [](const EntryKey&){ return true; };

	if (_name) {
		fn = [fn, rgx = _name.value()](const EntryKey& e) {
			return fn(e) && rgxEvaluation(rgx, e.name);
		};
	}

	if (_path) {
		fn = [fn, rgx = _path.value()](const EntryKey& e) {
			return fn(e) && rgxEvaluation(rgx, e.path);
		};
	}

	if (_type) {
		fn = [fn, type = _type.value()](const EntryKey& e) {
			return fn(e) && type == e.type;
		};
	}

	return Condition( fn );
}

ConditionBuilder& ConditionBuilder::setName(const std::string_view& rgx) {
	_name = std::optional<std::string>(rgx);
	return *this;
}

ConditionBuilder& ConditionBuilder::setPath(const std::string_view& rgx) {
	_path = std::optional<std::string>(rgx);
	return *this;
}

ConditionBuilder& ConditionBuilder::setType(const std::type_index& type) {
	_type = std::optional<std::type_index>(type);
	return *this;
}


}
