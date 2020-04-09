#ifndef MARLINMT_DATASOURCEPLUGIN_h
#define MARLINMT_DATASOURCEPLUGIN_h 1

// -- marlinmt headers
#include <marlinmt/Component.h>
#include <marlinmt/Logging.h>

// -- std headers
#include <functional>
#include <memory>
#include <string>

namespace marlinmt {

  class Application ;
  class EventStore ;
  class RunHeader ;

  /**
   *  @brief  DataSourcePlugin class
   *  Responsible for reading/getting LCEvent and LCRunHeader
   *  in the framework for further processing
   */
  class DataSourcePlugin : public Component {
  public:
    using EventFunction = std::function<void(std::shared_ptr<EventStore>)> ;
    using RunHeaderFunction = std::function<void(std::shared_ptr<RunHeader>)> ;

  public:
    virtual ~DataSourcePlugin() = default ;

    /**
     *  @brief  Constructor
     *
     *  @param  dstype the data source plugin type
     */
    DataSourcePlugin( const std::string &dstype ) ;

    /**
     *  @brief  Get the data source type
     */
    const std::string &type() const ;

    /**
     *  @brief  Get the data source description
     */
    const std::string &description() const ;

    /**
     *  @brief  Read one record from the input stream
     *
     *  Users must call processRunHeader() or processEvent()
     *  to forward it to the framework. Returns true on success.
     *  If the end of the stream is reached, return false.
     */
    virtual bool readOne() = 0 ;

    /**
     *  @brief  Read the full stream until the end
     *  See readOne() for details
     */
    virtual void readAll() ;

    /**
     *  @brief  Set the callback function to process on event read
     *
     *  @param  func the callback function
     */
    void onEventRead( EventFunction func ) ;

    /**
     *  @brief  Set the callback function to process on run header read
     *
     *  @param  func the callback function
     */
    void onRunHeaderRead( RunHeaderFunction func ) ;

  protected:
    /**
     *  @brief  Must be called by daughter classes in readStream()
     *  to process an event in the framework
     *
     *  @param  event the event to process
     */
    void processRunHeader( std::shared_ptr<RunHeader> rhdr ) ;

    /**
     *  @brief  Must be called by daughter classes in readStream()
     *  to process an event in the framework
     *
     *  @param  event the event to process
     */
    void processEvent( std::shared_ptr<EventStore> event ) ;
    
    /// Init data source
    virtual void initialize() override ;

  protected:
    ///< The data source description
    std::string              _description {"No description"} ;

  private:
    ///< The callback function on event read
    EventFunction            _onEventRead {nullptr} ;
    ///< The callback function on run header read
    RunHeaderFunction        _onRunHeaderRead {nullptr} ;
  };

}

#endif
