cmake_minimum_required(VERSION 3.12)

project(necessary-and-unnecessary-tiles LANGUAGES CXX)

option(ENABLE_NYANTEN "ENABLE_NYANTEN" on)
option(FIX_RANDOM_SEED "FIX_RANDOM_SEED" off)

find_package(TBB OPTIONAL_COMPONENTS Tbb)

add_library(compile_settings INTERFACE)
target_compile_features(compile_settings INTERFACE cxx_std_20)
target_compile_options(compile_settings INTERFACE
  $<$<CONFIG:Debug>:-O0 -Wall -Wextra -g>
  $<$<CONFIG:Release>:-O3 -Wall -Wextra>)

add_library(calsht_dw src/calsht_dw.cpp)
target_link_libraries(calsht_dw compile_settings)
target_include_directories(calsht_dw PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_definitions(calsht_dw INTERFACE
  $<BUILD_INTERFACE:INDEX_FILE_PATH="${PROJECT_BINARY_DIR}">
  $<INSTALL_INTERFACE:INDEX_FILE_PATH="${CMAKE_INSTALL_PREFIX}/share/data">
  PUBLIC $<$<BOOL:${ENABLE_NYANTEN}>:ENABLE_NYANTEN>)
set_target_properties(calsht_dw PROPERTIES PUBLIC_HEADER src/calsht_dw.hpp)
install(TARGETS calsht_dw compile_settings EXPORT CalshtDW)
install(FILES
  ${PROJECT_BINARY_DIR}/index_dw_s.bin
  ${PROJECT_BINARY_DIR}/index_dw_h.bin
  DESTINATION share/data)
install(EXPORT CalshtDW FILE CalshtDWConfig.cmake DESTINATION share/cmake)

add_executable(mkind src/mkind.cpp)
target_compile_definitions(mkind PRIVATE
  $<$<BOOL:${ENABLE_NYANTEN}>:ENABLE_NYANTEN>
  $<$<BOOL:${TBB_FOUND}>:ENABLE_PARALLEL>)
target_link_libraries(mkind compile_settings $<$<BOOL:${TBB_FOUND}>:TBB::tbb>)

add_executable(sample src/sample.cpp)
target_compile_definitions(sample PRIVATE $<$<BOOL:${FIX_RANDOM_SEED}>:FIX_RANDOM_SEED>)
target_link_libraries(sample calsht_dw)
