DEPEND_MAK = $(OBJ_DIR)/depend.mak

depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
