#include <marlin/ReaderListener.h>
#include <marlin/RunHeader.h>
#include <marlin/EventStore.h>
#include <jenkinsHash.h>

#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>

namespace marlin {

  void ReaderListener::onEventRead( EventFunction func ) {
    _onEventRead = func ;
  }

  //--------------------------------------------------------------------------

  void ReaderListener::onRunHeaderRead( RunHeaderFunction func ) {
    _onRunHeaderRead = func ;
  }

  //--------------------------------------------------------------------------

  void ReaderListener::processEvent( std::shared_ptr<EVENT::LCEvent> event ) {
    if( nullptr != _onEventRead ) {
      auto store = std::make_shared<EventStore>() ;
      store->setEvent( event ) ;
      // generate the event unique id
      auto evtn = event->getEventNumber() ;
      auto runn = event->getRunNumber() ;
      unsigned char * c = (unsigned char *) &evtn ;
      unsigned int uid = jenkins_hash( c, sizeof evtn, 0) ;
      c = (unsigned char *) &runn ;
      uid = jenkins_hash( c, sizeof runn, 0) ;
      store->setUID( uid ) ;
      _onEventRead( store ) ;
    }
  }

  //--------------------------------------------------------------------------

  void ReaderListener::processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) {
    if( nullptr != _onRunHeaderRead ) {
      auto header = std::make_shared<RunHeader>() ;
      header->setRunNumber( rhdr->getRunNumber() ) ;
      header->setDetectorName( rhdr->getDetectorName() ) ;
      header->setDescription( rhdr->getDescription() ) ;
      auto subdets = rhdr->getActiveSubdetectors() ;
      header->parameters().add( "ActiveSubdetectors", *subdets ) ;
      auto &lcparams = rhdr->parameters() ;
      EVENT::StringVec intKeys, floatKeys, strKeys ;
      lcparams.getIntKeys( intKeys ) ;
      lcparams.getFloatKeys( floatKeys ) ;
      lcparams.getStringKeys( strKeys ) ;
      header->parameters().add( "LCIntKeys", intKeys ) ;
      header->parameters().add( "LCFloatKeys", floatKeys ) ;
      header->parameters().add( "LCStrKeys", strKeys ) ;
      for( auto key : intKeys ) {
        EVENT::IntVec values ;
        lcparams.getIntVals( key, values ) ;
        header->parameters().add( key, values ) ;
      }
      for( auto key : intKeys ) {
        EVENT::FloatVec values ;
        lcparams.getFloatVals( key, values ) ;
        header->parameters().add( key, values ) ;
      }
      for( auto key : intKeys ) {
        EVENT::StringVec values ;
        lcparams.getStringVals( key, values ) ;
        header->parameters().add( key, values ) ;
      }
      _onRunHeaderRead( header ) ;
    }
  }

}
