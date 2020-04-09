#################################
# cmake file for building Gear component
# @author Remi Ete, DESY
#################################

find_package( GEAR REQUIRED )

marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

# create library
aux_source_directory( plugins MarlinMTGear_sources )
add_library( GearPlugins MODULE ${MarlinMTGear_sources} )
add_library( MarlinMT::GearPlugins ALIAS GearPlugins )
install( TARGETS GearPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
# For any stupid reason, Gear has two different include directories 
# exported: ${GEAR_DIR}/include and ${GEAR_DIR}/include/gear
# Need to go through a for loop... 
foreach( dir ${GEAR_INCLUDE_DIRS} )
  target_include_directories( 
    GearPlugins SYSTEM PUBLIC 
    $<BUILD_INTERFACE:${dir}>
  )
endforeach()
target_link_libraries( GearPlugins PUBLIC MarlinMT::Core ${GEAR_LIBRARIES} )

# Add MarlinMTGear library to list of MARLINMT_DLL libraries
marlinmt_add_plugin_library( TARGETS GearPlugins )
