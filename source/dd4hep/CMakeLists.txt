#################################
# cmake file for building DD4hep component in MarlinMT
# @author Remi Ete, DESY
#################################

# From v01-11, DD4hep uses imported targets
find_package( DD4hep 01.11 COMPONENTS DDRec DDParsers REQUIRED )

marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

# create library
aux_source_directory( plugins MarlinMTDD4hep_sources )
add_library( DD4hepPlugins MODULE ${MarlinMTDD4hep_sources} )
add_library( MarlinMT::DD4hepPlugins ALIAS DD4hepPlugins )
set_target_properties( DD4hepPlugins PROPERTIES OUTPUT_NAME MarlinMTDD4hepPlugins )
install( TARGETS DD4hepPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
target_link_libraries( 
  DD4hepPlugins PUBLIC
  MarlinMT::Core
  DD4hep::DDCore DD4hep::DDRec DD4hep::DDParsers 
)

# Add MarlinMTDD4hepPlugins library to list of MARLINMT_DLL libraries
marlinmt_add_plugin_library( TARGETS DD4hepPlugins )
