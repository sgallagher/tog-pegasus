ROOT = ..

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DIRS =  Pegasus Clients Providers Server
else
ifeq ($(OS),HPUX)
DIRS =  Pegasus Clients Providers Server
else
DIRS =  Pegasus slp Clients Providers Server utils
endif
endif

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository config

world: depend all repository config
	$(MAKE) -s tests

repository:
	@ $(MAKE) -SC Pegasus/Compiler/load repository

config:
	@ $(MAKE) -SC Providers/generic/ConfigSettingProvider/load config 
