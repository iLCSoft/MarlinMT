#pragma once

#include <memory>
#include <functional>

#include "marlin/book/Filler.h"
namespace marlin::book {
  template<class T>
  class Handle {
    std::size_t hash;
    std::shared_ptr<typename T::Filler> filler;
    std::function<const typename T::Type&(std::size_t)> get;
  public:
    Handle(
      const std::shared_ptr<Filler>& pFiller,
      std::size_t ihash,
      std::function<const typename T::Type&(std::size_t)> getFn)
      : hash{ihash}, filler{std::static_pointer_cast<typename T::Filler>(pFiller)}, get{getFn} {}

    const typename T::Type& Get() {
      return get(hash);
    };
    std::shared_ptr<typename T::Filler> operator->() {
      return filler;
    }
  };
}
