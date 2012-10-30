#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ([2.65])
AC_INIT(moflow900, 0.1, alan@eth0.org.uk)
AM_INIT_AUTOMAKE(moflow900, 0.1.0)
AC_OUTPUT(Makefile src/Makefile)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/main.c])
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
PKG_CHECK_MODULES([jsonglib], [json-glib-1.0])
PKG_CHECK_MODULES([moflow], [moflow])
PKG_CHECK_MODULES([gthread], [gthread-2.0])
PKG_CHECK_MODULES([gtk], [gtk+-2.0])
PKG_CHECK_MODULES([hildon], [hildon-1])

AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_REALLOC

AC_OUTPUT
