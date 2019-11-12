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
<<<<<<< HEAD
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
     *  @brief Base Class for Entry Data, to shorten class definitions.
     */
    template < class T> class EntryDataBase{};
    template<>
    class EntryDataBase<void>{
    public:
      EntryDataBase() = default ;
      EntryDataBase( const EntryDataBase&) = delete ;
      EntryDataBase& operator=(const EntryDataBase &) = delete ;
      EntryDataBase(EntryDataBase && )                  = default ;
      EntryDataBase & operator=(EntryDataBase &&) = default ;
    
      // template<typename ... Args_t>
      // void book(BookStore & store, Args_t... args){
      // }
    } ;
    
    /**
     *  @brief Helper class for easier booking.
     *  Specialisation for each object type, so there is no need 
     *  to specify the book function arguments by ourself.
     */
    template < class T, unsigned long long = 0 >
    class EntryData : public EntryDataBase<void> {} ;


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
       *  @throw std::out_of_range key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { return _entries[key.hash]; }

    public:
      template<class T>
      auto book(const std::string_view& path, const std::string_view& name, const T& data) {
        // TODO static assert and type check
        return data.template book<std::string_view, std::string_view>(*this, path, name)  ;
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

  } // end namespace book
||||||| merged common ancestors
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
     *  @brief Base Class for Entry Data, to shorten class definitions.
     */
    template < class T> class EntryDataBase{};
    template<>
    class EntryDataBase<void>{
    public:
      EntryDataBase() = default ;
      EntryDataBase( const EntryDataBase&) = delete ;
      EntryDataBase& operator=(const EntryDataBase &) = delete ;
      EntryDataBase(EntryDataBase && )                  = default ;
      EntryDataBase & operator=(EntryDataBase &&) = default ;
    
      // template<typename ... Args_t>
      // void book(BookStore & store, Args_t... args){
      // }
    } ;
    
    /**
     *  @brief Helper class for easier booking.
     *  Specialisation for each object type, so there is no need 
     *  to specify the book function arguments by ourself.
     */
    template < class T, unsigned long long = 0 >
    class EntryData : public EntryDataBase<void> {} ;


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
       *  @throw std::out_of_range key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { return _entries[key.hash]; }

    public:
      template<class T>
      auto book(const std::string_view& path, const std::string_view& name, const T& data) {
        // TODO static assert and type check
        return data.template book<std::string_view, std::string_view>(*this, path, name)  ;
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

  } // end namespace book
=======
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
       *  @throw std::out_of_range key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { return _entries[key.hash]; }

    public:
      template < class T >
      auto book( const std::string_view &path,
                 const std::string_view &name,
                 const T &               data ) {
        // TODO static assert and type check
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

  } // end namespace book
>>>>>>> Formatted code and add doxygen

} // end namespace marlin
