AC_PREREQ([2.68])
AC_INIT([Phorward Toolkit], [0.22.1])
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

LT_INIT([win32-dll])
AC_PROG_LIBTOOL

# Check if txt2tags is installed here
AC_CHECK_PROG([TXT2TAGS], [txt2tags], "yes", "no")
AM_CONDITIONAL([WITH_TXT2TAGS],[test "$TXT2TAGS" = "yes"])

# Checks for libraries.
AC_CHECK_LIB([rt], [sprintf])

# Checks for header files.
AC_HEADER_STDC
#AC_CHECK_HEADERS([fcntl.h limits.h unistd.h locale.h])
AC_CHECK_HEADER([wchar.h],[AC_DEFINE([UTF8],[1],[Enabling UTF8 build])])
AC_CHECK_HEADER([wctype.h],[AC_DEFINE([UNICODE],[1],[Enabling UNICODE build])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
#AC_CHECK_FUNCS([islower isupper mbtowc memmove memset strchr strlen strrchr strstr strtof strtol strtoul tolower toupper vsnprintf vswprintf wcscat wcschr wcscmp wcscpy wcsncmp wcsncpy wcsrchr wcsstr wcstod wcstol wcstoul wctomb])

#AC_CHECK_FUNCS([iswlower iswupper towlower towupper], [],
#	[AC_DEFINE([UNICODE],[0],[Disabling UNICODE build])])

AC_CONFIG_FILES([Makefile
                 src/Makefile
				 tools/Makefile
				 examples/Makefile])
AC_OUTPUT
