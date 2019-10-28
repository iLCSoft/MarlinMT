#include <typeinfo>
#include <typeindex>

#include "marlin/book/MemLayout.h"
#include "ROOT/RHist.hxx"
#include "ROOT/RHistData.hxx"
#include "marlin/book/Hist.h"
#include "marlin/book/Selection.h"
#include "marlin/book/Condition.h"

#include "marlin/book/BookStore.h"

namespace marlin::book {

	template<class T, typename ... Args_t>
	EntrySingle<T>
	BookStore::book(
		const std::string_view& path,
		const std::string_view& name,
		Args_t ... ctor_p ) {
		EntryKey key{std::type_index(typeid(T))};
		key.name = name;
		key.path = path;
		key.amt = 1;
		key.flags = Flags::Book::Single;

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
	BookStore::bookMultiCopy(
		std::size_t n,
		const std::string_view& path,
		const std::string_view& name,
		Args_t ... ctor_p
	) {
		EntryKey key{std::type_index(typeid(T))};
		key.name = name;
		key.path = path;
		key.amt = n;
		key.flags = Flags::Book::MultiCopy;

		auto entry = std::make_shared<EntryMultiCopy<T>>(
			Context(
				std::make_shared<SharedMemLayout<T, trait<T>::Merge, Args_t ...>>(
					n, ctor_p ...
				)
			)
		);

		addEntry(entry,  key);

		return *std::static_pointer_cast<const EntryMultiCopy<T>>( entry );
	}

	Selection BookStore::find(const Condition& cond) {
		return Selection::find(_entries.cbegin(), _entries.cend(), cond);
	}


	template EntryMultiCopy<RH<float, 1>>
	BookStore::bookMultiCopy<RH<float,1>, BookStore::AxisConfig>
	(std::size_t n, const std::string_view& path, const std::string_view& name, BookStore::AxisConfig axis);
	template EntryMultiCopy<RH<int, 1>>
	BookStore::bookMultiCopy<RH<int, 1>, BookStore::AxisConfig>
	(std::size_t, const std::string_view&, const std::string_view&, BookStore::AxisConfig);

	template EntrySingle<RH<float, 1>>
	BookStore::book<RH<float,1>, BookStore::AxisConfig>
	(const std::string_view&, const std::string_view&, BookStore::AxisConfig);
	template EntrySingle<RH<int, 1>>
	BookStore::book<RH<int, 1>, BookStore::AxisConfig>
	(const std::string_view&, const std::string_view
	&, BookStore::AxisConfig);
}
