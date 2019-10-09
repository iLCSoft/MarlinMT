#include <iostream>
#include <mutex>
#include <functional>
#include "marlin/book.h"

struct Filler {
  void Fill(int x) {
    std::cout << "fill filler: " <<  x << '\n';
  }
};

template<class HistT>
class HistHnd : public Hnd<HistT> {
  friend class BookStore;
  friend class HistHndFinalizer;

public:
  using CoordArray_t = typename HistT::CoordArray_t;
  using Weight_t = typename HistT::Weight_t;
  using Uncertainty_t = double;

private:  
  std::function<void(HistHnd&, CoordArray_t)> _fnFill;
  static void FillParallel(HistHnd& hnd, CoordArray_t x) {
    hnd._pHist->Fill(x);
  }
  static void FillSync(HistHnd& hnd, CoordArray_t x) {
    hnd._pFiller->Fill(x);
  }

  std::shared_ptr<HistT> _pHist;
  std::unique_ptr<Filler> _pFiller;
      
  BookStore::Entrie& _entrie;
  BookStore& _store;
  const BookStore::Flags _flags;

  HistHnd(
    std::shared_ptr<HistT>& hist,
    const BookStore::Flags flags,
    BookStore::Entrie& entrie,
    BookStore& store)
    : _pHist{hist},
      _flags{flags},
      _entrie{entrie},
      _store{store}
  {
    if (static_cast<bool>(
      flags & BookStore::Flags::MultiInstances)) {
      _fnFill = FillParallel;
    } else {
      _pFiller = std::make_unique<Filler>();
      _fnFill = FillSync;
    }
  }

  void
  Finalize() {
    _store.Finalize(_entrie);
  }
  

public:
  void
  Fill(CoordArray_t x) {
    _fnFill(*this, x);
  }

  int
  Get(CoordArray_t x) {
    if(_entrie.finalized) {
      return _pHist->Get(x);
    } else {
      Finalize(); 
      return Get(x); // TODO: problem??
    } 
  }
};


class RH1D;
template<> struct Hnd<RH1D> { using Type = HistHnd<RH1D>; };
class RH2D;
template<> struct Hnd<RH2D> { using Type = HistHnd<RH2D>; };

