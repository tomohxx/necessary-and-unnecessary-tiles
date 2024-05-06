cmake_minimum_required(VERSION 3.1)

project(necessary-and-unnecessary-tiles)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -DCHECK_HAND")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")
set(CMAKE_INSTALL_DATADIR share/data)

option(THREE_PLAYER "Three player mode" off)

if(THREE_PLAYER)
  add_definitions(-DTHREE_PLAYER)
endif()

add_library(calsht_dw STATIC src/calsht_dw.cpp)
set_target_properties(calsht_dw PROPERTIES PUBLIC_HEADER src/calsht_dw.hpp)
install(TARGETS calsht_dw EXPORT CalshtDW)
install(FILES
  ${PROJECT_BINARY_DIR}/index_dw_s.txt
  ${PROJECT_BINARY_DIR}/index_dw_h.txt
TYPE DATA)
install(EXPORT CalshtDW FILE CalshtDWConfig.cmake DESTINATION share/cmake)

add_executable(mkind2 src/mkind2.cpp src/judwin.cpp)
add_executable(sample src/sample.cpp)
target_compile_definitions(sample PUBLIC INDEX_FILE_PATH="${PROJECT_BINARY_DIR}")
target_link_libraries(sample calsht_dw)
add_custom_target(extract ALL ${CMAKE_COMMAND} -E
  tar zxvf ${PROJECT_SOURCE_DIR}/data.tar.gz
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
