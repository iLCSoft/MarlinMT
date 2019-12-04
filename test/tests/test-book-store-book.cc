#include <UnitTesting.h>
#include <thread>

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
  RAxisConfig            axis( "a", bins, min, max ) ;
  BookStore              store{} ;
  try {
    {
      Handle< Manager< RH1F > > entry
        = store.book( "/path/", "name", EntryData< RH1F >( axis ).single() ) ;

      Handle< RH1F > hnd = entry.handle( 0 ) ;
      std::vector< typename decltype( hnd )::CoordArray_t > xs ;
      std::vector< typename decltype( hnd )::Weight_t >     ws ;
      for ( int i = 0; i < nItrerations; ++i ) {
        xs.emplace_back( 1 ) ;
        ws.emplace_back( 1 ) ;
      }
      hnd.fillN( xs, ws ) ;

      auto hist = hnd.merged() ;
      test.test( "Single Hist Filling", hist.GetEntries() == nItrerations ) ;
    }
    {
      bool error = false ;
      try {
        Handle< Manager< RH1F > > entry
          = store.book( "/path/", "name", EntryData< RH1F >( axis ).single() ) ;
      } catch ( const marlin::BookStoreException & ) {
        error = true ;
      }
      test.test( "No Double booking.", error ) ;
    }
    {
      Handle e = store.book(
        "/path/", unicStr(), EntryData< RH1F >( "title", axis ).single() ) ;
      e.handle( 1 ).fill( {0}, 1 ) ;
      test.test( "Named Histograms", e.handle( 1 ).merged().GetEntries() == 1 ) ;
    }
    {
      Handle< Manager< RH1I > > entry = store.book(
        "/path_2/", "name", EntryData< RH1I >( axis ).multiCopy( 2 ) ) ;

      auto hnd = entry.handle( 0 ) ;
      hnd.fill( {0}, 1 ) ;

      auto hnd2 = entry.handle( 1 ) ;
      hnd2.fill( {0}, 1 ) ;

      auto hist = hnd.merged() ;
      test.test( "MultiCopy Hist Filling", hist.GetBinContent( {0} ) == 2 ) ;
    }
    {

      Handle< Manager< RH1I > > entry = store.book(
        "/path_3/", "name", EntryData< RH1I >( axis ).multiShared() ) ;
      auto hnd = entry.handle( 1 ) ;
      hnd.fill( {0}, 1 ) ;

      auto hnd2 = entry.handle( 2 ) ;
      hnd2.fill( {0}, 1 ) ;

      auto hist = hnd.merged() ;
      test.test( "MultiShared Hist Filling", hist.GetBinContent( {0} ) == 2 ) ;
    }
    {
      std::size_t n = store.find( ConditionBuilder() ).size() ;

      EntryData< RH1F > entry1( axis ) ;
      EntryData< RH2F > entry2( axis, axis ) ;
      EntryData< RH3F > entry3( axis, axis, axis ) ;
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
          store.book( "/path/", "name", EntryData< RH1F >( axis ).single() ) ;
        } catch ( const marlin::BookStoreException & ) {
          errorThrown = true ;
        }
      } ) ;
      t1.join() ;
      test.test( "prevent booking from other threads", errorThrown ) ;
    }
  } catch ( const marlin::BookStoreException &excp ) {
    test.test( std::string( "Unexpected exception: '" ) + excp.what() + "'",
               false ) ;
  }
  return 0 ;
}
