cmake_minimum_required(VERSION 2.8)
 
PROJECT(prog_scale_clouds)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(prog_scale_clouds prog_scale_clouds.cpp)
 
target_link_libraries(prog_scale_clouds ${VTK_LIBRARIES})