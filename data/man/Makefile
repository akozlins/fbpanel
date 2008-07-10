# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common

# backslashify slashes to avoid problems with sed
BPREFIX := $(subst /,\/,$(DATADIR))
SRC = fbpanel.1
TARGET := fbpanel.1.gz

all: $(TARGET)
$(TARGET): $(SRC)
	@$(call summary,TEXT,$@)
	${Q}VER=`awk 'BEGIN { FS="\"" } \
		/#define VERSION / { print $$2 }' $(TOPDIR)/version.h`; \
		sed  -e "s/@VERSION@/$${VER}/g" -e 's/@DATADIR@/$(BPREFIX)/g' < $(SRC) | gzip - > $@
		 

install: all
	install -d  $(MANDIR)/man1
	install -m 644 $(TARGET) $(MANDIR)/man1

uninstall:
	$(RM) $(MANDIR)/man1/$(TARGET)
