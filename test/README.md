The **offical documentation** for Damaris is available here: https://project.inria.fr/damaris/documentation/ 
  
The tests are created when the project is configured by CMake using ENABLE_TESTS flag. This will require the cppunit library to be avialable on the system.
  
e.g.  
  
```bash
cmake ../damaris -DENABLE_TESTS=ON <other configure args>
make -j4   # build the Damaris library and the tests

ctest                      # run the tests
ctest --output-on-failure  # run the tests with verbode output for failed test only
ctest -VV                  # run the tests with verbode output
```
