#include <filesystem>
#include <string>
#include <span>

using Flag_t = unsigned char;

//! \exception ObjectNotFinalized the object is not in a valid state to perform the action.
//! \exception ExternelError the action results in an Exception from an extern lib.

template<class HistT>
class HistHnd {
public:
  using CoordArray_t = HistT::CoordArray_t;
  using Weight_t = HistT::Weight_t;
  using Uncertainty_t = double;
  constexpr Weight_t OneWeight = static_cast<HistT>(1);

  /** add one entrie to the histogram
     */
  void
  Fill(const CoordArray_t& x, const Weight_t& weight = static_cast<Weight_t>(1)) noexcept;

  /** add mutiple weighted entries to the histogram
      */
  void
  FillN(const std::span<CoordArray_t>& xN, const std::span<Weight_t> weightN) noexcept;


  /** added mutiple <b>one(1)</b> weighted entries to the histogram
      */
  void
  FillN(const std::span<CoordArray_t>& xN) noexcept;

  
  /** \return number of entries histogramm is filled with.
    * \throws ObjectNotFinalized when the function is called before the end of the Processors.
    */
  uint64_t
  GetEntries() const;

  /** \return the content of the Bin (sum of wheights)
      * \throws ObjectNotFinalized when the function is called before the end of the Processors.
    * \throws ExternelError when Root failed
    */
    Weight_t
  GetBinContent(const CoordArray_t& x) const;

  /** \return the uncertainty of the Bin
      * \throws ObjectNotFinalized when function is called before the end of the Processors.
      * \throws ExternelError when Root failed
      */
  Uncertainty_t
  GetBinUncertainty(const CoordArray_t& x) const;
};

/*! magaed and store booked objects */
class BookStore {
public:
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
  HisHnd<BookT>
  Book(
    const std::filesystem::path& path,
    const std::string& name,
    const Flags& flags = Flags::Default
  );
};


template<class BookT>
HisHnd<BookT>
BookStore::HisHnd<BookT>(
  const std::filesystem::path&,
  const std::string&,
  const Flags&
) {
  static_assert(true, "Can't book object of this Type!");
}

/*! \fn HisHnd<BookT> BookStore::Book(const std::filesystem::path& path, const std::string& name, const Flags& fglas = Flags::Default)
 * \note <b>supported Types</b>
 *  - RH1D
 *  - RH1F
 *  - RH1C
 *  - RH1I
 *  - RH1LL
 */
