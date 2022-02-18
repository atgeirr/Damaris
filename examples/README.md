The **offical documentation** for Damaris is available here: https://project.inria.fr/damaris/documentation/ 
  
The Damaris examples are created when the project is configured by CMake using ENABLE_EXAMPLES flag. 
  
e.g.  
  
```bash
cmake ../damaris -DENABLE_EXAMPLES=ON <other configure args>
make -j4      # build the Damaris library and the tests
make install  # Install the Damaris library and its examples

cd <install dir>/examples/storage

# run an example (requires HDF5 library capability of Damaris)
mpirun -np 4 ./3dmesh 3dmesh.xml -v
```
