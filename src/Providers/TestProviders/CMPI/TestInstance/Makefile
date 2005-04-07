ROOT = ../../../../..

DIR = src/Providers/TestProviders/CMPI/TestInstance

LOCAL_DEFINES = -DPEGASUS_USE_EXPERIMENTAL

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    CXX = cc
    FLAGS += -W"c,SSCOMM"
endif

VMS_HAS_CC = yes

LIBRARY = cmpiTestInstanceProvider

SOURCES = \
	cmpiTestInstanceProvider.c

include $(ROOT)/mak/library.mak

tests:
