//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include "../../CGIQueryString.h"

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

////////////////////////////////////////////////////////////////////////////////
//
// Example input:
//     NameSpace=root%2Fcimv2&ClassName=&LocalOnly=true
//
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    Boolean verbose = (getenv("PEGASUS_TEST_VERBOSE")) ? true : false;

	char* input = 0;
    char defaultInput[] = "NameSpace=root%2Fcimv2&ClassName=&LocalOnly=true";

    if (argc != 2)
	input = defaultInput;
    else
	input = argv[1];

    CGIQueryString qs(input);

	if(verbose) {
		for (Uint32 i = 0; i < qs.getCount(); i++)
		{
		cout << "name: " << qs.getName(i) << endl;
		cout << "value: " << qs.getValue(i) << endl;
		}
	}

    if (input == defaultInput)
    {
	assert(strcmp(qs.getName(0), "NameSpace") == 0);
	assert(strcmp(qs.getValue(0), "root/cimv2") == 0);
	assert(strcmp(qs.getName(1), "ClassName") == 0);
	assert(strcmp(qs.getValue(1), "") == 0);
	assert(strcmp(qs.getName(2), "LocalOnly") == 0);
	assert(strcmp(qs.getValue(2), "true") == 0);
    }


    // added instance name test ks July 2001
    // added when correcting error in quoted fields.
    {
	char* input = 0;
	char defaultInput[] =
"InstanceName=person.name%3D%22mike%22&LocalOnly=true&PropertyList=NULL";
	input = defaultInput;

	CGIQueryString qs(input);
	if(verbose) {
		for (Uint32 i = 0; i < qs.getCount(); i++)
		{
			cout << "name: " << qs.getName(i) << endl;
			cout << "value: " << qs.getValue(i) << endl;
		}

	}

    assert(strcmp(qs.getName(0), "InstanceName") == 0);
    //KS Forgot how to excape the quote character
    //assert(strcmp(qs.getValue(0), "person.name.""mike""") == 0);

    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return 0;
}

