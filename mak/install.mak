################################################################################
##
## Default installation macro
##
################################################################################

## The INSTALL_LIBRARY and INSTALL_PROGRAM macros are defined in config.mak
## and in platform_<*>.mak files (if the default has to be changed)
ifdef LIBRARY
install:
	$(INSTALL_LIBRARY)
	$(ECHO) $(LIBRARY) $(SUFFIX)
    ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
	$(MAKE) --directory=$(DEST_LIB_DIR) -f $(PEGASUS_ROOT)/mak/install.mak install_ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=sl
    endif
    ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
	$(MAKE) --directory=$(DEST_LIB_DIR) -f $(PEGASUS_ROOT)/mak/install.mak install_ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=so
    endif
    ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
	$(MAKE) --directory=$(DEST_LIB_DIR) -f $(PEGASUS_ROOT)/mak/install.mak install_ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=so
    endif
endif

ifdef PROGRAM
install:
	$(INSTALL_PROGRAM)
endif

install_ln:
	ln -f -s $(LIBRARY)$(SUFFIX) $(LIBRARY).$(PLATFORM_SUFFIX)
	
