# Recurse the directories defined in DIRS

ROOT = ../..

include $(ROOT)/mak/config.mak

DIRS = \
	WMIProvider \
	PegServer \
	WMIServer

include $(ROOT)/mak/recurse.mak

mapper:
                $(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/PegServer -f Makefile copy2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper -f Makefile rebuild

rebuild:  clean depend all


