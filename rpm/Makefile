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
	$(COPY) cimserver_planned.conf $(DEST_ETC_DIR)

	$(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) install $(NL) )

