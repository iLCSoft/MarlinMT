#include "marlin/book/Condition.h"

// -- std includes
#include <iostream>
#include <string>


// -- MarlinBook includes
#include "marlin/book/EntryData.h"

namespace marlin {
	namespace book {
		///	@cond INTERNAL
		bool rgxEvaluation( const std::string_view &match,
		                    const std::string_view &str ) {
			return match == str ;
		}

		//--------------------------------------------------------------------------

		bool rgxEvaluation( const std::basic_regex< char > &rgx,
		                    const std::string_view &        str ) {
			return std::regex_search( str.begin(), str.end(), rgx ) ;
		}
		///	@endcond

		//--------------------------------------------------------------------------

		Condition::Condition()
		  : _fiterFn{[]( const EntryKey & ) { return true; }} {}

		//--------------------------------------------------------------------------

		Condition::Condition( const FilterFn_t &filterFn ) : _fiterFn{filterFn} {}

		//--------------------------------------------------------------------------

		ConditionBuilder::operator Condition() const { return condition(); }

		//--------------------------------------------------------------------------

		// FIXME: performance
		Condition ConditionBuilder::condition() const {
			typename Condition::FilterFn_t fn
			  = []( const EntryKey & ) { return true; } ;

			if ( _name ) {
				fn = [fn, rgx = _name.value()]( const EntryKey &e ) {
					return fn( e ) && rgxEvaluation( rgx, e.name ) ;
				} ;
			} else {
				fn = [fn, rgx = _rgxName]( const EntryKey &e ) {
					return fn( e ) && rgxEvaluation( rgx, e.name ) ;
				} ;
			}

			if ( _path ) {
				fn = [fn, rgx = _path.value()]( const EntryKey &e ) {
					return fn( e ) && rgxEvaluation( rgx, e.path ) ;
				} ;
			} else {
				fn = [fn, rgx = _rgxPath]( const EntryKey &e ) {
					return fn( e ) && rgxEvaluation( rgx, e.path ) ;
				} ;
			}

			if ( _type ) {
				fn = [fn, type = _type.value()]( const EntryKey &e ) {
					return fn( e ) && type == e.type ;
				} ;
			}

			return Condition( fn ) ;
		}

		//--------------------------------------------------------------------------

		ConditionBuilder &
		ConditionBuilder::setName( const std::string_view &name ) {
			_name = std::optional< std::string >( name ) ;
			return *this ;
		}

		//--------------------------------------------------------------------------

		ConditionBuilder &
		ConditionBuilder::setName( const std::basic_regex< char > &rgx ) {
			_rgxName = rgx ;
			return *this ;
		}

		//--------------------------------------------------------------------------

		ConditionBuilder &
		ConditionBuilder::setPath( const std::string_view &path ) {
			_path = std::optional< std::string >( path ) ;
			return *this ;
		}

		//--------------------------------------------------------------------------

		ConditionBuilder &
		ConditionBuilder::setPath( const std::basic_regex< char > &rgx ) {
			_rgxPath = rgx ;
			return *this ;
		}

		//--------------------------------------------------------------------------

		ConditionBuilder &ConditionBuilder::setType( const std::type_index &type ) {
			_type = std::optional< std::type_index >( type ) ;
			return *this ;
		}

	} // end namespace book
} // end namespace marlin
