#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([pvrchinachu], [0.0.3], [Yuki Mizuno])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

ADDONNAME=PVRChinachu.pvr
LIBEXT=so

case $host in
	*-apple-*)
		LIBEXT=dylib
		;;
	*-androideabi)
		ADDONNAME=libpvr.chinachu.so
		AC_SUBST(AM_CPPFLAGS_EXT, "-DANDROID")
		;;
esac

AC_SUBST(ADDONNAME)
AC_SUBST(LIBEXT)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([float.h inttypes.h locale.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([localeconv memset modf select strdup])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/chinachu/Makefile
                 src/pvr_client/Makefile])
AC_OUTPUT
