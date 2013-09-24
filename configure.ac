AC_PREREQ([2.68])
AC_INIT([Phorward Foundation Toolkit], [1.0])
AC_PREFIX_DEFAULT([/usr])

AC_CONFIG_SRCDIR([src/phorward.tpl.h])
#AC_CONFIG_HEADERS([include/config.h])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Enable Trace facilities?
AC_MSG_CHECKING([whether Phorward trace facilities will be integrated])
AC_ARG_WITH([trace],
	[AS_HELP_STRING([--with-trace],
		[enable trace macro facilities (huger binary files)])],
		[with_trace="yes" AC_DEFINE([DEBUG])],
		[with_trace="no"])
AC_MSG_RESULT([$with_trace])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Check for unsigned-char force flag

#AC_PROG_CC_CHAR_SUBSCRIPTS([UNSIGNED_CHAR])
#CFLAGS="$CFLAGS $UNSIGNED_CHAR"
#rm -f writetest.c

AM_PROG_AR
AM_PROG_CC_C_O

LT_INIT
AC_PROG_LIBTOOL

# Check if txt2tags is installed here
AC_CHECK_PROG([TXT2TAGS], [txt2tags], "yes", "no")
AM_CONDITIONAL([WITH_TXT2TAGS],[test "$TXT2TAGS" = "yes"])

# Checks for libraries.
AC_CHECK_LIB([rt], [sprintf])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h unistd.h])
AC_CHECK_HEADERS([locale.h wchar.h wctype.h],
	[AC_DEFINE([UNICODE],[1],[Enabling UNICODE build])
	AC_DEFINE([UTF8],[1],[Enabling UTF8 build])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strrchr])
AC_CHECK_FUNCS([strstr])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strtoul])
AC_CHECK_FUNCS([strtof])
AC_CHECK_FUNCS([wctomb])
AC_CHECK_FUNCS([mbtowc])
AC_CHECK_FUNCS([vsnprintf])
AC_CHECK_FUNCS([vswprintf])
AC_CHECK_FUNCS([wcscpy])
AC_CHECK_FUNCS([wcscat])
AC_CHECK_FUNCS([wcsncpy])
AC_CHECK_FUNCS([wcscmp])
AC_CHECK_FUNCS([wcsncmp])
AC_CHECK_FUNCS([wcstol])
AC_CHECK_FUNCS([wcstoul])
AC_CHECK_FUNCS([wcstod])
AC_CHECK_FUNCS([wcsstr])
AC_CHECK_FUNCS([wcschr])
AC_CHECK_FUNCS([wcsrchr])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 run/Makefile
				 DEMO/Makefile])
AC_OUTPUT
