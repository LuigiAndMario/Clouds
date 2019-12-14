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

add_executable(winds_vf winds_vf.cpp)
target_link_libraries(winds_vf ${VTK_LIBRARIES})

add_executable(compute_winds_vector_field compute_winds_vector_field.cpp)
target_link_libraries(compute_winds_vector_field ${VTK_LIBRARIES})

add_executable(winds_streamlines winds_streamlines.cpp)
target_link_libraries(winds_streamlines ${VTK_LIBRARIES})
