//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Repository/InstanceIndexFile.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const char PATH[] = "X.idx";

void _CreateIndexFile()
{
    Uint32 index;

    Boolean result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1001,key2=\"Hello World 1\""), index);
    assert(result);

    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1002,key2=\"Hello World 2\""), index);
    assert(result);

    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1003,key2=\"Hello World 3\""), index);
    assert(result);

    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1004,key2=\"Hello World 4\""), index);
    assert(result);

    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1005,key2=\"Hello World 5\""), index);
    assert(result);

    result = InstanceIndexFile::remove(PATH, 
	CIMReference("X.key2=\"Hello World 3\",key1=1003"));
    assert(result);
}

int main(int argc, char** argv)
{
    try
    {
	_CreateIndexFile();

	CIMReference ref = "X.key1=1666,key2=\"Hello World N\"";

	Uint32 i;
	Uint32 j;
	
	assert(InstanceIndexFile::insert(PATH, ref, i));
	assert(InstanceIndexFile::lookup(PATH, ref, j));
	assert(i == j);
	// assert(InstanceIndexFile::remove(PATH, ref));
    }

    catch (Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
