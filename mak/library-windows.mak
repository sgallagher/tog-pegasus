FULL_LIB = $(LIB_DIR)/$(LIBRARY).lib
FULL_DLL = $(BIN_DIR)/$(LIBRARY).dll
FULL_EXP = $(BIN_DIR)/$(LIBRARY).exp

##
## ws2_32.lib is needed to get the WINSOCK routines!
##

$(FULL_LIB): $(BIN_DIR)/target $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
	link -nologo -dll $(LINK_FLAGS) -out:$(FULL_DLL) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
	mu copy $(BIN_DIR)/$(LIBRARY).lib $(FULL_LIB)
	mu rm $(BIN_DIR)/$(LIBRARY).lib
	@ $(ECHO)

FILES_TO_CLEAN = \
    $(OBJECTS) $(FULL_LIB) $(FULL_DLL) $(FULL_EXP) depend.mak depend.mak.bak

clean-lib: $(ERROR)
	mu rm $(FULL_LIB) $(FULL_DLL) $(FULL_EXP)
