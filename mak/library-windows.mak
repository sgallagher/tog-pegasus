FULL_LIB = $(LIB_DIR)/$(LIBRARY).lib
FULL_DLL = $(BIN_DIR)/$(LIBRARY).dll
FULL_EXP = $(BIN_DIR)/$(LIBRARY).exp

$(FULL_LIB): $(BIN_DIR)/target $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	link -nologo -dll -out:$(FULL_DLL) $(OBJECTS) $(LIBRARIES)
	mu copy $(BIN_DIR)/$(LIBRARY).lib $(FULL_LIB)"
	mu rm $(BIN_DIR)/$(LIBRARY).lib"

FILES_TO_CLEAN = \
    $(OBJECTS) $(FULL_LIB) $(FULL_DLL) $(FULL_EXP) depend.mak depend.mak.bak

clean-lib: $(ERROR)
	mu rm $(FULL_LIB) $(FULL_DLL) $(FULL_EXP)
