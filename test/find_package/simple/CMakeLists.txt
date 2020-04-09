
cmake_minimum_required( VERSION 3.14 )
project( simple )

find_package( ILCUTIL COMPONENTS streamlog REQUIRED )
find_package( MarlinMT REQUIRED )

add_executable( simple simple.cc )
target_link_libraries( simple MarlinMT::Core )

target_include_directories( simple SYSTEM PUBLIC ${streamlog_INCLUDE_DIRS} )
