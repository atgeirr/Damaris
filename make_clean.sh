#!/bin/bash

#root directory
rm -rv .idea
rm -rf CMakeFiles/
rm -rf cmake-build-debug/
rm CMakeCache.txt 
rm DAMARIS.cbp 
rm Makefile 
rm cmake_install.cmake 
rm install_manifest.txt 

#examples
rm -rf examples/CMakeFiles/
rm -rf examples/storage/CMakeFiles/
rm -rf examples/storage/log
rm -rf examples/visit/log
rm -rf examples/visit/CMakeFiles
rm examples/visit/Makefile 
rm examples/storage/Makefile 
rm examples/storage/cmake_install.cmake 
rm examples/visit/cmake_install.cmake 
rm examples/storage/mandelbulb-hdf5
rm examples/visit/blocks
rm examples/visit/blocks.xml
rm examples/visit/curve
rm examples/visit/curve.xml
rm examples/visit/life
rm examples/visit/life.xml
rm examples/visit/mandelbulb
rm examples/visit/mandelbulb.xml
rm examples/visit/mesh
rm examples/visit/mesh.xml
rm examples/visit/point
rm examples/visit/point.xml
rm examples/visit/var
rm examples/visit/var.xml
rm examples/Example
rm examples/Makefile
rm examples/cmake_install.cmake 

#source and include files 
rm include/damaris/model/Model.hpp 
rm include/Damaris.h
rm include/DamarisFCMangle.h 
rm  src/model/Model.cpp 
rm  src/model/Model.hpp 

#libraries
rm -rf lib/CMakeFiles/
rm  lib/Makefile 
rm  lib/cmake_install.cmake 
rm  lib/damaris.mod 

