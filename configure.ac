AC_INIT([Tutorial Program], 1.0)
AM_INIT_AUTOMAKE([subdir-objects])
AC_PROG_CC
AC_PROG_CXX
LT_INIT

# Check for x11
PKG_CHECK_MODULES(X11, [
    x11
])
AC_CHECK_HEADERS([X11/XKBlib.h])

AC_CONFIG_FILES(
	Makefile
	src/Makefile
    src/IMdkit/Makefile
    src/ukengine/Makefile
)
AC_OUTPUT


