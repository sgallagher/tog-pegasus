
LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX)

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
LIBRARIES += $(LIB_DIR)/$(LIB_PREFIX)slp$(LIB_SUFFIX)	
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegconfig \
    -lpegrepository \
    -lpegclient \
    -lslp
endif
