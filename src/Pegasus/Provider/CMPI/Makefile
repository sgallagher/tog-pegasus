ROOT = ../../../..

DIR = Pegasus/Provider/CMPI

include $(ROOT)/mak/config.mak

ifdef PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER
LOCAL_DEFINES = -DPEGASUS_CMPI_PROVIDER_INTERNAL -DPEGASUS_INTERNALONLY
endif

LIBRARY = cmpiCppImpl

SOURCES = \
    CmpiImpl.cpp

include $(ROOT)/mak/library.mak
