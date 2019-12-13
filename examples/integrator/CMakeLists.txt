cmake_minimum_required(VERSION 2.8)
 
PROJECT(prog04)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(prog04 prog04.cpp flow.hpp math.hpp vtkhelper.hpp)
 
target_link_libraries(prog04 ${VTK_LIBRARIES})