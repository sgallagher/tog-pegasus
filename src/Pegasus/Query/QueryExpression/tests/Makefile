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
ROOT = ../../../../..

DIR = Pegasus/Query/QueryExpression/tests


ifneq ($(OS), HPUX) 
EXTRA_INCLUDES = -Int_includes
endif

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

LIBRARIES = \
    pegrepository \
    pegqueryexpression \
    pegwql \
    pegcql \
    pegquerycommon \
    pegconfig \
    pegcommon \

EXTRA_INCLUDES = $(SYS_INCLUDES)
#SYS_LIBS += -lfl

PROGRAM = testQueryExpression

SOURCES = QueryExpression.cpp 

QEXPRTESTNS=root/SampleProvider
QEXPRTESTNSDIRECTORY=$(REPOSITORY_ROOT)/root\#SampleProvider     

include $(ROOT)/mak/program.mak

repository:
	@ echo +++++ Loading QueryExpression Test Schema into $(QEXPRTESTNS) namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -N$(REPOSITORY_NAME) -M$(REPOSITORY_MODE) -n$(QEXPRTESTNS) QExprTestSchema.mof
	@ cimmofl -R$(REPOSITORY_DIR) -N$(REPOSITORY_NAME) -M$(REPOSITORY_MODE) -n$(QEXPRTESTNS) QExprTestSchemaR.mof

repositoryServer:
	@ echo +++++ Loading QueryExpression Test Schema into $(QEXPRTESTNS) namespace ...
	@ cimmof -n$(QEXPRTESTNS) QExprTestSchema.mof
	@ cimmof -n$(QEXPRTESTNS) QExprTestSchemaR.mof

tests: 
                                                                                
poststarttests:

clean:

