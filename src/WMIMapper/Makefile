# Recurse the directories defined in DIRS

ROOT = ../..

include $(ROOT)/mak/config.mak

DIRS = \
	WMIProvider \
	Client \
	PegServer \
	WMIServer \
	WMIOP
	

include $(ROOT)/mak/recurse.mak

mapper:
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/Client -f Makefile copy2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/PegServer -f Makefile copy2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/WMIServer -f Makefile copy2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper -f Makefile rebuild
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/PegServer -f Makefile clean2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/Client -f Makefile clean2
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/WMIMapper/WMIServer -f Makefile clean2

rebuild:  clean depend all


