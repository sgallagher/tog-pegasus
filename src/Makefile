ROOT = ..

include $(ROOT)/mak/config.mak

ifdef ($PEGASUS_SLP_CLASSES)
DIRS = utils slp Pegasus Clients Providers Server
else
DIRS = utils Pegasus Clients Providers Server
endif


include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository config

world: depend all repository config
	$(MAKE) -s tests

repository:
	@ $(MAKE) -SC Pegasus/Compiler/load repository

config:
	@ $(MAKE) -SC Providers/generic/ConfigSettingProvider/load config 
