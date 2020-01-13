#include "marlin/book/BookStore.h"

// -- std includes
#include <filesystem>
#include <unordered_map>

// -- MarlinBook includes
#include "marlin/book/Condition.h"
#include "marlin/book/Selection.h"
#include "marlin/book/StoreWriter.h"

class PathHash {
public:
  std::size_t operator()(const std::filesystem::path& path) const {
    return std::filesystem::hash_value(path);
  }
};

namespace marlin {
  namespace book {

    std::shared_ptr< Entry >
    BookStore::addEntry( const std::shared_ptr< EntryBase > &entry,
                         EntryKey                            key ) {
      key.hash = _entries.size() ;

      if ( !_idToEntry
              .insert(
                std::make_pair( Identifier( key.path ), key.hash ) )
              .second ) {
        MARLIN_BOOK_THROW( "Object already exist. Use store.book to avoid this." ) ;
      }
      _entries.push_back( std::make_shared< Entry >( Entry( entry, key ) ) ) ;
      return _entries.back() ;
    }
    
    //--------------------------------------------------------------------------
  
    void BookStore::store(StoreWriter& writer) const {
      writer.writeSelection(
        Selection::find(
          _entries.begin(),
          _entries.end(),
          ConditionBuilder())
      );  
    }
    
    //--------------------------------------------------------------------------

    Selection BookStore::find( const Condition &cond ) {
      return Selection::find( _entries.cbegin(), _entries.cend(), cond ) ;
    }

    //--------------------------------------------------------------------------

    void BookStore::remove( const EntryKey &key ) { get( key ).clear(); }

    //--------------------------------------------------------------------------

    void BookStore::remove( const Selection &selection ) {
      for ( const WeakEntry &e : selection ) {
        remove( e.key() ) ;
      }
    }

    //--------------------------------------------------------------------------

    void BookStore::clear() { _entries.resize( 0 ); }

    //--------------------------------------------------------------------------

    std::size_t BookStore::Identifier::Hash::
                operator()( const Identifier &id ) const {
      return std::filesystem::hash_value(id._path);
    }

    //--------------------------------------------------------------------------
    
    std::filesystem::path BookStore::normalizeDirPath(const std::filesystem::path& path) {
      if(!path.is_absolute() || path.has_filename()) {
        MARLIN_BOOK_THROW( std::string("'") 
            + path.string() 
            + "' is not an absolute path to folder!");
      }
      return std::filesystem::absolute(path);
    }


  } // end namespace book
} // end namespace marlin

