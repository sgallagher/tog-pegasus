
ROOT = ../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS =

BROKENXMLSCRIPTS = EnumerateQualifiers31000

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
          EnumerateQualifiers31001
endif

include $(ROOT)/test/configend.mak
