
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = DeleteInstance55000 DeleteInstance55001 DeleteInstance55003

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              DeleteInstance55002
endif

include $(ROOT)/test/configend.mak
