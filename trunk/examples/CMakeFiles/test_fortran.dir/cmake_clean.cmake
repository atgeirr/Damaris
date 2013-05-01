FILE(REMOVE_RECURSE
  "CMakeFiles/test_fortran.dir/test.f90.o"
  "test_fortran.pdb"
  "test_fortran"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang Fortran)
  INCLUDE(CMakeFiles/test_fortran.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
