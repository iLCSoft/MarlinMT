
cmake_minimum_required( VERSION 3.14 )
project( simple )

find_package( MarlinMT COMPONENTS LCIOPlugins REQUIRED )

add_executable( simple simple.cc )
# This should fail, as MarlinMT::LCIOPlugins is a module not a shared library
target_link_libraries( simple MarlinMT::Core MarlinMT::LCIOPlugins )

