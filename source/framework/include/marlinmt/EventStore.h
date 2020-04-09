#pragma once

// -- std headers
#include <string>
#include <memory>
#include <typeindex>

// -- marlin headers
#include <marlin/Extensions.h>

namespace marlin {

  /**
   *  @brief  EventStore class.
   *          Provide an interface to a user defined event object.
   */
  class EventStore {
  public:
    EventStore() = default ;
    ~EventStore() = default ;
    EventStore(const EventStore &) = delete ;
    EventStore &operator=(const EventStore &) = delete ;
    EventStore(EventStore &&) = default ;
    EventStore &operator=(EventStore &&) = default ;

    /**
     *  @brief  Set the event unique id
     *
     *  @brief  uid the unique id
     */
    void setUID( std::size_t uid ) ;

    /**
     *  @brief  Get the event unique id
     */
    std::size_t uid() const ;

    /**
     *  @brief  Get the underlying event to a specific type
     */
    template <typename T>
    std::shared_ptr<T> event() const ;

    /**
     *  @brief  Get the underlying event to a specific type.
     *          The event in the store is invalidated.
     */
    template <typename T>
    std::shared_ptr<T> takeEvent() ;

    /**
     *  @brief  Check whether the stored event is of a given type
     */
    template <typename T>
    bool isType() const ;

    /**
     *  @brief  Get the event type
     */
    const std::type_index &type() const ;

    /**
     *  @brief  Set the event pointer using a shared pointer
     *
     *  @param  event the event to store
     */
    template <typename T>
    void setEvent( std::shared_ptr<T> evt ) ;

    /**
     *  @brief  Set the event pointer using a bare pointer and
     *          an ownership flag.
     *
     *  @param  event a bare pointer to the event to store
     *  @param  owner whether to own the pointer
     */
    template <typename T>
    void setBareEvent( T *evt, bool owner ) ;

    /**
     *  @brief  Allocate a new event in the event store and return it.
     *          As the current event is replaced the old one might be deleted
     *          via the shared_ptr mechanism.
     */
    template <typename T, typename ...Args>
    std::shared_ptr<T> allocate(Args && ...args) ;

    /**
     *  @brief  Reset the internal pointer
     */
    void reset() ;

    /**
     *  @brief  Access the event extensions
     */
    Extensions &extensions() ;

    /**
     *  @brief  Access the event extensions
     */
    const Extensions &extensions() const ;

  private:
    ///
    std::size_t                 _uid {0} ;
    /// The underlying event store implementation
    std::shared_ptr<void>       _event {nullptr} ;
    /// The event implementtion type
    std::type_index             _eventType {typeid(nullptr)} ;
    /// The event extensions
    Extensions                  _extensions {} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  inline void EventStore::setUID( std::size_t id ) {
    _uid = id ;
  }

  //--------------------------------------------------------------------------

  inline std::size_t EventStore::uid() const {
    return _uid ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::shared_ptr<T> EventStore::event() const {
    return std::static_pointer_cast<T>( _event ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline std::shared_ptr<T> EventStore::takeEvent() {
    _eventType = std::type_index( typeid(nullptr) ) ;
    return std::static_pointer_cast<T>( _event ) ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline bool EventStore::isType() const {
    // FIXME: use std::decay_t
    return (std::type_index(typeid(T)) == _eventType) ;
  }

  //--------------------------------------------------------------------------

  inline const std::type_index &EventStore::type() const {
    return _eventType ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void EventStore::setEvent( std::shared_ptr<T> evt ) {
    _eventType = std::type_index(typeid(T)) ;
    _event = evt ;
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void EventStore::setBareEvent( T *evt, bool owner ) {
    if( owner ) {
      _event = std::shared_ptr<T>( evt ) ;
    }
    else {
      _event = std::shared_ptr<T>( evt, [](T *ptr){/* nop */} ) ;
    }
    _eventType = std::type_index(typeid(T)) ;
  }

  //--------------------------------------------------------------------------

  template <typename T, typename ...Args>
  inline std::shared_ptr<T> EventStore::allocate(Args && ...args) {
    auto ptr = std::make_shared<T>( args... ) ;
    setEvent( ptr ) ;
    return ptr ;
  }

  //--------------------------------------------------------------------------

  inline void EventStore::reset() {
    _eventType = std::type_index(typeid(nullptr)) ;
    _event.reset() ;
  }

  //--------------------------------------------------------------------------

  inline Extensions &EventStore::extensions() {
    return _extensions ;
  }

  //--------------------------------------------------------------------------

  inline const Extensions &EventStore::extensions() const {
    return _extensions ;
  }

}
