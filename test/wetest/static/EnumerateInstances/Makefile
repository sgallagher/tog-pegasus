
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateInstances51001 EnumerateInstances51002 EnumerateInstances51003

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
XMLSCRIPTS += EnumerateInstances51000
endif

include $(ROOT)/test/configend.mak
