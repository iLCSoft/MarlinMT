#include <iostream>
#include <mutex>
#include "marlin/book.h"

struct RH1D;
struct HistHndFinalizer;

template<class HistT>
class HistHnd : public Hnd<HistT> {
  friend class BookStore;
  friend class HistHndFinalizer;

  std::shared_ptr<HistT> _pHist;
  BookStore::Entrie& _entrie;
  BookStore& _store;
  const BookStore::Flags _flags;

  HistHnd(
    std::shared_ptr<HistT>& hist,
    BookStore::Flags flags,
    BookStore::Entrie& entrie,
    BookStore& store)
    : _pHist{hist},
      _flags{flags},
      _entrie{entrie},
      _store{store}
  {}

  void
  Finalize() {
    _store.Finalize(_entrie);
  }

public:
  using CoordArray_t = typename HistT::CoordArray_t;
  using Weight_t = typename HistT::Weight_t;
  using Uncertainty_t = double;
  
  void
  Fill(CoordArray_t x) {
    if(static_cast<bool>(
      _flags & BookStore::Flags::MultiInstances)) {
      _pHist->Fill(x);
    } else {
      _pHist->Fill(x);
    }
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


template<>
class Hnd<RH1D> {
public:
  using Type = HistHnd<RH1D>;
};
