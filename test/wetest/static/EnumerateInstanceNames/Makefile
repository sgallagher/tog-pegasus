ROOT = ../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateInstanceNames50000 EnumerateInstanceNames50002

ifndef SECURITY_ENABLED
XMLSCRIPTS += EnumerateInstanceNames50001
endif

include $(ROOT)/test/configend.mak
