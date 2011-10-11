#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////
ROOT = ../../..

DIR = Clients/cimcli
include $(ROOT)/mak/config.mak

CIMCLI = @cimcli

LIBRARIES = \
    pegclient \
    peggeneral \
    pegcommon

 LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimcli

SOURCES = CIMCLICommand.cpp \
	  CIMCLIClient.cpp  \
	  ObjectBuilder.cpp \
	  CIMCLIOutput.cpp \
	  CIMCLIHelp.cpp \
	  CIMCLIOptions.cpp \
	  CIMCLICommon.cpp \
	  CIMCLIOptionStruct.cpp \
	  CIMCLIOperations.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

############################################################################
## create instance operation with embedded instance array property
c5:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=501 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=502 \
		    comment="First embedded Instance" } \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=503 \
		    comment="Second embedded Instance" } >> $(RESULTFILE)

t5:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=501 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=502 \
		    comment="First embedded Instance" } \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=503 \
		    comment="Second embedded Instance" } >> $(RESULTFILE) \
		>> $(RESULTFILE)

d5:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=501 \
		>> $(RESULTFILE)

g5:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=501 \
		>> $(RESULTFILE)

## Build an instance with embedded instance, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct5: c5 g5 t5 d5
	@$(ECHO) +++++ passed test ct5

