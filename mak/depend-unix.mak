DEPEND_MAK = $(OBJ_DIR)/depend.mak

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
DEPEND_INCLUDES = -DPEGASUS_OS_TYPE_UNIX -I/usr/include -I/usr/include/g++ 
depend: $(OBJ_DIR)/target $(ERROR)
	touch $(DEPEND_MAK)
	makedepend -v $(LOCAL_DEFINES) $(DEFINES) $(DEPEND_DEFINES) $(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) $(INCLUDES) $(SOURCES) -f $(DEPEND_MAK) -p $(OBJ_DIR)
else
depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)
endif

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
