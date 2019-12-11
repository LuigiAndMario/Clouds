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
