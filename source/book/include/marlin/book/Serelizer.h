#pragma once

// -- std includes
#include <filesystem>


namespace marlin {
  namespace book {

    // -- MarlinBook forward declaration
    class Entry;
    class Selection;

    class ISerelizeStore {
    public:
      virtual void 
      WriteSelection  (
        const Selection             &sel
      ) = 0;
    };

    class Root6SerelizerStore : public ISerelizeStore {
    public:
      Root6SerelizerStore(std::filesystem::path path)
        : _path(std::move(path)){}
      Root6SerelizerStore() = default ;
      void WriteSelection (
        const Selection             &sel
      ) final;
    private:
      std::filesystem::path _path{""};
    };

  } // end namespace book
} // end namespace marlin

