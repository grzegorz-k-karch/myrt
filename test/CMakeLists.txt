cmake_minimum_required(VERSION 3.22.1)

project(myrt_tests LANGUAGES CXX CUDA)

find_package(Boost 1.74 REQUIRED COMPONENTS unit_test_framework)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -gencode arch=compute_75,code=sm_75 -rdc=true")
set(CMAKE_CUDA_FLAGS_DEBUG "${CMAKE_CUDA_FLAGS_DEBUG} -G")

file(GLOB_RECURSE CU_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu)
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(spectrum ${CMAKE_CURRENT_SOURCE_DIR}/src/spectrum.cpp)
target_include_directories(spectrum PRIVATE "/usr/local/cuda/include")
target_include_directories(spectrum PRIVATE ${CMAKE_SOURCE_DIR}/src/external/)
target_include_directories(spectrum PRIVATE ${CMAKE_SOURCE_DIR}/src/)

IF(UNIX)
  target_link_libraries(spectrum ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
ENDIF(UNIX)
add_test(spectrum spectrum)

