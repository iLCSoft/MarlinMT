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

    class ToRoot6 : public ISerelizeStore {
    public:
      ToRoot6(std::filesystem::path path)
        : _path(std::move(path)){}
      ToRoot6() = default ;
      void WriteSelection (
        const Selection             &sel
      ) final;
    private:
      std::filesystem::path _path{""};
    };

  } // end namespace book
} // end namespace marlin

