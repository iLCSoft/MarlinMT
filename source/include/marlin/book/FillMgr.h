#pragma once

#include <memory>
#include <functional>

#include "marlin/book/Flags.h"
#include "marlin/book/Filler.h"
#include "marlin/book/MemLayout.h"

#include "ROOT/RHistConcurrentFill.hxx"

namespace marlin::book {
  class BookStore;
  class FillMgr {
    friend BookStore;
    static constexpr std::size_t BufferSize = 1024;
  public:
    virtual 
    std::shared_ptr<Filler>
    createFiller(std::size_t idx) = 0;
    virtual ~FillMgr() = default;
  };

  template<class T>
  class FillMgrHist : public FillMgr {
    Flag_t _flags;
    std::shared_ptr<void> _filler;
  public:
    FillMgrHist(const std::shared_ptr<MemLayout>& layout, const Flag_t& flags) 
      : _flags{flags}, _filler{nullptr} {
      if(flags.Contains(Flags::Book::MultiInstance)) {
        _filler = layout;
      } else {
        _filler = std::make_shared<ROOT::Experimental::RHistConcurrentFillManager<T, BufferSize>>(
          *(layout->template at<T>(0)) // FIXME: depends on implimentation
        );
      }
    }
    std::shared_ptr<Filler>
    createFiller( std::size_t idx ) override final {
      if(!_flags.Contains(Flags::Book::MultiInstance))     {
        auto filler = std::make_shared<
          ROOT::Experimental::RHistConcurrentFiller<T, BufferSize>>
          (*std::static_pointer_cast<ROOT::Experimental::RHistConcurrentFillManager<T, BufferSize>>(_filler));
        auto fill = [filler = filler]
          (const typename T::CoordArray_t& x, const typename T::Weight_t& w){
            filler->Fill(x, w);
          };
        auto fillN = [filler = filler]
          (const std::span<typename T::CoordArray_t>& x, const std::span<typename T::Weight_t>& w) {
            filler->FillN(x, w);
          };
        auto flush = [filler = filler]
          () -> void{
          filler->Flush();
        };
        return std::make_shared<FillerHist<T>>(
          fill, fillN, flush);

      } else {
        auto filler = std::static_pointer_cast<MemLayout>(_filler)->template at<T>(idx);
        auto fill = [filler = filler] 
          (const typename T::CoordArray_t& x, const typename T::Weight_t& w) {
            filler->Fill(x, w);
          };
        auto fillN = [filler = filler]
          (const std::span<typename T::CoordArray_t>& x, const std::span<typename T::Weight_t>& w) {
            filler->FillN(x, w);
          };
        auto flush = [filler = filler]
          () {
        };
        return  std::make_shared<FillerHist<T>>(
          fill, fillN, flush);
      }
    }
  };
}
