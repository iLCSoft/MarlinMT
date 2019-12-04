#include "marlin/book/Serializer.h"

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



namespace marlin {
  namespace book {

    void Root6SerializerStore::writeSelection(
      const Selection             &selection
    ) {
      DirectoryMap dirs{};

      TFile* root = TFile::Open(_path.string().c_str(), "new");

      for(const WeakEntry& h : selection) {
        if(!h.valid()) { continue; }

        const EntryKey& key = h.key();
        const std::type_index type = key.type;

        std::string path = std::filesystem::relative(key.path, "/").remove_filename().string();
        path.pop_back();
        root->mkdir(path.c_str());
        TDirectory *file = root->GetDirectory(path.c_str());
        if(file == nullptr) {
          throw std::string("failed create: ") + path + '\n';
        }
        if (type == std::type_index(typeid(types::RH1F))) {
          writeObject<types::RH1F>(file, key.path.filename().string(), h.handle<types::RH1F>().merged());
        } else if (type == std::type_index(typeid(types::RH1D))){ 
          writeObject<types::RH1D>(file, key.path.filename().string(), h.handle<types::RH1D>().merged());
        } else if (type == std::type_index(typeid(types::RH1I))){
          writeObject<types::RH1I>(file, key.path.filename().string(), h.handle<types::RH1I>().merged());
        } else if (type == std::type_index(typeid(types::RH2F))){ 
          writeObject<types::RH2F>(file, key.path.filename().string(), h.handle<types::RH2F>().merged());
        } else if (type == std::type_index(typeid(types::RH2D))){
          writeObject<types::RH2D>(file, key.path.filename().string(), h.handle<types::RH2D>().merged());
        } else if (type == std::type_index(typeid(types::RH2I))){ 
          writeObject<types::RH2I>(file, key.path.filename().string(), h.handle<types::RH2I>().merged());
        } else if (type == std::type_index(typeid(types::RH3F))){
          writeObject<types::RH3F>(file, key.path.filename().string(), h.handle<types::RH3F>().merged());
        } else if (type == std::type_index(typeid(types::RH3D))){ 
          writeObject<types::RH3D>(file, key.path.filename().string(), h.handle<types::RH3D>().merged());
        } else if (type == std::type_index(typeid(types::RH3I))){
          writeObject<types::RH3I>(file, key.path.filename().string(), h.handle<types::RH3I>().merged());
        } else {
          MARLIN_THROW_T( BookStoreException, "can't store object, no known operation");
        }
      }
      root->Close();
        
    }

  } // end namespace book
} // end namespace marlin
