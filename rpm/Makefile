ROOT = ../

include $(ROOT)/mak/config.mak

#Defines subdirectories to go to recursively

DIRS =  manLinux

define NL
endef

all:

depend:

tests:

poststarttests:

clean:

misc:

messages:

install: $(RECURSE_DEPENDS) $(ERROR)
	$(MKDIRHIER) $(DEST_ETC_DIR)
	$(MKDIRHIER) $(SYSCONF_PREFIX)/init.d
	$(MKDIRHIER) $(SYSCONF_PREFIX)/pam.d
	$(COPY) cimserver_planned.conf $(DEST_ETC_DIR)
	$(COPY) lsb-pegasus.rc $(SYSCONF_PREFIX)/init.d/lsb-pegasus
	$(COPY) wbem $(SYSCONF_PREFIX)/pam.d
	$(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) install $(NL) )

