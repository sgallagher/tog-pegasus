ROOT = .

include $(ROOT)/mak/config.mak

DIRS = src cgi

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all
	$(MAKE) -s tests

world: depend all
	$(MAKE) -s tests
