#pragma once

// -- std includes
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

// -- MarlinBook includes
#include "marlin/book/Condition.h"
#include "marlin/book/Entry.h"
#include "marlin/book/MemLayout.h"

namespace marlin {
	namespace book {

		// -- MarlinBook forward declaration
		class BookStore ;
		template< typename >
		class Manager;

		/**
		 *  @brief Contains references to entries.
		 *  Which satisfy the condition.
		 *  Used to doing action on a range of entries.
		 */
		class Selection {

		public:
			/**
			 *  @brief Construct Selection from range of Entries.
			 *  @param begin,end range of Entries
			 *  @param cond Condition to filter Entries.
			 *  @tparam T iterator type
			 *  @note For Library  internal usage, only instances for some types.
			 */
			template < typename T >
			static Selection find( T begin, T end, const Condition &cond ) ;

			class Hit{
				friend Selection;
			public:
				explicit Hit(const std::shared_ptr<const Entry>& entry) : _entry{entry}{}
				explicit Hit(const std::shared_ptr<Entry>& entry) : _entry{entry}{}
				[[nodiscard]]
				bool valid() const {
					return !_entry.expired() && _entry.lock()->valid();
				}
				[[nodiscard]]
				const EntryKey& key() const {
					return _entry.lock()->key();
				}
				template<typename T>
				[[nodiscard]]
				Handle<Manager<T>> bind() const {
					return Handle<Manager<T>>(_entry.lock());
				}
			private:
				std::weak_ptr<const Entry> _entry;
			};
		
			using ItrBase = typename std::vector<Hit>::const_iterator;

			/// type for iteration through a Selection.
			class const_iterator {
				friend Selection;
				explicit const_iterator(ItrBase itr) : _itr{itr}{}
			public:
				using value_type = Hit;
				const_iterator() = default;
				const_iterator(const const_iterator& itr) 
					: _itr{itr.base()}{}
				const_iterator& operator=(const const_iterator& itr) {
					_itr = itr.base();
					return *this;
				}
				const_iterator(const_iterator && )                 = default;
				const_iterator & operator=(const_iterator && )      = default ;
				~const_iterator() = default;

				const_iterator operator+(std::size_t n) {
					return const_iterator(_itr + n);
				}
				const_iterator& operator+=(std::size_t n) {
					_itr += n;
					return *this;
				}
				bool operator==( const const_iterator& itr) const {
					return _itr == itr._itr;
				}
				bool operator!=(const const_iterator& itr) const {
					return !(*this == itr);
				}
				const std::unique_ptr<Hit>& operator->() {
					_hit = std::make_unique<Hit>(Hit(*_itr));
					return _hit;
				}
				Hit operator*() const {
					return Hit(*_itr);
				}
				const_iterator operator++(int){
					const_iterator last = *this;
					operator++();
					return last;
				} 

				const_iterator& operator++() {
					++_itr;
					return *this; 
				}
				[[nodiscard]]
				ItrBase base() const {
					return _itr;
				}
			private:
				std::unique_ptr<Hit> _hit{}; // FIXME: performance
				ItrBase _itr{};
			};

			/// Possibilities to compose Conditions when creating sub selections.
			/// Composed the new condition with the condition from the super
			/// selection.
			enum struct ComposeStrategy { AND, ONLY_CHILD, ONLY_PARENT } ;

			/// default constructor. Construct empty selection.
			Selection() = default ;

			Selection( const Selection & ) = delete ;
			Selection& operator=( const Selection & ) = delete ;

			/// move constructor. Default
			Selection( Selection && ) = default ;
			Selection& operator=(Selection&&) = default;

			~Selection() = default;
			/**
			 *  @brief construct sub selection.
			 *  @param sel super selection
			 *  @param cond condition for promotion in sub selection
			 *  @param strategy to compose selection with sub selection condition.
			 */
			Selection( const Selection &sel,
								 const Condition &cond,
								 ComposeStrategy strategy = ComposeStrategy::AND ) ;

			/// getter for Condition which every Entry full fill.
			[[nodiscard]]
			const Condition &condition() const { return _condition; }

