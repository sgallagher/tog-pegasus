
ROOT = ../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateInstanceNames25000 EnumerateInstanceNames25002

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
           EnumerateInstanceNames25001
endif

include $(ROOT)/test/configend.mak
