
## recursion make
.DEFAULT_GOAL := all
.PHONY : subdirs all clean clean_tmp install install_lib install_emod install_script $(SUBDIRS)
all clean install : subdirs
subdirs : $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
unexport SUBDIRS


## nice summary output for compilation tasks
Q := @
summary = @echo "$(1)" $(subst $(TOPDIR),,$(CURDIR)/)$(2)

## variable twiking
CFLAGS += -I$(TOPDIR) -I$(TOPDIR)/panel $(GTK_CFLAGS) -fPIC
LDFLAGS += $(GTK_LIBS) -lXmu
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)



# code compilation rule
%.o : %.c
	$(call summary,CC  ,$@)
	$Q$(CC) $(CFLAGS) -c -o $@ $<

    
%.d : %.c
	$(call summary,DEP ,$@)
	$Q$(CC) $(CFLAGS) -M $< | sed 's,\(.*\)\.o[ :]*,$(@D)/\1.o $@ : ,g' > $@




# make binary
ifneq (,$(BINTARGET))
all : $(BINTARGET)
$(BINTARGET) : $(OBJS)
	$(call summary,LD  ,$@)
	$Q$(CC) $(OBJS) -o $@ $(LDFLAGS) 

install : install_bin

clean : clean_tmp

endif


# linking rule
ifneq (,$(LIBTARGET))
all : $(LIBTARGET)
$(LIBTARGET) : $(OBJS)
	$(call summary,LD  ,$@)
	$Q$(CC) $(OBJS) -o $@ $(LDFLAGS) -shared

install : install_lib

clean : clean_tmp

endif


ifneq (,$(ARTARGET))
all : $(ARTARGET)
$(ARTARGET) : $(OBJS)
	@echo AR   $@
	$(call summary,CC  ,$@)
	@$(AR) r $@ $(OBJS)

clean : clean_tmp

endif

ifneq (,$(COFIG))
install : install_conf

endif

ifneq (,$(SCRIPT))
install : install_script

endif

install_bin :
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(BINTARGET) $(DESTDIR)$(BINDIR)

install_emod :
	install -d $(DESTDIR)$(LIBDIR)
	install -m 755 $(EMODTARGET) $(DESTDIR)$(LIBDIR)

install_lib :
	install -d $(DESTDIR)$(LIBDIR)
	install -m 755 $(LIBTARGET) $(DESTDIR)$(LIBDIR)

install_conf :
	install -d $(DESTDIR)$(SYSCONFDIR) 
	install -m 644 $(CONF) $(DESTDIR)$(SYSCONFDIR)

install_script :
	install -d $(DESTDIR)$(LIBEXECDIR)
	install -m 755 $(SCRIPT) $(DESTDIR)$(LIBEXECDIR)


clean_tmp :
	rm -f $(EMODTARGET) $(LIBTARGET) $(ARTARGET) $(BINTARGET)
	find -name "*.o" -o -name "*.d" -o -name "*~" | xargs rm -f 

# define these targets for makefiles without them will work
clean: 
all:
install:


ifeq ($(MAKECMDGOALS), all)
-include $(CDEPS)
endif

