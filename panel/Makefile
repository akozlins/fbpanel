## miniconf makefiles ## 1.1 ##

TOPDIR := ..

panel_src = bg.c \
    ev.c \
    gconf.c \
    gconf_panel.c \
    gconf_plugins.c \
    gtkbar.c \
    gtkbgbox.c \
    misc.c \
    panel.c \
    plugin.c \
    run.c \
    xconf.c
panel_cflags = $(GTK2_CFLAGS) $(GMODULE2_CFLAGS) $(X11_CFLAGS) 
panel_libs = $(GTK2_LIBS) $(GMODULE2_LIBS) $(X11_LIBS) -lm
panel_type = bin 

include $(TOPDIR)/.config/rules.mk
