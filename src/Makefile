ROOT = ..

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
DIRS =  slp Pegasus Clients Providers Server
else
DIRS =  Pegasus slp Clients Providers Server
endif

include $(ROOT)/mak/recurse.mak

rebuild: clean depend all

world: depend all
	$(MAKE) -s tests
