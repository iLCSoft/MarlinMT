#################################
# cmake file for building MarlinBOOK component
# @author Remi Ete, Julian Benda, DESY
#################################


# Find ROOT package
set( ROOT_COMPONENTS Hist RIO )
find_package( Filesystem REQUIRED )

if( NOT "${MARLIN_BOOK_IMPL}" STREQUAL "dummy" )
  find_package( ROOT 6.18 COMPONENTS ${ROOT_COMPONENTS} REQUIRED )
  foreach( comp ${ROOT_COMPONENTS} )
    list( APPEND ROOT_COMPONENTS_LIBRARIES ${ROOT_${comp}_LIBRARY} )
  endforeach()
  if( "${MARLIN_BOOK_IMPL}" STREQUAL "root7" )
    # check ROOT 7 requirements
    marlin_check_root_requirements()
  endif()
endif()

marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY include MATCHING_PATTERN *.h )
marlin_add_doxygen_sources( GLOB_RECURSE DIRECTORY src MATCHING_PATTERN *.cc )

aux_source_directory( src MarlinBook_sources )

# option dependant source files
if( TARGET ROOT::Core )
  list( APPEND MarlinBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/StoreWriter.cc )
else()
  list( APPEND MarlinBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/StoreWriterDummy.cc )
endif()
if( "${MARLIN_BOOK_IMPL}" STREQUAL "root7" )
  list( APPEND MarlinBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/RootHistV7ToV6Conversion.cc )
endif()

add_library( Book SHARED ${MarlinBook_sources} )
add_library( Marlin::Book ALIAS Book )
set_target_properties( Book PROPERTIES OUTPUT_NAME MarlinBook )
install( TARGETS Book EXPORT Marlin LIBRARY )

target_include_directories( 
  Book BEFORE PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
if( TARGET ROOT::Core )
  target_link_libraries(
    Book PUBLIC
    ROOT::Core ROOT::RIO ROOT::Hist
  )
endif()

target_link_libraries(
	Book PUBLIC
	std::filesystem
)
install( DIRECTORY include/marlin TYPE INCLUDE )

