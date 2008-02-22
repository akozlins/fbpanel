# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common

# backslashify slashes to avoid problems with sed
RSTR := $(subst /,\/,$(DATADIR))

CFG := $(wildcard *.cfg)
TARGET := $(CFG:%.cfg=%)

ICONS = dclock_glyphs.png default.xpm gnome-applications.png gnome-devel.svg \
  gnome-emacs.svg gnome-fs-desktop.svg gnome-fs-home.svg gnome-globe.svg \
  gnome-graphics.png gnome-joystick.svg gnome-lockscreen.png \
  gnome-multimedia.svg gnome-session-halt.png gnome-session-hibernate.png \
  gnome-session-logout.png gnome-session-reboot.png gnome-session-suspend.png \
  gnome-session-switch.png gnome-settings.svg gnome-system.png \
  gnome_terminal.svg gnome-util.svg mozilla-firefox.svg star.png

all: $(TARGET)

%: %.cfg
	$(call summary,TEXT,$@)
	$(Q)sed 's/@DATADIR@/$(RSTR)/g' < $< > $@
	@#echo "DATADIR=$(DATADIR) RSTR=$(RSTR)"

install: 
	install -d  $(DATADIR)
	install -m 644 $(TARGET) $(DATADIR) 
	install -d  $(DATADIR)/images
	cd images; install -m 644 $(ICONS) $(DATADIR)/images

uninstall:
	cd $(DATADIR); $(RM)  $(TARGET)
	cd $(DATADIR)/images; $(RM) $(ICONS)


