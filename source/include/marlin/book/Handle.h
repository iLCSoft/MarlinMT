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
    std::shared_ptr<MemLayout> _mem ;
    std::shared_ptr<T> _obj ; 

  protected:
    BaseHandle(
      const std::shared_ptr<MemLayout>& mem,
      const std::shared_ptr<T>& obj) 
        : _mem{mem},
          _obj{obj}{}
  
    T& get() { return *_obj; }
  public:

    const T& merged() {
      return *_mem->template merged<T>();
    }
  };

  template <typename T>
  class Handle : public BaseHandle<T> {
  public:
    Handle(
      const std::shared_ptr<T>& obj, 
      const std::shared_ptr<MemLayout>& mem)
    : BaseHandle<T>(obj, mem){}
  };


  } // end namespace marlin::book
