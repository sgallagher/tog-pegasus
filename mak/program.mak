INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)

ifeq ($(OS_TYPE),windows)
OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
EXE_OUTPUT = $(EXE_OUT)$(FULL_PROGRAM)
else
OBJECTS = $(TMP_OBJECTS:.cpp=.o)
EXE_OUTPUT = $(EXE_OUT) $(FULL_PROGRAM)
endif

$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(LIBRARIES) $(ERROR)
ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)

##
## build images with -l<name> syntax for needed shared libraries
## DYNAMIC_LIBRARIES is defined appropriately in libraries.mak and Makefile 
## files
##

     ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
	$(LINK_WRAPPER) $(CXX) $(PR_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
     else
         ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
	$(LINK_WRAPPER) $(CXX) $(FLAGS) -Xlinker -rpath -Xlinker $(LIB_DIR) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
         else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
         endif
     endif

	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)
else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(LIBRARIES) $(SYS_LIBS)
endif
	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)

include $(ROOT)/mak/objects.mak

FILES_TO_CLEAN = $(OBJECTS) $(FULL_PROGRAM)

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
