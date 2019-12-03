#include "marlin/book/Serelizer.h"

// -- std includes
#include <filesystem>
#include <vector>
#include <unordered_map>

// -- MarlinBook includes
#include "marlin/book/Selection.h"
#include "marlin/book/Handle.h"
#include "marlin/book/Hist.h"
#include "marlin/book/Entry.h"

// -- histogram conversion include 
#include "marlin/book/RootHistV7ToV6Conversion.h"

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

template<typename T>  
void writeObject( TDirectory* file, const std::string& name, const T& obj) {

  auto root6Obj = into_root6_hist(
        obj,
        name.c_str()) ;

  file->WriteTObject(
      &root6Obj,
      name.c_str()
  ) ;
}
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
    std::string name = itr->string();
    TDirectory* next = dir->GetDirectory(name.c_str());
    dir = next == nullptr
      ? dir->mkdir(name.c_str())
      : next;
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



namespace marlin {
  namespace book {

    void ToRoot6::WriteSelection(
      const Selection             &selection
    ) {
      DirectoryMap dirs{};

      TFile* root = TFile::Open(_path.string().c_str(), "new");

      for(const WeakEntry& h : selection) {
        if(!h.valid()) { continue; }

        const EntryKey& key = h.key();
        const std::type_index type = key.type;
        TDirectory* file = getDirectory(dirs, root, key.path);

        if (type == std::type_index(typeid(types::RH1F))) {
          writeObject<types::RH1F>(file, key.path.filename().string(), h.bind<types::RH1F>().merged());
        } else if (type == std::type_index(typeid(types::RH1D))){ 
          writeObject<types::RH1D>(file, key.path.filename().string(), h.bind<types::RH1D>().merged());
        } else if (type == std::type_index(typeid(types::RH1I))){
          writeObject<types::RH1I>(file, key.path.filename().string(), h.bind<types::RH1I>().merged());
        } else if (type == std::type_index(typeid(types::RH2F))){ 
          writeObject<types::RH2F>(file, key.path.filename().string(), h.bind<types::RH2F>().merged());
        } else if (type == std::type_index(typeid(types::RH2D))){
          writeObject<types::RH2D>(file, key.path.filename().string(), h.bind<types::RH2D>().merged());
        } else if (type == std::type_index(typeid(types::RH2I))){ 
          writeObject<types::RH2I>(file, key.path.filename().string(), h.bind<types::RH2I>().merged());
        } else if (type == std::type_index(typeid(types::RH3F))){
          writeObject<types::RH3F>(file, key.path.filename().string(), h.bind<types::RH3F>().merged());
        } else if (type == std::type_index(typeid(types::RH3D))){ 
          writeObject<types::RH3D>(file, key.path.filename().string(), h.bind<types::RH3D>().merged());
        } else if (type == std::type_index(typeid(types::RH3I))){
          writeObject<types::RH3I>(file, key.path.filename().string(), h.bind<types::RH3I>().merged());
        } else {
          MARLIN_THROW_T( BookStoreException, "can't store object, no known operation");
        }
      }
      root->Close();
        
    }

  } // end namespace book
} // end namespace marlin
