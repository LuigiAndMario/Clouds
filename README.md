# Clouds visualisation

## Organisation
The folder must follow the following structure
```
+-- build
|  +-- Debug
|      +-- winds.cpp
|      +-- water.cpp
|      +-- pressure.cpp
|      +-- ice.cpp
+-- cloud_data
    +-- cli
    |   +-- cli_10.vti_scaled.vti
    |   +-- cli_20.vti_scaled.vti
    |   +-- cli_30.vti_scaled.vti
    +-- clw
    |   +-- clw_10.vti_scaled.vti
    |   +-- ...
    +-- ...
```
## Compiling
The `CMakeLists.txt` contains rules to compile every file

## Running
The resulting executables do not take any argument, and read the files in their correct location (explained on the previous folder structure)
 * `ice`: displays the cloud ice data as slices
 * `water`: displays the cloud water data as slices
 * `pressure`: displays the air pressure data as slices
 * `compute_winds_vector_field`: reads the wind data, downsamples it, then writes it as a vector field. This program should be executed prior to any of the following.
 * `winds_vf`: displays the wind data as it is
 * `winds_streamlines`: displays the streamlines of the wind data
 * `winds_vortcity`: displays the vorticity of the wind data

## Important note
Before running either one of the programs `winds_vf`, `winds_vorticity`, or `winds_streamlines`, you need to create the folder `.../cloud_data/winds/` and run the program `compute_winds_vector_field` in order to create the winds data set.

### Modifying the downsampling rate
The chosen downsampling rate for `compute_winds_vector_field` is 32. This can be changed at line 32 of `compute_winds_vector_field.cpp`.
