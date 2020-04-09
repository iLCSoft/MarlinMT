// -- marlin headers
#include <marlin/Parameter.h>
#include <UnitTesting.h>

using namespace marlin::test ;
using namespace marlin ;

#define PASS_VALIDATOR( testname, param, value, mustPass ) \
  { \
    bool pass = true ; \
    try { param.set( value ) ; pass = true ; } \
    catch( Exception &e ) { std::cout << e.what() << std::endl ; pass = false ; } \
    test.test( testname , (pass == mustPass) ) ; \
  }

int main( int /*argc*/, char ** /*argv*/ ) {
  
  UnitTest test( "Validator" ) ;
  
  IntParameter iparam( "iparam", "An int parameter", 42 ) ;
  
  // standard passing tests
  PASS_VALIDATOR( "set_neg", iparam, -48511, true )
  PASS_VALIDATOR( "set_eps", iparam, std::numeric_limits<int>::epsilon(), true )
  PASS_VALIDATOR( "set_inf", iparam, std::numeric_limits<int>::infinity(), true )
  PASS_VALIDATOR( "set_pos", iparam, 75, true )
  
  // str conversion test
  test.test( "str_conv", iparam.str() == "75" ) ;
  test.test( "str_type", iparam.typeStr() == "int" ) ;
  test.test( "str_default", iparam.defaultStr() == "42" ) ;
    
  // validate only positive number
  iparam.setValidator( [](auto i){ return i>0 ; } ) ;
  PASS_VALIDATOR( "set_valid", iparam, 75, true )
  PASS_VALIDATOR( "set_unvalid0", iparam, 0, false )  
  PASS_VALIDATOR( "set_unvalid_neg", iparam, -1, false )
  
  // unset the validator must also work
  iparam.setValidator( nullptr ) ;
  PASS_VALIDATOR( "set_unset_validator", iparam, -1, true )
  
  // Test built-in validators
  iparam.setValidator( validator::greater( 12 ) ) ;
  PASS_VALIDATOR( "v_greater_12_pass", iparam, 13, true )
  PASS_VALIDATOR( "v_greater_12_fail", iparam, 10, false )
  PASS_VALIDATOR( "v_greater_12_eqfail", iparam, 12, false )
  
  iparam.setValidator( validator::greaterEqual( 67 ) ) ;
  PASS_VALIDATOR( "v_greatereq_67_pass", iparam, 785, true )
  PASS_VALIDATOR( "v_greatereq_67_fail", iparam, 21, false )
  PASS_VALIDATOR( "v_greatereq_67_eqpass", iparam, 67, true )
  
  iparam.setValidator( validator::less( 12 ) ) ;
  PASS_VALIDATOR( "v_less_12_pass", iparam, 9, true )
  PASS_VALIDATOR( "v_less_12_fail", iparam, 45, false )
  PASS_VALIDATOR( "v_less_12_eqfail", iparam, 12, false )
  
  iparam.setValidator( validator::lessEqual( 67 ) ) ;
  PASS_VALIDATOR( "v_lesseq_67_pass", iparam, 21, true )
  PASS_VALIDATOR( "v_lesseq_67_fail", iparam, 852, false )
  PASS_VALIDATOR( "v_lesseq_67_eqpass", iparam, 67, true )
  
  iparam.setValidator( validator::inRange( 10, 100 ) ) ;
  PASS_VALIDATOR( "v_range_10_100_pass", iparam, 67, true )
  PASS_VALIDATOR( "v_range_10_100_bound10", iparam, 10, true )
  PASS_VALIDATOR( "v_range_10_100_bound100", iparam, 100, true )
  PASS_VALIDATOR( "v_range_10_100_high", iparam, 102, false )
  PASS_VALIDATOR( "v_range_10_100_low", iparam, 2, false )
  
  // combinations of validators
  iparam.setValidator( validator::combineOr(
    validator::inRange( -100, -50 ), validator::inRange( 50, 100 ) 
  ) ) ;
  PASS_VALIDATOR( "v_or_range_pass_67", iparam, 67, true )
  PASS_VALIDATOR( "v_or_range_pass_n78", iparam, -78, true )
  PASS_VALIDATOR( "v_or_range_fail_n78", iparam, -780, false )
  PASS_VALIDATOR( "v_or_range_fail_0", iparam, 0, false )
  
  iparam.setValidator( validator::combineAnd(
    validator::greater( 50 ), validator::less( 100 ) 
  ) ) ;
  PASS_VALIDATOR( "v_and_range_pass_67", iparam, 67, true )
  PASS_VALIDATOR( "v_and_range_fail_10", iparam, 10, false )
  PASS_VALIDATOR( "v_and_range_fail_50", iparam, 50, false )
  PASS_VALIDATOR( "v_and_range_fail_150", iparam, 150, false )
  PASS_VALIDATOR( "v_and_range_fail_100", iparam, 100, false )
  
  
  FloatVectorParameter fvparam( "fvparam", "A float vector parameter", {45.f, 78.} ) ;
  fvparam.setValidator( validator::forEach( validator::notInfinity<float>() ) ) ;
  
  PASS_VALIDATOR( "fv_inf_fail", fvparam, {std::numeric_limits<float>::infinity()}, false )
  // contains a comma, not compatible with the macro ...
  auto args = {15.f, std::numeric_limits<float>::infinity()} ;
  PASS_VALIDATOR( "fv_inf_fail2", fvparam, args, false )
  
  fvparam.setValidator( validator::forEach( validator::greater<float>( 0.f ) ) ) ;
  args = {455., 10., 25.} ;
  PASS_VALIDATOR( "fv_positive_pass", fvparam, args, true )
  args = {-5., 10., 25.} ;
  PASS_VALIDATOR( "fv_positive_fail", fvparam, args, false )
  
  fvparam.setValidator( validator::notEmpty<FloatVectorParameter::ValueType>() ) ;
  args = {} ;
  PASS_VALIDATOR( "fv_notempty_fail", fvparam, args, false )
  args = {45., 78541.} ;
  PASS_VALIDATOR( "fv_notempty_pass", fvparam, args, true )
  
  StringParameter svparam( "sparam", "A string parameter" ) ;
  test.test( "no_default", not sparam.hasDefault() ) ;
  
  sparam.setValidator( validator::constraints( { "toto", "tata", "tutu" } ) ) ;
  PASS_VALIDATOR( "sp_contraints_pass", sparam, "toto", true )
  PASS_VALIDATOR( "sp_contraints_fail", sparam, "bibou", false )
  
  return 0 ;
}
