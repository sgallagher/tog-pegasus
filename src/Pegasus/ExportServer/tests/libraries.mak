ROOT=../../../../..

LIBRARIES= \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES= \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver.x
else
 DYNAMIC_LIBRARIES=-lpegcommon -lpegexportserver -lpegprovider2 -lpegrepository
endif
