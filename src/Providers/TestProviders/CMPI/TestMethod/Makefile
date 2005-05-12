ROOT = ../../../../..

DIR = Providers/TestProviders/CMPI/TestMethod

LOCAL_DEFINES = -DPEGASUS_USE_EXPERIMENTAL 

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    CXX = cc
    FLAGS += -W"c,SSCOMM"
endif

VMS_HAS_CC = yes

LIBRARY = cmpiTestMethodProvider

SOURCES = \
	cmpiTestMethodProvider.c

include $(ROOT)/mak/library.mak

tests:
