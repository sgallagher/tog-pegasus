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
DEPEND_MAK = $(OBJ_DIR)/depend.mak

ifeq ($(PEGASUS_PLATFORM),SOLARIS_SPARC_CC)
depend: $(OBJ_DIR)/target $(ERROR)
	$(CXX) -xM1 $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $(SOURCES) | sed -e 's=^\(.*:\)='$(OBJ_DIR)'/\1=' > $(DEPEND_MAK)
	
else
ifdef PEGASUS_HAS_MAKEDEPEND
DEPEND_INCLUDES += -DPEGASUS_OS_TYPE_UNIX -I/usr/include $(SYS_INCLUDES)

depend: $(OBJ_DIR)/target $(ERROR)
	touch $(DEPEND_MAK)
	makedepend -v $(LOCAL_DEFINES) $(DEFINES) $(DEPEND_DEFINES) $(PRE_DEPEND_INCLUDES) $(DEPEND_INCLUDES) $(INCLUDES) $(SOURCES) -f $(DEPEND_MAK) -p $(OBJ_DIR)/
else
depend: $(OBJ_DIR)/target $(ERROR)
	mu depend -O$(OBJ_DIR) $(INCLUDES) $(SOURCES) > $(DEPEND_MAK)
endif
endif

clean-depend:
	$(RM) $(OBJ_DIR)/depend.mak
