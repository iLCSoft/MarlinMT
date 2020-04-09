
// -- marlinmt headers
#include "marlinmt/GeometryPlugin.h"
#include "marlinmt/PluginManager.h"

namespace marlinmt {

  /**
   *  @brief  EmptyGeometry class
   *  Implement an empty geometry
   */
  class EmptyGeometry : public GeometryPlugin {
  public:
    EmptyGeometry(const EmptyGeometry &) = delete ;
    EmptyGeometry& operator=(const EmptyGeometry &) = delete ;

  public:
    EmptyGeometry() : GeometryPlugin( "Empty" ) { /* nop */ }

  protected:
    void loadGeometry() override { /* nop */ }
    const void *handle() const override { return nullptr ; }
    void destroy() override { /* nop */ }
    std::type_index typeIndex() const override { return std::type_index( typeid(nullptr) ) ; }
    void dumpGeometry() const override { /* nop */ }
  };

  MARLINMT_DECLARE_GEOMETRY( EmptyGeometry )
  
}