#pragma once

// -- std includes
#include <memory>
#include <string>
#include <typeindex>

// -- MarlinBook includes
#include "marlin/book/Flags.h"

namespace marlin {
  namespace book {
    // -- MarlinBook forward declaration
    class MemLayout ;

    /**
     *  @brief Data selection to identify and manage an Entry.
     */
    struct EntryKey {

      /// default constructor.
      EntryKey() : type{std::type_index( typeid( void ) )} {}

      // TODO: clean solution
      /// Construct typed EntryKey.
      EntryKey( const std::type_index &t ) : type{t} {}

      /// virtual Entry path
      std::string path{""},
        /// Entry name
        name{""} ;
      /// number of memory instances
      std::size_t amt{0} ;
      /// Type of object stored in Entry.
      std::type_index type ;
      /// Status flags from Entry.
      Flag_t< 0 > flags{} ;
      /// unique number for Entry
      std::size_t hash{0} ;
    } ;

    /**
     *  @brief Data selection for the Entry to work properly.
     */
    struct Context {

      /// default constructor
      Context() = default ;

      /// constructor
      Context( const std::shared_ptr< MemLayout > &memLayout )
        : mem{memLayout} {}

      /// reference to Memory object. For editing and reading data.
      std::shared_ptr< MemLayout > mem{nullptr} ;
    } ;

    /// Base type for Entries. To avoid void pointer.
    class EntryBase {} ;

  } // end namespace book
} // end namespace marlin
