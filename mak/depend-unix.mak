
DEPEND_MAK = $(OBJ_DIR)/depend.mak

ifeq ($(OS_TYPE),unix)
DEPEND_DEFINES = -DPEGASUS_OS_TYPE_UNIX
endif

ifeq ($(NOT_DEFINED),"1")
depend: $(OBJ_DIR)/target $(ERROR)
	@ touch $(DEPEND_MAK)
	makedepend $(LOCAL_DEFINES) $(DEFINES) $(DEPEND_DEFINES) $(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) $(INCLUDES) $(SOURCES) -f $(DEPEND_MAK) -p$(OBJ_DIR)/
else
depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)
endif

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
	$(TOUCH) $(OBJ_DIR)/depend.mak
