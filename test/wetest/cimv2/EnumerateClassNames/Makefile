# Wbemexec test associated w/EnumerateClassNames
#

ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateClassNames20001 EnumerateClassNames20003 EnumerateClassNames20004 EnumerateClassNames20005 EnumerateClassNames20006 EnumerateClassNames20007 EnumerateClassNames20008

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClassNames20002
endif

include $(ROOT)/test/configend.mak
