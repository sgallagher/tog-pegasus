ifeq ($(COMPILER),deccxx)
  LINK_COMMAND = cxxlink
  LINK_ARGUMENTS =
  LINK_OUT = 
endif

LIBRARY_COMMAND = library/create/object

FULL_LIB=$(LIB_DIR)/lib$(LIBRARY)$(LIB_SUFFIX)
FULL_VMSLIB=$(LIB_VMSDIRA)]lib$(LIBRARY)$(LIB_SUFFIX)

CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
OBJECTS = $(CPP_OBJECTS:.c=.obj)

## Rule for all library builds

$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(ERROR) 
	@ $(TOUCH) $(FULL_VMSLIB)
	@ $(ECHO)

clean-lib: $(ERROR)
	delete $(FULL_VMSLIB);

FILES_TO_CLEAN = $(OBJECTS) $(FULL_VMSLIB);
