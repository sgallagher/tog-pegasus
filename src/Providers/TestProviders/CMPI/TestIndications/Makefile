ROOT = ../../../../..

DIR = src/Providers/TestProviders/CMPI/TestIndications

LOCAL_DEFINES = -DPEGASUS_USE_EXPERIMENTAL

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    CXX = cc
    FLAGS += -W"c,SSCOMM"
endif

VMS_HAS_CC = yes

LIBRARY = cmpiTestIndicationProvider

SOURCES = \
	cmpiTestIndicationProvider.c

include $(ROOT)/mak/library.mak

tests:
