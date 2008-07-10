
## recursion make
.DEFAULT_GOAL := all
.PHONY : all clean clean_tmp distclean
.PHONY : install install_lib install_emod install_script 
.PHONY : subdirs $(SUBDIRS)
all clean install : subdirs FORCE
# this one to prevent printing make[1]: Nothing to be done for `all'
FORCE:
	@#
subdirs : $(SUBDIRS)
$(SUBDIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)
unexport SUBDIRS


## nice summary output for compilation tasks
Q := @
summary = @echo "$(1)" $(subst $(TOPDIR)/,,$(CURDIR)/)$(2)
MAKEFLAGS += --no-print-directory

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

clean : clean_obj
	rm -f $(BINTARGET)

endif


# linking rule
ifneq (,$(LIBTARGET))
all : $(LIBTARGET)
$(LIBTARGET) : $(OBJS)
	$(call summary,LD  ,$@)
	$Q$(CC) $(OBJS) -o $@ $(LDFLAGS) -shared

install : install_lib

clean : clean_obj
	rm -f $(LIBTARGET)

endif


ifneq (,$(ARTARGET))
all : $(ARTARGET)
$(ARTARGET) : $(OBJS)
	@echo AR   $@
	$(call summary,CC  ,$@)
	@$(AR) r $@ $(OBJS)

clean : clean_obj
	rm -f $(ARTARGET)

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

install_lib :
	install -d $(DESTDIR)$(LIBDIR)/fbpanel
	install -m 755 $(LIBTARGET) $(DESTDIR)$(LIBDIR)/fbpanel

install_conf :
	install -d $(DESTDIR)$(SYSCONFDIR) 
	install -m 644 $(CONF) $(DESTDIR)$(SYSCONFDIR)

install_script :
	install -d $(DESTDIR)$(LIBEXECDIR)
	install -m 755 $(SCRIPT) $(DESTDIR)$(LIBEXECDIR)


clean_obj :
	rm -f $(OBJS)

# define these targets for makefiles without them will work
clean: 
all:
install:

distclean : 
	$(MAKE) clean
	find . -name "*.in" | sed -e 's/\.in$$//g' | xargs rm -f
	rm -f config.h config.mk subst.sed

ifeq ($(MAKECMDGOALS), all)
-include $(CDEPS)
endif

