
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = ModifyClass43000 ModifyClass43001 ModifyClass43003 ModifyClass43004 ModifyClass43005

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              ModifyClass43002
endif

include $(ROOT)/test/configend.mak

