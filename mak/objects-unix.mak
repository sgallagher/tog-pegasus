################################################################################
##
## An ugly trick is used here to overcome a bug in g++ v2.9.5. G++ fails to
## cleanup object files that are placed in directories other than the
## current one (using the -o option). To overcome this bug, we use -o.tmp.o 
## and then move the file to the object directory.  Note, this is only done for
## v2.9.5 of g++.
##
################################################################################

ifeq ($(findstring _GNU, $(PEGASUS_PLATFORM)), _GNU)
GCC_VERSION=$(word 3, $(shell $(CXX) --version))
ifneq ($(GCC_VERSION), 2.9.5)
    _NO_TMP_O=yes
endif
else
    _NO_TMP_O=yes
endif

_TMP_O = $(PEGASUS_PLATFORM).o


ifeq ($(_NO_TMP_O), yes)
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
	$(CXX) -c -o $@ $(FLAGS) $(EXTRA_CXX_FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(TOUCH) $@
	@ $(ECHO)
else
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
	$(CXX) -c -o $(_TMP_O) $(FLAGS) $(EXTRA_CXX_FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	@ $(COPY) $(_TMP_O) $@
	@ $(RM) $(_TMP_O)
	@ $(TOUCH) $@
	@ $(ECHO)
endif

ifeq ($(_NO_TMP_O), yes)
$(OBJ_DIR)/%.o: %.c $(ERROR)
	$(CC) -c -o $@ $(FLAGS) $(EXTRA_C_FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.c
	@ $(TOUCH) $@
	@ $(ECHO)
else
$(OBJ_DIR)/%.o: %.c $(ERROR)
	$(CC) -c -o $(_TMP_O) $(FLAGS) $(EXTRA_C_FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.c
	@ $(COPY) $(_TMP_O) $@
	@ $(RM) $(_TMP_O)
	@ $(TOUCH) $@
	@ $(ECHO)
endif
