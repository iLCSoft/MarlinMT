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

namespace marlin::book {
  
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
        fillMgr->createFiller(idx, memory);
      }
    };
    std::unordered_multimap<std::size_t, std::shared_ptr<Filler>> _fillers;
  public:
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
      using Fill_t = typename book_trait<T>::Filler;
      using Value_t = typename book_trait<T>::Type;
>>>>>>> new book store iteration

      std::size_t hash = …;
      
      auto itrO = _objs.find(hash);
      if ( obj == _objs.end() ) {
        std::shared_ptr<FillMgr> fillMgr(nullptr);
        std::shared_ptr<MemLayout> memLayout(nullptr);

        if(flags.Contains(
          Flags::Book::MultiInstance))
        {
          memLayout = SingleMemLayout<
            Value_t,
            book_trait<T>::mearge,
            Args_t ... >(amt, ctor_p ...);
        } else {
          memLayout = SharedMemLayout<
            Value_t,
            Args_t ...>(ctor_p ...);
        }
        
        fillMgr = FillMgr_t(
          memLayout,
          flags
        );

        itrO = _objs.insert(std::make_pair(
          hash,
          FillableMemObj(
            fillMgr,
            memLayout
          )
        ))->first;
      }

      FillableMemObj& obj = itrO->second;
      std::shared_ptr<Filler> filler
        = obj.CreateFiller(idx);
      _fillers.insert(hash, filler);
      
      return Handle(filler, hash, *this);
    }
  };
  
  /** return finalized object with hash.
    * \attention only for internal usage
    * \param hash of the object in internal databasa
    */  
  template<typename T>
  const& T
  Read(std::size_t hash) {
    auto beItr = _fillers.equal_range(hash);
    bool modified = false;
    for(auto itr = beItr.first, itr != beItr.second; ++itr)
    {
      if(itr->second->IsModified()) {
        itr->second->Flush();
        modified = true;
      }
    }

    // TODO: mod flag for MemLayer
    return *_objs.find(hash)->second->merged();
  }
}
