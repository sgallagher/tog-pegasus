ROOT = ../../../../../

DIR = RepositoryUpgrade/SSPModule/SampleSSPModule

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_CONFIG_INTERNAL -DPEGASUS_INTERNALONLY

ifeq ($(OS_TYPE),vms)
 VMS_VECTOR = PegasusCreateSSPModule
endif

LIBRARY = pegsamplesspmodule

LIBRARIES = \
	pegcommon

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
EXTRA_LIBRARIES += -lCstd
endif

SOURCES = \
	SampleSSPModule.cpp 

include $(ROOT)/mak/dynamic-library.mak
