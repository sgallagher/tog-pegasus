ROOT=../../../../..
LIBRARIES= \
    $(LIB_DIR)/$(LIB_PREFIX)pegcompiler$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES= \
    $(LIB_DIR)/$(LIB_PREFIX)pegcompiler.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x
else
DYNAMIC_LIBRARIES= \
    -lpegcompiler \
    -lpegrepository \
    -lpegcommon
endif
