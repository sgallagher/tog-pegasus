
ROOT = ../../../../../../../../..
include $(ROOT)/mak/config.mak
#include $(ROOT)/test/config.mak
include ../config.mak

XMLSCRIPTS = GetInstance52000 GetInstance52001 GetInstance52002 GetInstance52003 GetInstance52005 GetInstance52006

# Note: this is done because the server on windows returns 3 digits
# for the exponent of a real32 or real64, while other platforms return 2.
ifeq ($(OS_TYPE),windows)
XMLSCRIPTS += GetInstanceAlt52004
else
XMLSCRIPTS += GetInstance52004
endif

include $(ROOT)/test/configend.mak
