FULL_LIB=$(LIB_DIR)/lib$(LIBRARY).a 
              
$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	nld -r -o $(FULL_LIB) $(OBJECTS)
	@ $(ECHO)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
