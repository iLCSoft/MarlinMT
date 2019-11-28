#pragma once

// -- std includes
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>
#include <atomic>
#include <thread>

// -- Marlin includes
#include "marlin/Exceptions.h"
#include "marlin/book/ROOTAdapter.h"

// -- MarlinBook includes
#include "marlin/book/Entry.h"
#include "marlin/book/Condition.h"
#include "marlin/book/EntryData.h"
#include "marlin/book/Flags.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/Selection.h"

namespace marlin {
  /// contains classes needed to book and managed booked objects.
  namespace book {

    // -- MarlinBook forward declaration
    class MemLayout ;
    template < typename, typename... >
    class SingleMemLayout ;
    template < typename T,
               void(*)( const std::shared_ptr< T > &,
                     const std::shared_ptr< T > & ),
               typename... >
    class SharedMemLayout ;
    template<typename T>
    class Manager{};

    template<typename T>
    class Handle<Manager<T>> {
      friend BookStore;
      friend Selection::Hit;
      using IdMap_t = std::unordered_map<std::size_t, std::size_t>;
      explicit Handle(std::shared_ptr<const Entry> entry)
        : _entry{std::move(entry)},
          _mapping(std::make_unique<IdMap_t>()){}
      std::size_t unmap(std::size_t id) {
        auto itr = _mapping->find(id);
        if(itr == _mapping->end()) {
          itr = _mapping->insert(std::make_pair(id, _n++)).first;
        }
        return itr->second;
      }
    public:
      Handle(const Handle&) = delete;
      Handle& operator=(const Handle &) = delete ;
      Handle(Handle && hnd) noexcept
      : _entry(nullptr), _mapping(nullptr), _n(hnd._n){
        _entry = hnd._entry;
        _mapping = std::move(hnd._mapping);

        hnd._entry.reset();
      }
      Handle &operator=(Handle && hnd) noexcept {
        _entry = hnd._entry;
        _mapping = std::move(hnd._mapping);
        _n = hnd.load();

        hnd._entry.reset();
      }
      ~Handle() = default;
      Handle<T> handle(std::size_t id) {
          return _entry->handle<T>(unmap(id));  
      }

    private:
      std::shared_ptr<const Entry> _entry{nullptr};
      std::unique_ptr<IdMap_t> _mapping{};
      std::atomic<std::size_t> _n{0};
    };



    /**
     *  @brief Managed Access and creation to Objects.
     */
    class BookStore {
      template <typename,unsigned long long>
      friend class EntryData;
      class Identifier {
      public:
        struct Hash {
          std::size_t operator()(const Identifier& id) const {
            std::hash<decltype(Identifier::_name)> hasher;  
            std::size_t hash = hasher(id._name);
            constexpr std::size_t salt = 0x9e3779b9;
            constexpr std::array<std::size_t, 2> offsets = {6, 2};
            return hash ^= hasher(id._path) + salt + (hash<<offsets[0]) + (hash>>offsets[1]);
          } 
        };
        Identifier(
            const std::string_view& path,
            const std::string_view& name)
          : _path{path}, _name{name}{}
        bool operator==(const Identifier& id) const {
          return _name == id._name && _path == id._path;
        }
      private:
        std::string _path;
        std::string _name;

      };

      /**
       *  @brief register Entry in store.
       *  generate id for Entry.
       *  @return shared pointer to new Entry
       */
      std::shared_ptr<Entry> addEntry( const std::shared_ptr< EntryBase > &entry, EntryKey key ) ;

      /**
       *  @brief get Entry from key.
       *  @throw BookStoreException key not exist in Store.
       */
      Entry &get( const EntryKey &key ) { 
        return get(key.hash);
      }

