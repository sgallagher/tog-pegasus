FULL_LIB = $(LIB_DIR)/$(LIBRARY)$(LIB_SUFFIX)
FULL_DLL = $(BIN_DIR)/$(LIBRARY)$(DLL)
FULL_EXP = $(BIN_DIR)/$(LIBRARY)$(EXP)
FULL_ILK = $(BIN_DIR)/$(LIBRARY)$(ILK)
FULL_PDB = $(BIN_DIR)/$(LIBRARY)$(PDB)

##
## ws2_32.lib is needed to get the WINSOCK routines!
##

$(FULL_LIB): $(BIN_DIR)/target $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
	link -nologo -dll $(LINK_FLAGS) -out:$(FULL_DLL) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
	mu copy $(BIN_DIR)/$(LIBRARY).lib $(FULL_LIB)
	mu rm $(BIN_DIR)/$(LIBRARY).lib
	@ $(ECHO)

FILES_TO_CLEAN = \
    $(OBJECTS) $(FULL_LIB) $(FULL_DLL) $(FULL_EXP) $(FULL_ILK) $(FULL_PDB) $(OBJ_DIR)/vc60$(PDB) $(OBJ_DIR)/vc70$(PDB) depend.mak depend.mak.bak

clean-lib: $(ERROR)
	mu rm $(FULL_LIB) $(FULL_DLL) $(FULL_EXP)
