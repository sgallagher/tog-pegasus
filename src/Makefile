ROOT = ..

include $(ROOT)/mak/config.mak

DIRS =  Pegasus slp Clients Providers Server

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository

world: depend all repository
	$(MAKE) -s tests

repository:
	@ $(MAKE) -SC Pegasus/Compiler/load repository
