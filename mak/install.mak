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
endif

ifdef PROGRAM
install:
	$(INSTALL_PROGRAM)
endif
