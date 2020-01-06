#pragma once

// -- std includes
#include <iostream>
#include <memory>
#include <tuple>
#include <typeinfo>
#include <variant>

// -- Marlin includes
#include "marlin/Exceptions.h"

// -- MarlinBook includes
#include "marlin/book/Flags.h"
#include "marlin/book/EntryData.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/Types.h"

namespace marlin {
  namespace book {

    /**
     *  @brief minimal entry for Object.
     *  @note not for multithreading.
     */
    template < typename T >
    class EntrySingle : public EntryBase {
      friend BookStore ;

      /// constructor
      explicit EntrySingle( Context context )
        : _context{std::move( context )} {}

    public:
      static constexpr Flag_t Flag = Flags::Book::Single;
      /// default constructor
      EntrySingle() = default ;

      /// Creates a new Handle for the object.
      Handle< T > handle() {
        return Handle( _context.mem, _context.mem->at< T >( 0 ) ) ;
      }

    private:
      /// Context of the Entry. Containing needed references.
      Context _context ;
    } ;

    /**
     *  @brief entry for object to be used Multithreaded.
     *  contains multiple instances to avoid synchronisation.
     *  @note keep the memory consumption in mind.
     */
    template < typename T >
    class EntryMultiCopy : public EntryBase {
      friend BookStore ;

      /// constructor
      explicit EntryMultiCopy( Context context )
        : _context{std::move( context )} {}

    public:
      static constexpr Flag_t Flag = Flags::Book::MultiCopy;
      /// default constructor
      EntryMultiCopy() = default ;

      /**
       *  @brief creates a new handle for the object.
       *  @param idx id of instance to get handle.
       *  @attention Use one instance only in one thread at the same time.
       */
      Handle< T > handle( std::size_t idx ) {
        return Handle( _context.mem, _context.mem->at< T >( idx ) ) ;
      }

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;

    /**
     *  @brief entry for object to be used Multithreaded.
     *  contain only one Instance and a thread save way to write.
     *  @note keep synchronisation points in mind.
     */
    template < typename T >
    class EntryMultiShared : public EntryBase {
      friend BookStore ;

      /// constructor
      explicit EntryMultiShared( Context context )
        : _context{std::move( context )} {}

    public:
      static constexpr Flag_t Flag = Flags::Book::MultiShared;
      /// default constructor
      EntryMultiShared() = default ;

      /**
       *  @brief creates a new handle for the object.
       *  Each handle may contain some kind of Buffer to reduce sync-points.
       */
      Handle< T > handle() {
        return Handle( _context.mem, _context.mem->at< T >( 0 ) ) ;
      }

    private:
      /// \see {EntrySingle::_context}
      Context _context ;
    } ;

    template<typename Type>
    using EntryTypes = std::tuple<
      EntrySingle<Type>,
      EntryMultiCopy<Type>,
      EntryMultiShared<Type>
    >;

    /**
     *  @brief class to store and manage objects in BookStore.
     */
    class Entry {
      friend BookStore ;

      /// constructor
      Entry( std::shared_ptr< EntryBase > entry, EntryKey key )
        : _key{std::move( key )}, _entry{std::move( entry )} {}

      /// reduce Entry to default constructed version.
      void clear() {
        _key = EntryKey{} ;
        _entry.reset() ;
      }


      struct GetFromEntry {
        static void ThrowIfOutOfBound(const EntryKey& key, std::size_t idx) {
          if(idx < 0 || idx >= key.mInstances) {
            auto itoa = [](std::size_t id){
              return std::to_string(id);
            };
            MARLIN_THROW_T(
              BookStoreException,
              (std::string("Try to access instances '") + itoa(idx) 
              + "', which is outside of [0;" 
                + itoa(key.mInstances-1) + "]"));
          } 
        }


        template<typename R, typename ET, R(ET::*)(std::size_t)>
        struct need_index {};

        template<typename T, typename ET>
        static constexpr bool handle_need_index(
            need_index<Handle<T>, ET, &ET::handle>* ) {
          return true;
        }
        template<typename T, typename ET>
        static constexpr bool handle_need_index(...) { return false; }

        template<typename T, typename ET>
        static constexpr bool handle_need_index_v = handle_need_index<T,ET>(0);

        template<typename T, std::size_t I = 0>
        [[nodiscard]]
        static Handle<T> handle(
            std::shared_ptr<EntryBase> entry,
            const EntryKey& key,
            std::size_t idx) {
          using EntryType = std::tuple_element_t<I, EntryTypes<T>>;

          if (key.flags == EntryType::Flag) {
            if constexpr (handle_need_index_v<T, EntryType>) {

              ThrowIfOutOfBound(key, idx);

              return std::static_pointer_cast<EntryType>(entry)->handle(idx);
            } else {
              return std::static_pointer_cast<EntryType>(entry)->handle();
            }
          }
          if constexpr (I + 1 < (std::tuple_size_v<EntryTypes<T>>)) {
            return handle<T, I + 1>(entry, key, idx);
          } else {
            MARLIN_THROW_T(
              BookStoreException,
              "Entry has an invalid Flag combination! Can't create Handle!" ) ; 
          }
        }
      };

    public:
      /// default constructor. Not valid!
      Entry() = default ;

      /**
       *  @brief creates an handle for the entry.
       *  @param idx of instance, only used for multi copy entries
       *  @throw BookStoreException when the Entry has Invalid Flags, or
       *  type mismatch with demanded Handle.
       *  @return empty optional if the type or the configuration not matches.
       */
      template < class T >
      [[nodiscard]] Handle< T > handle( std::size_t idx = -1 ) const {
        if ( std::type_index( typeid( T ) ) != _key.type ) {
          MARLIN_THROW_T( BookStoreException,
                          "Entry is not demanded type. Can't create Handle!" ) ;
        }
        
        return GetFromEntry::handle<T>(_entry, key(), idx);

      }

      /// access key data from entry.
      [[nodiscard]] const EntryKey &key() const { return _key; }

      /**
       *  @brief check if entry is valid.
       *  check if there is a reference stored or not.
       *  @return true if entry is valid.
       */
      [[nodiscard]] bool valid() const { return _entry != nullptr; }

    private:
      /// Key data for Entry.
      EntryKey _key{std::type_index( typeid( void ) )} ;
      /// reference to entry data.
      std::shared_ptr< EntryBase > _entry{nullptr} ;
    } ;

  } // end namespace book
} // end namespace marlin
