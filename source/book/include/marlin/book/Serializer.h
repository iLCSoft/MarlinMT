#pragma once

// -- std includes
#include <filesystem>


namespace marlin {
  namespace book {

    // -- MarlinBook forward declaration
    class Entry;
    class Selection;

    class ISerializerStore {
    public:
      virtual void 
      WriteSelection  (
        const Selection             &sel
      ) = 0;
    };

    class Root6SerializerStore : public ISerializerStore {
    public:
      Root6SerializerStore(std::filesystem::path path)
        : _path(std::move(path)){}
      Root6SerializerStore() = default ;
      void WriteSelection (
        const Selection             &sel
      ) final;
    private:
      std::filesystem::path _path{""};
    };

  } // end namespace book
} // end namespace marlin

