bin_PROGRAMS = unikey-xim
unikey_xim_SOURCES = \
	main.c \
    IC.c \
    wrapper.c \
	$(NULL)
    
# A small trick to force G++ as a linker
#nodist_EXTRA_unikey_xim_SOURCES = dummy.cpp    
    
unikey_xim_LDADD = \
	$(top_srcdir)/src/IMdkit/libIMdkit.la \
    $(top_srcdir)/src/viEngine/libvi-engine.la \
    @X11_LIBS@ \
	$(NULL)

unikey_xim_CFLAGS = \
    @X11_CFLAGS@ \
    -I$(top_srcdir)/src/IMdkit \
    -I$(top_srcdir)/src \
    -I$(top_srcdir)/src/viEngine \
    $(NULL)
    