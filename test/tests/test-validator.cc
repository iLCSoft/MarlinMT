// -- marlin headers
#include <marlin/Parameter.h>
#include <UnitTesting.h>

using namespace marlin::test ;
using namespace marlin ;

int main( int /*argc*/, char ** /*argv*/ ) {
  
  UnitTest test( "Validator" ) ;
  
  IntParameter iparam( "iparam", "An int parameter", 42 ) ;
  
  bool setTestPassed = false ;
  try {
    iparam.set( 75 ) ;
    iparam.set( -48511 ) ;
    iparam.set( std::numeric_limits<int>::epsilon() ) ;
    iparam.set( std::numeric_limits<int>::infinity() ) ;
    setTestPassed = true ;
  }
  catch( Exception &e ) {
  }
  test.test( "set", setTestPassed ) ;
  
  // validate only positive number
  iparam.setValidator( [](auto i){ return i>0 ; } ) ;
  
  try {
    iparam.set( 75 ) ;
    setTestPassed = true ;
  }
  catch( Exception &e ) {
    setTestPassed = false ;
  }
  test.test( "set_valid", setTestPassed ) ;
  
  try {
    iparam.set( 0 ) ;
    setTestPassed = true ;
  }
  catch( Exception &e ) {
    setTestPassed = false ;
  }
  test.test( "set_unvalid0", not setTestPassed ) ;
  
  try {
    iparam.set( -1 ) ;
    setTestPassed = true ;
  }
  catch( Exception &e ) {
    setTestPassed = false ;
  }
  test.test( "set_unvalid_neg", not setTestPassed ) ;
  
  // unset the validator must also work
  iparam.setValidator( nullptr ) ;
  
  try {
    iparam.set( -1 ) ;
    setTestPassed = true ;
  }
  catch( Exception &e ) {
    setTestPassed = false ;
  }
  test.test( "set_unset_validator", setTestPassed ) ;

  return 0 ;
}
