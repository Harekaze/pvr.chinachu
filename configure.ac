#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([pvrchinachu], [3.3.0], [Harekaze project])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

ADDONNAME=pvr.chinachu.so
LIBEXT=so

case $host in
	arm-apple-darwin)
		ADDONNAME=pvr.chinachu-ios.dylib
		LIBEXT=dylib
		CXX=$(xcrun --sdk iphoneos --find clang++)
		LD=$(xcrun --sdk iphoneos --find ld)
		SYSROOT=$(xcrun --sdk iphoneos --show-sdk-path)
		AC_SUBST(AM_CPPFLAGS_EXT, "-arch armv7 -D__arm__ -isysroot $SYSROOT")
		AC_SUBST(LDFLAGS_EXT, "-arch armv7 -isysroot $SYSROOT")
		;;
	*-apple-*)
		ADDONNAME=pvr.chinachu.dylib
		LIBEXT=dylib
		;;
	arm*-*-linux-gnu*)
		ADDONNAME=pvr.chinachu.armel.so
		AC_SUBST(AM_CPPFLAGS_EXT, "-D_ARMEL")
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