      /**
       *  @brief get Entry from key.
       *  @throw BookStoreException key not exist in Store.
       */
      Entry &get( std::size_t const key) { 
        try {
          return *_entries[key]; 
        } catch(const std::out_of_range&) {
          MARLIN_THROW_T(BookStoreException, "Invalid key.");
        }
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
      std::shared_ptr<Entry> bookSingle( const std::string_view &path,
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
      template < 
          class T, 
          void(*MERGE)(const std::shared_ptr<T>&,const std::shared_ptr<T>&),   
          typename... Args_t >
      std::shared_ptr<Entry> bookMultiCopy( std::size_t             n,
                                         const std::string_view &path,
                                         const std::string_view &name,
                                         Args_t... ctor_p ) ;

      /**
       *  @brief creates an Entry for parallel access.
       *  Creates one object in Memory and modifiers.
       *  \see BookStore::book
       */
      template < class T, typename... Args_t >
      std::shared_ptr<Entry> bookMultiShared( const std::string_view &path,
                                             const std::string_view &name,
                                             Args_t... ctor_p ) ;

    public:
      explicit BookStore(bool allowMoving = false) : 
        _constructThread(std::this_thread::get_id()),
        _allowMoving{allowMoving}{}

      template < class T>
      Handle<Manager<typename T::Object_t>> book( const std::string_view path,
                 const std::string_view name,
                 const T &               data ) {

        if( !_allowMoving && std::this_thread::get_id() != _constructThread) {
          MARLIN_THROW_T(BookStoreException, "Booking is only allowed "
          "from the construction Thread");    
        }

        auto entry = _idToEntry.find({path, name});
        if(entry == _idToEntry.end()) {
          return Handle<Manager<typename T::Object_t>>
            (data.template book< std::string_view, std::string_view >(
            *this, path, name ) );
        }
        return Handle<Manager<typename T::Object_t>>(_entries[entry->second]);
      }

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
      void remove( const EntryKey &key ) ;

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
      std::vector< std::shared_ptr<Entry> > _entries{} ;
      /// stores path+name -> Entry Id
      std::unordered_map<Identifier, std::size_t, Identifier::Hash> _idToEntry{};
      std::thread::id _constructThread;
      const bool _allowMoving{false};
    } ;


    //--------------------------------------------------------------------------


    std::shared_ptr<Entry> BookStore::addEntry( const std::shared_ptr< EntryBase > &entry,
                              EntryKey                          key ) {
      key.hash     = _entries.size() ;

      if(!_idToEntry.insert(std::make_pair(Identifier(key.path, key.name), key.hash)).second) {
        MARLIN_THROW_T(BookStoreException, "Object already exist. Use store.book to avoid this.");
      }
      _entries.push_back( std::make_shared<Entry>( Entry(entry, key) ) ) ;
      return _entries.back();
    }

    //--------------------------------------------------------------------------

    template < class T, typename... Args_t >
    std::shared_ptr<Entry> 
    BookStore::bookSingle( const std::string_view &path,
                                            const std::string_view &name,
                                            Args_t... ctor_p ) {
      EntryKey key{std::type_index( typeid( T ) )} ;
      key.name  = name ;
      key.path  = path ;
      key.mInstances   = 1 ;
      key.flags = Flags::Book::Single ;

      auto entry = std::make_shared< EntrySingle< T >>( Context(
        std::make_shared< SingleMemLayout< T, Args_t... > >( ctor_p... ) ) ) ;

      return addEntry( entry, key ) ;
    }

    //--------------------------------------------------------------------------

    template <
             class T,
             void(*MERGE)(const std::shared_ptr<T>&,const std::shared_ptr<T>&),
             typename... Args_t >
    std::shared_ptr<Entry>
    BookStore::bookMultiCopy( std::size_t             n,
                              const std::string_view &path,
                              const std::string_view &name,
                              Args_t... ctor_p ) {
      EntryKey key{std::type_index( typeid( T ) )} ;
      key.name   = name ;
      key.path   = path ;
      key.mInstances    = n ;
      key.flags  = Flags::Book::MultiCopy ;
      auto entry = std::make_shared< EntryMultiCopy< T > >( Context(
        std::make_shared< SharedMemLayout< T, 
        MERGE
        , Args_t... > >(
          n, ctor_p... ) ) ) ;

      return addEntry( entry, key ) ;
    }

    Selection BookStore::find( const Condition &cond ) {
      return Selection::find( _entries.cbegin(), _entries.cend(), cond ) ;
    }

    //--------------------------------------------------------------------------

    void BookStore::remove( const EntryKey &key ) { get( key ).clear(); }

    //--------------------------------------------------------------------------

    void BookStore::remove( const Selection &selection ) {
      for (const Selection::Hit& e : selection ) {
        remove( e.key() ) ;
      }
    }

    //--------------------------------------------------------------------------

    void BookStore::clear() { _entries.resize( 0 ); }

    //--------------------------------------------------------------------------

    template < class T, typename... Args_t >
    std::shared_ptr<Entry>
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

      return addEntry( entry, key ) ;
    }

  } // end namespace book

} // end namespace marlin

