ROOT = ..

include $(ROOT)/mak/config.mak

DIRS =  Pegasus slp Clients Providers Server

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository config

world: depend all repository config
	$(MAKE) -s tests

repository:
	@ $(MAKE) -SC Pegasus/Compiler/load repository

config:
	@ $(MAKE) -SC Providers/generic/ConfigSettingProvider/load config 
