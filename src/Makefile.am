bin_PROGRAMS = unikey-xim
unikey_xim_SOURCES = \
	main.c \
    IC.c \
	$(NULL)
    
unikey_xim_LDADD = \
	$(top_srcdir)/src/IMdkit/libIMdkit.la \
    @X11_LIBS@ \
	$(NULL)

unikey_xim_CFLAGS = \
    @X11_CFLAGS@ \
    -I$(top_srcdir)/src/IMdkit \
    -I$(top_srcdir)/src \
    $(NULL)
    