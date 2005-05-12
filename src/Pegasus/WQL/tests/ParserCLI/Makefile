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

DIR = Pegasus/WQL/tests/ParserCLI

LIBRARIES = pegwql pegcommon pegquerycommon

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestParserCLI
SOURCES = ParserCLI.cpp

include $(ROOT)/mak/program.mak

# Warning: These tests are based on s specific set of paramaters and their
# values in ParserCLI. the values of w,x,y,z are preset and the
# end result must be true.   Thus, for example w is always TRUE

tests: test1 test2 test2a test3 test4 test5 test5a test6 test7

ifdef PEGASUS_SNIA_INTEROP_TEST
tests += test8
endif

test1:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName"

test2:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE OR w = FALSE"

test2a:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND w != FALSE"	

test3:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\"" 

test4:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE (w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\") AND NOT w = TRUE IS NOT TRUE"

test5:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE NOT NOT NOT x < 5"

test5a:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w <> FALSE"

test6:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE v IS NULL"

test7:
	@ $(PROGRAM) "SELECT * FROM CIM_InstCreation WHERE SourceInstance ISA CIM_ComputerSystem"

poststarttests:

