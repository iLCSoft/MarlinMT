#include <UnitTesting.h>
#include <thread>

#include "marlin/book/configs/ROOTv7.h"
#include "marlin/book/BookStore.h"
#include "marlin/book/Handle.h"
#include "marlin/book/Hist.h"

using namespace marlin::book ;
using namespace marlin::book::types ;

std::string unicStr() {
  static std::size_t num = 0 ;
  return std::to_string( ++num ) ;
}

int main( int /*argc*/, char * /*argv*/[] ) {

  marlin::test::UnitTest test( " BookStore: Store/Fill " ) ;
  constexpr std::size_t  bins         = 3 ;
  constexpr float        min          = -1.F ;
  constexpr float        max          = 5.F ;
  constexpr int          nItrerations = 10 ;
  AxisConfig<double>            axis( "a", bins, min, max ) ;
  BookStore              store{} ;
  try {
    {
      Handle< Entry< H1F > > entry
        = store.book( "/path/", "name", EntryData< H1F >( axis ).single() ) ;

      Handle< H1F > hnd = entry.handle() ;
      std::vector< typename decltype( hnd )::Point_t> xs ;
      std::vector< typename decltype( hnd )::Weight_t >     ws ;
      for ( int i = 0; i < nItrerations; ++i ) {
        xs.emplace_back( typename decltype(hnd)::Point_t{1} ) ;
        ws.emplace_back( 1 ) ;
      }
      hnd.fillN( xs, ws ) ;

      auto hist = hnd.merged() ;
      test.test( "Single Hist Filling", hist.get().GetEntries() == nItrerations ) ;
    }
    {
      bool error = false ;
      try {
        Handle< Entry< H1F > > entry
          = store.book( "/path/", "name", EntryData< H1F >( axis ).single() ) ;
      } catch ( const exceptions::BookStoreException & ) {
        error = true ;
      }
      test.test( "No Double booking.", error ) ;
    }
    {
      Handle e = store.book(
        "/path/", unicStr(), EntryData< H1F >( "title", axis ).single() ) ;
      e.handle().fill( {0}, 1 ) ;
      test.test( "Named Histograms", e.handle().merged().get().GetEntries() == 1 ) ;
    }
    {
      Handle< Entry< H1I > > entry = store.book(
        "/path_2/", "name", EntryData< H1I >( axis ).multiCopy( 3 ) ) ;

      std::thread t1([&entry]() { 
          auto hnd = entry.handle(); 
          hnd.fill({0}, 1);});

      std::thread t2([&entry]() { 
          auto hnd = entry.handle(); 
          hnd.fill({0}, 1);});

      t1.join(); t2.join();

      auto hist = entry.handle().merged() ;
      test.test( "MultiCopy Hist Filling", hist.get().GetBinContent( {0} ) == 2 ) ;
    }
    {

      Handle< Entry< H1I > > entry = store.book(
        "/path_3/", "name", EntryData< H1I >( axis ).multiShared(2) ) ;

      std::thread t1([&entry]() { 
          auto hnd = entry.handle(); 
          hnd.fill({0}, 1);});

      std::thread t2([&entry]() { 
          auto hnd = entry.handle(); 
          hnd.fill({0}, 1);});

      t1.join(); t2.join();

      auto hist = entry.handle().merged() ;
      test.test( "MultiShared Hist Filling", hist.get().GetBinContent( {0} ) == 2 ) ;
    }
    {
      std::size_t n = store.find( ConditionBuilder() ).size() ;

      EntryData< H1F > entry1( axis ) ;
      EntryData< H2F > entry2( axis, axis ) ;
      EntryData< H3F > entry3( axis, axis, axis ) ;
      store.book( "/path/", unicStr(), entry1.single() ) ;
      store.book( "/path/", unicStr(), entry2.single() ) ;
      store.book( "/path/", unicStr(), entry3.single() ) ;
      store.book( "/path/", unicStr(), entry1.multiShared() ) ;
      store.book( "/path/", unicStr(), entry1.multiCopy( 3 ) ) ;

      std::size_t n2 = store.find( ConditionBuilder() ).size() ;

      test.test( "BookHelper usage", n + nItrerations / 2 == n2 ) ;
    }
    {
      bool        errorThrown = false ;
      std::thread t1( [&store, &errorThrown, &axis]() {
        try {
          store.book( "/path/", "name", EntryData< H1F >( axis ).single() ) ;
        } catch ( const exceptions::BookStoreException & ) {
          errorThrown = true ;
        }
      } ) ;
      t1.join() ;
      test.test( "prevent booking from other threads", errorThrown ) ;
    }
  } catch ( const exceptions::BookStoreException &excp ) {
    test.test( std::string( "Unexpected exception: '" ) + excp.what() + "'",
               false ) ;
  }
  return 0 ;
}
