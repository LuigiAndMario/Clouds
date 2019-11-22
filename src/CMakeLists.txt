cmake_minimum_required(VERSION 2.8)
 
PROJECT(clouds)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(clouds clouds.cpp vtkEasyTransfer.hpp)
 
target_link_libraries(clouds ${VTK_LIBRARIES})
