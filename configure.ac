AC_INIT([A Vietnamese Input Method for X Windows], 0.1.0)
AM_INIT_AUTOMAKE([subdir-objects])
AC_PROG_CC
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
    src/viEngine/Makefile
)
AC_OUTPUT
AC_MSG_RESULT([
  Install prefix                $prefix
])