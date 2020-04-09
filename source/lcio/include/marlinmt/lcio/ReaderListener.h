#ifndef MARLIN_READERLISTENER_h
#define MARLIN_READERLISTENER_h 1

// -- std headers
#include <functional>
#include <memory>

// -- lcio headers
#include <MT/LCReaderListener.h>

namespace EVENT {
  class LCEvent ;
  class LCRunHeader ;
}

namespace marlin {

  class EventStore ;
  class RunHeader ;

  /**
   *  @brief  ReaderListener class.
   *
   *  Simple implementation of a reader listener.
   *  Callback functions can be set using lambda function, std::function objects
   *  or resulting call of std::bind call.
   *
   *  Example with lambda functions:
   *  @code{cpp}
   *  ReaderListener listener ;
   *  listener.onEventRead( [](std::shared_ptr<EventStore> event){
   *    std::cout << "Read event uid " << event->uid() << std::endl ;
   *  }) ;
   *  listener.onRunHeaderRead( [](std::shared_ptr<RunHeader> rhdr){
   *    std::cout << "Read run info no " << rhdr->runNumber() << std::endl ;
   *  }) ;
   *  @endcode
   *
   *  Example with std::bind and custom class method:
   *  @code{cpp}
   *  using namespace std::placeholders ;
   *  UserClass user ;
   *  ReaderListener listener ;
   *  listener.onEventRead( std::bind(&UserClass::processEvent, &user, _1) ) ;
   *  listener.onRunHeaderRead( std::bind(&UserClass::processRunHeader, &user, _1) ) ;
   *  @endcode
   */
  class ReaderListener : public MT::LCReaderListener {
  public:
    using EventFunction = std::function<void(std::shared_ptr<EventStore>)> ;
    using RunHeaderFunction = std::function<void(std::shared_ptr<RunHeader>)> ;

  public:
    ReaderListener() = default ;
    ~ReaderListener() = default ;
    ReaderListener(const ReaderListener&) = delete ;
    ReaderListener &operator=(const ReaderListener&) = delete ;

    /**
     *  @brief  Set the callback function to process on event read
     */
    void onEventRead( EventFunction func ) ;

    /**
     *  @brief  Set the callback function to process on run info read
     */
    void onRunHeaderRead( RunHeaderFunction func ) ;

  protected:
    void processEvent( std::shared_ptr<EVENT::LCEvent> event ) override ;
    void processRunHeader( std::shared_ptr<EVENT::LCRunHeader> rhdr ) override ;

  private:
    /// Callback function on event read
    EventFunction          _onEventRead {nullptr} ;
    /// Callback function on run info read
    RunHeaderFunction      _onRunHeaderRead {nullptr} ;
  };

}

#endif
