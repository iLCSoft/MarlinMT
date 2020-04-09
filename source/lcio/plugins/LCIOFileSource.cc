#ifndef MARLINMT_LCIOFILESOURCE_h
#define MARLINMT_LCIOFILESOURCE_h 1

#include <marlinmt/lcio/ReaderListener.h>

// -- marlinmt headers
#include <marlinmt/DataSourcePlugin.h>
#include <marlinmt/PluginManager.h>
#include <marlinmt/Logging.h>
#include <marlinmt/EventStore.h>
#include <marlinmt/RunHeader.h>
#include <jenkinsHash.h>

// -- lcio headers
#include <EVENT/LCEvent.h>
#include <EVENT/LCRunHeader.h>
#include <MT/LCReader.h>
#include <MT/LCReaderListener.h>

// -- std headers
#include <functional>

using namespace std::placeholders ;

namespace marlinmt {

  /**
   *  @brief  LCIOFileSource class
   */
  class LCIOFileSource : public DataSourcePlugin {
    using FileReader = MT::LCReader ;
    using FileReaderPtr = std::shared_ptr<FileReader> ;

  public:
    LCIOFileSource() ;
    ~LCIOFileSource() = default ;

    // from base
    void initialize() override ;
    bool readOne() override ;

  private:
    void onLCEventRead( std::shared_ptr<EVENT::LCEvent> event ) ;
    void onLCRunHeaderRead( std::shared_ptr<EVENT::LCRunHeader> rhdr ) ;

  private:
    StringVectorParameter _inputFileNames {*this, "LCIOInputFiles",
                "The list of LCIO input files" } ;

    IntParameter _maxRecordNumber {*this, "MaxRecordNumber",
                "The maximum number of records to read", 0 } ;

    IntParameter _skipNEvents {*this, "SkipNEvents",
                "The number of events to skip on file open", 0 } ;

    StringVectorParameter _readCollectionNames {*this, "LCIOReadCollectionNames",
                "An optional list of LCIO collections to read from event" } ;

    BoolParameter _lazyUnpack {*this, "LazyUnpack",
                "Set to true to perform a lazy unpacking after reading out an event", false } ;

    ///< The LCIO file listener
    ReaderListener              _listener {} ;
    ///< The LCIO file reader
    FileReaderPtr               _fileReader {nullptr} ;
    ///< The current number of read records
    int                         _currentReadRecords {0} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  LCIOFileSource::LCIOFileSource() :
    DataSourcePlugin("LCIOReader") {
    setDescription( "Read LCIO events and run headers from files on disk" ) ;
  }

  //--------------------------------------------------------------------------

  void LCIOFileSource::initialize() {
    DataSourcePlugin::initialize() ;
    auto flag = FileReader::directAccess ;
    if( _lazyUnpack ) {
      flag |= FileReader::lazyUnpack ;
    }
    _fileReader = std::make_shared<FileReader>( flag ) ;
    _listener.onRunHeaderRead( std::bind( &LCIOFileSource::processRunHeader, this, _1 ) ) ;
    _listener.onEventRead( std::bind( &LCIOFileSource::processEvent, this, _1 ) ) ;

    if( _inputFileNames.empty() ) {
      throw Exception( "LCIOFileSource::init: LCIO input file list is empty" ) ;
    }
    _fileReader->open( _inputFileNames ) ;
    if ( _skipNEvents > 0 ) {
      log<WARNING>() << " --- Will skip first " << _skipNEvents << " event(s)" << std::endl ;
      _fileReader->skipNEvents( _skipNEvents ) ;
    }
    if ( not _readCollectionNames.empty() ) {
      log<WARNING>()
        << " *********** Parameter LCIOReadCollectionNames given - will only read the following collections: **** "
        << std::endl ;
      for( auto collection : _readCollectionNames ) {
        log<WARNING>()  << "     " << collection << std::endl ;
      }
      log<WARNING>()
        << " *************************************************************************************************** " << std::endl ;
      _fileReader->setReadCollectionNames( _readCollectionNames ) ;
    }
  }

  //--------------------------------------------------------------------------

  bool LCIOFileSource::readOne() {
    try {
      _fileReader->readNextRecord( &_listener ) ;
      ++_currentReadRecords ;
      if( (_maxRecordNumber > 0) and (_currentReadRecords >= _maxRecordNumber) ) {
        return false ;
      }
    }
    catch( IO::EndOfDataException &e ) {
      return false ;
    }
    return true ;
  }

  MARLINMT_DECLARE_DATASOURCE_NAME( LCIOFileSource, "LCIOReader" )

}

#endif
