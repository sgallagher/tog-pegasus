FULL_LIB=$(LIB_DIR)/lib$(LIBRARY).so

$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	g++ -shared -o$(FULL_LIB) $(OBJECTS)
	@ $(ECHO)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
