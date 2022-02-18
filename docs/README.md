The **offical documentation** for Damaris is available here: https://project.inria.fr/damaris/documentation/ 
  
The Doxyfile.in is processed when the project is configured by CMake using ENABLE_DOCS flag. The will allow the project HTML based anotated C++ documentation to be built via the make command.
  
e.g.  
  
```bash
cmake ../damaris -DENABLE_DOCS=ON <other configure args>

make doc
```
