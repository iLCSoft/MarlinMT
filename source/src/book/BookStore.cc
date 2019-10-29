#include <typeinfo>
#include <typeindex>

#include "marlin/book/ROOTAdapter.h"

#include "marlin/book/MemLayout.h"
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

	void BookStore::remove(const Entry& e) {
		get(e.key()).clear();
	}

	void BookStore::remove(const Selection& selection) {
		for(const Entry& e : selection) {
			remove(e);
		}
	}

	// invalidates all Entryes and handles !!
	void BookStore::clear() {
		_entries.resize(0);
	}

	template<class T, typename ... Args_t>
	EntryMultiShared<T>
	BookStore::bookMultiShared(
		const std::string_view& path,
		const std::string_view& name,
		Args_t ... ctor_p
	) {
		EntryKey key{std::type_index(typeid(T))};
		key.name = name;
		key.path = path;
		key.amt = 1;
		key.flags = Flags::Book::MultiShared;

		auto entry = std::make_shared<EntryMultiShared<T>>(
			Context(
				std::make_shared<SingleMemLayout<T, Args_t ...>>(
					ctor_p ...
				)
			)
		);

		addEntry(entry, key);

		return *std::static_pointer_cast<const EntryMultiShared<T>>( entry );
	}


#define LinkTypeHist1( TYPE ) \
	template EntryMultiCopy<TYPE> \
	BookStore::bookMultiCopy<TYPE, BookStore::AxisConfig> \
	(std::size_t n, const std::string_view& path, const std::string_view& name, BookStore::AxisConfig axis); \
	template EntrySingle<TYPE> \
	BookStore::book<TYPE, BookStore::AxisConfig> \
	(const std::string_view&, const std::string_view&, BookStore::AxisConfig); \
	template EntryMultiShared<TYPE> \
	BookStore::bookMultiShared<TYPE, BookStore::AxisConfig> \
	(const std::string_view&, const std::string_view&, BookStore::AxisConfig)

LinkTypeHist1(RH1F);
LinkTypeHist1(RH1I);
LinkTypeHist1(RH1D);
}
