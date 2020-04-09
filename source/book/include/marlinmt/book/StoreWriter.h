#pragma once

// -- std includes
#include <filesystem>


namespace marlinmt {
  namespace book {

    // -- MarlinBook forward declaration
    namespace details {
      class Entry;
    }
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
} // end namespace marlinmt

