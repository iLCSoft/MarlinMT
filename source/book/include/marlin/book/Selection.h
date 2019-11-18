#pragma once

// -- std includes
#include <vector>

// -- MarlinBook includes
#include "marlin/book/Condition.h"
#include "marlin/book/Entry.h"

namespace marlin {
  namespace book {

    // -- MarlinBook forward declaration
    class BookStore ;

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

      /// type for iteration through a Selection.
      using const_iterator = typename std::vector< Entry >::const_iterator ;

      /// Possibilities to compose Conditions when creating sub selections.
      /// Composed the new condition with the condition from the super
      /// selection.
      enum struct ComposeStrategy { AND, ONLY_CHILD, ONLY_PARENT } ;

      /// default constructor. Construct empty selection.
      Selection() = default ;

      Selection( const Selection & ) = delete ;

      /// move constructor. Default
      Selection( Selection && ) = default ;

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
      const Condition &condition() const { return _condition; }

      /// begin iterator to iterate through entries.
      const_iterator begin() const { return _entries.cbegin(); }

      /// end iterator for entries. First not valid iterator.
      const_iterator end() const { return _entries.cend(); }

      /// @return number of entries included in the selection.
      std::size_t size() const { return _entries.size(); }

      /**
       *  @brief construct sub selection.
       *  @param cond condition for promotion in sub selection.
       *  @param strategy to compos selection with sub selection condition.
       */
      Selection find( const Condition &cond,
                      ComposeStrategy strategy = ComposeStrategy::AND ) ;

      /**
       *  @brief get entry at position.
       *  @param i position of entry of interest.
       */
      const Entry &get( std::size_t i ) { return _entries[i]; }

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
      void remove( const_iterator itr ) ;

      /**
       *  @brief remove entry range.
       *  remove entries from begin to end, including begin, excluding end.
       *  @param begin first entry which should be removed.
       *  @param end first entry which should not be removed.
       */
      void remove( const_iterator begin, const_iterator end ) ;

    private:
      /// entries which included in selection.
      std::vector< Entry > _entries{}; // FIXME: no key copying
      /// condition which every entry full fill.
      Condition _condition{} ;
    } ;

  } // end namespace book
} // end namespace marlin
