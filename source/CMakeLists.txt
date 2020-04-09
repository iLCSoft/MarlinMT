#################################
# cmake file for building MarlinMT
# @author Remi Ete, DESY
#################################

set( MARLINMT_3RDPARTY ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty )

# MarlinMTBook and framework
add_subdirectory( book )
add_subdirectory( framework )

# ----- MarlinMTDD4hep 
if( MARLINMT_DD4HEP )
  add_subdirectory( dd4hep )
endif()

# MarlinMTGear 
if( MARLINMT_GEAR )
  add_subdirectory( gear )
endif()

# MarlinMTLCIO 
if( MARLINMT_LCIO )
  add_subdirectory( lcio )
endif()

# TODO: Remove this shit!
# MarlinMTGUI 
set( MARLINMT_GUI OFF )
if( MARLINMT_GUI )
  add_subdirectory( gui )
endif()



