LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegconfig \
    -lpegrepository \
    -lpegserver \
    -lpegprovider \
    -lpegprovider2
endif
