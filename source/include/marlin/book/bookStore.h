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
#include <functional>
#include <iostream>

#include "marlin/book/Flags.h"
#include "marlin/book/util.h"
#include "marlin/book/Filler.h"
#include "marlin/book/FillMgr.h"
#include "marlin/book/Handle.h"

#include "ROOT/RHist.hxx"

namespace marlin::book {  
  template<class T>
  struct book_trait {
    using Filler = void;
    using Type = void;
    using FillMgr = void;   
    static void mearge(const std::shared_ptr<Type>&,const std::shared_ptr<Type>&) {
      std::cout << "notimplimented\n";
    }
  };

  class BookStore {
    struct FillableMemObj {
      FillableMemObj(
        const std::shared_ptr<FillMgr>& mgr,
        const std::shared_ptr<MemLayout>& mem
      ) : fillMgr{mgr}, memory{mem}
      {}      

      std::shared_ptr<FillMgr> fillMgr;
      std::shared_ptr<MemLayout> memory;

      std::shared_ptr<Filler>
      createFiller(std::size_t idx) {
        return fillMgr->createFiller(idx);
      }
    };
    std::unordered_map<std::size_t, FillableMemObj> _objs;
  public:
    BookStore() : _objs{}, _fillers{}{}
    /** return finalized object with hash.
      * \attention only for internal usage
      * \param hash of the object in internal databasa
      */  
    template<typename T>
    const T&
    Read(std::size_t hash) {
      bool modified = false;
      auto beItr = _fillers.equal_range(hash);
      for(auto itr = beItr.first; itr != beItr.second; ++itr)
      {
        if(itr->second->IsModified()) {
          itr->second->Flush();
          modified = true;
        }
      }

      // TODO: mod flag for MemLayer
      return *_objs.find(hash)->second.memory->merged<T>();
    }
    /** book Object and create Handle.
      * \tparam T object Type to be booked
      * \param name name of object
      * \param path to objcet
      * \param idx instance id
      * \param amt amount of instances
      * \param flags booking configuration Flags
        * \param ctor_p arguments to construct Object
      * \return handle for booked object
      */
    template<class T, typename ... Args_t>
    Handle<book_trait<T>>
    book(
      const std::string& name,
      const std::string& path,
      std::size_t idx,
      std::size_t amt, ///< amount of threads
      Flag_t flags,
      Args_t... ctor_p
    ) { 
      using FillMgr_t = typename book_trait<T>::FillMgr;
      using Value_t = typename book_trait<T>::Type;

      std::size_t hash = std::hash<std::string>{}(name + path);
      
      auto itrO = _objs.find(hash);
      if ( itrO == _objs.end() ) {
        std::shared_ptr<FillMgr> fillMgr(nullptr);
        std::shared_ptr<MemLayout> memLayout(nullptr);

        if(flags.Contains(
          Flags::Book::MultiInstance))
        {
          memLayout = std::make_shared<SingleMemLayout<
            Value_t,
        } else {
          memLayout = std::make_shared<SharedMemLayout<
            Value_t,
            book_trait<T>::mearge,
            Args_t ...>>(amt, ctor_p ...);
        }
        
        fillMgr = std::make_shared<FillMgr_t>(
          memLayout,
          flags
        );

        itrO = _objs.insert(std::make_pair(
          hash,
          FillableMemObj(
            fillMgr,
            memLayout
          )
        )).first;
      }

      FillableMemObj& obj = itrO->second;
      std::shared_ptr<Filler> filler
        = obj.createFiller(idx);
      _fillers.insert(std::make_pair(hash, filler));
      
      auto finalFn = [store = this](std::size_t idHash) -> const T&{
        return store->template Read<T>(idHash);
      };
    }
  
  };
}
