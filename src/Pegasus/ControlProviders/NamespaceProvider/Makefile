# Makes the dynamic load libs for the files named in DIR
ROOT = $(PEGASUS_ROOT)

MODULE_NAME=NamespaceProvider

DIR = Pegasus/ControlProviders/$(MODULE_NAME)

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_NAMESPACEPROVIDER_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = NamespaceProvider

LIBRARIES = \
    pegcommon \
    pegconfig \
    pegprovider

SOURCES = $(MODULE_NAME).cpp

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
NEEDS_LIBRARIES = -L$(LIB_DIR) -lpegcommon -lpegconfig -lpegprovider
endif

include $(ROOT)/mak/library.mak
