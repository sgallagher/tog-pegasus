LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportclient$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX)

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider2.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportclient.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportserver.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient.x 
 
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository \
    -lpegprotocol \
    -lpegserver \
    -lpegprovider\
    -lpegprovider2 \
    -lpegexportclient \
    -lpegexportserver \
    -lpegclient
 
endif
