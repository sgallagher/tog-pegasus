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
################################################################################
##
################################################################################

INCLUDES = "/Pegasus_Blddsk","$(ROOT)/src","$(ROOT)/src/standardincludes/cincludes$(EXTRA_VMSINCLUDES)

DEFINES +=,__USE_STD_IOSTREAM,PEGASUS_USE_RELEASE_DIRS,PEGASUS_USE_RELEASE_CONFIG_OPTIONS

$(OBJ_DIR)/%.obj: %.c $(ERROR)
	$(CC)/object=$(OBJ_VMSDIRA)] $(CCFLAGS)/DEFINE=(__NEW_STARLET) $*.c$(VMS_CLIB)
	@ $(TOUCH) $(OBJ_VMSDIRA)]$*.obj
	@ $(ECHO)

$(OBJ_DIR)/%.obj: %.cpp $(ERROR)
	$(CXX)/object=$(OBJ_VMSDIRA)] $(CFLAGS)/define=(VMS$(TMP_FLAGS)$(TMP_DEFINES)$(TMP_LDEFINES))/include=($(INCLUDES)$(TMP_SINCLUDES)","$(OPENSSL_VMSHOME)") $*.cpp$(VMS_LIB)
ifndef EXE_OUTPUT
	library/replace $(FULL_VMSLIB) $(OBJ_VMSDIRA)]$*.obj
endif
	@ $(TOUCH) $(OBJ_VMSDIRA)]$*.obj
	@ $(ECHO)
