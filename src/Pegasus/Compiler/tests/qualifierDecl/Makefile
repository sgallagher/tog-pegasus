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
ROOT = ../../../../..
DIR = Pegasus/Compiler/tests/qualifierDecl
include $(ROOT)/mak/config.mak
include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = qualifierDecl
SOURCES = qualifierDecl.cpp

include $(ROOT)/mak/program.mak

REPOSITORYDIR = $(TMP_DIR)
RESULTFILE = $(TMP_DIR)/result

RESULTMASTERFILE = $(ROOT)/src/$(DIR)/result.master

## test addition of selected qualifierDecls. Note that this is not a complete
## qualifierDecl test today but largely for fixes in bug 133 and a number
## of errors in parsing the qualifierDecl
tests:
	@ $(RMREPOSITORY) $(REPOSITORYDIR)/repository
	@ cimmofl "-R$(REPOSITORYDIR)" "-M$(REPOSITORY_MODE)" -nroot testqualifiers.mof
	@ $(PROGRAM)
	@ $(RMREPOSITORY) $(REPOSITORYDIR)/repository
	@ $(ECHO) +++ $(PROGRAM) program test passed successfully +++

##  Error tests. One mof file for each syntax error test. 1 - 9 are
##  Flavor error. 10-16 are Scope errors.
	@ $(RM) $(RESULTFILE)
	-@ cimmofl -E -W badQualifierDecl1.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl2.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl3.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl4.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl5.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl6.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl10.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl11.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl12.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl13.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl14.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl15.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl16.mof >> $(RESULTFILE) 2>&1
	-@ cimmofl -E -W badQualifierDecl20.mof >> $(RESULTFILE) 2>&1
	@ $(DIFF) $(RESULTFILE) $(RESULTMASTERFILE)
	@ $(RM) $(RESULTFILE)
	@ $(ECHO) +++ $(PROGRAM) Syntax error tests passed successfully +++
poststarttests:

