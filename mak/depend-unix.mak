
DEPEND_MAK = $(OBJ_DIR)/depend.mak

depend: $(OBJ_DIR)/target $(ERROR)
	@ touch $(DEPEND_MAK)
	makedepend $(LOCAL_DEFINES) $(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) $(INCLUDES) $(SOURCES) -f $(DEPEND_MAK) -p$(OBJ_DIR)/

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
	$(TOUCH) $(OBJ_DIR)/depend.mak
