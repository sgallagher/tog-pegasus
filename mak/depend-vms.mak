DEPEND_MAK = $(OBJ_VMSDIRA)]depend.mak

#depend: $(OBJ_DIR)/target $(ERROR)
#	$(MU) depend "-O$(OBJ_VMSDIRA)]" $(SOURCES) > $(DEPEND_MAK)

depend: $(OBJ_DIR)/target $(ERROR)
#	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)
	$(MUDEPEND) "-O$(OBJ_DIR)" *.cpp > $(DEPEND_MAK)

clean-depend:
	$(RM) $(OBJ_VMSDIRA)]depend.mak;*
