# Wbemexec test associated w/EnumerateClassNames
#

ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak


##
## If ordering defects, use DIFFSORT rules
##
ifdef PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED
XMLSCRIPTS_DS = EnumerateClassNames20007

XMLSCRIPTS = EnumerateClassNames20001 EnumerateClassNames20003 EnumerateClassNames20004 EnumerateClassNames20005 EnumerateClassNames20006 EnumerateClassNames20008

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClassNames20002
endif


else
##
## no ordering defects so use normal DIFF rules
##

XMLSCRIPTS = EnumerateClassNames20001 EnumerateClassNames20003 EnumerateClassNames20004 EnumerateClassNames20005 EnumerateClassNames20006 EnumerateClassNames20007 EnumerateClassNames20008

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClassNames20002
endif

endif
include $(ROOT)/test/configend.mak
