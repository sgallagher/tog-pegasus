
#include $(ROOT)/mak/common.mak

################################################################################
##
## Build list of bundle names.
##
################################################################################

# If we are using ICU resource bundles, then the compiled bundles are .res files
ifdef ICU_ROOT
	TMP_MSG_BUNDLES = $(foreach i,$(MSG_SOURCES),$(MSG_DIR)/$i)
	MSG_BUNDLES = $(TMP_MSG_BUNDLES:.txt=.res)
endif

################################################################################
##
## Message compile rule:
##
################################################################################

# Rule for ICU resource bundles
ifdef ICU_ROOT
$(MSG_DIR)/%.res: %.txt $(ERROR)
	$(MKDIRHIER) $(MSG_DIR)
	$(MSG_COMPILE) -d $(MSG_DIR) $(MSG_FLAGS) -p $(MSG_PACKAGE) $*.txt
	@ $(ECHO)
endif

################################################################################
##
## Clean rules:
##
################################################################################

FILES_TO_CLEAN = $(MSG_BUNDLES)

include $(ROOT)/mak/clean.mak

################################################################################
##
## Build list of object names:
##
################################################################################

include $(ROOT)/mak/objects.mak

include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/docxx.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak


tests: $(ERROR)

poststarttests: $(ERROR)
