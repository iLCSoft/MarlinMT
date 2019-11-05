#
# CMake utilities for Marlin unit tests
#
# @author: Remi Ete
# @date: Oct 2019
#


#
# Function to add a unit test from the src directory
#
function ( marlin_add_test test_name )
  cmake_parse_arguments(ARG "BUILD_EXEC" "" "COMMAND;DEPENDS;EXEC_ARGS;REGEX_PASS;REGEX_PASSED;REGEX_FAIL;REGEX_FAILED;REQUIRES" ${ARGN} )
  set ( missing )
  set ( use_test 1 )

  if ( ${ARG_BUILD_EXEC} )
    add_executable ( ${test_name} tests/${test_name}.cc )
    target_link_libraries( ${test_name} Marlin )
    target_include_directories( ${test_name} PUBLIC include )
    set_target_properties( ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )
  endif()

  set ( cmd ${ARG_COMMAND} )
  if ( "${cmd}" STREQUAL "" )
    if( ${ARG_BUILD_EXEC} )
      set ( cmd ${CMAKE_BINARY_DIR}/bin/${test_name} )
    else()
      message( SEND_ERROR "No command no executable provided" )
    endif()
  endif()

  set ( passed ${ARG_REGEX_PASS} ${ARG_REGEX_PASSED} )
  if ( "${passed}" STREQUAL "NONE" )
    unset ( passed )
  elseif ( "${passed}" STREQUAL "" )
    set ( passed "TEST_PASSED" )
  endif()

  set ( failed ${ARG_REGEX_FAIL} ${ARG_REGEX_FAILED} )
  if ( "${failed}" STREQUAL "NONE" )
    unset ( failed )
  endif()

  set ( args ${ARG_EXEC_ARGS} )
  add_test(NAME t_${test_name} COMMAND ${cmd} ${args} )
  if ( NOT "${passed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${passed}" )
  endif()
  if ( NOT "${failed}" STREQUAL "" )
    set_tests_properties( t_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${failed}" )
  endif()
  # Set test dependencies if present
  foreach ( _dep ${ARG_DEPENDS} )
    set_tests_properties( t_${test_name} PROPERTIES DEPENDS t_${_dep} )
  endforeach()
endfunction()

#
#
#
function( marlin_add_processor_test test_name )
  cmake_parse_arguments(ARG "" "STEERING_FILE;REGEX_PASS;REGEX_FAIL" "INPUT_FILES;MARLIN_ARGS;MARLIN_DLL" ${ARGN} )
  if( NOT test_name )
    message( FATAL_ERROR "[UNIT_TESTS] Configuring processor test without name" )
  endif()
  if( NOT ARG_STEERING_FILE )
    message( FATAL_ERROR "[UNIT_TESTS ${test_name}] Configuring processor test without steering file" )
  endif()
  list( APPEND ARG_INPUT_FILES ${ARG_STEERING_FILE} )
  foreach( ifile ${ARG_INPUT_FILES} )
    if( NOT EXISTS ${ifile} )
      message( FATAL_ERROR "[UNIT_TEST ${test_name}] Input files doesn't exists ${ifile}" )
    endif()
  endforeach()
  set( MARLIN_INPUT_FILES ${ARG_INPUT_FILES} )
  get_filename_component( MARLIN_STEERING_FILE ${ARG_STEERING_FILE} NAME )
  # set( MARLIN_STEERING_FILE ${ARG_STEERING_FILE} )
  foreach( marg ${ARG_MARLIN_ARGS} )
    set( MARLIN_ARGS "${MARLIN_ARGS} ${marg}" )
  endforeach()
  set( MARLIN_DLL "$<TARGET_FILE:MarlinUnitTest>" )
  list( APPEND MARLIN_DLL ${ARG_MARLIN_DLL} )
  list( JOIN MARLIN_DLL ":" MARLIN_DLL )
  # configure cmake test file.
  # Read the file content and use file( GENERATE )
  # because the MARLIN_DLL contains generator expressions
  file( READ ${CMAKE_CURRENT_SOURCE_DIR}/cmake/runmarlin.cmake.in TEST_FILE_CONTENT )
  string( CONFIGURE ${TEST_FILE_CONTENT} TEST_FILE_FINAL @ONLY )
  set( TEST_CMAKE_FILE ${CMAKE_CURRENT_BINARY_DIR}/test_processor_${test_name}.cmake )
  if( EXISTS ${TEST_CMAKE_FILE} )
    file( REMOVE ${TEST_CMAKE_FILE} )
  endif()
  message( STATUS "Generating test file: ${TEST_CMAKE_FILE} ..." )
  file(
    GENERATE
    OUTPUT ${TEST_CMAKE_FILE}
    CONTENT "${TEST_FILE_FINAL}"
  )
  # add the unit test
  add_test( t_processor_${test_name} "${CMAKE_COMMAND}" -P ${TEST_CMAKE_FILE} )
  if( NOT "${ARG_REGEX_PASS}" STREQUAL "" )
    set_tests_properties( t_processor_${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${ARG_REGEX_PASS}" )
  endif()
  if( NOT "${ARG_REGEX_FAIL}" STREQUAL "" )
    set_tests_properties( t_processor_${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${ARG_REGEX_FAIL}" )
  endif()
endfunction()


#
