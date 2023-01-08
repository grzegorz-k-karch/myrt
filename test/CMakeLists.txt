cmake_minimum_required(VERSION 3.22.1)

project(myrt_tests LANGUAGES CXX CUDA)

find_package(Boost 1.74 REQUIRED COMPONENTS unit_test_framework log_setup log)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -gencode arch=compute_75,code=sm_75 -rdc=true")
set(CMAKE_CUDA_FLAGS_DEBUG "${CMAKE_CUDA_FLAGS_DEBUG} -G")

file(GLOB_RECURSE CPP_FILES ${CMAKE_SOURCE_DIR}/src/spectrum.cpp)

add_executable(spectrum ${CMAKE_CURRENT_SOURCE_DIR}/src/spectrum.cpp ${CU_FILES} ${CPP_FILES})

set_target_properties(spectrum PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

target_include_directories(spectrum PRIVATE "/usr/local/cuda/include")
target_include_directories(spectrum PRIVATE ${CMAKE_SOURCE_DIR}/src/external/)
target_include_directories(spectrum PRIVATE ${CMAKE_SOURCE_DIR}/src/)

IF(UNIX)
  target_link_libraries(spectrum
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    boost_program_options
    ${Boost_LIBRARIES}
)
ENDIF(UNIX)
add_test(spectrum spectrum)

