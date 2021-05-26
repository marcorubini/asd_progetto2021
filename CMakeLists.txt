cmake_minimum_required(VERSION 3.16)
project(asd_progetto2021)



add_library(asd_progetto2021 INTERFACE)
target_include_directories(asd_progetto2021 INTERFACE include)
target_compile_features(asd_progetto2021 INTERFACE cxx_std_11)

add_executable(solution solution.cpp)
target_link_libraries(solution PRIVATE asd_progetto2021)

option(ENABLE_SUPERBUILD "Generate single header using python script." ON)
if(ENABLE_SUPERBUILD)
  add_subdirectory(superbuild)
endif()
