//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <cassert>
#include <Pegasus/Repository/CIMRepository.h>
//#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Repository/InstanceIndexFile.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const char PATH[] = "X.idx";

void _RenameTempFile()
{
    Boolean result;

    //
    // Remove the original instance index file
    //
    String realFilePath;
    if (FileSystem::existsNoCase(PATH, realFilePath))
    {
        result = FileSystem::removeFileNoCase(realFilePath);
        assert(result);
    }

    //
    // Rename the temporary instance index file
    //
    String tmpFilePath = PATH;
    result = FileSystem::renameFile(tmpFilePath + ".tmp", PATH);
    assert(result);
}

void _CreateIndexFile()
{
    Uint32 index;
    Uint32 size;

    size = 1427;
    index = 0;
    Boolean result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1001,key2=\"Hello World 1\""), size, index);
    assert(result);
    _RenameTempFile();

    size = 1433;
    index = 1427;
    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1002,key2=\"Hello World 2\""), size, index);
    assert(result);
    _RenameTempFile();

    size = 1428;
    index = 2860;
    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1003,key2=\"Hello World 3\""), size, index);
    assert(result);
    _RenameTempFile();

    size = 1433;
    index = 4288;
    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1004,key2=\"Hello World 4\""), size, index);
    assert(result);
    _RenameTempFile();

    size = 1431;
    index = 5721;
    result = InstanceIndexFile::insert(PATH, 
	CIMReference("X.key1=1005,key2=\"Hello World 5\""), size, index);
    assert(result);
    _RenameTempFile();

    result = InstanceIndexFile::remove(PATH, 
          CIMReference("X.key2=\"Hello World 3\",key1=1003"));
    assert(result);
    _RenameTempFile();
}

int main(int argc, char** argv)
{
    try
    {
	_CreateIndexFile();

	Uint32 sizeIn;
	Uint32 indexIn;
	Uint32 sizeOut;
	Uint32 indexOut;
	
	CIMReference ref = "X.key1=1666,key2=\"Hello World N\"";

        sizeIn = 1431;
        indexIn = 7152;
	assert(InstanceIndexFile::insert(PATH, ref, sizeIn, indexIn));
        _RenameTempFile();

	assert(InstanceIndexFile::lookup(PATH, ref, sizeOut, indexOut));
	assert(sizeIn == sizeOut);
	assert(indexIn == indexOut);

        sizeIn = 1427;
        indexIn = 8583;
	assert(InstanceIndexFile::modify(PATH, ref, sizeIn, indexIn));
        _RenameTempFile();

        //
        // lookup a non-existent instance
        //
	ref = "X.key1=8888,key2=\"Hello World N\"";
	if (!InstanceIndexFile::lookup(PATH, ref, sizeOut, indexOut))
        {
            cout << "Expected result: Object does not exists. " << endl;
        }
        else
        {
            throw Exception ("Unexpected result.");
        }
    }

    catch (Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
