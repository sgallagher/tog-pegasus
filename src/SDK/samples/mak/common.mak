ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    DYNAMIC_LIBRARIES = $(addprefix $(PEGASUS_LIB_DIR)/$(LIB_PREFIX), \
	$(addsuffix .x, $(LIBRARIES)))
else
    DYNAMIC_LIBRARIES = $(addprefix -L$(PEGASUS_LIB_DIR) -l, $(LIBRARIES))
endif
