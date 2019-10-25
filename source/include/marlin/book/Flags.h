#pragma once

// -- std header
#include <bitset>

namespace marlin {
  namespace book {

    /** Flag type for flags in marlin::book */
    class Flag_t {
      static constexpr std::size_t AmtFlags = 8 ;

    public:
      
      /**
       *  @brief Consturct flag from number.
       */
      constexpr Flag_t(unsigned long long val) : _val {val} {}

      /**
       *  @brief Construct flag from bitset.
       */
      Flag_t(const std::bitset<AmtFlags>& val) : _val {val}{}

      /**
       *  @brief Default Constructor; 
       */
      constexpr Flag_t() : _val {0} {} ;
      /**
       *  @brief locical AND  on towe set of Flags.
       */
      Flag_t operator& (const Flag_t& f) const {
        return Flag_t(_val & f._val) ;
      }

      /**
       *  @brief lolical OR on towe set of Flags.
       */
      Flag_t operator| (const Flag_t& f) const {
        return Flag_t(_val & f._val) ;
      }

      /**
       *  @brief cast to bool.
       *  @return true if any flag is set.
       */
      operator bool() const {
        return _val.any() ;
      }

      /**
       *  @brief cast to bitset.
       */
      operator const std::bitset<AmtFlags>& () const {
        return _val ;
      }

      /**
       *  @brief check for equality.
       */
      bool operator==(const Flag_t& f) const {
        return _val == f._val ;
      }

      /**
       *  @brief check for inequality.
       */
      bool operator!=(const Flag_t& f) const {
        return _val != f._val ;
      }

      /**
       *  @brief check if the flags is a subset of the other.
       *  @param flag which is may/maynot the subset
       *  @return true if f is a subset of this
       */
      bool Contains(const Flag_t& f) const {
        return (_val & f._val) == f._val ;
      }

    private:
      /// flags saved as bits
      std::bitset<AmtFlags> _val {0} ;
    };

    /// Flags for differnt purpose in marlin::book
    namespace Flags { 

      /// flags for booking
      namespace Book {

        /// create mutible instances of booked object (if possible) to avoid sync points
        constexpr Flag_t Single( 1 << 0 ) ;
        constexpr Flag_t MultiShared( 1 << 1 );
        constexpr Flag_t MultiCopy( 1 << 2 );
      }

      /// flags for Permission handling in booking System
      namespace Permission {
        /// Bokking is allowed
        constexpr Flag_t Booking {1 << 0} ;
        /// Filling is allowed
        constexpr Flag_t Filling {1 << 1} ;
        /// Merging is allowed
        constexpr Flag_t Merging {1 << 2} ;
      }
    }
 }  
}
