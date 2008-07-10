# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common
#$(warning INCS=$(INCS))

INCS += -I../
SRC := egg-marshal.c   eggtraymanager.c  fixedtip.c main.c  



ifneq (1,$(STATIC_PLUGINS))
# tray as shared libs 
override CFLAGS += -shared -fPIC
TARGET = tray.so
$(TARGET): $(OBJ) 
	$(call summary,LD  ,$@)
	$(Q)$(CC) $(CFLAGS) $(INCS) $(OBJ) -o $@
ifeq (,$(ENABLE_DEVEL))
	$(Q)strip $@
endif

else
# tray built in
TARGET = tray.a
$(TARGET): $(OBJ) 
	$(call summary,AR  ,$@)
	$(Q)$(AR) -cq $@ $(OBJ)
endif
all:$(TARGET)


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
	install -m 644 $(TARGET)  $(LIBEXECDIR)

uninstall:
	cd $(LIBEXECDIR); $(RM) $(TARGET)

endif
