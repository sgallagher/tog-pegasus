ROOT = ../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

#Defines subdirectories to go to recursively

DIRS = cimv2 \
       static

LOADDIRS = \
   cimv2/Load \
   static/Load 

# This is a recurse make file

include $(ROOT)/mak/recurse.mak

repository: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(LOADDIRS), $(MAKESH) $(MAKE) -SC $(i) repository $(NL) )

repositoryServer: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(LOADDIRS), $(MAKESH) $(MAKE) -SC $(i) repositoryServer $(NL) )

removerepository: $(RECURSE_DEPENDS) $(ERROR)
	@ $(foreach i, $(LOADDIRS), $(MAKESH) $(MAKE) -SC $(i) removerepository $(NL) )

#DO NOT DELETE THIS LINE
