#include "marlin/book/BookStore.h"

// -- std includes
#include <filesystem>
#include <unordered_map>

// -- MarlinBook includes
#include "marlin/book/Handle.h"
#include "marlin/book/Hist.h"

// -- histogram conversion include 
#include "histConv.hpp"

// -- ROOT includes
#include "TDirectory.h"
#include "TFile.h"
#include "TDirectoryFile.h"

class PathHash {
public:
	std::size_t operator()(const std::filesystem::path& path) const {
		return std::filesystem::hash_value(path);
	}
};
using DirectoryMap = std::unordered_map<std::filesystem::path, TDirectory*, PathHash>;

namespace marlin {
	namespace book {
		template<typename T>	
		void writeEntry( TDirectory* file, const Entry& entry) {
			std::string name = entry.key().path.filename() ;

			auto obj = into_root6_hist(
						entry.handle<T>(0).merged(),
						name.c_str()) ;

			file->WriteTObject(
					&obj,
					name.c_str()
			) ;
		}

		std::shared_ptr< Entry >
		BookStore::addEntry( const std::shared_ptr< EntryBase > &entry,
		                     EntryKey                            key ) {
			key.hash = _entries.size() ;

			if ( !_idToEntry
			        .insert(
			          std::make_pair( Identifier( key.path ), key.hash ) )
			        .second ) {
				MARLIN_THROW_T( BookStoreException,
				                "Object already exist. Use store.book to avoid this." ) ;
			}
			_entries.push_back( std::make_shared< Entry >( Entry( entry, key ) ) ) ;
			return _entries.back() ;
		}
		
		//--------------------------------------------------------------------------
		
		TDirectory* mkdir(TDirectory* root, const std::filesystem::path& path) {
			TDirectory* dir = root;
			std::vector<std::filesystem::path> stack;
			std::filesystem::path
				p = path,
				parent = p.parent_path();
			while((p = parent).has_parent_path() && !((parent = p.parent_path()) == p)) {
				stack.emplace_back(std::filesystem::relative(p, parent));
			}

			for(auto itr = stack.crbegin(); itr != stack.crend(); ++itr) {
				dir = dir->mkdir(itr->string().c_str());
			}
			return dir;
		}

		TDirectory*
		getDirectory(DirectoryMap& dirs, 
				TDirectory* root,
				const std::filesystem::path& path) {
			auto hit = dirs.find(path.parent_path());
			if( hit != dirs.end()) {
				return hit->second;
			}
			
			return dirs.insert(std::make_pair(path.parent_path(), mkdir(root, path))).first->second;
		}

		std::filesystem::path BookStore::normalizeDirPath(const std::filesystem::path& path) {
			if(!path.is_absolute() || path.has_filename()) {
				MARLIN_THROW_T( BookStoreException, std::string("'") + path.string() + "' is not an absolute path to folder!");
			}
			return std::filesystem::absolute(path);
		}
		
		void BookStore::store(const std::filesystem::path& path) const {
			DirectoryMap dirs{};

			TFile* root = TFile::Open(path.string().c_str(), "new");

			for(const std::shared_ptr<Entry>& e : _entries) {
				TDirectory* file = getDirectory(dirs, root, e->key().path);

				if (e->key().type == std::type_index(typeid(types::RH1F))) {
					writeEntry<types::RH1F>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH1D))){	
					writeEntry<types::RH1D>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH1I))){
					writeEntry<types::RH1I>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH2F))){	
					writeEntry<types::RH2F>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH2D))){
					writeEntry<types::RH2D>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH2I))){	
					writeEntry<types::RH2I>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH3F))){
					writeEntry<types::RH3F>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH3D))){	
					writeEntry<types::RH3D>(file, *e);
				} else if (e->key().type == std::type_index(typeid(types::RH3I))){
					writeEntry<types::RH3I>(file, *e);
				} else {
					MARLIN_THROW_T( BookStoreException, "can't store object, no known operation");
				}
			}
			root->Close();
		}
		
		//--------------------------------------------------------------------------

		Selection BookStore::find( const Condition &cond ) {
			return Selection::find( _entries.cbegin(), _entries.cend(), cond ) ;
		}

		//--------------------------------------------------------------------------

		void BookStore::remove( const EntryKey &key ) { get( key ).clear(); }

		//--------------------------------------------------------------------------

		void BookStore::remove( const Selection &selection ) {
			for ( const Selection::Hit &e : selection ) {
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




	} // end namespace book
} // end namespace marlin

