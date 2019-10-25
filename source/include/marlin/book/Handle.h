#pragma once

// -- std includes
#include <memory>
#include <functional>
#include <typeinfo>

// #include "ROOT/RHist.hxx"
#include "marlin/book/MemLayout.h"

namespace ROOT::Experimental {
  template<int, class>
  class RHistStatContent;

  template<int, class>
  class RHistStatUncertainty;

  template<int, class, template<int, class> class ...>
  class RHist;
}



namespace marlin::book {

  class BookStore;

  template<typename T, int D>
  using RH = ROOT::Experimental::RHist<
    D,
    T, 
    ROOT::Experimental::RHistStatContent,
    ROOT::Experimental::RHistStatUncertainty>;


  template <typename T>
  class BaseHandle {
    std::shared_ptr<MemLayout> _obj ;

  protected:
    BaseHandle(const std::shared_ptr<MemLayout>& obj) : _obj{obj}{}

  public:

    const T& get() {
      return *_obj->merged<T>();
    }
  };

  template <typename T>
  class Handle : public BaseHandle<T> {
  public:
    Handle(const std::shared_ptr<MemLayout>& obj)
    : BaseHandle<T>(obj){}
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
    Handle(std::shared_ptr<MemLayout> obj, const FillFn_t& fillFn)
      :  BaseHandle<RH<T,D>>{obj}, _fillFn{fillFn}
    {}
    void fill(const CoordArray_t& x, const Weight_t& w) {
      _fillFn(x, w);
    }

  private:
    FillFn_t _fillFn;
  };
} // end namespace marlin::book
