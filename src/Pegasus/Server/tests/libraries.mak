LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportclient$(LIB_SUFFIX) \

DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegrepository \
    -lpegprotocol \
    -lpegserver \
    -lpegprovider \
    -lpegexportclient \
