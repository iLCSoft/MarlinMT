########################################################
# cmake file for building Marlin
# @author Remi Ete, DESY
cmake_minimum_required( VERSION 3.14 FATAL_ERROR )
########################################################

#--- Project name
project( Marlin
  VERSION 1.16.0
  DESCRIPTION "Modular Analysis and Reconstruction for LINear collider"
  LANGUAGES CXX
)

#--- Load cmake config
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( MarlinBuild )
marlin_set_cxx_standard()

#--- Sources to compile
add_subdirectory( source )

#--- Unit tests
include( CTest )
if( BUILD_TESTING )
  add_subdirectory( test )
endif()

#-- Benchmarking
if ( MARLIN_BUILD_BENCHMARKS )
	add_subdirectory( benchmarking )
endif()

#--- Documentation
add_subdirectory( doc )

#--- Generate and install thismarlin.sh
marlin_generate_thismarlin()

#--- Display summary and write variables to cache
marlin_cmake_summary()

#--- Generate configuration files
# See: https://cmake.org/cmake/help/v3.14/module/CMakePackageConfigHelpers.html
include( CMakePackageConfigHelpers )

configure_package_config_file( 
  ${PROJECT_SOURCE_DIR}/cmake/MarlinConfig.cmake.in ${PROJECT_BINARY_DIR}/MarlinConfig.cmake 
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
write_basic_package_version_file(
  MarlinConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion 
)

#--- Install configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MarlinConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/MarlinConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION} 
)

#--- Generate targets file
install( EXPORT Marlin
  FILE MarlinConfig-targets.cmake
  NAMESPACE Marlin::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}
 )


