#pragma once

// -- std includes
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>

// -- Marlin includes
#include "marlin/Exceptions.h"

// -- MarlinBook includes
#include "marlin/book/ROOTAdapter.h"
#include "marlin/book/Entry.h"
#include "marlin/book/Condition.h"
#include "marlin/book/EntryData.h"
#include "marlin/book/Flags.h"
#include "marlin/book/Hist.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/Selection.h"

namespace marlin {
  /// contains classes needed to book and managed booked objects.
  namespace book {

    // -- MarlinBook forward declaration
    template < unsigned long long >
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

    /**
     *  @brief Base Class for Entry Data, for similar behavior.
     */
    template < class T >
    class EntryDataBase {} ;

    template <>
    class EntryDataBase< void > {
    public:
      EntryDataBase()                                   = default ;
      EntryDataBase( const EntryDataBase & )            = delete ;
      EntryDataBase &operator=( const EntryDataBase & ) = delete ;
      EntryDataBase( EntryDataBase && )                 = delete ;
      EntryDataBase &operator=( EntryDataBase && )      = delete ;

      // template<typename ... Args_t>
      // void book(BookStore & store, Args_t... args){
      // }
    } ;

    /**
     * @brief Container for data to construct and setup booked object.
     */
    template < class T, unsigned long long = 0 >
    class EntryData : public EntryDataBase< void > {} ;

    /**
     *  @brief Managed Access and creation to Objects.
     */
    class BookStore {

      /**
       *  @brief register Entry in store.
       *  generate id for Entry.
       */
      void addEntry( const std::shared_ptr< EntryBase > &entry, EntryKey &key ) ;

      /**
       *  @brief get Entry from key.
       *  @throw BookStoreException key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { 
        try {
          return _entries[key.hash]; 
        } catch(const std::out_of_range&) {
          MARLIN_THROW_T(BookStoreException, "Invalid key.");
        }
      
      }

    public:
      template < class T >
      auto book( const std::string_view &path,
                 const std::string_view &name,
                 const T &               data ) {
        return data.template book< std::string_view, std::string_view >(
          *this, path, name ) ;
      }

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


    //--------------------------------------------------------------------------


    void BookStore::addEntry( const std::shared_ptr< EntryBase > &entry,
                              EntryKey &                          key ) {
      EntryKey k = key ;
      k.hash     = _entries.size() ;
      _entries.push_back( Entry( entry, k ) ) ;
    }

    //--------------------------------------------------------------------------

    template < class T, typename... Args_t >
    EntrySingle< T > BookStore::bookSingle( const std::string_view &path,
                                            const std::string_view &name,
                                            Args_t... ctor_p ) {
      EntryKey key{std::type_index( typeid( T ) )} ;
      key.name  = name ;
      key.path  = path ;
      key.mInstances   = 1 ;
      key.flags = Flags::Book::Single ;

      auto entry = std::make_shared< EntrySingle< T > >( Context(
        std::make_shared< SingleMemLayout< T, Args_t... > >( ctor_p... ) ) ) ;

      addEntry( entry, key ) ;

      return *std::static_pointer_cast< const EntrySingle< T > >( entry ) ;
    }

    //--------------------------------------------------------------------------

    template < class T, typename... Args_t >
    EntryMultiCopy< T > BookStore::bookMultiCopy( std::size_t             n,
                                                  const std::string_view &path,
                                                  const std::string_view &name,
                                                  Args_t... ctor_p ) {
      EntryKey key{std::type_index( typeid( T ) )} ;
      key.name   = name ;
      key.path   = path ;
      key.mInstances    = n ;
      key.flags  = Flags::Book::MultiCopy ;
      auto entry = std::make_shared< EntryMultiCopy< T > >( Context(
        std::make_shared< SharedMemLayout< T, merge<T>, Args_t... > >(
          n, ctor_p... ) ) ) ;

      addEntry( entry, key ) ;

      return *std::static_pointer_cast< const EntryMultiCopy< T > >( entry ) ;
    }

    Selection BookStore::find( const Condition &cond ) {
      return Selection::find( _entries.cbegin(), _entries.cend(), cond ) ;
    }

    //--------------------------------------------------------------------------

    void BookStore::remove( const Entry &e ) { get( e.key() ).clear(); }

    //--------------------------------------------------------------------------

    void BookStore::remove( const Selection &selection ) {
      for ( const Entry &e : selection ) {
        remove( e ) ;
      }
    }

    //--------------------------------------------------------------------------

    void BookStore::clear() { _entries.resize( 0 ); }

    //--------------------------------------------------------------------------

    template < class T, typename... Args_t >
    EntryMultiShared< T >
    BookStore::bookMultiShared( const std::string_view &path,
                                const std::string_view &name,
                                Args_t... ctor_p ) {
      EntryKey key{std::type_index( typeid( T ) )} ;
      key.name  = name ;
      key.path  = path ;
      key.mInstances = 1 ;
      key.flags = Flags::Book::MultiShared ;

      auto entry = std::make_shared< EntryMultiShared< T > >( Context(
        std::make_shared< SingleMemLayout< T, Args_t... > >( ctor_p... ) ) ) ;

      addEntry( entry, key ) ;

      return *std::static_pointer_cast< const EntryMultiShared< T > >( entry ) ;
    }

  } // end namespace book

} // end namespace marlin
