
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = ModifyInstance54000 ModifyInstance54003 ModifyInstance54004 ModifyInstance54005 ModifyInstance54006 

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              ModifyInstance54001 \
              ModifyInstance54002
endif

include $(ROOT)/test/configend.mak
