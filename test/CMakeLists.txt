#
# CMakeLists.txt for MarlinMT unit tests
#
# @author: Remi Ete, DESY
# @date: Oct 2019
#

# add unit test library
set( MarlinMTUnitTest_sources processors/TestProcessorEventSeeder.cc )
if( MARLINMT_LCIO )
  find_package( LCIO REQUIRED QUIET )
	list( APPEND MarlinMTUnitTest_sources processors/TestEventModifier.cc )
	list( APPEND MarlinMTUnitTest_sources processors/TestHistogram.cc )
endif()
add_library( UnitTest SHARED ${MarlinMTUnitTest_sources} )
add_library( MarlinMT::UnitTest ALIAS UnitTest )
set_target_properties( UnitTest PROPERTIES OUTPUT_NAME MarlinMTUnitTest )
target_link_libraries( UnitTest PUBLIC MarlinMT::Core )
if( MARLINMT_LCIO )
  target_include_directories( UnitTest SYSTEM PUBLIC ${LCIO_INCLUDE_DIRS} )
  target_link_libraries( UnitTest PUBLIC ${LCIO_LIBRARIES} )
endif()
# NOTE: The unit test library is not installed

# Add unit tests
marlinmt_add_test (
  test-thread-pool
  BUILD_EXEC
  REGEX_FAIL "TEST_FAILED"
)

marlinmt_add_test (
  test-clock
  BUILD_EXEC
  REGEX_FAIL "TEST_FAILED"
)

marlinmt_add_test (
  test-validator
  BUILD_EXEC
  REGEX_FAIL "TEST_FAILED"
)

if("${MARLINMT_BOOK_IMPL}" STREQUAL "root7")
	marlinmt_add_test (
		test-mem-layout
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
		COMPONENTS MarlinMT::Book
	)

	marlinmt_add_test (
		test-book-store-book
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
		COMPONENTS MarlinMT::Book
	)

	marlinmt_add_test (
		test-book-store-find
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
		COMPONENTS MarlinMT::Book
	)

	marlinmt_add_test (
		test-book-store-store
		BUILD_EXEC
		REGEX_FAIL "TEST_FAILED"
		COMPONENTS MarlinMT::Book
	)
endif()

marlinmt_add_test (
  marlinmtminusx
  COMMAND $<TARGET_FILE:bin_MarlinMT>
  EXEC_ARGS -x test_marlinmtminusx.xml
  REGEX_PASS NONE
  MARLINMT_DLL MarlinMT::CorePlugins
)

marlinmt_add_test (
  check-marlinmtminusx
  COMMAND cat
  EXEC_ARGS test_marlinmtminusx.xml
  REGEX_PASS "<parameter name=\"RandomSeed\">1234567890</parameter>"
  DEPENDS marlinmtminusx
  MARLINMT_DLL MarlinMT::CorePlugins
)

marlinmt_add_processor_test (
	histogram
	STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/histogram.xml
	INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
	REGEX_PASS "success"
	MARLINMT_DLL MarlinMT::CorePlugins MarlinMT::LCIOPlugins 
  MARLINMT_ARGS -j 3
)

marlinmt_add_processor_test (
  eventmodifier
  STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/eventmodifier.xml
  INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
  REGEX_PASS "TestEventModifier modified 3 events in 1 run"
  MARLINMT_DLL MarlinMT::CorePlugins MarlinMT::LCIOPlugins
)

marlinmt_add_processor_test (
  processoreventseeder
  STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/processoreventseeder.xml
  INPUT_FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
  REGEX_PASS "TestProcessorEventSeeder failed to register processor to event seed generator"
  REGEX_FAIL "ERROR .TestProcessorEventSeeder.* Seeds don't match"
  MARLINMT_DLL MarlinMT::CorePlugins MarlinMT::LCIOPlugins
)

marlinmt_add_processor_test (
  includeandconstants
  STEERING_FILE ${CMAKE_CURRENT_SOURCE_DIR}/steer/base-eventmodifier.xml
  INPUT_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/data/simjob.slcio
    ${CMAKE_CURRENT_SOURCE_DIR}/steer/include-eventmodifier.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/steer/constants-eventmodifier.xml
  REGEX_PASS "TestEventModifier modified 3 events in 1 run"
  MARLINMT_DLL MarlinMT::CorePlugins MarlinMT::LCIOPlugins
)
