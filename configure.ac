AC_INIT([A Vietnamese Input Method for X Windows], 0.1.0)
AM_INIT_AUTOMAKE([subdir-objects])
AC_PROG_CC
LT_INIT

AC_ARG_WITH([gtk], AS_HELP_STRING([--with-gtk], [Build with the GTK+ interface]))

# Check for x11
PKG_CHECK_MODULES(X11, [
    x11
])
AC_CHECK_HEADERS([X11/XKBlib.h])

PKG_CHECK_MODULES([GTK3], [gtk+-3.0])

AC_CONFIG_FILES(
	Makefile
	src/Makefile
    src/IMdkit/Makefile
    src/viEngine/Makefile
    src/appindicator/Makefile
)
AC_OUTPUT
AC_MSG_RESULT([
  Install prefix                $prefix
])
