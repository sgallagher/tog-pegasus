
build: $(OBJ_DIR)/target $(BIN_DIR)/target $(LIB_DIR)/target $(ERROR)

define BUILD_NL


endef

ifeq ($(PEGASUS_OS_TYPE),VMS)

$(OBJ_DIR)/target: $(ERROR)
ifndef VMSPROGRAM
	library/create/object $(FULL_VMSLIB)
endif
	@ $(MKDIRHIER) $(REP_DIR)
	@ $(MKDIRHIER) $(OBJ_DIR)
	@ $(TOUCH) $(OBJ_VMSDIR)]target

$(BIN_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(BIN_DIR)
	@ $(TOUCH) $(BIN_VMSDIR)]target

$(LIB_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(LIB_DIR)
	@ $(TOUCH) $(LIB_VMSDIR)]target

$(OPT_DIR)/target: $(ERROR)
	@ $(MKDIRHIER) $(OPT_DIR)
	@ $(TOUCH) $(OPT_VMSDIR)]target
else
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
endif
