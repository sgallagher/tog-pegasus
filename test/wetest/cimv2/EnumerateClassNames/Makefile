# Wbemexec test associated w/EnumerateClassNames
#

ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateClassNames20001 EnumerateClassNames20003 BadAttributeName20004 BadBooleanValue20005 BadCIMEndTag20006 BadReqEndTag20007 BadClassNameStartTag20008 BadIParamvalueEndTag20009 BadIParamvalueStartTag20010 BadMessageEndTag20011 BadMethodCallEndTag20012 BadMethodName20013 

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClassNames20002
endif

include $(ROOT)/test/configend.mak
