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
	
