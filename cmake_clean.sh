#!/bin/bash

#root directory
rm -rf .idea
rm -rf CMakeFiles/
rm -rf cmake-build-debug/
rm CMakeCache.txt 
rm DAMARIS.cbp 
rm Makefile 
rm cmake_install.cmake 
rm install_manifest.txt 
rm CMakeLists.txt.user
rm -rf Testing

#CTestTestfile.cmake
rm ./CTestTestfile.cmake
rm lib/CTestTestfile.cmake
rm examples/CTestTestfile.cmake
rm examples/storage/CTestTestfile.cmake
rm examples/visit/CTestTestfile.cmake
rm examples/fortran/CTestTestfile.cmake
rm examples/plugin/CTestTestfile.cmake
rm examples/paraview/CTestTestfile.cmake
rm test/CTestTestfile.cmake

#examples - CMakeFiles
rm -rf examples/CMakeFiles/
rm -rf examples/storage/CMakeFiles/
rm -rf examples/storage/log
rm -rf examples/visit/log
rm -rf examples/visit/CMakeFiles
rm -rf examples/plugin/CMakeFiles
rm -rf examples/fortran/CMakeFiles
rm -rf examples/paraview/CMakeFiles
rm -rf test/CMakeFiles

#Makefiles
rm examples/Makefile
rm examples/visit/Makefile 
rm examples/storage/Makefile 
rm examples/plugin/Makefile
rm examples/fortran/Makefile
rm examplea/paraview/Makefile
rm test/Makefile

#examples - binaries
rm examples/fortran/cube
rm examples/plugin/libgc.so
rm examples/storage/vector
rm examples/storage/2dmesh
rm examples/storage/3dmesh
rm examples/storage/blocks-hdf5
rm examples/storage/mandelbulb-hdf5
rm examples/storage/mandelbulb-hdf5
rm examples/visit/blocks
rm examples/visit/curve
rm examples/visit/life
rm examples/visit/mandelbulb
rm examples/visit/mesh
rm examples/visit/point
rm examples/visit/var
rm examples/Example
rm examples/paraview/image
rm examples/paraview/sector
rm examples/paraview/plate
rm examples/paraview/lollipop
rm lib/libdamaris.*

#tests - binaries
rm test/TestAction
rm test/TestActionManager
rm test/TestBuffer
rm test/TestChannel
rm test/TestDataSpace
rm test/TestEventLogger
rm test/TestLayoutManager
rm test/TestReactor
rm test/TestVariable
rm test/TestVariableManager

#examples - xml files
rm examples/visit/blocks.xml
rm examples/visit/curve.xml
rm examples/visit/mandelbulb.xml
rm examples/visit/life.xml
rm examples/visit/mesh.xml
rm examples/visit/point.xml
rm examples/visit/var.xml

rm examples/cmake_install.cmake 
rm examples/storage/cmake_install.cmake 
rm examples/visit/cmake_install.cmake 
rm examples/fortran/cmake_install.cmake 
rm examples/plugin/cmake_install.cmake 
rm examples/paraview/cmake_install.cmake
rm test/cmake_install.cmake

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
rm  lib/libdamaris.a

#logs
rm -rf examples/paraview/log
rm -rf examples/log
rm -rf examples/fortran/log
rm -rf examples/plugin/log
rm -rf examples/visit/log
rm -rf examples/storage/log
