FULL_LIB=$(LIB_DIR)/lib$(LIBRARY).so

ifeq ($(COMPILER),xlc)
  LINK_COMMAND = /usr/ibmcxx/bin/makeC++SharedLib
  LINK_ARGUMENTS = -p 0
  LINK_OUT = -o
endif

ifeq ($(COMPILER),gnu)
  LINK_COMMAND = g++
  LINK_ARGUMENTS = -shared
  LINK_OUT = -o
endif

$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT)$(FULL_LIB) $(OBJECTS) $(LIBRARIES)
	@ $(ECHO)

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
