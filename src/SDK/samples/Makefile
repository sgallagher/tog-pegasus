ROOT = .

include $(ROOT)/mak/config.mak

DIRS = \
    Clients \
    Providers

include $(ROOT)/mak/recurse.mak

setupSDK:
	@$(MAKE) --directory $(ROOT)/Providers/Load createrepository
	@$(MAKE) --directory $(ROOT)/Providers/Load registerproviders
