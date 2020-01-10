#pragma once

// -- std includes
#include <filesystem>


namespace marlin {
  namespace book {

    // -- MarlinBook forward declaration
    class Entry;
    class Selection;

    class StoreWriter {
    public:
      explicit StoreWriter(std::filesystem::path path)
        : _path(std::move(path)){}
      StoreWriter() = default ;
      void writeSelection (
        const Selection             &sel
      ) ;
    private:
      std::filesystem::path _path{""};
    };



  } // end namespace book
} // end namespace marlin

