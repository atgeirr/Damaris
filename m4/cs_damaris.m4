
# CS_AC_TEST_DAMARIS
#--------------------
# modifies or sets cs_have_damaris, DAMARIS_CPPFLAGS, DAMARIS_LDFLAGS,
# and DAMARIS_LIBS depending on libraries found

AC_DEFUN([CS_AC_TEST_DAMARIS], [

cs_have_damaris=no
# cs_have_damaris_catalyst is not used as it is currently assumed
cs_have_damaris_catalyst=no

             
             
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



# Now check for libraries
#------------------------

if test "x$with_damaris" != "xno"; then

  saved_CPPFLAGS="$CPPFLAGS"
  saved_LDFLAGS="$LDFLAGS"
  saved_LIBS="$LIBS"

  DAMARIS_CPPFLAGS="${DAMARIS_CPPFLAGS} ${XSD_CPPFLAGS}"
  DAMARIS_LIBS="-rdynamic -Wl,--whole-archive -ldamaris -Wl,--no-whole-archive"
  DAMARIS_LDFLAGS="${DAMARIS_LDFLAGS}"

  if test "x$cs_have_mpi" != "xno"; then

    AC_MSG_CHECKING([for Damaris library])

    CPPFLAGS="${CPPFLAGS} ${DAMARIS_CPPFLAGS} ${MPI_CPPFLAGS}"
    LDFLAGS="${LDFLAGS} ${DAMARIS_LDFLAGS} ${MPI_LDFLAGS}"
    LIBS="${DAMARIS_LIBS} ${MPI_LIBS}"

    AC_LINK_IFELSE([AC_LANG_PROGRAM(
[[#include <stdint.h>
#include <Damaris.h>]],
(void)damaris_initialize("name", MPI_COMM_SELF);)],
                   [cs_have_damaris=yes],
                   [cs_have_damaris=no])

  fi


 
  # Report Damaris support
  #------------------------

  if test "x$cs_have_damaris" = "xyes" ; then
    AC_DEFINE([HAVE_DAMARIS], 1, [Damaris asynchronous I/O support])
  elif test "x$cs_have_damaris" = "xno" ; then
    if test "x$with_damaris" != "xcheck" ; then
      AC_MSG_FAILURE([Damaris asynchronous I/O support requested, but test for Damaris failed!])
    else
      AC_MSG_WARN([no Damaris asynchronous I/O support])
    fi
  fi

  AC_MSG_RESULT($cs_have_damaris)

  if test "x$cs_have_damaris" != "xyes"; then
    DAMARIS_LIBS=""
  fi

  CPPFLAGS="$saved_CPPFLAGS"
  LDFLAGS="$saved_LDFLAGS"
  LIBS="$saved_LIBS"

  unset saved_CPPFLAGS
  unset saved_LDFLAGS
  unset saved_LIBS

fi

AM_CONDITIONAL(HAVE_DAMARIS, test x$cs_have_damaris = xyes)


cs_py_have_plugin_damaris=False
if test x$cs_have_plugin_damaris = xyes ; then
  cs_py_have_plugin_damaris=True
fi

AC_SUBST(cs_have_damaris)
AC_SUBST(DAMARIS_CPPFLAGS)
AC_SUBST(DAMARIS_LDFLAGS)
AC_SUBST(DAMARIS_LIBS)
AC_SUBST(DAMARISRUNPATH)

])dnl
