#pragma once

#include <string>
#include <tuple>
#include <array>
#include <bitset>
#include <unordered_map>
#include <memory>
#include <utility>
#include <typeinfo>
#include <vector>

#include "marlin/book/Flags.h"
#include "marlin/book/hist.h"
#include "marlin/Exceptions.h"

namespace marlin {
  
  template<std::size_t DIM, typename T> 
  struct hnd_hist_trait{
    using Type = void;
  };

  class BookStore {
    using Key_t = std::string;
    struct Obj {
      std::size_t typeHash;
      Flag_t flags;
      std::shared_ptr<void> pObj;
      std::shared_ptr<void> pFillManager;
    };
    std::unordered_map<Key_t, Obj> _objs;
  public:
    BookStore() 
      : _objs{}
    {
    }
    using count_t = int;
    using calculation_t = double;
    using default_t = float;
    template<std::size_t DIM, typename T>
    using histCtor_t = std::array<
        std::tuple<
          std::string,
          count_t,
          calculation_t,
          calculation_t>,
        DIM>;

    template<std::size_t DIM, typename T>
    typename hnd_hist_trait<DIM, T>::Type
    bookHist(
      const std::string& name,
      const std::string& path,
      const histCtor_t<DIM, T>& ctor_p,
      const Flag_t& flags = BookFlags::Default) {
      static_assert(DIM == 1 || DIM == 2 || DIM == 3, "Only supports 1,2 and 3 Dimensions");

      using Hist_t = typename hnd_hist_trait<DIM, T>::Hist_t;
      using FillMgr_t = ROOT::Experimental::RHistConcurrentFillManager<Hist_t>;
      using Filler_t = ROOT::Experimental::RHistConcurrentFiller<Hist_t, FillerBufferSize>;
      const Key_t key = name + path;
  
      auto pair = _objs.insert(std::make_pair(key, Obj{}));
      bool bNewObj = pair.second;
      Obj& obj = pair.first->second;
      if (bNewObj) {
        std::shared_ptr<Hist_t> pHist = 
          std::static_pointer_cast<Hist_t>(NewHisto<DIM, T>(name, ctor_p));

        obj.typeHash = typeid(Hist_t).hash_code();
        obj.flags = flags;
        obj.pObj = pHist;
        if(!flags.Contains(BookFlags::MultiInstance)) {
          obj.pFillManager = std::make_shared<FillMgr_t>(*pHist);
        }
      } else {
        if(obj.typeHash != typeid(Hist_t).hash_code())
          MARLIN_THROW_T(BookStoreException, "object already booked with other type");
        if(obj.flags != flags)
          MARLIN_THROW_T(BookStoreException, "object already booked with other flags");
      }
      if (flags.Contains(BookFlags::MultiInstance)) {
        void_ptr filler = 
          std::make_shared<Hist_t>(*
            std::static_pointer_cast<Hist_t>(
              obj.pObj
            ));
        typename hnd_hist_trait<DIM, T>::Type res(
          flags,
          filler,
          obj.pObj,
          _mergeMgr,
          key);
        MergeMgr::MergeObj mobj{};
        mobj.src = filler;
        mobj.dst = obj.pObj;
        mobj.merge = res.GetMergeFunction();
        _mergeMgr->insert(std::make_pair(key, std::move(mobj)));
        return std::move(res);
      } else {
        std::shared_ptr<void> filler = 
          std::make_shared<Filler_t>(
            *std::static_pointer_cast<FillMgr_t>(
              obj.pFillManager));

        typename hnd_hist_trait<DIM, T>::Type res(
          flags,
          filler,
          obj.pObj,
          _mergeMgr,
          key);
        MergeMgr::MergeObj mobj{};
        mobj.src = filler;
        mobj.dst = obj.pObj;
        mobj.merge = res.GetMergeFunction(); 
        _mergeMgr->insert(std::make_pair(key, std::move(mobj)));
        return std::move(res);
      }
    }
    
  private:
    template<std::size_t DIM, typename T>
    std::shared_ptr<void>
    NewHisto(
      const std::string& name,
      const histCtor_t<DIM, T>& ctor_p) {
      using namespace ROOT::Experimental;
      using Hist_t = typename hnd_hist_trait<DIM, T>::Hist_t;
        if constexpr (DIM == 1) {
          return std::make_shared<Hist_t>(
            name,
            RAxisConfig(
              std::get<0>(ctor_p[0]),
              std::get<1>(ctor_p[0]),
              std::get<2>(ctor_p[0]),
              std::get<3>(ctor_p[0])));
        } else if constexpr (DIM == 2) {
          return std::make_shared<Hist_t>(
            name,
            RAxisConfig(
              std::get<0>(ctor_p[0]),
              std::get<1>(ctor_p[0]),
              std::get<2>(ctor_p[0]),
              std::get<3>(ctor_p[0])),
            RAxisConfig(
              std::get<0>(ctor_p[1]),
              std::get<1>(ctor_p[1]),
              std::get<2>(ctor_p[1]),
              std::get<3>(ctor_p[1])));
        } else if constexpr (DIM == 3) {
          return std::make_shared<Hist_t>(
            name,
            RAxisConfig(
              std::get<0>(ctor_p[0]),
              std::get<1>(ctor_p[0]),
              std::get<2>(ctor_p[0]),
              std::get<3>(ctor_p[0])),
            RAxisConfig(
              std::get<0>(ctor_p[1]),
              std::get<1>(ctor_p[1]),
              std::get<2>(ctor_p[1]),
              std::get<3>(ctor_p[1])),
            RAxisConfig(
              std::get<0>(ctor_p[2]),
              std::get<1>(ctor_p[2]),
              std::get<2>(ctor_p[2]),
              std::get<3>(ctor_p[2])));
        }
    }
  };
}
