
ifeq ($(COMPILER),xlc)
  LINK_COMMAND = makeC++SharedLib
  LINK_ARGUMENTS = -p 0
  LINK_OUT = -o
endif

ifeq ($(COMPILER),acc)
  LINK_COMMAND = aCC -b
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

ifeq ($(COMPILER),gnu)
  LINK_COMMAND = g++
  LINK_ARGUMENTS = -shared
  LINK_OUT = -o
endif

FULL_LIB=$(LIB_DIR)/lib$(LIBRARY)$(LIB_SUFFIX)

ifneq ($(COMPILER),xlc)
$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT)$(FULL_LIB) $(OBJECTS) $(LIBRARIES)
	@ $(ECHO)
else
$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	ar crv $(OS_TYPE).lib $(OBJECTS) $(LIBRARIES)
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT)$(FULL_LIB) $(PEGASUS_PLATFORM).lib
	rm -f $(PEGASUS_PLATFORM).lib
	@ $(ECHO)
endif

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
