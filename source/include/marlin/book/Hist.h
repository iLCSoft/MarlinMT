#pragma once

#include <functional>
#include <iostream>

#include "marlin/book/EntryData.h"

#include "marlin/book/ROOTAdapter.h"

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
  

  template<int D, typename T, template<int, class>class ... STAT>
  struct trait<RH<D, T, STAT ... >> {
  static void Merge(
      const std::shared_ptr<RH<D, T, STAT ...>>& dst,
      const std::shared_ptr<RH<D, T, STAT ...>>& src) {
      Add(*dst, *src);
  }
  };



  template <int D, typename T, template<int, class>class ... STAT>
  class Handle<
    RH<D, T, STAT ...>> : public BaseHandle<RH<D, T, STAT ...>> {
    friend BookStore;

  public:
    using Type = RH<D, T, STAT ...>;
    using CoordArray_t = typename Type::CoordArray_t;
    using Weight_t = typename Type::Weight_t;
    using FillFn_t = std::function<void(const CoordArray_t&, const Weight_t&)>;

  public:
    Handle(
      const std::shared_ptr<MemLayout>& mem,
      const std::shared_ptr<Type>& obj,
      const FillFn_t& fillFn );
    void fill(const CoordArray_t& x, const Weight_t& w);

  private:
    FillFn_t _fillFn;
  };

  template<int D, typename T, template<int, class>class ... STAT>
  class EntrySingle<RH<D, T, STAT ... >> : public EntryBase { 
    
    friend BookStore;

  public:
    using Type = RH<D, T, STAT ... >;

    EntrySingle(const Context& context);

    EntrySingle() = default;

    Handle<Type> handle();
  
  private:
    Context _context;
  };

  template<int D, typename T, template<int, class>class ... STAT>
  class EntryMultiCopy<RH<D, T, STAT ... >> : public EntryBase {

    friend BookStore;

  public:
    using Type = RH<D, T, STAT...>;

    EntryMultiCopy(const Context& context);

    EntryMultiCopy() = default;

    Handle<Type> handle(std::size_t idx);
  
  private:
    Context _context;
  };
}
