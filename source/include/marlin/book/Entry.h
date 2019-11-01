#pragma once

// -- std includes
#include <iostream>
#include <memory>
#include <typeinfo>
#include <variant>

// -- MarlinBook includes
#include "marlin/book/EntryData.h"
#include "marlin/book/MemLayout.h"

namespace marlin {
  namespace book {

    // -- MarlinBook forward declarations
    class BookStore ;
    template < typename T >
    class Handle ;

    /**
     *  @brief trait for type Specific Data, Function and types.
     */
    template < typename T >
    struct trait {

      /// Function to merge two objects of type T.
      static void Merge( const std::shared_ptr< T > &,
                         const std::shared_ptr< T > & ) {
        throw " ... " ;
      } // TODO: add proper error
    } ;

    /**
     *  @brief minimal entry for Object.
     *  @note not for multithreading.
     */
    template < typename T >
    class EntrySingle : public EntryBase {
      friend BookStore ;

      /// constructor
      EntrySingle( const Context &context ) : _context{context} {}

    public:
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
      EntryMultiCopy( const Context &context ) : _context{context} {}

    public:
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
      EntryMultiShared( const Context &context ) : _context{context} {}

    public:
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
    
    /**
     *  @brief class to store and manage objects in BookStore.
     */
    class Entry {
      friend BookStore ;
      
      /// constructor
      Entry( const std::shared_ptr< EntryBase > &entry, const EntryKey &key )
        : _key{key}, _entry{entry} {}

      /// reduce Entry to default constructed version.
      void clear() {
        _key = EntryKey{} ;
        _entry.reset() ;
      }

    public:
      /// default constructor. Not valid!
      Entry() = default ;

      /// access actual entry.
      std::shared_ptr< const EntryBase > entry() const { return _entry; }

      /// access key data from entry.
      const EntryKey &key() const { return _key; }

      /**
       *  @brief check if entry is valid.
       *  @return true if entry is valid.
       */
      bool valid() const { return static_cast< bool >( _entry ); }

    private:
      /// Key data for Entry.
      EntryKey                     _key{std::type_index( typeid( void ) )} ;
      /// reference to entry data.
      std::shared_ptr< EntryBase > _entry{nullptr} ;
    } ;

  } // end namespace book
} // end namespace marlin