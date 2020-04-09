#################################
# cmake file for building MarlinMTBook component
# @author Remi Ete, Julian Benda, DESY
#################################


# Find ROOT package
set( ROOT_COMPONENTS Hist RIO )
find_package( Filesystem REQUIRED )

if( NOT "${MARLINMT_BOOK_IMPL}" STREQUAL "dummy" )
  find_package( ROOT 6.18 COMPONENTS ${ROOT_COMPONENTS} REQUIRED )
  foreach( comp ${ROOT_COMPONENTS} )
    list( APPEND ROOT_COMPONENTS_LIBRARIES ${ROOT_${comp}_LIBRARY} )
  endforeach()
  if( "${MARLINMT_BOOK_IMPL}" STREQUAL "root7" )
    # check ROOT 7 requirements
    marlinmt_check_root_requirements()
  endif()
endif()

marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY include MATCHING_PATTERN *.h )
marlinmt_add_doxygen_sources( GLOB_RECURSE DIRECTORY src MATCHING_PATTERN *.cc )

aux_source_directory( src MarlinMTBook_sources )

# option dependant source files
if( TARGET ROOT::Core )
  list( APPEND MarlinMTBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/StoreWriter.cc )
else()
  list( APPEND MarlinMTBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/StoreWriterDummy.cc )
endif()
if( "${MARLINMT_BOOK_IMPL}" STREQUAL "root7" )
  list( APPEND MarlinMTBook_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/impl/RootHistV7ToV6Conversion.cc )
endif()

add_library( Book SHARED ${MarlinMTBook_sources} )
add_library( MarlinMT::Book ALIAS Book )
set_target_properties( Book PROPERTIES OUTPUT_NAME MarlinMTBook )
install( TARGETS Book EXPORT MarlinMT LIBRARY )

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
install( DIRECTORY include/marlinmt TYPE INCLUDE )

