ROOT = ..

include $(ROOT)/mak/config.mak

DIRS = Pegasus

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all

world: depend all
	$(MAKE) -s tests
