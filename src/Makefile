ROOT = ..

include $(ROOT)/mak/config.mak

DIRS = slp Pegasus Clients Providers Server

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all

world: depend all
	$(MAKE) -s tests
