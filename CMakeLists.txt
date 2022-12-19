cmake_minimum_required(VERSION 3.22.1)

project(myrt LANGUAGES CXX CUDA)

find_package(Boost 1.74 REQUIRED COMPONENTS log_setup log)
find_package(PNGwriter 0.7.0)

# resolves error:
# /usr/include/c++/11/cstdlib:75:15: fatal error: stdlib.h: No such file or directory
#   75 | #include_next <stdlib.h>
set(NO_SYSTEM_FROM_IMPORTED ON)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -gencode arch=compute_75,code=sm_75 -rdc=true")
set(CMAKE_CUDA_FLAGS_DEBUG "${CMAKE_CUDA_FLAGS_DEBUG} -G")

file(GLOB_RECURSE CU_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu)
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(myrt ${CU_FILES} ${CPP_FILES})

set_target_properties(myrt PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
# target_compile_features(myrt PUBLIC cxx_std_17)

target_include_directories(myrt PRIVATE "/usr/local/cuda/include")

target_include_directories(myrt PRIVATE ${CMAKE_SOURCE_DIR}/src/external/)
# ply_io
# target_include_directories(myrt PRIVATE ${CMAKE_SOURCE_DIR}/src/external/ply_io/include)
# target_link_directories(myrt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/external/ply_io/build)

target_link_libraries(myrt
  boost_program_options
  ${Boost_LIBRARIES}
  # ply_io
  # PNGwriter::PNGwriter
)
