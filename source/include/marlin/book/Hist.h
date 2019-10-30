#pragma once

#include <functional>
#include <iostream>
#include <vector>

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
  class EntryMultiShared;
  template <typename T>
  struct trait;
  

  template<int D, typename T, template<int, class>class ... STAT>
  struct trait<types::RH<D, T, STAT ... >> {
  static void Merge(
      const std::shared_ptr<types::RH<D, T, STAT ...>>& dst,
      const std::shared_ptr<types::RH<D, T, STAT ...>>& src) {
      Add(*dst, *src);
  }
  };



  template <int D, typename T, template<int, class>class ... STAT>
  class Handle<
    types::RH<D, T, STAT ...>> : private BaseHandle<types::RH<D, T, STAT ...>> {
    friend BookStore;

  public:
    using Type = types::RH<D, T, STAT ...>;
    using CoordArray_t = typename Type::CoordArray_t;
    using Weight_t = typename Type::Weight_t;
    using FillFn_t = std::function<void(const CoordArray_t&, const Weight_t&)>;
    using FillNFn_t = std::function<void(
      const std::span<CoordArray_t>&,
      const std::span<Weight_t>&)>;
    using FinalizeFn_t = std::function<void()>;

  public:
    Handle(
      const std::shared_ptr<MemLayout>& mem,
      const std::shared_ptr<Type>& obj,
      const FillFn_t& fillFn,
      const FillNFn_t& fillNFn,
      const FinalizeFn_t& finalFn);
    void fill(const CoordArray_t& x, const Weight_t& w);
    void fillN(
      const std::span<CoordArray_t>& x,
      const std::span<Weight_t>& w);
    const Type& merged();

  private:
    FillFn_t _fillFn;
    FillNFn_t _fillNFn;
    FinalizeFn_t _finalFn;
  };

  template<int D, typename T, template<int, class>class ... STAT>
  class EntrySingle<types::RH<D, T, STAT ... >> : public EntryBase {  
    
    friend BookStore;

  public:
    using Type = types::RH<D, T, STAT ... >;

    EntrySingle(const Context& context);

    EntrySingle() = default;

    Handle<Type> handle();
  
  private:
    Context _context;
  };

  template<int D, typename T, template<int, class>class ... STAT>
  class EntryMultiCopy<types::RH<D, T, STAT ... >> : public EntryBase {

    friend BookStore;

  public:
    using Type = types::RH<D, T, STAT...>;

    EntryMultiCopy(const Context& context);

    EntryMultiCopy() = default;

    Handle<Type> handle(std::size_t idx);
    
  
  private:
    Context _context;
  };

  template<int D, typename T, template<int, class>class ... STAT>
  class EntryMultiShared<types::RH<D, T, STAT ...>> : public EntryBase {
    friend BookStore;

  public:
    using Type = types::RH<D, T, STAT ...>;

    EntryMultiShared(const Context& context);

    // EntryMultiShared() = default;

    Handle<Type> handle();

    void flush();
  
  private:
    Context _context;
    std::shared_ptr<types::RHistConcurrentFillManager<Type>> _fillMgr;  
    std::vector<
      std::shared_ptr<types::RHistConcurrentFiller<Type>>> _fillers;
  };
}