			/// begin iterator to iterate through entries.
			[[nodiscard]]
			const_iterator begin() const { return const_iterator(_entries.cbegin()); }

			/// end iterator for entries. First not valid iterator.
			[[nodiscard]]
			const_iterator end() const { return const_iterator(_entries.cend()); }

			/// @return number of entries included in the selection.
			[[nodiscard]]
			std::size_t size() const { return _entries.size(); }

			/**
			 *  @brief construct sub selection.
			 *  @param cond condition for promotion in sub selection.
			 *  @param strategy to compos selection with sub selection condition.
			 */
			Selection find( const Condition &cond,
											ComposeStrategy strategy = ComposeStrategy::AND ) ;

			/**
			 *  @brief get Hit at position.
			 *  @param i position of entry of interest.
			 */
			const Hit &get( std::size_t i ) { return _entries[i]; }

			/**
			 *  @brief remove entry at position.
			 *  @param i position of entry to remove.
			 */
			void remove( std::size_t i ) ;

			/**
			 *  @brief remove entry range.
			 *  @param i position of first entry to remove.
			 *  @param n number of entries to remove.
			 */
			void remove( std::size_t i, std::size_t n ) ;

			/**
			 *  @brief remove entry.
			 *  @param itr iterator from entry which should be removed.
			 */
			void remove( const const_iterator& itr ) ;

			/**
			 *  @brief remove entry range.
			 *  remove entries from begin to end, including begin, excluding end.
			 *  @param begin first entry which should be removed.
			 *  @param end first entry which should not be removed.
			 */
			void remove( const const_iterator& begin, const const_iterator& end ) ;

		private:
			/// entries which included in selection.
			std::vector< Hit  > _entries{};
			/// condition which every entry full fill.
			Condition _condition{} ;
		} ;

		//--------------------------------------------------------------------------
		
		Selection::Selection( const Selection &sel,
													const Condition &cond,
													ComposeStrategy strategy )
			: Selection{find( sel.begin(), sel.end(), cond )} {
			switch ( strategy ) {
				case ComposeStrategy::AND:
					_condition = sel.condition() & cond ;
					break ;
				case ComposeStrategy::ONLY_CHILD:
					_condition = cond ;
					break ;
				case ComposeStrategy::ONLY_PARENT:
					_condition = sel.condition() ;
					break ;
				default:
					MARLIN_THROW_T(BookStoreException, "Condition compose strategy is not defined.");
			}
		}

		//--------------------------------------------------------------------------

		template < typename T >
		Selection Selection::find( T begin, T end, const Condition &cond ) {
			Selection res{} ;
			res._condition = cond ;

			auto dst = std::back_inserter(res._entries);

			auto fn = [&c = cond](const typename T::value_type& i ) -> bool {
				const Hit h = static_cast<Hit>(i);
				return h.valid() && c( h.key() ) ;
			};

			for(auto itr = begin; itr != end; ++itr) {
				if(fn(*itr)) {
					*dst++ = static_cast<Hit>(*itr);
				}
			}
			return res ;
		}

		//--------------------------------------------------------------------------

		Selection Selection::find( const Condition &cond,
															 ComposeStrategy strategy ) {
			return Selection( *this, cond, strategy ) ;
		}

		//--------------------------------------------------------------------------

		void Selection::remove( std::size_t id ) {
			_entries.erase( _entries.cbegin() + id ) ;
		}

		//--------------------------------------------------------------------------

		void Selection::remove( std::size_t id, std::size_t n ) {
			auto beg = _entries.cbegin() + id ;
			_entries.erase( beg, beg + n ) ;
		}

		//--------------------------------------------------------------------------

		void Selection::remove( const const_iterator& itr ) { _entries.erase( itr.base() ); }

		//--------------------------------------------------------------------------

		void Selection::remove( const const_iterator& begin, const const_iterator& end ) {
			_entries.erase( begin.base(), end.base() ) ;
		}

		//--------------------------------------------------------------------------

		template Selection
		Selection::find< Selection::const_iterator >( Selection::const_iterator begin,
																						Selection::const_iterator end,
																						const Condition &   cond ) ;

	} // end namespace book
} // end namespace marlin
