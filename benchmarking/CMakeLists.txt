#################################
# cmake file for building benchmarking component
# @author Remi Ete, Benda Julian, DESY
#################################

# create library
aux_source_directory( plugins BenchmarkPlugins_sources )
add_library( BenchmarkPlugins MODULE ${BenchmarkPlugins_sources} )
add_library( Marlin::BenchmarkPlugins ALIAS BenchmarkPlugins )
install( TARGETS BenchmarkPlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
target_link_libraries( BenchmarkPlugins PUBLIC Marlin::Core)

marlin_add_plugin_library( TARGETS BenchmarkPlugins )
