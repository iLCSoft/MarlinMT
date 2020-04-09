#pragma once

// -- std includes
#include  <filesystem>
#include  <memory>
#include  <string>
#include  <typeindex>

// -- MarlinBook includes
#include  "marlinmt/book/Flags.h"
#include  "marlinmt/book/Types.h"

namespace marlinmt {
  namespace book {
    // -- MarlinBook forward declaration
    class MemLayout ;

    /**
     *  @brief Data selection to identify and manage an Entry.
     */
    struct EntryKey {

      /// default constructor.
      EntryKey() = default ;

      /// Construct typed EntryKey.
      explicit EntryKey( const std::type_index &t ) : type{t} {}

      /// virtual Entry path
      std::filesystem::path path{""} ;
      /// number of memory instances
      std::size_t mInstances{0} ;
      /// Type of object stored in Entry.
      std::type_index type{typeid( void )} ;
      /// Status flags from Entry.
      Flag_t flags{} ;
      /// unique number for Entry
      std::size_t idx{std::numeric_limits<std::size_t>::max()} ;
    } ;

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
      EntryDataBase( EntryDataBase<void> && )           = delete ;
      EntryDataBase &operator=( EntryDataBase && )      = delete ;
      ~EntryDataBase()                                  = default ;

      // template<typename ... Args_t>
      // void book(BookStore & store, Args_t... args){
      // }
    } ;

    /**
     * @brief Container for data to construct and setup booked object.
     */
    template < class T, unsigned long long = 0>
    class EntryData : public EntryDataBase< void > {} ;

    /**
     *  @brief Data selection for the Entry to work properly.
     */
    struct Context {

      /// default constructor
      Context() = default ;

      /// constructor
      explicit Context( std::shared_ptr< MemLayout > memLayout, 
          std::size_t numInstances)
        : mem{std::move( memLayout )},
          nInstances{numInstances} {}

      /// reference to Memory object. For editing and reading data.
      std::shared_ptr< MemLayout > mem{nullptr} ;
      std::size_t nInstances{0};
    } ;

    /// Base type for Entries. To avoid void pointer.
    class EntryBase {} ;

  } // end namespace book
} // end namespace marlinmt

namespace std {
  template<>
  struct less<marlinmt::book::EntryKey> {
    bool operator()(
      const marlinmt::book::EntryKey& lh,
      const marlinmt::book::EntryKey& rh) const 
    {
      return lh.idx < rh.idx;
    }
  };
} // end namespace std

