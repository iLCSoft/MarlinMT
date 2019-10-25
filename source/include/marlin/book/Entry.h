#pragma once

#include <variant>
#include <memory>

#include "marlin/book/EntryData.h"

namespace marlin::book {
	class BookStore;
	class MemLayout;
	template<typename T>
	class Handle;

	

	
	template<typename T>
	class EntrySingle : public EntryBase {
		friend BookStore;
		
		EntrySingle(const Context& context)
		: _context {context} {}

	public:
		EntrySingle() = default;
		Handle<T> handle() {
			return Handle(_context.mem);
		}

	private:
		Context _context;
	};




	class Entry {
		friend BookStore;


		Entry(
			const std::shared_ptr<EntryBase>& entry,
			const EntryKey& key)
			: _key {key}, _entry{entry}
			{}

	public:
		std::shared_ptr<const EntryBase> entry() const {
			return _entry;
		}

		const EntryKey& key() const {
			return _key;
		}

	private:
		EntryKey _key;
		std::shared_ptr<EntryBase> _entry {nullptr};
	};

}
