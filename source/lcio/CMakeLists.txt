#################################
# cmake file for building MarlinLCIO component
# @author Remi Ete, DESY
#################################

FIND_PACKAGE( LCIO REQUIRED )

# include directories
include_directories( BEFORE include )
include_directories( SYSTEM BEFORE ${LCIO_INCLUDE_DIRS} )

# install header files
FILE( GLOB lcio_plugin_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/marlin/lcio/*.h )
INSTALL( FILES ${lcio_plugin_headers} DESTINATION include/marlin/lcio )

AUX_SOURCE_DIRECTORY( src lcio_plugin_sources )
AUX_SOURCE_DIRECTORY( plugins lcio_plugin_sources )

# create library
ADD_SHARED_LIBRARY( MarlinLCIO ${lcio_plugin_sources} )
INSTALL_SHARED_LIBRARY( MarlinLCIO DESTINATION lib )
TARGET_LINK_LIBRARIES( MarlinLCIO Marlin ${LCIO_LIBRARIES}  )
