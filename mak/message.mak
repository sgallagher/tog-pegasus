#include $(ROOT)/mak/common.mak

################################################################################
##
## Build list of bundle names.
##
###############################################################################

# If we are using ICU resource bundles, then the compiled bundles are .res files
# since the individual makefiles do not specify a root package file, one is appended to the target list
ifdef ICU_ROOT
	TMP_MSG_BUNDLES = $(foreach i,$(MSG_SOURCES),$(MSG_DIR)/$i)
	MSG_BUNDLES = $(TMP_MSG_BUNDLES:.txt=.res)
	MSG_BUNDLES += $(MSG_DIR)/$(MSG_PACKAGE)_root.res
endif

################################################################################
##
## Message compile rules: the normal rule is from package_lang.txt -> package_lang.res
##                  					   the exception to the rule: since package_root.txt does not exist,
##									   one has to be generated from one of the existing package_lang.txt files.
##									   this defaults to package_en.txt unless the appropriate vars are set.
##									   the selected/or default package file is processed and converted into
##									   an intermediate package_root.rb file, this file is then compiled into its corresponding
##									   package_root.res file
##
################################################################################

# Rule for ICU resource bundles
ifdef ICU_ROOT

$(MSG_DIR)/$(MSG_PACKAGE)_root.res: $(MSG_PACKAGE)_root.rb $(ERROR)
	$(MKDIRHIER) $(MSG_DIR)
	$(MSG_COMPILE) -d $(MSG_DIR) $(MSG_FLAGS) $(MSG_PACKAGE)_root.rb
	@ $(ECHO)

$(MSG_PACKAGE)_root.rb: $(MSG_PACKAGE)$(MSG_ROOT_SOURCE).txt $(ERROR)
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/utils/cnv2rootbundle -f Makefile 
	$(CNV_ROOT_CMD) $(CNV_ROOT_FLAGS) $(MSG_PACKAGE)$(MSG_ROOT_SOURCE).txt

$(MSG_DIR)/%.res: %.txt $(ERROR)
	$(MKDIRHIER) $(MSG_DIR)
	$(MSG_COMPILE) -d $(MSG_DIR) $(MSG_FLAGS) $*.txt
	@ $(ECHO)

endif

################################################################################
##
## Clean rules:
##
################################################################################

# these files are cleaned when a user runs make clean, the intermediate package_root.rb file is removed 
# from the source directory
FILES_TO_CLEAN = $(MSG_BUNDLES)
FILES_TO_CLEAN += $(ROOT)/src/$(DIR)/*.rb

ifdef ICU_ROOT
include $(ROOT)/mak/clean.mak
else
clean:
endif

################################################################################
##
## Build list of object names:
##
################################################################################

include $(ROOT)/mak/objects.mak

depend: 

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/docxx.mak

include $(ROOT)/mak/sub.mak

include $(ROOT)/mak/misc.mak

tests: $(ERROR)

poststarttests: $(ERROR)

#include $(ROOT)/mak/common.mak

################################################################################
##
## Build list of bundle names.
##
###############################################################################

# If we are using ICU resource bundles, then the compiled bundles are .res files
# since the individual makefiles do not specify a root package file, one is appended to the target list
ifdef ICU_ROOT
	TMP_MSG_BUNDLES = $(foreach i,$(MSG_SOURCES),$(MSG_DIR)/$i)
	MSG_BUNDLES = $(TMP_MSG_BUNDLES:.txt=.res)
	MSG_BUNDLES += $(MSG_DIR)/$(MSG_PACKAGE)_root.res
endif

################################################################################
##
## Message compile rules: the normal rule is from package_lang.txt -> package_lang.res
## the exception to the rule: since package_root.txt does not exist,
## one has to be generated from one of the existing package_lang.txt files.
## this defaults to package_en.txt unless the appropriate vars are set.
## the selected/or default package file is processed and converted into
## an intermediate package_root.rb file, this file is then compiled into its corresponding
## package_root.res file
##
################################################################################

# Rule for ICU resource bundles
ifdef ICU_ROOT

$(MSG_DIR)/$(MSG_PACKAGE)_root.res: $(MSG_PACKAGE)_root.rb $(ERROR)
	$(MKDIRHIER) $(MSG_DIR)
	$(MSG_COMPILE) -d $(MSG_DIR) $(MSG_FLAGS) $(MSG_PACKAGE)_root.rb
	@ $(ECHO)

$(MSG_PACKAGE)_root.rb: $(MSG_PACKAGE)$(MSG_ROOT_SOURCE).txt $(ERROR)
	$(MAKE) --directory=$(PEGASUS_ROOT)/src/utils/cnv2rootbundle -f Makefile
	$(CNV_ROOT_CMD) $(CNV_ROOT_FLAGS) $(MSG_PACKAGE)$(MSG_ROOT_SOURCE).txt

$(MSG_DIR)/%.res: %.txt $(ERROR)
	$(MKDIRHIER) $(MSG_DIR)
	$(MSG_COMPILE) -d $(MSG_DIR) $(MSG_FLAGS) $*.txt
	@ $(ECHO)

endif

################################################################################
##
## Clean rules:
##
################################################################################

# these files are cleaned when a user runs make clean, the intermediate package_root.rb file is removed
# from the source directory
FILES_TO_CLEAN = $(MSG_BUNDLES)
FILES_TO_CLEAN += $(ROOT)/src/$(DIR)/*.rb

ifdef ICU_ROOT
include $(ROOT)/mak/clean.mak
else
clean:
endif

################################################################################
##
## Build list of object names:
##
################################################################################

include $(ROOT)/mak/objects.mak

depend: 

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/docxx.mak

include $(ROOT)/mak/sub.mak

include $(ROOT)/mak/misc.mak

tests: $(ERROR)

poststarttests: $(ERROR)
