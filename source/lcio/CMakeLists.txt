#################################
# cmake file for building MarlinMTLCIO component
# @author Remi Ete, DESY
#################################

find_package( LCIO REQUIRED )

marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY include MATCHING_PATTERN *.h )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY src MATCHING_PATTERN *.cc )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

aux_source_directory( src LCIOPlugins_sources )
aux_source_directory( plugins LCIOPlugins_sources )

add_library( LCIOPlugins MODULE ${LCIOPlugins_sources} )
add_library( MarlinMT::LCIOPlugins ALIAS LCIOPlugins )
set_target_properties( LCIOPlugins PROPERTIES OUTPUT_NAME MarlinMTLCIOPlugins )
install( TARGETS LCIOPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

target_include_directories( 
  LCIOPlugins SYSTEM PUBLIC
  $<BUILD_INTERFACE:${LCIO_INCLUDE_DIRS}>
)
target_include_directories( 
  LCIOPlugins BEFORE PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries( 
  LCIOPlugins PUBLIC
  MarlinMT::Core
  ${LCIO_LIBRARIES}
)

# Add MarlinMTLCIO library to list of MARLINMT_DLL libraries
marlinmt_add_plugin_library( TARGETS LCIOPlugins )
