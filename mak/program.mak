INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

ifeq ($(OS_TYPE),windows)
OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
else
OBJECTS = $(TMP_OBJECTS:.cpp=.o)
endif

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	$(LINK_WRAPPER) $(CXX) $(FLAGS) -L $(LIB_DIR) $(EXE_OUT)$(FULL_PROGRAM) $(OBJECTS) -lpegcommon -lpegrepository -lpegprotocol -lpegserver -lpegclient -lpegcompiler -lpegprovider -lpeggetoopt -llslp-perl-lib $(SYS_LIBS)
	@ $(ECHO)
else
$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUT)$(FULL_PROGRAM) $(OBJECTS) $(LIBRARIES) $(SYS_LIBS)
	@ $(ECHO)
endif

include $(ROOT)/mak/objects.mak

FILES_TO_CLEAN = $(OBJECTS) $(FULL_PROGRAM)

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
