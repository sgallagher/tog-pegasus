
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = ModifyInstance54000 ModifyInstance54003 ModifyInstance54004 ModifyInstance54005 ModifyInstance54006 

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              ModifyInstance54002
endif

ATTN-SF-P3-2002/03/20: 
	Does not seem to validate the class name. Seems to be returning a wrong response. ( Response: A general error occurred that is not covered by a more specific error code: &quot;rollback failed ).

XmlScriptDefects: ModifyInstance54001

include $(ROOT)/test/configend.mak
