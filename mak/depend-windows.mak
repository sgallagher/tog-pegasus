
define NEWLINE


endef

DEPEND_MAK = $(OBJ_DIR)/depend.mak

depend: $(OBJ_DIR)/target $(ERROR)
	@ mu rm $(DEPEND_MAK)
	@ echo === depend:
	$(foreach i, $(SOURCES), \
	    @ $(CXX) -P $(FLAGS) $(LOCAL_DEFINES) $(DEFINES)  \
		$(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) \
		$(SYS_INCLUDES) $(INCLUDES) $(i) $(NEWLINE) \
	    @ mu depend \
		$(subst .cpp,.i,$(i)) $(DEPEND_MAK) $(OBJ_DIR)/ $(NEWLINE) \
	)

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
	$(TOUCH) $(OBJ_DIR)/depend.mak
