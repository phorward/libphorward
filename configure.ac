AC_PREREQ([2.68])
AC_INIT([Phorward Foundation Library], [0.17])

AC_CONFIG_SRCDIR([src/phorward.tpl.h])
#AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_RANLIB
AC_PROG_MAKE_SET

AM_PROG_AR
AM_PROG_CC_C_O

LT_INIT
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h unistd.h])
AC_CHECK_HEADERS([locale.h wchar.h wctype.h],
	[AC_DEFINE([UNICODE],[1],[Enabling UNICODE build])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset setlocale strchr strcspn strdup strrchr strspn strstr strtol])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 run/Makefile
				 min_lalr1/Makefile
				 DEMO/Makefile])
AC_OUTPUT
