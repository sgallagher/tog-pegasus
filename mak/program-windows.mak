INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

OBJECTS = $(TMP_OBJECTS:.cpp=.obj)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)
FULL_PROGPDB=$(BIN_DIR)/$(PROGRAM)$(PDB)
FULL_PROGILK=$(BIN_DIR)/$(PROGRAM)$(ILK)
FULL_PROGLIB=$(BIN_DIR)/$(PROGRAM)$(LIB_SUFFIX)
FULL_PROGEXP=$(BIN_DIR)/$(PROGRAM)$(EXP)

EXE_OUTPUT = $(EXE_OUT)$(FULL_PROGRAM)

$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)

##
## build images with -l<name> syntax for needed shared libraries
## DYNAMIC_LIBRARIES is defined appropriately in libraries.mak and Makefile 
## files
##

	$(LINK_WRAPPER) $(CXX) $(FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
endif
	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)

include $(ROOT)/mak/objects.mak

FILES_TO_CLEAN = $(OBJECTS) $(FULL_PROGRAM) $(FULL_PROGPDB) $(FULL_PROGILK) $(FULL_PROGLIB) $(FULL_PROGEXP) $(OBJ_DIR)/vc60$(PDB) $(OBJ_DIR)/vc70$(PDB)

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
