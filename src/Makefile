ROOT = ..

include $(ROOT)/mak/config.mak

ifdef PEGASUS_ENABLE_SLP
DIRS = Platforms utils Pegasus slp Clients Providers Server
else
DIRS = Platforms utils Pegasus Clients Providers Server
endif


include $(ROOT)/mak/recurse.mak

rebuild: clean depend all repository config

world: depend all repository config
	$(MAKE) -s tests

# Note: Arguments must be quoted to preserve upper case characters in VMS.
repository:
	@ $(MAKE) "-SC" Pegasus/Compiler/load repository

config:
	@ $(MAKE) "-SC" Providers/generic/ConfigSettingProvider/load config 

vms:
	@ $(MAKE) "-SC" Pegasus/ProviderManager2/Default -f makefile_vms
