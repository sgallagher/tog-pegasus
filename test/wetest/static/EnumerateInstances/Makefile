
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateInstances51003

# Note: this is done because the server on windows returns 3 digits
# for the exponent of a real32 or real64, while other platforms return 2.
ifeq ($(OS_TYPE),windows)
XMLSCRIPTS += \
              EnumerateInstancesAlt51000 \
	      DeepLocalTFAlt51005 \
	      DeepLocalFFAlt51007 \
              DeepLocalTFAlt51008
else
XMLSCRIPTS += \
              EnumerateInstances51000 \
	      DeepLocalTF51005 \
	      DeepLocalFF51007 \
              DeepLocalTF51008
endif

ifndef SECURITY_ENABLED
XMLSCRIPTS += \
              EnumerateInstances51001 \
              EnumerateInstances51002 
endif

ATTN-WO-P3-2002/05/01: 
	The DeepInheritance and LocalOnly flags have not been fully \
implemented. The following tests should be run when these are fully enabled.

XmlScriptDefects: DeepLocalFT51006 \
	          DeepLocalTT51004

include $(ROOT)/test/configend.mak
