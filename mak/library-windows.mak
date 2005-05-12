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
FULL_LIB = $(LIB_DIR)/$(LIBRARY)$(LIB_SUFFIX)
FULL_DLL = $(BIN_DIR)/$(LIBRARY)$(DLL)
FULL_EXP = $(BIN_DIR)/$(LIBRARY)$(EXP)
FULL_ILK = $(BIN_DIR)/$(LIBRARY)$(ILK)
FULL_PDB = $(BIN_DIR)/$(LIBRARY)$(PDB)

##
## ws2_32.lib is needed to get the WINSOCK routines!
##

$(FULL_LIB): $(BIN_DIR)/target $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) $(ERROR)
	link -nologo -dll $(LINK_FLAGS) $(EXTRA_LINK_FLAGS) -out:$(FULL_DLL) -implib:$(FULL_LIB) $(OBJECTS) $(FULL_LIBRARIES) $(SYS_LIBS) $(EXTRA_LIBRARIES)

FILES_TO_CLEAN = \
    $(OBJECTS) $(FULL_LIB) $(FULL_DLL) $(FULL_EXP) $(FULL_ILK) $(FULL_PDB) $(OBJ_DIR)/vc60$(PDB) $(OBJ_DIR)/vc70$(PDB) depend.mak depend.mak.bak

clean-lib: $(ERROR)
	mu rm $(FULL_LIB) $(FULL_DLL) $(FULL_EXP)
