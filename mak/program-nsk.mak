INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

OBJECTS = $(TMP_OBJECTS:.cpp=.o)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)

$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
	$(LINK_WRAPPER) $(LXX) $(EXE_OUT) $(FULL_PROGRAM) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
	@ $(ECHO)

include $(ROOT)/mak/objects.mak

# include $(ROOT)/mak/headers.mak

FILES_TO_CLEAN = $(OBJECTS) $(FULL_PROGRAM)

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/headers.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
