ROOT=../../../../..

LIBRARIES= \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegexportclient$(LIB_SUFFIX)

DYNAMIC_LIBRARIES=-lpegcommon -lpegexportclient
