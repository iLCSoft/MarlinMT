#include "marlin/book/BookStore.h"
// -- std includes
#include <typeindex>
#include <typeinfo>

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

		template < class T >
		const BookHelper< T, 0 > BookStore::book( const std::string_view &path,
		                                          const std::string_view &name ) {
			return BookHelper< T, 0 >( *this, path, name ) ;
		}

		//--------------------------------------------------------------------------
		template < class T, typename... Args_t >
		EntrySingle< T > BookStore::bookSingle( const std::string_view &path,
		                                        const std::string_view &name,
		                                        Args_t... ctor_p ) {
			EntryKey key{std::type_index( typeid( T ) )} ;
			key.name  = name ;
			key.path  = path ;
			key.amt   = 1 ;
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
			key.amt    = n ;
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
			key.amt   = 1 ;
			key.flags = Flags::Book::MultiShared ;

			auto entry = std::make_shared< EntryMultiShared< T > >( Context(
			  std::make_shared< SingleMemLayout< T, Args_t... > >( ctor_p... ) ) ) ;

			addEntry( entry, key ) ;

			return *std::static_pointer_cast< const EntryMultiShared< T > >( entry ) ;
		}

		//--------------------------------------------------------------------------

#define LinkT( TYPE )                                                          \
	template const BookHelper< TYPE, 0 > BookStore::book< TYPE >(                \
	  const std::string_view &, const std::string_view & )

#define LinkH1( TYPE )                                                         \
	template EntrySingle< TYPE >                                                 \
	BookStore::bookSingle< TYPE, const RAxisConfig & >(                          \
	  const std::string_view &, const std::string_view &, const RAxisConfig & ); \
	template EntryMultiCopy< TYPE >                                              \
	BookStore::bookMultiCopy< TYPE, const RAxisConfig & >(                       \
	  std::size_t,                                                               \
	  const std::string_view &,                                                  \
	  const std::string_view &,                                                  \
	  const RAxisConfig & );                                                     \
	template EntryMultiShared< TYPE >                                            \
	BookStore::bookMultiShared< TYPE, const RAxisConfig & >(                     \
	  const std::string_view &, const std::string_view &, const RAxisConfig & ); \
	LinkT( TYPE )

#define LinkH2( TYPE )                                                         \
	template EntryMultiCopy< TYPE >                                              \
	BookStore::bookMultiCopy< TYPE, const RAxisConfig &, const RAxisConfig & >(  \
	  std::size_t,                                                               \
	  const std::string_view &,                                                  \
	  const std::string_view &,                                                  \
	  const RAxisConfig &,                                                       \
	  const RAxisConfig & );                                                     \
	template EntrySingle< TYPE >                                                 \
	BookStore::bookSingle< TYPE, const RAxisConfig &, const RAxisConfig & >(     \
	  const std::string_view &,                                                  \
	  const std::string_view &,                                                  \
	  const RAxisConfig &,                                                       \
	  const RAxisConfig & );                                                     \
	template EntryMultiShared< TYPE > BookStore::                                \
	  bookMultiShared< TYPE, const RAxisConfig &, const RAxisConfig & >(         \
	    const std::string_view &,                                                \
	    const std::string_view &,                                                \
	    const RAxisConfig &,                                                     \
	    const RAxisConfig & );                                                   \
	LinkT( TYPE )

#define LinkH3( TYPE )                                                         \
	template EntryMultiShared< TYPE >                                            \
	BookStore::bookMultiShared< TYPE,                                            \
	                            const RAxisConfig &,                             \
	                            const RAxisConfig &,                             \
	                            const RAxisConfig & >( 													 \
		const std::string_view &, 																								 \
	  const std::string_view &, 																								 \
	  const RAxisConfig &,      																								 \
	  const RAxisConfig &,      																								 \
	  const RAxisConfig & );    																								 \
	template EntryMultiCopy< TYPE >                                              \
	                             BookStore::bookMultiCopy< TYPE,                 \
                            const RAxisConfig &,                               \
                            const RAxisConfig &,                               \
                            const RAxisConfig & >(														 \
		 std::size_t,                																							 \
		 const std::string_view &,   																							 \
		 const std::string_view &,   																							 \
		 const RAxisConfig &,        																							 \
		 const RAxisConfig &,        																							 \
		 const RAxisConfig & );      																							 \
	template EntrySingle< TYPE > BookStore::bookSingle< TYPE,                    \
	                                                    const RAxisConfig &,     \
	                                                    const RAxisConfig &,     \
	                                                    const RAxisConfig & >(   \
	  const std::string_view &,                                                  \
	  const std::string_view &,                                                  \
	  const RAxisConfig &,                                                       \
	  const RAxisConfig &,                                                       \
	  const RAxisConfig & );                                                     \
	LinkT( TYPE )

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
