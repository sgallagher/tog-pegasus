INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

ifeq ($(OS_TYPE),windows)
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
OBJECTS = $(CPP_OBJECTS:.c=.obj)
else
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.o)
OBJECTS = $(CPP_OBJECTS:.c=.o)
endif

FULL_PROGRAM=$(BIN_DIR)/$(PROGRAM)$(EXE)

EXE_OUTPUT = $(EXE_OUT) $(FULL_PROGRAM)

ifdef PEGASUS_HAS_MESSAGES
    ifdef ICU_ROOT
        ifdef ICU_INSTALL
          SYS_LIBS += -L${ICU_INSTALL}/lib -licui18n -licuuc
        endif
    endif
endif

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
## ICU_INSTALL - Specifies the directory path where the ICU lib directory is located.
##               This will set runtime library search path for ICU libraries to ${ICU_INSTALL}/lib
##               
     ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
	$(LINK_WRAPPER) $(CXX) $(PR_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
     else
      ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
        ifdef PEGASUS_HAS_MESSAGES  
          ifdef ICU_ROOT
            ifdef ICU_INSTALL
              ifdef  PEGASUS_USE_RELEASE_DIRS
	        $(LINK_WRAPPER) $(CXX) $(LDFLAGS) -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR) -Xlinker -rpath-link -Xlinker $(LIB_DIR) -Xlinker -rpath -Xlinker ${ICU_INSTALL}/lib -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
              else
	        $(LINK_WRAPPER) $(CXX) $(LDFLAGS) -Xlinker -rpath -Xlinker $(LIB_DIR) -Xlinker -rpath -Xlinker ${ICU_INSTALL}/lib -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
              endif
            endif
          endif
        else
          ifdef  PEGASUS_USE_RELEASE_DIRS
	    $(LINK_WRAPPER) $(CXX) $(LDFLAGS) -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR) -Xlinker -rpath-link -Xlinker $(LIB_DIR) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
          else
	    $(LINK_WRAPPER) $(CXX) $(LDFLAGS) -Xlinker -rpath -Xlinker $(LIB_DIR) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
          endif
        endif      
      else
       ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
         ifdef  PEGASUS_USE_RELEASE_DIRS
	   $(LINK_WRAPPER) $(CXX) -Wl,-brtl -blibpath:/usr/linux/lib:/usr/lib:/lib -Wl,-bhalt:$(AIX_LD_HALT) $(LDFLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
         else
	   $(LINK_WRAPPER) $(CXX) -Wl,-brtl -Wl,-bhalt:$(AIX_LD_HALT) $(LDFLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
         endif
       else
	$(LINK_WRAPPER) $(CXX) $(LDFLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
       endif
      endif
     endif

	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)
else
	$(LINK_WRAPPER) $(CXX) $(LDFLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
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
