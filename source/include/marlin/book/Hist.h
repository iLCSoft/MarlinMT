#pragma once

#include <functional>
#include <iostream>

#include "marlin/book/EntryData.h"


#include "ROOT/RHist.hxx"
namespace ROOT::Experimental {
  class EntryBase;
  template<int, class>
  class RHistStatContent;

  template<int, class>
  class RHistStatUncertainty;

  template<int, class, template<int, class> class ...>
  class RHist;
}

namespace marlin::book {
  class BookStore;
  template <typename T>
  class BaseHandle;
  template <typename T>
  class Handle;
  template <typename T>
  class EntrySingle;
  template <typename T>
  class EntryMultiCopy;
  template <typename T>
  struct trait;
  
  template<typename T, int D>
  using RH = ROOT::Experimental::RHist<
    D,
    T, 
    ROOT::Experimental::RHistStatContent,
    ROOT::Experimental::RHistStatUncertainty>;

  using H1F = RH<float, 1>;
  using H1I = RH<int, 1>;


  template<typename T, int D>
  struct trait<RH<T,D>> {
  static void Merge(const std::shared_ptr<RH<T, D>>& dst, const std::shared_ptr<RH<T,D>>& src) {
      ROOT::Experimental::Add(*dst, *src);
  }
  };



  template <typename T, int D>
  class Handle<
    RH<T, D>> : public BaseHandle<RH<T, D>> {
    friend BookStore;

  public:
    using Type = RH<T, D>;
    using CoordArray_t = typename Type::CoordArray_t;
    using Weight_t = typename Type::Weight_t;
    using FillFn_t = std::function<void(const CoordArray_t&, const Weight_t&)>;

  public:
    Handle(std::shared_ptr<MemLayout> obj, const FillFn_t& fillFn);
    void fill(const CoordArray_t& x, const Weight_t& w);

  private:
    FillFn_t _fillFn;
  };

  template<typename T, int D>
  class EntrySingle<RH<T, D>> : public EntryBase {  
  public:
    using Type = RH<T, D>;

    EntrySingle(const Context& context);

    EntrySingle() = default;

    Handle<Type> handle();
  
  private:
    Context _context;
  };

  template<typename T, int D>
  class EntryMultiCopy<RH<T, D>> : public EntryBase {
  public:
    using Type = RH<T, D>;

    EntryMultiCopy(const Context& context);

    EntryMultiCopy() = default;

    Handle<Type> handle(std::size_t idx);
  
  private:
    Context _context;
  };
}
