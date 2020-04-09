#ifndef MARLIN_GEOMETRYPLUGIN_h
#define MARLIN_GEOMETRYPLUGIN_h 1

// -- std headers
#include <string>
#include <typeindex>

// -- marlin headers
#include "marlin/Exceptions.h"
#include "marlin/Component.h"

namespace marlin {

  class GeometryManager ;
  class Application ;

  /**
   *  @brief  GeometryPlugin class
   *  Responsible for loading geometry in Marlin and providing
   *  access to it through the GeometryManager
   */
  class GeometryPlugin : public Component {
    friend class GeometryManager ;

  public:
    GeometryPlugin() = delete ;
    virtual ~GeometryPlugin() = default ;
    GeometryPlugin(const GeometryPlugin &) = delete ;
    GeometryPlugin& operator=(const GeometryPlugin &) = delete ;

  protected:
    /**
     *  @brief  Constructor. To be called by sub classes
     *
     *  @param  gtype the geometry type
     */
    GeometryPlugin( const std::string &gtype ) ;

  protected:
    /**
     *  @brief  Load the geometry
     */
    virtual void loadGeometry() = 0 ;

    /**
     *  @brief  Get a handle on the geometry instance
     */
    virtual const void *handle() const = 0 ;

    /**
     *  @brief  Cleanup geometry
     */
    virtual void destroy() = 0 ;

  public:
    /**
     *  @brief  Get a type index object from the geometry handle
     */
    virtual std::type_index typeIndex() const = 0 ;

    /**
     *  @brief  Dump the geometry in the console
     */
    virtual void dumpGeometry() const = 0 ;

    /**
     *  @brief  Print the complete geometry plugin description.
     */
    void print() const ;
    
  protected:
    /// Init the geometry plugin
    virtual void initialize() override ; 

  protected:
    /// Whether to dump the geometry on creation
    BoolParameter       _dumpGeometry {*this, "DumpGeometry", "Whether to dump the geometry on creation", false} ;
  };

} // end namespace marlin

#endif
