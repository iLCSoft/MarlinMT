#pragma once

// -- std headers
#include <type_traits>
#include <limits>
#include <cmath>
#include <functional>

namespace marlinmt {
  
  template <typename T>
  using ValidatorFunctionT = std::function<bool(const T &)> ;
  
  namespace validator {

    /**
     *  @brief  Validator function checking for non-infinite values.
     *  Works for scalar values where std::numeric_limits<T>::has_infinity is defined.
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> notInfinity() {
      static_assert( std::numeric_limits<T>::has_infinity, "Infinity not defined for this type" ) ;
      return [] (const T &value) { 
        return not std::isinf(value) ;
      };
    } 
    
    /**
     *  @brief  Apply a validator on each element of the vector.
     *  Only valid for vector type validation.
     * 
     *  @param  func the validator function to apply on each element of the vector
     */
    template <typename T>
    constexpr ValidatorFunctionT<std::vector<T>> forEach( ValidatorFunctionT<T> func ) {
      return [func=std::move(func)] (const std::vector<T> &value) {
        return ( value.end() == std::find_if_not(value.begin(), value.end(), func ) ) ;
      };
    } 
    
    /**
     *  @brief  Validator function checking for the value to be greater than a limit.
     *  Only valid is the operator > is defined for the input type
     * 
     *  @param  limit the limit value
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> greater( T limit ) {
      return  [limit=std::move(limit)] (const T &value) { 
        return value > limit ;
      };
    }
    
    /**
     *  @brief  Validator function checking for the value to be greater or equal to a limit.
     *  Only valid is the operator >= is defined for the input type
     * 
     *  @param  limit the limit value
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> greaterEqual( T limit ) {
      return  [limit=std::move(limit)] (const T &value) { 
        return value >= limit ;
      };
    }
    
    /**
     *  @brief  Validator function checking for the value to be less than a limit.
     *  Only valid is the operator < is defined for the input type
     * 
     *  @param  limit the limit value
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> less( T limit ) {
      return  [limit=std::move(limit)] (const T &value) { 
        return value < limit ;
      };
    }
    
    /**
     *  @brief  Validator function checking for the value to be less or equal to a limit.
     *  Only valid is the operator <= is defined for the input type
     * 
     *  @param  limit the limit value
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> lessEqual( T limit ) {
      return  [limit=std::move(limit)] (const T &value) { 
        return value <= limit ;
      };
    }
    
    /**
     *  @brief  Validator function checking for the value to be within a specific range, 
     *  boundaries included
     *  Only valid is the operator < is defined for the input type
     * 
     *  @param  low the lower boundary value
     *  @param  high the higer boundary value
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> inRange( T low, T high ) {
      return  [low=std::move(low),high=std::move(high)] (const T &value) { 
        return !(value < low) && !(high < value) ;
      };
    } 
    
    /**
     *  @brief  Validator function checking for the value to have 
     *  a size multiple of the input value
     *  Only valid if the input type has a method size() (containers, string, etc...) 
     * 
     *  @param  mod the modulo to apply
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> sizeModulo( std::size_t mod ) {
      return  [mod] (const T &value) { 
        return (value.size() % mod) != 0 ;
      };
    } 
    
    /**
     *  @brief  Validator function checking for the value to have an exact size.
     *  Only valid if the input type has a method size()
     * 
     *  @param  s the exact size to check
     */
    template <typename T>
    constexpr inline  ValidatorFunctionT<T> expectSize( std::size_t s ) {
      return  [s] (const T &value) { 
        return (value.size() == s) ;
      };
    }
    
    /**
     *  @brief  Validator function checking for the value to be non-empty.
     *  Only valid if the input type has a method empty()
     */
    template <typename T>
    constexpr inline  ValidatorFunctionT<T> notEmpty() {
      return  [] (const T &value) { 
        return not value.empty() ;
      };
    }
    
    /**
     *  @brief  Create a combination of two validator function with 'or' combination.
     *  Returns a new validator function containing the combination
     * 
     *  @param  lhs the first validator
     *  @param  rhs the second validator
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> combineOr( ValidatorFunctionT<T> lhs, ValidatorFunctionT<T> rhs ) {
      return [lhs=std::move(lhs),rhs=std::move(rhs)]( const T &value ) {
        return ( lhs(value) || rhs(value) ) ; 
      } ;
    }
    
    /**
     *  @brief  Create a combination of two validator function with 'and' combination.
     *  Returns a new validator function containing the combination
     * 
     *  @param  lhs the first validator
     *  @param  rhs the second validator
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> combineAnd( ValidatorFunctionT<T> lhs, ValidatorFunctionT<T> rhs ) {
      return [lhs=std::move(lhs),rhs=std::move(rhs)]( const T &value ) {
        return ( lhs(value) && rhs(value) ) ; 
      } ;
    }
    
    /**
     *  @brief  Create a validator checking for the value that is constrained to the input list
     * 
     *  @param  cnsts the input list of constraints
     */
    template <typename T>
    constexpr ValidatorFunctionT<T> constraints( std::set<T> cnsts ) {
      return [cnsts=std::move(cnsts)]( const T &value ) {
        return cnsts.find( value ) != cnsts.end() ; 
      } ;
    }

  }
  
} // end namespace marlinmt
