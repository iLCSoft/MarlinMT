#pragma once

// -- std includes
#include <functional>
#include <optional>
#include <regex>
#include <string_view>
#include <typeindex>

// -- MarlinBook includes
#include "marlin/book/EntryData.h"

namespace marlin {
<<<<<<< HEAD
  namespace book {
    // -- MarlinBook forward declaration
    class EntryKey ;
    
    /**
     *  @brief wrapper class for an Entry filter function.
     */
    class Condition {

    public:
      using FilterFn_t = std::function< bool( const EntryKey & ) > ;

      /// default constructor.
      Condition() ;
      
      /// construct Condition from filter function.
      Condition( const FilterFn_t &filterFn ) ;

      /// copy constructor.
      Condition( const Condition & ) = default ;

      /**
       *  @brief test Condition on key.
       *  @return true if key matches the condition.
       */
      bool operator()( const EntryKey &key ) const { return _fiterFn( key ); }

      // FIXME: 'and' and 'or' are keywords in c++(and = &&, or = ||)
      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with. 
       *  @return condition which accepts a key
       *  when this \b and rhs are accepted this. 
       */
      Condition And( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) && rh( key ) ;
        } ) ;
      }
      
      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with.
       *  @return condition which accepts a key
       *  when this \b or rhs are accepted this.
       */
      Condition Or( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) || rh( key ) ;
        } ) ;
      }

      /**
       *  @brief creates a condition
       *  @return condition which accepts a key
       *  when this denied it.
       */
      Condition Not() const {
        return Condition( [fn = _fiterFn]( const EntryKey &key ) -> bool {
          return !fn( key ) ;
        } ) ;
      }

    private:
      /// actually filter function.
      FilterFn_t _fiterFn ;
    } ;

    /**
     *  @brief helper to create a Condition.
     */
    class ConditionBuilder {
    public:
      /// implicit cast to Condition.
      operator Condition() const ;
      
      /// construct new Condition.
      Condition condition() const ;

      /**
       *  @brief only accept when key.name is perfect match to name.
       *  @param name string which must perfect match key.name to accept key.
       *  @note when this is set, regex for the key.name will be ignored.
       */
      ConditionBuilder &setName( const std::string_view &name ) ;

      /**
       *  @brief only accept when key.name includes a match.
       *  @param rgx regular expression.
       *  @note only used when no prefect match for name is set.
       *  \see ConditionBuilder::setName(const std::string_view &name)
       */
      ConditionBuilder &setName( const std::basic_regex< char > &rgx ) ;

      /**
       *  @brief only accept when key.path is perfect match to path.
       *  @param path string which must perfect match key.path to accept key.
       *  @note when this is set, regex for key.path will be ignored.
       */
      ConditionBuilder &setPath( const std::string_view &path ) ;

      /**
       *  @brief only accept when key.path includes a match.
       *  @param rgx regular expression.
       *  @note only used when no perfect match for path is set.
       *  \see ConditionBuilder::setPath(const std::string_view &name)
       */
      ConditionBuilder &setPath( const std::basic_regex< char > &rgx ) ;
  
      /**
       *  @brief only accept when Entry type matches.
       *  @param type type_index of type to match.
       */
      ConditionBuilder &setType( const std::type_index &type ) ;

      /**
       *  @brief only accept when entry type matches.
       *  @tparam T type to match. 
       */
      template < typename T >
      ConditionBuilder &setType() {
        return setType( std::type_index( typeid( T ) ) ) ;
      }

    private:
      /// stores perfect match for name, when setted.
      std::optional< std::string >     _name{} ;
      /// stores name regex. Default matches every string. 
      std::basic_regex< char >         _rgxName{".*", std::regex::optimize} ;
      /// stores perfect match for path, when setted. 
      std::optional< std::string >     _path{} ;
      /// stores path regex. Default matches every string.
      std::basic_regex< char >         _rgxPath{".*", std::regex::optimize} ;
      /// stores type index when setted. 
      std::optional< std::type_index > _type{} ;
    } ;

  } // end namespace book
