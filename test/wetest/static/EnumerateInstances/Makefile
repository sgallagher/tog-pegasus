
ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = EnumerateInstances51003

ifeq ($(OS),HPUX)
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
