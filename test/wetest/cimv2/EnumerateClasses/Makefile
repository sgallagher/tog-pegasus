
ROOT = ../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak


##
## If ordering defects, use DIFFSORT rules
##
ifdef PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED
XMLSCRIPTS_DS = EnumerateClasses42003 EnumerateClasses42005 

XMLSCRIPTS = EnumerateClasses42000 EnumerateClasses42002  EnumerateClasses42004

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClasses42001
endif

else
##
## no ordering defects so use normal DIFF rules
##
XMLSCRIPTS = EnumerateClasses42000 EnumerateClasses42002 EnumerateClasses42003 EnumerateClasses42004 EnumerateClasses42005



ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateClasses42001
endif 


endif

include $(ROOT)/test/configend.mak
