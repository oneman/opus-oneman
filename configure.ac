#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT(src/hybrid.h)
AM_INIT_AUTOMAKE(ietfcodec,20100708)

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_SUBDIRS([celt])
AC_CONFIG_SUBDIRS([silk])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT