#ifndef MARLINMT_GEOMETRYMANAGER_h
#define MARLINMT_GEOMETRYMANAGER_h 1

// -- std headers
#include <typeindex>

// -- marlinmt headers
#include <marlinmt/Logging.h>
#include <marlinmt/Exceptions.h>
#include <marlinmt/Component.h>
#include <marlinmt/GeometryPlugin.h>

namespace marlinmt {

  class Application ;

  /**
   *  @brief  GeometryManager class.
   *  Handle a user plugin in charge of loading the geometry
   *  in the framework and providing access to it.
   */
  class GeometryManager : public Component {
  public:
    GeometryManager(const GeometryManager &) = delete ;
    GeometryManager& operator=(const GeometryManager &) = delete ;
    ~GeometryManager() = default ;

    /**
     *  @brief  Default constructor
     */
    GeometryManager() ;

    /**
     *  @brief  Get the underlying geometry handle
     *  Example:
     *
     *  @code{cpp}
     *  // assume Gear geometry has been loaded
     *  const gear::GearMgr* gearGeo = mgr->geometry<gear::GearMgr>() ;
     *  // assume DD4hep geometry has been loaded
     *  const dd4hep::Detector* dd4hepGeo = mgr->geometry<dd4hep::Detector>() ;
     *  @endcode
     */
    template <typename T>
    const T *geometry() const ;

    /**
     *  @brief  Get the underlying geometry type info
     */
    std::type_index typeIndex() const ;

    /**
     *  @brief  Clear the geometry content
     */
    void clear() ;
    
  private:
    /// Initialize geometry manager
    void initialize() override ;

  private:
    /// The geometry plugin created on initialization
    std::shared_ptr<GeometryPlugin>      _plugin {nullptr} ;
    /// The geometry type, read as <geometry type="DD4hepGeometry">
    StringParameter                      _geometryType {*this, "GeometryType", "The geometry plugin type", "EmptyGeometry"} ;
  };

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  inline const T *GeometryManager::geometry() const {
    if ( nullptr == _plugin ) {
      throw Exception( "GeometryManager::geometry: geometry plugin not created !" ) ;
    }
    auto ptr = _plugin->handle() ;
    if ( nullptr == ptr ) {
      return nullptr ;
    }
    const T *castHandle = static_cast<const T*>( ptr ) ;
    if ( nullptr == castHandle ) {
      throw Exception( "GeometryManager::geometry: invalid geometry cast !" ) ;
    }
    return castHandle ;
  }

} // end namespace marlinmt

#endif
