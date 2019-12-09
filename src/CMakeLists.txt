cmake_minimum_required(VERSION 2.8)
 
PROJECT(clouds)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(ice ice.cpp)
target_link_libraries(ice ${VTK_LIBRARIES})

add_executable(water water.cpp)
target_link_libraries(water ${VTK_LIBRARIES})

add_executable(pressure pressure.cpp)
target_link_libraries(pressure ${VTK_LIBRARIES})

add_executable(winds winds.cpp)
target_link_libraries(winds ${VTK_LIBRARIES})
