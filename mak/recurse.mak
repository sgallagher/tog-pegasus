
CLEAN_DIRS = $(DIRS) $(RECURSE_EXTRA_CLEAN_DIRS)

define NL


endef

all: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) $(NL) )

depend: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) depend $(NL) )

tests: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) tests $(NL) )

poststarttests: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) poststarttests $(NL) )

clean: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) clean $(NL))

misc: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) misc $(NL))

docxx: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) docxx $(NL))

sub: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) sub $(NL))

general: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(DIRS),$(MAKESH) $(MAKE) -SC $(i) general OPTION=$(OPTION) $(NL))

strip-license: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) strip-license $(NL) )

prepend-license: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) prepend-license $(NL) )

#l10n
messages: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) messages $(NL) )
