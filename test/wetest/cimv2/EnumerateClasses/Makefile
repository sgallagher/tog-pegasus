
ROOT = ../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateClasses42000 EnumerateClasses42002 EnumerateClasses42003 EnumerateClasses42004 EnumerateClasses42005

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClasses42001
endif 

include $(ROOT)/test/configend.mak
