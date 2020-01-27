#################################
# cmake file for building Marlin Core
# @author Remi Ete, DESY
#################################

# Find required packages
find_package( ILCUTIL COMPONENTS streamlog REQUIRED )
find_package( TinyXML REQUIRED )
find_package( Threads REQUIRED )

#--- Generate MarlinConfig.h
string( TIMESTAMP Marlin_RELEASE_DATE "%b %d %Y" )
string( TIMESTAMP Marlin_RELEASE_TIME "%H:%M:%S" )
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/MarlinConfig.h.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/marlin/MarlinConfig.h
  @ONLY
)

#--- Generate MarlinBookConfig.h
# Map the options available in MARLIN_BOOK_IMPL variable
set( MARLIN_BOOK_HEADER_dummy Dummy.h )
set( MARLIN_BOOK_HEADER_root6 Dummy.h ) # No ROOT 6 implementation for the moment
set( MARLIN_BOOK_HEADER_root7 ROOTv7.h )
# Select the implementation config
set( MARLIN_BOOK_IMPL_HEADER ${MARLIN_BOOK_HEADER_${MARLIN_BOOK_IMPL}} )
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/MarlinBookConfig.h.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/marlin/MarlinBookConfig.h
  @ONLY
)

# MarlinCore library
aux_source_directory( src MarlinCore_sources )
aux_source_directory( src/concurrency MarlinCore_sources )
marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY include MATCHING_PATTERN *.h )
marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY src MATCHING_PATTERN *.cc )

add_library( Core SHARED ${MarlinCore_sources} )
add_library( Marlin::Core ALIAS Core )
set_target_properties( Core PROPERTIES OUTPUT_NAME MarlinCore )
install( TARGETS Core EXPORT Marlin LIBRARY )
# Dependencies include dirs as system one to avoid unnecessary warnings
target_include_directories( 
  Core SYSTEM PUBLIC 
  $<BUILD_INTERFACE:${streamlog_INCLUDE_DIRS}> $<BUILD_INTERFACE:${TinyXML_INCLUDE_DIR}>
)
target_include_directories( 
  Core BEFORE PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries( 
  Core PUBLIC
  Threads::Threads
  Marlin::Book
  ${streamlog_LIBRARIES} 
  ${TinyXML_LIBRARY}
  ${CMAKE_DL_LIBS} 
)
set_target_properties( 
  Core
  PROPERTIES 
    COMPILE_FLAGS ${MARLIN_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLIN_CXX_STANDARD}
)
install( DIRECTORY include/marlin TYPE INCLUDE )


# MarlinCorePlugins module
aux_source_directory( plugins MarlinCorePlugins_sources )
marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY plugins MATCHING_PATTERN *.cc )

add_library( CorePlugins MODULE ${MarlinCorePlugins_sources} )
add_library( Marlin::CorePlugins ALIAS CorePlugins )
set_target_properties( CorePlugins PROPERTIES OUTPUT_NAME MarlinCorePlugins )
install( TARGETS CorePlugins LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

set_target_properties( 
  CorePlugins
  PROPERTIES 
    COMPILE_FLAGS ${MARLIN_COMPILE_OPTIONS}
    VERSION ${${PROJECT_NAME}_VERSION}
    CXX_STANDARD ${MARLIN_CXX_STANDARD}
)
target_link_libraries( 
  CorePlugins PUBLIC Marlin::Core
)

# Add MarlinCorePlugins library to list of MARLIN_DLL libraries
marlin_add_plugin_library( TARGETS CorePlugins )

# Create all executables out of the main directory
set( MarlinMT_BINARIES MarlinShellCompletion MarlinDumpPlugins Marlin MarlinMT )
foreach( binary ${MarlinMT_BINARIES} )
  add_executable( bin_${binary} main/${binary}.cc )
  target_link_libraries( bin_${binary} Marlin::Core )
  set_target_properties( bin_${binary} PROPERTIES OUTPUT_NAME ${binary} )
  install( TARGETS bin_${binary} RUNTIME )
endforeach()

install( FILES scripts/marlin-completion.bash TYPE BIN )

