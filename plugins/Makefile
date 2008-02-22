# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common


INCS += -I../
SRC = image.c menu.c separator.c taskbar.c wincmd.c \
	dclock.c launchbar.c pager.c space.c test.c \
	icons.c deskno.c deskno2.c tclock.c

ifneq (,$(ENABLE_CPU))
SRC += cpu.c
endif

ifneq (1,$(STATIC_PLUGINS))
# plugins as shared libs 
override CFLAGS += -shared -fPIC
SOBJ = $(SRC:%.c=%.so)
all:$(SOBJ)
%.so: %.o
	$(call summary,LD  ,$@)
	$(Q)$(CC) $(CFLAGS) $(INCS) $< -o $@
ifeq (,$(ENABLE_DEVEL))
	$(Q)strip $@
endif

else
# plugins built in

all: plugins.a
plugins.a: $(OBJ)
	$(call summary,AR  ,$@)
	$(Q)$(AR) -cq $@ $(OBJ)
endif



ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),tar)
-include $(DEP)
endif
endif
endif


ifeq (0,$(STATIC_PLUGINS))
install: 
	install -d  $(LIBEXECDIR)
	install -m 644 $(SOBJ) $(LIBEXECDIR)

uninstall:
	cd $(LIBEXECDIR); $(RM) $(SOBJ)
endif
