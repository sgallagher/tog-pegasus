
ifeq ($(OS_TYPE),windows)
$(OBJ_DIR)/%.obj: %.cpp $(ERROR)
else
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
endif
	$(CXX) -c $(OBJ_OUT)$@ $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(ECHO)
