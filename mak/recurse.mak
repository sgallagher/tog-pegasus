
CLEAN_DIRS = $(DIRS) $(RECURSE_EXTRA_CLEAN_DIRS)

define NEWLINE


endef

all: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) $(NEWLINE) )

depend: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) depend $(NEWLINE) )

tests: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(DIRS), $(MAKESH) $(MAKE) -SC $(i) tests $(NEWLINE) )

clean: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) clean $(NEWLINE))

misc: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i,$(CLEAN_DIRS),$(MAKESH) $(MAKE) -SC $(i) misc $(NEWLINE))