||||||| merged common ancestors
  namespace book {
    // -- MarlinBook forward declaration
    class EntryKey ;
    
    /**
     *  @brief wrapper class for an Entry filter function.
     */
    class Condition {

    public:
      using FilterFn_t = std::function< bool( const EntryKey & ) > ;

      /// default constructor.
      Condition() ;
      
      /// construct Condition from filter function.
      Condition( const FilterFn_t &filterFn ) ;

      /// copy constructor.
      Condition( const Condition & ) = default ;

      /**
       *  @brief test Condition on key.
       *  @return true if key matches the condition.
       */
      bool operator()( const EntryKey &key ) const { return _fiterFn( key ); }

      // FIXME: 'and' and 'or' are keywords in c++(and = &&, or = ||)
      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with. 
       *  @return condition which accepts a key
       *  when this \b and rhs are accepted this. 
       */
      Condition And( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) && rh( key ) ;
        } ) ;
      }
      
      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with.
       *  @return condition which accepts a key
       *  when this \b or rhs are accepted this.
       */
      Condition Or( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) || rh( key ) ;
        } ) ;
      }

      /**
       *  @brief creates a condition
       *  @return condition which accepts a key
       *  when this denied it.
       */
      Condition Not() const {
        return Condition( [fn = _fiterFn]( const EntryKey &key ) -> bool {
          return !fn( key ) ;
        } ) ;
      }

    private:
      /// actually filter function.
      FilterFn_t _fiterFn ;
    } ;

    /**
     *  @brief helper to create a Condition.
     */
    class ConditionBuilder {
    public:
      /// implicit cast to Condition.
      operator Condition() const ;
      
      /// construct new Condition.
      Condition condition() const ;

      /**
       *  @brief only accept when key.name is perfect match to name.
       *  @param name string which must perfect match key.name to accept key.
       *  @note when this is set, regex for the key.name will be ignored.
       */
      ConditionBuilder &setName( const std::string_view &name ) ;

      /**
       *  @brief only accept when key.name includes a match.
       *  @param rgx regular expression.
       *  @note only used when no prefect match for name is set.
       *  \see ConditionBuilder::setName(const std::string_view &name)
       */
      ConditionBuilder &setName( const std::basic_regex< char > &rgx ) ;

      /**
       *  @brief only accept when key.path is perfect match to path.
       *  @param path string which must perfect match key.path to accept key.
       *  @note when this is set, regex for key.path will be ignored.
       */
      ConditionBuilder &setPath( const std::string_view &path ) ;

      /**
       *  @brief only accept when key.path includes a match.
       *  @param rgx regular expression.
       *  @note only used when no perfect match for path is set.
       *  \see ConditionBuilder::setPath(const std::string_view &name)
       */
      ConditionBuilder &setPath( const std::basic_regex< char > &rgx ) ;
  
      /**
       *  @brief only accept when Entry type matches.
       *  @param type type_index of type to match.
       */
      ConditionBuilder &setType( const std::type_index &type ) ;

      /**
       *  @brief only accept when entry type matches.
       *  @tparam T type to match. 
       */
      template < typename T >
      ConditionBuilder &setType() {
        return setType( std::type_index( typeid( T ) ) ) ;
      }

    private:
      /// stores perfect match for name, when setted.
      std::optional< std::string >     _name{} ;
      /// stores name regex. Default matches every string. 
      std::basic_regex< char >         _rgxName{".*", std::regex::optimize} ;
      /// stores perfect match for path, when setted. 
      std::optional< std::string >     _path{} ;
      /// stores path regex. Default matches every string.
      std::basic_regex< char >         _rgxPath{".*", std::regex::optimize} ;
      /// stores type index when setted. 
      std::optional< std::type_index > _type{} ;
    } ;

  } // end namespace book
