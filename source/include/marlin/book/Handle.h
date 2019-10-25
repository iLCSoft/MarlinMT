#pragma once

// -- std includes
#include <memory>
#include <functional>
#include <typeinfo>

// #include "ROOT/RHist.hxx"
#include "marlin/book/MemLayout.h"




namespace marlin::book {

  class BookStore;
  class MemLayout;

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


  } // end namespace marlin::book
