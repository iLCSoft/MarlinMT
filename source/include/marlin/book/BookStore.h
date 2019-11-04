#pragma once

// -- std includes
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

// -- map ROOT to MarlinBook types
#include "marlin/book/ROOTAdapter.h"

// -- MarlinBook includes
#include "marlin/book/Entry.h"

namespace marlin {
  /// contains classes needed to book and managed booked objects.
  namespace book {

    // -- MarlinBook forward declaration
    template<unsigned long long>
    class Flag_t ;
    class MemLayout ;
    template < typename, typename... >
    class SingleMemLayout ;
    template < typename T,
               void( const std::shared_ptr< T > &,
                     const std::shared_ptr< T > & ),
               typename... >
    class SharedMemLayout ;
    class Condition ;
    class Selection ;
    
    class BookHelperBase {
    public:
      BookHelperBase(const BookHelperBase&) = delete;
      BookHelperBase(BookHelperBase&&) = delete;
      BookHelperBase() = delete;
      BookHelperBase& operator=(const BookHelperBase&) = delete;
      BookHelperBase& operator=(BookHelperBase&&) = delete;

    protected:
      const std::string_view& _name;
      const std::string_view& _path;
      BookStore& _store;
      BookHelperBase(BookStore& store, const std::string_view& path, const std::string_view& name) 
      : _name{name},_path{path}, _store{store} {}
    };

    template< class T , unsigned long long>
    class BookHelper : public BookHelperBase{} ;

    /**
     *  @brief Managed Access and creation to Objects.
     */
    class BookStore {

      /**
       *  @brief register Entry in store.
       *  generate id for Entry.
       */
      void addEntry( const std::shared_ptr< EntryBase > &entry,
                     EntryKey                           &key ) ;

      /**
       *  @brief get Entry from key.
       *  @throw std::out_of_range key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { return _entries[key.hash]; }

    public:
      template < class T >
      const BookHelper< T, 0>  book( const std::string_view &path, const std::string_view &name);

      /**
       *  @brief creates an Entry for a default Object.
       *  @tparam T object which should be booked.
       *  @tparam Args_t types of parameters to construct T.
       *  @param path location to store the Entry.
       *  @param name name of the Entry.
       *  @param ctor_p parameters to construct the object.
       */
      template < class T, typename... Args_t >
      EntrySingle< T > bookSingle( const std::string_view &path,
                             const std::string_view &name,
                             Args_t... ctor_p ) ;


      /**
       *  @brief creates an Entry for parallel access.
       *  Creates Multiple copy's of the object to avoid locks.
       *  \see BookStore::book
       *  @note getting the results is triggers a merging →  expensive.
       *  @param n number of instances which should be created.
       *  (max level of pluralism)
       */
      template < class T, typename... Args_t >
      EntryMultiCopy< T > bookMultiCopy( std::size_t             n,
                                         const std::string_view &path,
                                         const std::string_view &name,
                                         Args_t... ctor_p ) ;

      /**
       *  @brief creates an Entry for parallel access.
       *  Creates one object in Memory and modifiers.
       *  \see BookStore::book
       */
      template < class T, typename... Args_t >
      EntryMultiShared< T > bookMultiShared( const std::string_view &path,
                                             const std::string_view &name,
                                             Args_t... ctor_p ) ;
      /**
       *  @brief select every Entry which matches the condition.
       *  @return Selection with matches Entries.
       */
      Selection find( const Condition &cond ) ;

      /**
       *  @brief removes an Entry from BookStore.
       *  the handles are keeping a reference to the result.
       *  @attention modifying removed objects results in undefined behavior.
       */
      void remove( const Entry &e ) ;

      /**
       *  @brief removes every Entry from the selection from the BookStore.
       *  calls BookStore::remove(const Entry &e)
       *  for every Entry in Selection.
       */
      void remove( const Selection &selection ) ;

      /**
       *  @brief clears the store.
       *  the handles keep a reference to the result.
       *  @attention modifying removed objects results in undefined behavior.
       */
      void clear() ;

    private:
      /// stores Entries created by BookStore.
      std::vector< Entry > _entries{} ;
    } ;

  } // end namespace book

} // end namespace marlin
