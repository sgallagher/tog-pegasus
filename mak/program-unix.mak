INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

OBJECTS = $(TMP_OBJECTS:.cpp=.o)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)

EXE_OUTPUT = $(EXE_OUT) $(FULL_PROGRAM)

ifdef PEGASUS_PURIFY
    PUREOPTIONS = -follow-child-processes=yes -locking=no \
        -always-use-cache-dir -cache-dir=$(PURIFY_TMP)/cache \
        -view-file=$(PURIFY_TMP)/$(PROGRAM).pv \
        -log-file=$(PURIFY_TMP)/$(PROGRAM).log
    LINK_WRAPPER = $(PURIFY_HOME)/purify $(PUREOPTIONS)
    SYS_LIBS += $(PURIFY_HOME)/libpurify_stubs.a
endif

$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
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
             ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
	$(LINK_WRAPPER) $(CXX) -Wl,-brtl -Wl,-bhalt:$(AIX_LD_HALT) $(FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
             else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
             endif
         endif
     endif

	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)
else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
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
