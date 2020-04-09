#pragma once

// -- std header
#include <bitset>

namespace marlin {
  namespace book {

    class Flag_t ;
    namespace Flags {
      constexpr unsigned long long value( const Flag_t &flag ) ;
    } // end namespace Flags

    /** Flag type for flags in marlin::book */
    class Flag_t {
      static constexpr std::size_t AmtFlags = 8 ;

    public:
      /**
       *  @brief constructor flag from number.
       */
      explicit constexpr Flag_t( unsigned long long val )
        : _val{val}, _initValue{val} {}

      /**
       *  @brief Construct flag from bitset.
       */
      explicit Flag_t( const std::bitset< AmtFlags > &val ) : _val{val} {}

      /**
       *  @brief bitwise not for Flags 
       */
      Flag_t operator~() const {
        return Flag_t(~_val);
      }

      /**
       *  @brief logical  AND on tow set of Flags.
       */
      Flag_t operator&( const Flag_t &f ) const {
        return Flag_t( _val & f._val ) ;
      }

      /**
       *  @brief logical OR on tow set of Flags.
       */
      Flag_t operator|( const Flag_t &f ) const {
        return Flag_t( _val | f._val ) ;
      }

      /**
       *  @brief cast to bool.
       *  @return true if any flag is set.
       */
      explicit operator bool() const { return _val.any(); }

      /**
       *  @brief cast to bitset.
       */
      explicit operator const std::bitset< AmtFlags > &() const { return _val; }

      /**
       *  @brief check for equality.
       */
      bool operator==( const Flag_t &f ) const { return _val == f._val; }

      /**
       *  @brief check for inequality.
       */
      bool operator!=( const Flag_t &f ) const { return _val != f._val; }

      /**
       *  @brief check if the flags is a subset of the other.
       *  @param flag which is may/mayn't the subset
       *  @return true if f is a subset of this
       */
      [[nodiscard]] bool contains( const Flag_t &f ) const {
        return ( _val & f._val ) == f._val ;
      }

      explicit operator unsigned long long() const { return _val.to_ullong(); }
      Flag_t()                 = default ;
      Flag_t( const Flag_t & ) = default ;
      Flag_t &operator=( const Flag_t &flag ) {
        _val = flag._val ;
        return *this ;
      }
      Flag_t( Flag_t && ) noexcept = default ;
      Flag_t &operator=( Flag_t &&flag ) noexcept {
        _val = flag._val ;
        return *this ;
      }
      ~Flag_t() = default ;

    private:
      /// flags saved as bits
      std::bitset< AmtFlags >  _val{0} ;
      const unsigned long long _initValue{0} ;
      friend constexpr unsigned long long
      Flags::value( const Flag_t & /*flag*/ ) ;
    } ;


    /// Flags for different purposes in marlin::book
    namespace Flags {
      constexpr unsigned long long value( const Flag_t &flag ) {
        return flag._initValue ;
      }
      /// flags for booking
      namespace Book {
        /// use default arguments
        constexpr Flag_t Default( 0 ) ;
        /// vanilla object.
        constexpr Flag_t Single( 1U << 0U ) ;
        /// create one instance witch concurrent access.
        constexpr Flag_t MultiShared( 1U << 1U ) ;
        /// create multiple instances of booked object (if possible) to avoid
        /// sync points
        constexpr Flag_t MultiCopy( 1U << 2U ) ;
        /// store object in file at end of lifetime
        constexpr Flag_t Store( 1U << 3U ) ;
      } // end namespace Book

    } // end namespace Flags

    /// Masks to filter certain Flags
    namespace Masks {
      /// Masks for the Book Flags
      namespace Book {
        /// Mask for Flags with memory layout options
        constexpr Flag_t MemoryLayout (
          Flags::value(Flags::Book::Single)
          | Flags::value(Flags::Book::MultiShared)
          | Flags::value(Flags::Book::MultiCopy)) ; 

        /// Mask for Flags with store option
        constexpr Flag_t StoreOptions(
          Flags::Book::Store) ;
      } // end namespace Book
      
    } // end namespace Masks

  } // end namespace book
} // end namespace marlin
