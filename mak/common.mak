################################################################################
##
## Common definitions used by both program.mak and libraries.mak
##
################################################################################


FULL_LIBRARIES = $(addprefix $(LIB_DIR)/$(LIB_PREFIX), \
    $(addsuffix $(LIB_SUFFIX), $(LIBRARIES)))

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)

    DYNAMIC_LIBRARIES = $(addprefix $(LIB_DIR)/$(LIB_PREFIX), \
	$(addsuffix .x, $(LIBRARIES)))

else

    DYNAMIC_LIBRARIES = $(addprefix -l, $(LIBRARIES))

endif