=======
  namespace book {
    // -- MarlinBook forward declaration
    class EntryKey ;

    /**
     *  @brief wrapper class for an Entry filter function.
     */
    class Condition {

    public:
      using FilterFn_t = std::function< bool( const EntryKey & ) > ;

      /// default constructor.
      Condition() ;

      /// construct Condition from filter function.
      Condition( const FilterFn_t &filterFn ) ;

      /// copy constructor.
      Condition( const Condition & ) = default ;

      /**
       *  @brief test Condition on key.
       *  @return true if key matches the condition.
       */
      bool operator()( const EntryKey &key ) const { return _fiterFn( key ); }

      // TODO: 'and' and 'or' are keywords in c++(and = &&, or = ||)
      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with.
       *  @return condition which accepts a key
       *  when this \b and rhs are accepted this.
       */
      Condition And( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) && rh( key ) ;
        } ) ;
      }

      /**
       *  @brief creates a composed condition.
       *  @param rhs condition to compose with.
       *  @return condition which accepts a key
       *  when this \b or rhs are accepted this.
       */
      Condition Or( const Condition &rhs ) const {
        return Condition( [lh = _fiterFn, rh = rhs]( const EntryKey &key ) {
          return lh( key ) || rh( key ) ;
        } ) ;
      }

      /**
       *  @brief creates a condition
       *  @return condition which accepts a key
       *  when this denied it.
       */
      Condition Not() const {
        return Condition( [fn = _fiterFn]( const EntryKey &key ) -> bool {
          return !fn( key ) ;
        } ) ;
      }

    private:
      /// actually filter function.
      FilterFn_t _fiterFn ;
    } ;

    /**
     *  @brief helper to create a Condition.
     */
    class ConditionBuilder {
    public:
      /// implicit cast to Condition.
      operator Condition() const ;

      /// construct new Condition.
      Condition condition() const ;

      /**
       *  @brief only accept when key.name is perfect match to name.
       *  @param name string which must perfect match key.name to accept key.
       *  @note when this is set, regex for the key.name will be ignored.
       */
      ConditionBuilder &setName( const std::string_view &name ) ;

      /**
       *  @brief only accept when key.name includes a match.
       *  @param rgx regular expression.
       *  @note only used when no prefect match for name is set.
       *  \see ConditionBuilder::setName(const std::string_view &name)
       */
      ConditionBuilder &setName( const std::basic_regex< char > &rgx ) ;

      /**
       *  @brief only accept when key.path is perfect match to path.
       *  @param path string which must perfect match key.path to accept key.
       *  @note when this is set, regex for key.path will be ignored.
       */
      ConditionBuilder &setPath( const std::string_view &path ) ;

      /**
       *  @brief only accept when key.path includes a match.
       *  @param rgx regular expression.
       *  @note only used when no perfect match for path is set.
       *  \see ConditionBuilder::setPath(const std::string_view &name)
       */
      ConditionBuilder &setPath( const std::basic_regex< char > &rgx ) ;

      /**
       *  @brief only accept when Entry type matches.
       *  @param type type_index of type to match.
       */
      ConditionBuilder &setType( const std::type_index &type ) ;

      /**
       *  @brief only accept when entry type matches.
       *  @tparam T type to match.
       */
      template < typename T >
      ConditionBuilder &setType() {
        return setType( std::type_index( typeid( T ) ) ) ;
      }

    private:
      /// stores perfect match for name, when setted.
      std::optional< std::string >     _name{} ;
      /// stores name regex. Default matches every string.
      std::basic_regex< char >         _rgxName{".*", std::regex::optimize} ;
      /// stores perfect match for path, when setted.
      std::optional< std::string >     _path{} ;
      /// stores path regex. Default matches every string.
      std::basic_regex< char >         _rgxPath{".*", std::regex::optimize} ;
      /// stores type index when setted.
      std::optional< std::type_index > _type{} ;
    } ;

  } // end namespace book
>>>>>>> Formatted code and add doxygen
} // end namespace marlin
