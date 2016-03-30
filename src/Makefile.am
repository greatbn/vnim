bin_PROGRAMS = vnim-xim
vnim_xim_SOURCES = \
	main.c \
    IC.c \
    wrapper.c \
    xim.c \
	$(NULL)
    
vnim_xim_LDADD = \
	$(top_srcdir)/src/IMdkit/libIMdkit.la \
    $(top_srcdir)/src/viEngine/libvi-engine.la \
    @X11_LIBS@ \
	$(NULL)

vnim_xim_CFLAGS = \
    @X11_CFLAGS@ \
    -I$(top_srcdir)/src/IMdkit \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/viEngine \
    $(NULL)
    