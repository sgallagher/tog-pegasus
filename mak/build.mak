
build: $(OBJ_DIR)/target $(BIN_DIR)/target $(LIB_DIR)/target $(ERROR)

$(OBJ_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(OBJ_DIR)
	@ touch $(OBJ_DIR)/target

$(BIN_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(BIN_DIR)
	@ touch $(BIN_DIR)/target

$(LIB_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(LIB_DIR)
	@ touch $(LIB_DIR)/target
