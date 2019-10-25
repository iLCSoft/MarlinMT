#
# CMakeLists.txt for MarlinMT unit tests
#
# @author: Remi Ete, DESY
# @date: Oct 2019
#

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include( MarlinUnitTesting )

# include directories
include_directories( BEFORE ./include ${PROJECT_SOURCE_DIR}/source/include  ${PROJECT_BINARY_DIR} )

if( MARLIN_LCIO )
  find_package( LCIO REQUIRED QUIET )
  include_directories( SYSTEM ${LCIO_INCLUDE_DIRS} )
  link_libraries( ${LCIO_LIBRARIES} )
endif()

# add unit test library

aux_source_directory( ./processors library_sources )
set( library_sources processors/TestProcessorEventSeeder.cc )
if( MARLIN_LCIO )
  list( APPEND library_sources processors/TestEventModifier.cc )
endif()

add_shared_library( MarlinUnitTest ${library_sources} )
target_link_libraries( MarlinUnitTest Marlin )

# Add unit tests
marlin_add_test (
  test-thread-pool
  BUILD_EXEC
  REGEX_FAIL "TEST_FAILED"
)

marlin_add_test (
  test-clock
  BUILD_EXEC
  REGEX_FAIL "TEST_FAILED"
)

if(MARLIN_BOOK)
#Marlin_book tests
	marlin_add_test (
		test-book-store
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
	)

	marlin_add_test (
		test-mem-layout
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
	)
	
endif()

marlin_add_test (
  marlinminusx
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/Marlin
  EXEC_ARGS -x test_marlinminusx.xml
  REGEX_PASS NONE
)

marlin_add_test (
  check-marlinminusx
  COMMAND cat
  EXEC_ARGS test_marlinminusx.xml
  REGEX_PASS "<parameter name=\"RandomSeed\" value=\"1234567890\" />"
  DEPENDS marlinminusx
)

if( MARLIN_LCIO )
  marlin_add_processor_test (
    eventmodifier
    STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/eventmodifier.xml
    INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
    REGEX_PASS "TestEventModifier modified 3 events in 1 run"
    MARLIN_DLL "$<TARGET_FILE:MarlinLCIO>"
  )

  marlin_add_processor_test (
    processoreventseeder
    STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/processoreventseeder.xml
    INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
    REGEX_PASS "TestProcessorEventSeeder failed to register processor to event seed generator"
    REGEX_FAIL "ERROR .TestProcessorEventSeeder.* Seeds don't match"
    MARLIN_DLL "$<TARGET_FILE:MarlinLCIO>"
  )

  marlin_add_processor_test (
    includeandconstants
    STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/base-eventmodifier.xml
    INPUT_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
      ${CMAKE_CURRENT_SOURCE_DIR}/steer/include-eventmodifier.xml
      ${CMAKE_CURRENT_SOURCE_DIR}/steer/constants-eventmodifier.xml
    REGEX_PASS "TestEventModifier modified 3 events in 1 run"
    MARLIN_DLL "$<TARGET_FILE:MarlinLCIO>"
  )
endif()
