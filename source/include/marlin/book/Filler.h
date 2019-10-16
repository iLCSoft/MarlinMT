#pragma once

#include <functional>

#include "ROOT/RSpan.hxx"
#include "marlin/book/Flags.h"

namespace marlin::book {
  class Filler {
  public:
    virtual void Flush() = 0;
    virtual bool IsModified() const = 0;
    virtual ~Filler() = default;
  };

  template<class T>
  class FillerHist : public Filler{
    using fill_t = std::function<void(const typename T::CoordArray_t&, const typename T::Weight_t&)>;
    fill_t fill;

    using fillN_t = std::function<void(const std::span<typename T::CoordArray_t>&,const std::span<typename T::Weight_t>&)>;
    fillN_t fillN;
    std::function<void(void)> flush;
    bool _modified = false;
  public:
    ~FillerHist() override = default;
    FillerHist(
      fill_t fillFn,
      fillN_t fillNFn,
      std::function<void(void)> flushFn
      ) : 
      fill { fillFn },
      fillN { fillNFn },
      flush { flushFn } {}  
    void Flush() override final {
      _modified = false;
      flush();
    }
    
    bool IsModified() const  override final {
      return _modified;
    }
    void Fill(const typename T::CoordArray_t& x, const typename T::Weight_t& w) {
      _modified = true;
      fill(x, w);     
    }
    void FillN(const std::span<typename T::CoordArray_t>& x, const typename std::span<typename T::Weight_t>& w) {
      _modified = true;
      fillN(x, w);
    }
  };
}
