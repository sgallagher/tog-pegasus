DEPEND_MAK = $(OBJ_DIR)/depend.mak

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
depend: $(OBJ_DIR)/target $(ERROR)
	$(CXX) -xM1 $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $(SOURCES) | sed -e 's=^\(.*:\)='$(OBJ_DIR)'/\1=' | grep -v $(SOLARIS_COMPILER_PATH) > $(DEPEND_MAK)
	
else
ifdef PEGASUS_HAS_MAKEDEPEND
DEPEND_INCLUDES = -DPEGASUS_OS_TYPE_UNIX -I/usr/include $(SYS_INCLUDES)

depend: $(OBJ_DIR)/target $(ERROR)
	touch $(DEPEND_MAK)
	makedepend -v $(LOCAL_DEFINES) $(DEFINES) $(DEPEND_DEFINES) $(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) $(INCLUDES) $(SOURCES) -f $(DEPEND_MAK) -p $(OBJ_DIR)/
else
depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)
endif
endif

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
