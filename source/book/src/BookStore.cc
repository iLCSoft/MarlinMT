#include "marlin/book/BookStore.h"
// -- std includes
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <iostream>

// -- MarlinBook includes
#include "marlin/book/Condition.h"
#include "marlin/book/EntryData.h"
#include "marlin/book/Flags.h"
#include "marlin/book/Hist.h"
#include "marlin/book/MemLayout.h"
#include "marlin/book/ROOTAdapter.h"
#include "marlin/book/Selection.h"

namespace marlin {
  namespace book {
    using namespace types ;

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
        std::make_shared< SharedMemLayout< T, trait< T >::Merge, Args_t... > >(
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

    //--------------------------------------------------------------------------

#define BFN_H1( RET, NAME, TYPE, ... )                                         \
  template RET< TYPE >                                                         \
  BookStore::NAME< TYPE, const std::string_view &, const RAxisConfig & >(      \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const std::string_view &,                                                  \
    const RAxisConfig & );                                                     \
  template RET< TYPE > BookStore::NAME< TYPE, const RAxisConfig & >(           \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const RAxisConfig & )

#define LinkH1( TYPE )                                                         \
  BFN_H1( EntrySingle, bookSingle, TYPE, );                                    \
  BFN_H1( EntryMultiShared, bookMultiShared, TYPE, );                          \
  BFN_H1( EntryMultiCopy, bookMultiCopy, TYPE, std::size_t, )

#define BFN_H2( RET, NAME, TYPE, ... )                                         \
  template RET< TYPE > BookStore::NAME< TYPE,                                  \
                                        const std::string_view &,              \
                                        const RAxisConfig &,                   \
                                        const RAxisConfig & >(                 \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const std::string_view &,                                                  \
    const RAxisConfig &,                                                       \
    const RAxisConfig & );                                                     \
  template RET< TYPE >                                                         \
  BookStore::NAME< TYPE, const RAxisConfig &, const RAxisConfig & >(           \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const RAxisConfig &,                                                       \
    const RAxisConfig & )

#define LinkH2( TYPE )                                                         \
  BFN_H2( EntrySingle, bookSingle, TYPE, );                                    \
  BFN_H2( EntryMultiShared, bookMultiShared, TYPE, );                          \
  BFN_H2( EntryMultiCopy, bookMultiCopy, TYPE, std::size_t, )

#define BFN_H3( RET, NAME, TYPE, ... )                                         \
  template RET< TYPE > BookStore::NAME< TYPE,                                  \
                                        const std::string_view &,              \
                                        const RAxisConfig &,                   \
                                        const RAxisConfig &,                   \
                                        const RAxisConfig & >(                 \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const std::string_view &,                                                  \
    const RAxisConfig &,                                                       \
    const RAxisConfig &,                                                       \
    const RAxisConfig & );                                                     \
  template RET< TYPE > BookStore::NAME< TYPE,                                  \
                                        const RAxisConfig &,                   \
                                        const RAxisConfig &,                   \
                                        const RAxisConfig & >(                 \
    __VA_ARGS__ const std::string_view &,                                      \
    const std::string_view &,                                                  \
    const RAxisConfig &,                                                       \
    const RAxisConfig &,                                                       \
    const RAxisConfig & )

#define LinkH3( TYPE )                                                         \
  BFN_H3( EntrySingle, bookSingle, TYPE, );                                    \
  BFN_H3( EntryMultiShared, bookMultiShared, TYPE, );                          \
  BFN_H3( EntryMultiCopy, bookMultiCopy, TYPE, std::size_t, )

    LinkH1( RH1I ) ;
    LinkH1( RH1F ) ;
    LinkH1( RH1D ) ;
    LinkH2( RH2I ) ;
    LinkH2( RH2F ) ;
    LinkH2( RH2D ) ;
    LinkH3( RH3I ) ;
    LinkH3( RH3F ) ;
    LinkH3( RH3D ) ;

  } // end namespace book
} // end namespace marlin
