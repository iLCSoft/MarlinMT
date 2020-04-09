#################################
# cmake file for building MarlinMT Core
# @author Remi Ete, DESY
#################################

# Find required packages
find_package( ILCUTIL COMPONENTS streamlog REQUIRED )
find_package( TinyXML REQUIRED )
find_package( Threads REQUIRED )

#--- Generate MarlinMTConfig.h
string( TIMESTAMP MarlinMT_RELEASE_DATE "%b %d %Y" )
string( TIMESTAMP MarlinMT_RELEASE_TIME "%H:%M:%S" )
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/MarlinMTConfig.h.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/marlinmt/MarlinMTConfig.h
  @ONLY
)

#--- Generate MarlinMTBookConfig.h
# Map the options available in MARLINMT_BOOK_IMPL variable
set( MARLINMT_BOOK_HEADER_dummy Dummy.h )
set( MARLINMT_BOOK_HEADER_root6 Dummy.h ) # No ROOT 6 implementation for the moment
set( MARLINMT_BOOK_HEADER_root7 ROOTv7.h )
# Select the implementation config
set( MARLINMT_BOOK_IMPL_HEADER ${MARLINMT_BOOK_HEADER_${MARLINMT_BOOK_IMPL}} )
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/MarlinMTBookConfig.h.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/marlinmt/MarlinMTBookConfig.h
  @ONLY
)

# MarlinMTCore library
aux_source_directory( src MarlinMTCore_sources )
aux_source_directory( src/concurrency MarlinMTCore_sources )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY include MATCHING_PATTERN *.h )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY src MATCHING_PATTERN *.cc )

add_library( Core SHARED ${MarlinMTCore_sources} )
add_library( MarlinMT::Core ALIAS Core )
set_target_properties( Core PROPERTIES OUTPUT_NAME MarlinMTCore )
install( TARGETS Core EXPORT MarlinMT LIBRARY )
# Dependencies include dirs as system one to avoid unnecessary warnings
target_include_directories( 
  Core SYSTEM PUBLIC 
  $<BUILD_INTERFACE:${streamlog_INCLUDE_DIRS}> 
  $<BUILD_INTERFACE:${MARLINMT_3RDPARTY}/tclap/include>  # 3rd party tclap includes
)
target_include_directories( 
  Core BEFORE PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries( 
  Core PUBLIC
  Threads::Threads
  MarlinMT::Book
  ${streamlog_LIBRARIES} 
  ${CMAKE_DL_LIBS} 
)
set_target_properties( 
  Core
  PROPERTIES 
    COMPILE_FLAGS ${MARLINMT_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLINMT_CXX_STANDARD}
)
install( DIRECTORY include/marlinmt TYPE INCLUDE )


# MarlinMTCorePlugins module
aux_source_directory( plugins MarlinMTCorePlugins_sources )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

add_library( CorePlugins MODULE ${MarlinMTCorePlugins_sources} )
add_library( MarlinMT::CorePlugins ALIAS CorePlugins )
set_target_properties( CorePlugins PROPERTIES OUTPUT_NAME MarlinMTCorePlugins )
install( TARGETS CorePlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set_target_properties( 
  CorePlugins
  PROPERTIES 
    COMPILE_FLAGS ${MARLINMT_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLINMT_CXX_STANDARD}
)
target_include_directories(
  CorePlugins SYSTEM PUBLIC 
  $<BUILD_INTERFACE:${TinyXML_INCLUDE_DIR}>
)
target_link_libraries( 
  CorePlugins PUBLIC MarlinMT::Core ${TinyXML_LIBRARY}
)

# Add MarlinMTCorePlugins library to list of MARLINMT_DLL libraries
marlinmt_add_plugin_library( TARGETS CorePlugins )

# Create all executables out of the main directory
set( MarlinMT_BINARIES MarlinMTShellCompletion MarlinMTDumpPlugins MarlinMTMT )
foreach( binary ${MarlinMT_BINARIES} )
  add_executable( bin_${binary} main/${binary}.cc )
  target_link_libraries( bin_${binary} MarlinMT::Core )
  set_target_properties( bin_${binary} PROPERTIES OUTPUT_NAME ${binary} )
  install( TARGETS bin_${binary} RUNTIME )
endforeach()

install( FILES scripts/marlinmt-completion.bash TYPE BIN )
install( FILES scripts/marlinmt-shell-functions.bash TYPE BIN )

