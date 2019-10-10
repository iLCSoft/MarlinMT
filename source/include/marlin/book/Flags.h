#include <bitset>
#pragma once

namespace marlin {

	constexpr std::size_t AmtFlags = 8;

	class Flag_t {
		std::bitset<AmtFlags> _val;
	protected:
	public:
		Flag_t(unsigned long long val) : _val{val} {}
		Flag_t(const std::bitset<AmtFlags>& val) : _val{val}{}
		Flag_t() : _val{0} {}
		Flag_t operator& (const Flag_t& f) const {
			return Flag_t(_val & f._val);
		}
		Flag_t operator| (const Flag_t& f) const {
			return Flag_t(_val & f._val);
		}
		operator bool() const {
			return _val.any();
		}
		operator const std::bitset<AmtFlags>& () const {
			return _val;
		}
		bool operator==(const Flag_t& f) const {
			return _val == f._val;
		}
		bool operator!=(const Flag_t& f) const {
			return _val != f._val;
		}
		bool Contains(const Flag_t& f) const {
			return (_val & f._val) == f._val;
		}
	};

	namespace BookFlags {
		const Flag_t MultiInstance(1);
		const Flag_t Default(MultiInstance);

	}
}
