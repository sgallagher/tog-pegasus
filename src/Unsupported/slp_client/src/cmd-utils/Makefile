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
#/*****************************************************************************
# *
# *  $Header: /cvs/MSB/pegasus/src/Unsupported/slp_client/src/cmd-utils/Attic/Makefile,v 1.2 2005/05/12 15:33:31 martin Exp $ 	                                                            
# *               					                    
# *  Copyright (c) 2001 - 2003  IBM                                          
# *  Copyright (c) 2000 - 2003 Michael Day                                    
# *                                                                           
# *  Permission is hereby granted, free of charge, to any person obtaining a  
# *  copy of this software and associated documentation files (the "Software"),
# *  to deal in the Software without restriction, including without limitation 
# *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# *  and/or sell copies of the Software, and to permit persons to whom the     
# *  Software is furnished to do so, subject to the following conditions:       
# * 
# *  The above copyright notice and this permission notice shall be included in 
# *  all copies or substantial portions of the Software.
# * 
# * 
# *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# *  DEALINGS IN THE SOFTWARE.
# *
# *****************************************************************************/






PROJECT = cmd-utils
SRC_DIR := $(shell pwd)

export OBJ_DIR := $(OBJ_DIR_CLIENT)
COMPONENTS = all
CLEAN = clean
INSTALL = install

export SRC_CMD_UTILS := $(SRC_DIR) 
export SRC_CLIENT := $(SRC_DIR)/slp_client
export SRC_SRVREG := $(SRC_DIR)/slp_srvreg
export SRC_QUERY := $(SRC_DIR)/slp_query


SRC_DIRS := $(SRC_CLIENT) $(SRC_SRVREG) $(SRC_QUERY) 

all: $(COMPONENTS)
.PHONY: all

clean: $(CLEAN)
.PHONY: clean

install: $(INSTALL)
.PHONY: install

$(COMPONENTS) $(INSTALL):
	$(foreach dir, $(SRC_DIRS),cd $(dir) && $(MAKE) $@;)

$(CLEAN) :
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.o
	$(IGNORE)$(LIBTOOL) $(RM) $(RM_FLAGS) *.lo
	$(foreach dir, $(SRC_DIRS),cd $(dir) && $(MAKE) -k $@;)	
