
build: $(OBJ_DIR)/target $(BIN_DIR)/target $(LIB_DIR)/target $(ERROR)

define BUILD_NL


endef

$(OBJ_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(OBJ_DIR)
	$(foreach i, $(OBJECT_DIRS), $(MKDIRHIER) $(OBJ_DIR)/$(i) $(BUILD_NL) )
	@ $(TOUCH) $(OBJ_DIR)/target

$(BIN_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(BIN_DIR)
	@ $(TOUCH) $(BIN_DIR)/target

$(LIB_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(LIB_DIR)
	@ $(TOUCH) $(LIB_DIR)/target
