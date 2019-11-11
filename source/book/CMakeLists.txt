#################################
# cmake file for building MarlinBOOK component
# @author Remi Ete, DESY
#################################

#include directories
include_directories( BEFORE include )

#install header files
FILE( GLOB MarlinBook_header_files ${CMAKE_CURRENT_SOURCE_DIR}/include/marlin/book/*.h )
INSTALL( FILES ${MarlinBook_header_files} DESTINATION include/marlin/book )

AUX_SOURCE_DIRECTORY( src MarlinBook_sources )

#create library
ADD_SHARED_LIBRARY( MarlinBook ${MarlinBook_sources} )
INSTALL_SHARED_LIBRARY( MarlinBook DESTINATION lib )
TARGET_LINK_LIBRARIES( MarlinBook Marlin )

