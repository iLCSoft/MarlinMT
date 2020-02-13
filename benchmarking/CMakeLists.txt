#################################
# cmake file for building benchmarking component
# @author Remi Ete, Benda Julian, DESY
#################################

# create library
aux_source_directory( plugins MarlinBenchmarkPlugins_sources )
add_library( MarlinBenchmarkPlugins MODULE ${MarlinBenchmarkPlugins_sources} )
add_library( Marlin::BenchmarkPlugins ALIAS MarlinBenchmarkPlugins )
install( TARGETS MarlinBenchmarkPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
target_include_directories( MarlinBenchmarkPlugins SYSTEM PUBLIC ${LCIO_INCLUDE_DIRS})
target_link_libraries( MarlinBenchmarkPlugins PUBLIC Marlin::Core ${LCIO_LIBRARIES})

marlin_add_plugin_library( TARGETS MarlinBenchmarkPlugins )
