ifdef VMS_HAS_CC
 CPPVMSOBJECTS = $(SOURCES:.cpp=.obj,)
 TMPVMSOBJECTS += $(CPPVMSOBJECTS:.c=.obj,)

 TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)
 CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
 OBJECTS = $(CPP_OBJECTS:.c=.obj)
else
 TMPVMSOBJECTS = $(SOURCES:.cpp=.obj,)

 TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)
 OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
endif

VMSOBJECTS = $(OBJ_VMSDIRA)]$(TMPVMSOBJECTS)

FULL_LIB=$(LIB_DIR)/lib$(LIBRARY)$(LIB_SUFFIX)
FULL_VMSLIB=$(LIB_VMSDIRA)]lib$(LIBRARY)$(LIB_SUFFIX)

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)
FULL_VMSPROGRAM=$(BIN_VMSDIRA)]$(PROGRAM)$(EXE)

EXE_OUTPUT =$(FULL_PROGRAM)
EXE_VMSOUTPUT =/exe=$(FULL_VMSPROGRAM)

#OPT = $(VMSROOT)[src$(VMSDIRA)]$(PROGRAM)/opt
OPT = $(OPT_VMSDIRA)]$(PROGRAM)/opt
VMSPROGRAM = YES


$(FULL_PROGRAM): $(OBJ_DIR)/target $(BIN_DIR)/target $(OPT_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)

ifdef OBJECTS_IN_OPTIONFILE
	@ take $(PLATFORM_VMSDIRA)]vms_create_optfile.com "$(OPT_VMSDIRA)]" "$(VMSROOT)[src$(VMSDIR)]" "$(PROGRAM)" "$(strip $(LIBRARIES))" "$(SHARE_COPY)" "$(VMS_VECTOR)" "$(SOURCES)" "$(OBJ_VMSDIRA)]" 
	cxxlink$(LFLAGS)$(VMSSHARE)$(EXE_VMSOUTPUT)/reposit=$(CXXREPOSITORY_VMSROOT) $(OPT)
else
	@ take $(PLATFORM_VMSDIRA)]vms_create_optfile.com "$(OPT_VMSDIRA)]" "$(VMSROOT)[src$(VMSDIR)]" "$(PROGRAM)" "$(strip $(LIBRARIES))" "$(SHARE_COPY)" "$(VMS_VECTOR)" "$(OBJ_VMSDIRA)]"
	cxxlink$(LFLAGS)$(VMSSHARE)$(EXE_VMSOUTPUT)/reposit=$(CXXREPOSITORY_VMSROOT) $(VMSOBJECTS)$(OPT)
endif
	@ $(TOUCH) $(FULL_VMSPROGRAM)
ifdef SHARE_COPY
#	$(COPY) $(FULL_VMSPROGRAM) sys$$share:$(PROGRAM)$(EXE)
	$(COPY) "$(FULL_VMSPROGRAM)" "$(PEGASUS_SYSSHARE)$(PROGRAM)$(EXE)"
endif
	@ $(ECHO)

include $(ROOT)/mak/objects.mak

FILES_TO_CLEAN = $(VMSOBJECTS) $(FULL_VMSPROGRAM);

include $(ROOT)/mak/clean.mak

-include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak
	
