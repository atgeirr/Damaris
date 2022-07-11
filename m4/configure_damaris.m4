
# CONF_AC_TEST_DAMARIS
#--------------------
# modifies or sets conf_have_damaris, DAMARIS_CPPFLAGS, DAMARIS_LDFLAGS,
# and DAMARIS_LIBS depending on libraries found
# Sets the following for infomation about Damaris plugins:
# conf_have_damaris_catalyst, conf_have_damaris_catalyst, conf_have_damaris_hdf5,
# conf_have_damaris_python
# 
# In configure.ac use something like the following:
# ...
# CONF_AC_TEST_DAMARIS
# ...
# echo " Damaris (asynchronous I/O) support: "$conf_have_damaris""
# echo " Damaris has support for :"
# echo "                Catalyst : "$conf_have_damaris_catalyst""
# echo "                   VisIt : "$conf_have_damaris_visit""
# echo "                    HDF5 : "$conf_have_damaris_hdf5""
# echo "                  Python : "$conf_have_damaris_python""


AC_DEFUN([CONF_AC_TEST_DAMARIS], [

conf_have_damaris=no
conf_have_damaris_catalyst=no
conf_have_damaris_visit=no
conf_have_damaris_hdf5=no
conf_have_damaris_python=no
             
             
# Configure options for Damaris paths
#------------------------------------

AC_ARG_WITH(damaris,
            [AS_HELP_STRING([--with-damaris=PATH],
                            [specify prefix directory for DAMARIS])],
            [if test "x$withval" = "x"; then
               with_damaris=no
             fi],
            [with_damaris=no])

AC_ARG_WITH(damaris-include,
            [AS_HELP_STRING([--with-damaris-include=DIR],
                            [specify directory for damaris include files])],
            [if test "x$with_damaris" = "xcheck"; then
               with_damaris=yes
             fi
             DAMARIS_CPPFLAGS="-I$with_damaris_include"],
            [if test "x$with_damaris" != "xno" -a "x$with_damaris" != "xyes" \
	          -a "x$with_damaris" != "xcheck"; then
               DAMARIS_CPPFLAGS="-I$with_damaris/include"
             fi])

AC_ARG_WITH(damaris-lib,
            [AS_HELP_STRING([--with-damaris-lib=DIR],
                            [specify directory for damaris shared library])],
            [if test "x$with_damaris" = "xcheck"; then
               with_damaris=yes
             fi
             DAMARIS_LDFLAGS="-L$with_damaris_lib"
             # Add the libdir to the runpath as damaris is not libtoolized
             DAMARISRUNPATH="-R$with_damaris_lib"],
            [if test "x$with_damaris" != "xno" -a "x$with_damaris" != "xyes" \
	          -a "x$with_damaris" != "xcheck"; then
               DAMARIS_LDFLAGS="-L$with_damaris/lib"
               # Add the libdir to the runpath as damaris is not libtoolized
               DAMARISRUNPATH="-R$with_damaris/lib"
             fi])



if test "x$with_damaris" != "xno"; then
    if test -f "$with_damaris/share/cmake/damaris/DamarisConfig.cmake"  ; then
    
      # Check if Damaris was compiled with Paraview/Catalyst support
      #------------------------
      ON_OR_OFF=`grep Damaris_HAS_CATALYST  ${with_damaris}/share/cmake/damaris/DamarisConfig.cmake | grep ON`
      if test "x$ON_OR_OFF" != "x"  ; then
        conf_have_damaris_catalyst=yes
      fi 
    
      # Check if Damaris was compiled with Visit support
      #------------------------
      ON_OR_OFF=`grep Damaris_HAS_VISIT  ${with_damaris}/share/cmake/damaris/DamarisConfig.cmake | grep ON`
      if test "x$ON_OR_OFF" != "x"  ; then
        conf_have_damaris_visit=yes
      fi

       
      # Check if Damaris was compiled with HDF5 support
      #------------------------   
      ON_OR_OFF=`grep Damaris_HAS_HDF5  ${with_damaris}/share/cmake/damaris/DamarisConfig.cmake | grep ON`
      if test "x$ON_OR_OFF" != "x"  ; then
        conf_have_damaris_hdf5=yes
      fi
     
      # Check if Damaris was compiled with Python support
      #------------------------
      ON_OR_OFF=`grep Damaris_HAS_PYTHON  ${with_damaris}/share/cmake/damaris/DamarisConfig.cmake | grep ON`
      if test "x$ON_OR_OFF" != "x"  ; then
        conf_have_damaris_python=yes
      fi
     
    else
      echo "Damaris asynchronous I/O test for file  ${with_damaris}/share/cmake/damaris/DamarisConfig.cmake failed. Therefore cannot test for Damaris plugins support."
    fi   
fi
  

  
# Now check for libraries
#------------------------

if test "x$with_damaris" != "xno"; then

  saved_CPPFLAGS="$CPPFLAGS"
  saved_LDFLAGS="$LDFLAGS"
  saved_LIBS="$LIBS"

  DAMARIS_CPPFLAGS="${DAMARIS_CPPFLAGS} ${XSD_CPPFLAGS}"
  DAMARIS_LIBS="-rdynamic -Wl,--whole-archive -ldamaris -Wl,--no-whole-archive"
  DAMARIS_LDFLAGS="${DAMARIS_LDFLAGS}"

  
  if test "x$conf_have_mpi" != "xno"; then

    AC_MSG_CHECKING([for Damaris library])

    CPPFLAGS="${CPPFLAGS} ${DAMARIS_CPPFLAGS} ${MPI_CPPFLAGS}"
    LDFLAGS="${LDFLAGS} ${DAMARIS_LDFLAGS} ${MPI_LDFLAGS}"
    LIBS="${DAMARIS_LIBS} ${MPI_LIBS}"

    AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[#include <stdint.h>
#include <Damaris.h>]],
(void)damaris_initialize("name", MPI_COMM_SELF);)],
                   [conf_have_damaris=yes],
                   [conf_have_damaris=no])

  fi


 
  # Report Damaris support
  #------------------------

  if test "x$conf_have_damaris" = "xyes" ; then
    AC_DEFINE([HAVE_DAMARIS], 1, [Damaris asynchronous I/O support])
  elif test "x$conf_have_damaris" = "xno" ; then
    if test "x$with_damaris" != "xcheck" ; then
      AC_MSG_FAILURE([Damaris asynchronous I/O support requested, but test for Damaris failed!])
    else
      AC_MSG_WARN([no Damaris asynchronous I/O support])
    fi
  fi

  AC_MSG_RESULT($conf_have_damaris)

  if test "x$conf_have_damaris" != "xyes"; then
    DAMARIS_LIBS=""
  fi

  CPPFLAGS="$saved_CPPFLAGS"
  LDFLAGS="$saved_LDFLAGS"
  LIBS="$saved_LIBS"

  unset saved_CPPFLAGS
  unset saved_LDFLAGS
  unset saved_LIBS

fi

AM_CONDITIONAL(HAVE_DAMARIS, test x$conf_have_damaris = xyes)


conf_py_have_plugin_damaris=False
if test x$conf_have_plugin_damaris = xyes ; then
  conf_py_have_plugin_damaris=True
fi

AC_SUBST(conf_have_damaris)
AC_SUBST(DAMARIS_CPPFLAGS)
AC_SUBST(DAMARIS_LDFLAGS)
AC_SUBST(DAMARIS_LIBS)
AC_SUBST(DAMARISRUNPATH)

])dnl
