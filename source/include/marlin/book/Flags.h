#pragma once

// -- std header
#include <bitset>

namespace marlin {
	namespace book {

		/** Flag type for flags in marlin::book */
		template < unsigned long long INIT = 0 >
		class Flag_t {
			static constexpr std::size_t AmtFlags = 8 ;
			template < unsigned long long >
			friend class Flag_t ;

		public:
			static constexpr unsigned long long VAL_INIT = INIT ;

			/**
			 *	@brief constructor flag from number.
			 */
			constexpr Flag_t( unsigned long long val ) : _val{val} {}

			/**
			 *	@brief Construct flag from bitset.
			 */
			Flag_t( const std::bitset< AmtFlags > &val ) : _val{val} {}

			/**
			 *	@brief Default Constructor ;
			 */
			constexpr Flag_t() : _val{INIT} {} ;

			/**
			 *	@brief logical  AND on tow set of Flags.
			 */
			template < unsigned long long I >
			Flag_t operator&( const Flag_t< I > &f ) const {
				return Flag_t( _val & f._val ) ;
			}

			/**
			 *	@brief logical OR on tow set of Flags.
			 */
			template < unsigned long long I >
			Flag_t operator|( const Flag_t< I > &f ) const {
				return Flag_t( _val & f._val ) ;
			}

			/**
			 *	@brief cast to bool.
			 * 	@return true if any flag is set.
			 */
			operator bool() const { return _val.any(); }

			/**
			 *	@brief cast to bitset.
			 */
			operator const std::bitset< AmtFlags > &() const { return _val; }

			/**
			 *	@brief check for equality.
			 */
			template < unsigned long long I >
			bool operator==( const Flag_t< I > &f ) const {
				return _val == f._val ;
			}

			/**
			 *	@brief check for inequality.
			 */
			template < unsigned long long I >
			bool operator!=( const Flag_t< I > &f ) const {
				return _val != f._val ;
			}

			/**
			 *	@brief check if the flags is a subset of the other.
			 * 	@param flag which is may/mayn't the subset
			 *  @return true if f is a subset of this
			 */
			template < unsigned long long I >
			bool Contains( const Flag_t< I > &f ) const {
				return ( _val & f._val ) == f._val ;
			}

			template < unsigned long long I >
			Flag_t< INIT > &operator=( const Flag_t< I > &f ) {
				_val = f._val ;
				return *this ;
			}

		private:
			/// flags saved as bits
			std::bitset< AmtFlags > _val{0} ;
		} ;

		/// Flags for different purposes in marlin::book
		namespace Flags {
			/// flags for booking
			namespace Book {

				/// create multiple instances of booked object (if possible) to avoid
				/// sync points
				constexpr Flag_t< ( 1 << 2 ) > MultiCopy{} ;
				///	vanilla object.
				constexpr Flag_t< ( 1 << 0 ) > Single{} ;
				/// create one instance witch concurrent access.
				constexpr Flag_t< ( 1 << 1 ) > MultiShared{} ;
			} // end namespace Book

			/// flags for Permission handling in booking System
			namespace Permission {
				/// Booking is allowed
				constexpr Flag_t Booking{1 << 0} ;
				/// Filling is allowed
				constexpr Flag_t Filling{1 << 1} ;
				/// Merging is allowed
				constexpr Flag_t Merging{1 << 2} ;
			} // end namespace Permission
		} // end namespace Flags
	} // end namespace book
} // end namespace marlin
