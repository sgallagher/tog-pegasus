ROOT = .

include $(ROOT)/mak/config.mak

# DIRS = src cgi
DIRS = src

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository
	$(MAKE) -s tests

world: depend all repository
	$(MAKE) -s tests

repository:
	$(MAKE) -SC src/Pegasus/Compiler/load repository
