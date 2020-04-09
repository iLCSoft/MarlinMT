#################################
# cmake file for building benchmarking component
# @author Remi Ete, Benda Julian, DESY
#################################

# create library
aux_source_directory( plugins MarlinMTBenchmarkPlugins_sources )
add_library( MarlinMTBenchmarkPlugins MODULE ${MarlinMTBenchmarkPlugins_sources} )
add_library( MarlinMT::BenchmarkPlugins ALIAS MarlinMTBenchmarkPlugins )
install( TARGETS MarlinMTBenchmarkPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
target_include_directories( MarlinMTBenchmarkPlugins SYSTEM PUBLIC ${LCIO_INCLUDE_DIRS})
target_link_libraries( MarlinMTBenchmarkPlugins PUBLIC MarlinMT::Core ${LCIO_LIBRARIES})

marlinmt_add_plugin_library( TARGETS MarlinMTBenchmarkPlugins )
