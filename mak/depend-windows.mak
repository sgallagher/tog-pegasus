
#DEPEND_MAK_TMP = $(OBJ_DIR)/depend.mak
#DEPEND_MAK = $(subst /,\\,$(DEPEND_MAK_TMP))

DEPEND_MAK = $(OBJ_DIR)/depend.mak

##
## Notice that the system includes are excluded. That is because this
## depend utility doesn't require that all the includes be resolved (just
## the ones that matter (the ones the user put on the path).
##

depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
	$(TOUCH) $(OBJ_DIR)/depend.mak
