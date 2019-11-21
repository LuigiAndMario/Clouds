# Clouds visualisation

## To compile
* Open cmake-gui
* Select the path to `clouds.cpp` for the source code
* Select the path to the parent folder of `clouds.cpp`, and add `/build` for the binaries
* Press configure
* cmake-gui will crash because it hasn't found the path to VTK - enter the path to VTK in the correct field
* Press configure again
* Press generate
* Press open project
