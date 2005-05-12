#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
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

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    ifdef PEGASUS_HAS_MESSAGES
        ifdef ICU_ROOT
            ifdef ICU_INSTALL
              FLAGS += -L${ICU_INSTALL}/lib
	      PRFLAGS += -L${ICU_INSTALL}/lib
	      SYS_LIBS += ${ICU_INSTALL}/lib/libicui18n.x ${ICU_INSTALL}/lib/libicuuc.x
            endif
        endif
    endif
else
ifdef PEGASUS_HAS_MESSAGES
    ifdef ICU_ROOT
        ifdef ICU_INSTALL
          SYS_LIBS += -L${ICU_INSTALL}/lib -licui18n -licuuc
        endif
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
	$(LINK_WRAPPER) $(CXX) $(PR_FLAGS) $(EXTRA_LINK_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
     else
      ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
        ifdef PEGASUS_HAS_MESSAGES  
          ifdef ICU_ROOT
            ifdef ICU_INSTALL
              ifdef  PEGASUS_USE_RELEASE_DIRS
	        $(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR) -Xlinker -rpath-link -Xlinker $(LIB_DIR) -Xlinker -rpath -Xlinker ${ICU_INSTALL}/lib -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
              else
	        $(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) -Xlinker -rpath -Xlinker $(LIB_DIR) -Xlinker -rpath -Xlinker ${ICU_INSTALL}/lib -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
              endif
            endif
          endif
        else
          ifdef  PEGASUS_USE_RELEASE_DIRS
	    $(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR) -Xlinker -rpath-link -Xlinker $(LIB_DIR) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
          else
	    $(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) -Xlinker -rpath -Xlinker $(LIB_DIR) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS) 
          endif
        endif      
      else
       ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
         ifdef  PEGASUS_USE_RELEASE_DIRS
           ifdef PEGASUS_HAS_MESSAGES
             ifdef ICU_ROOT
               ifdef ICU_INSTALL
	         $(LINK_WRAPPER) $(CXX) -Wl,-brtl -blibpath:/usr/linux/lib:/usr/lib:/lib:$(ICU_INSTALL)/lib -Wl,-bhalt:$(AIX_LD_HALT) $(FLAGS) $(EXTRA_LINK_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
               endif
             endif
           else
	     $(LINK_WRAPPER) $(CXX) -Wl,-brtl -blibpath:/usr/linux/lib:/usr/lib:/lib -Wl,-bhalt:$(AIX_LD_HALT) $(FLAGS) $(EXTRA_LINK_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
           endif
         else
	   $(LINK_WRAPPER) $(CXX) -Wl,-brtl -Wl,-bhalt:$(AIX_LD_HALT) $(FLAGS) $(EXTRA_LINK_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
         endif
       else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) -L$(LIB_DIR) $(EXE_OUTPUT) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)
       endif
      endif
     endif

	$(TOUCH) $(FULL_PROGRAM)
	@ $(ECHO)
else
	$(LINK_WRAPPER) $(CXX) $(FLAGS) $(EXTRA_LINK_FLAGS) $(EXE_OUTPUT) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS)
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
