#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <mutex>

#include "marlin/Exceptions.h"

MARLIN_DEFINE_EXCEPTION( BookException );

// may in uttility
using Flag_t = unsigned char;

//! \exception ObjectNotFinalized the object is not in a valid state to perform the action.
//! \exception ExternelError the action results in an Exception from an extern lib.

struct ObjectNotFinalized {
  const char* message;
};

/*! Maps BookT to Handle Type */
template<class BookT>
class Hnd {
public:
  using Type = void;
};


/*! magaed and store booked objects */
class BookStore {
public:
  enum struct Flags : Flag_t;
  enum struct State {Init, Processing, End} _state;
  using Count_t = unsigned char;
  using size_t = unsigned int;
private:
  struct Entrie {
    Count_t nrHistInstances;
    size_t begin;
    Flags flags;
    std::size_t typeHash;
    bool finalized;
  };
public:
  using EntrieMap = std::unordered_map
    <std::string,Entrie>;
private:
  const Count_t _maxInstances;

  EntrieMap  _pathToHist;

  std::vector<std::shared_ptr<void>> _hists;

  
  EntrieMap::iterator
  AddEntrie(
    const std::string& path,
    const Flags& flags,
    std::size_t typeHash);

public:
  void Finalize(Entrie& entrie) {};
  BookStore(Count_t maxInstances)
    : _maxInstances{maxInstances},
      _state{State::Init}
  {}
  void SetState(const State& state) { _state = state; }

  
  /** Modification Flags for Booking
   * \note not every Flag have for every Object a meaning
   */
  enum struct Flags : Flag_t {
    MultiInstances = 1 << 0, ///< use more memory to avoid mutex
    Default = 0b1
  };

  /** Book Object and return a Handle to it, 
     * if Object not alreadey exist, create a new one
   * \param path booking path for the Object
     * \param name of the instance
     * \param flags Flag to control the behavior
     */
  template<class BookT>
  typename Hnd<BookT>::Type
  Book(
    const std::string& path,
    const Flags& flags = Flags::Default
  ) {
    static_assert(true, "Can't book object of this Type!");
  }

};


inline BookStore::Flags
operator|(const BookStore::Flags& l, const BookStore::Flags& r) {
    return static_cast<BookStore::Flags>(static_cast<Flag_t>(l) | static_cast<Flag_t>(r));
 }
inline BookStore::Flags&
operator|=(BookStore::Flags& l, const BookStore::Flags& r) {
  l = static_cast<BookStore::Flags>(
    static_cast<Flag_t>(l) | static_cast<Flag_t>(r)
  );
  return l;
}
inline BookStore::Flags
operator&(const BookStore::Flags& l, const BookStore::Flags& r) {
  return static_cast<BookStore::Flags>(
    static_cast<Flag_t>(l) & static_cast<Flag_t>(r));

}

/*! \fn Hnd<BookT> BookStore::Book(const std::experimental::filesystem::path& path, const std::string& name, const Flags& fglas = Flags::Default)
 * \note <b>supported Types</b>
 *       - RH1D
 *     - RH1F
 *     - RH1C
 *     - RH1I
 *     - RH1LL
 */
