$(OBJ_DIR)/%.obj: %.cpp $(ERROR)
	$(CXX) -c $(OBJ_OUT)$@ $(FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(ECHO)
