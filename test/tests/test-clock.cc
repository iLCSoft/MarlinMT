// -- marlinmt headers
#include <marlinmt/Utils.h>
#include <UnitTesting.h>

#include <future>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>

using namespace marlinmt::test ;


int main( int /*argc*/, char ** /*argv*/ ) {
  
  UnitTest test( "Clock" ) ;
  test.test( "obvious", true ) ;

  std::vector<std::future<marlinmt::clock::duration_rep>> futures ;
  std::atomic<marlinmt::clock::duration_rep> timediff {0.f} ;
  auto start = marlinmt::clock::now() ;
  const marlinmt::clock::duration_rep crunchTime = 1 ; 
  
  for( auto i=0u ; i<std::thread::hardware_concurrency() ; ++i ) {
    futures.push_back( std::async( std::launch::async, [&](){
      auto localStart = marlinmt::clock::now() ;
      marlinmt::clock::crunchFor<marlinmt::clock::seconds>(crunchTime) ;
      auto localEnd = marlinmt::clock::now() ;
      auto localDiff = marlinmt::clock::time_difference<marlinmt::clock::milliseconds>( localStart, localEnd ) ;
      // the last task to exit set the total time diff
      timediff = marlinmt::clock::elapsed_since<marlinmt::clock::milliseconds>( start ) ;
      return localDiff ;
    })) ;
  }
  
  float timetot = 0.f ;
  for( auto &f : futures ) {
    timetot += f.get() ;
  }
  
  std::cout << "Accumulated time: " << timetot << " ms" << std::endl ;
  std::cout << "Local time diff:  " << timediff << " ms" << std::endl ;
    
  return 0 ;
}
