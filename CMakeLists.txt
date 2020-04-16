########################################################
# cmake file for building MarlinMT
# @author Remi Ete, DESY
cmake_minimum_required( VERSION 3.14 FATAL_ERROR )
########################################################

#--- Project name
project( MarlinMT
  VERSION 0.1.0
  DESCRIPTION "Modular Analysis and Reconstruction for LINear collider (Multi-Threaded)"
  LANGUAGES CXX
)

#--- Load cmake config
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
include( MarlinMTBuild )
marlinmt_set_cxx_standard()

#--- Sources to compile
add_subdirectory( source )

#--- Unit tests
include( CTest )
if( BUILD_TESTING )
  add_subdirectory( test )
endif()

#-- Benchmarking
if ( MARLINMT_BUILD_BENCHMARKS )
	add_subdirectory( benchmarking )
endif()

#--- Documentation
add_subdirectory( doc )

#--- Generate and install thismarlinmt.sh
marlinmt_generate_thismarlinmt()

#--- Display summary and write variables to cache
marlinmt_cmake_summary()

#--- Generate configuration files
# See: https://cmake.org/cmake/help/v3.14/module/CMakePackageConfigHelpers.html
include( CMakePackageConfigHelpers )

configure_package_config_file( 
  ${PROJECT_SOURCE_DIR}/cmake/MarlinMTConfig.cmake.in ${PROJECT_BINARY_DIR}/MarlinMTConfig.cmake 
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)
write_basic_package_version_file(
  MarlinMTConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion 
)

#--- Install configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MarlinMTConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/MarlinMTConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION} 
)

#--- Generate targets file
install( EXPORT MarlinMT
  FILE MarlinMTConfig-targets.cmake
  NAMESPACE MarlinMT::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}-${PROJECT_VERSION}
 )


