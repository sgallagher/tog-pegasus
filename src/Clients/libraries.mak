
LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprotocol$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)slp$(LIB_SUFFIX)	

DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegconfig \
    -lpegrepository \
    -lpegprotocol \
    -lpegclient \
    -lslp
