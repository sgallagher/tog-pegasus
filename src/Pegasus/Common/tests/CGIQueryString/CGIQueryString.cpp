//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: CGIQueryString.cpp,v $
// Revision 1.1.1.1  2001/01/14 19:53:44  mike
// Pegasus import
//
//
//END_HISTORY

#include <cassert>
#include <Pegasus/Common/CGIQueryString.h>

using namespace Pegasus;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
//
// Example input:
//     NameSpace=root%2Fcimv20&ClassName=&LocalOnly=true
//
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    char* input = 0;
    char defaultInput[] = "NameSpace=root%2Fcimv20&ClassName=&LocalOnly=true";

    if (argc != 2)
	input = defaultInput;
    else
	input = argv[1];

    CGIQueryString qs(input);

#if 0
    for (Uint32 i = 0; i < qs.getCount(); i++)
    {
	cout << "name: " << qs.getName(i) << endl;
	cout << "value: " << qs.getValue(i) << endl;
    }
#endif

    if (input == defaultInput)
    {
	assert(strcmp(qs.getName(0), "NameSpace") == 0);
	assert(strcmp(qs.getValue(0), "root/cimv20") == 0);
	assert(strcmp(qs.getName(1), "ClassName") == 0);
	assert(strcmp(qs.getValue(1), "") == 0);
	assert(strcmp(qs.getName(2), "LocalOnly") == 0);
	assert(strcmp(qs.getValue(2), "true") == 0);
    }

    cout << "+++++ passed all tests" << endl;
    exit(0);
    return 0;
}
