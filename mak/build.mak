
build: $(OBJ_DIR)/target $(BIN_DIR)/target $(LIB_DIR)/target $(ERROR)

$(OBJ_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(OBJ_DIR)
	@ $(TOUCH) $(OBJ_DIR)/target

$(BIN_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(BIN_DIR)
	@ $(TOUCH) $(BIN_DIR)/target

$(LIB_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(LIB_DIR)
	@ $(TOUCH) $(LIB_DIR)/target
