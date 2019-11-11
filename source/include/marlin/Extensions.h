#pragma once

// -- std headers
#include <memory>
#include <map>
#include <typeindex>

// -- marlin headers
#include <marlin/Exceptions.h>

namespace marlin {

  class Extension {
  public:
    Extension() = delete ;
    Extension( const Extension & ) = delete ;
    Extension &operator =( const Extension & ) = delete ;
    ~Extension() = default ;

  public:
    template <typename T>
    inline Extension( T *obj, bool isOwned ) :
      _isOwned(isOwned) ,
      _typeidx(typeid(T)) {
      if( _isOwned ) {
        _object = std::shared_ptr<T>( obj ) ;
      }
      else {
        _object = std::shared_ptr<T>( obj, [](T *){ /* nop */ } ) ;
      }
    }

    template <typename T>
    inline const T *object() const {
      return static_cast<const T*>(_object.get()) ;
    }

    template <typename T>
    inline T *object() {
      return static_cast<T*>(_object.get()) ;
    }

    template <typename T>
    inline std::shared_ptr<const T> shared() const {
      return std::static_pointer_cast<const T>(_object) ;
    }

    template <typename T>
    inline std::shared_ptr<T> shared() {
      return std::static_pointer_cast<T>(_object);
    }

    inline bool isOwned() const {
      return _isOwned ;
    }

    inline const std::type_index &type() const {
      return _typeidx ;
    }

  private:
    bool                    _isOwned {} ;
    std::type_index         _typeidx ;
    std::shared_ptr<void>   _object {} ;
  };

  /**
   *  @brief  Extensions class.
   *          Provide an interface to a user defined event object.
   */
  class Extensions {
  public:
    using ExtensionMap = std::map<std::size_t, std::shared_ptr<Extension>> ;

  public:
    Extensions() = default ;
    ~Extensions() = default ;
    Extensions(const Extensions &) = delete ;
    Extensions &operator=(const Extensions &) = delete ;
    Extensions(Extensions &&) = default ;
    Extensions &operator=(Extensions &&) = default ;

    template <typename K>
    inline bool exits() const {
      return ( _extensions.find( std::type_index(typeid(K)).hash_code() ) != _extensions.end() ) ;
    }

    template <typename K, typename T>
    inline void add( T *ptr, bool isOwned = true ) {
      std::type_index typeidx( typeid(K) ) ;
      auto iter = _extensions.find( typeidx.hash_code() ) ;
      if( iter != _extensions.end() ) {
        MARLIN_THROW( "Extension of type " + std::string(typeidx.name()) + " already present" ) ;
      }
      auto ext = std::make_shared<Extension>( ptr, isOwned ) ;
      _extensions.insert( { typeidx.hash_code(), ext } ) ;
    }

    template <typename K, typename T, typename ...Args>
    inline T* create( bool isOwned, Args ...args ) {
      std::type_index typeidx( typeid(K) ) ;
      auto iter = _extensions.find( typeidx.hash_code() ) ;
      if( iter != _extensions.end() ) {
        MARLIN_THROW( "Extension of type " + std::string(typeidx.name()) + " already present" ) ;
      }
      auto ext = std::make_shared<Extension>( new T( args... ), isOwned ) ;
      _extensions.insert( { typeidx.hash_code(), ext } ) ;
      return ext->template object<T>() ;
    }

    template <typename K, typename T>
    inline T *get() {
      std::type_index typeidx( typeid(K) ) ;
      auto iter = _extensions.find( typeidx.hash_code() ) ;
      if( iter == _extensions.end() ) {
        MARLIN_THROW( "Extension of type " + std::string(typeidx.name()) + " doesn't exists" ) ;
      }
      return iter->second->object<T>() ;
    }

    template <typename K, typename T>
    inline const T *get() const {
      std::type_index typeidx( typeid(K) ) ;
      auto iter = _extensions.find( typeidx.hash_code() ) ;
      if( iter == _extensions.end() ) {
        MARLIN_THROW( "Extension of type " + std::string(typeidx.name()) + " doesn't exists" ) ;
      }
      return iter->second->object<T>() ;
    }

    template <typename K>
    inline void remove() {
      std::type_index typeidx( typeid(K) ) ;
      auto iter = _extensions.find( typeidx.hash_code() ) ;
      if( iter == _extensions.end() ) {
        MARLIN_THROW( "Extension of type " + std::string(typeidx.name()) + " doesn't exists" ) ;
      }
      _extensions.erase( iter ) ;
    }

  private:
    /// The extension map
    ExtensionMap         _extensions {} ;
  };

}
