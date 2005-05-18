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
// Modified By:  Carol Ann Krug Graves, Hewlett-Packard Company
//                   (carolann_graves@hp.com)
//               Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//               David Dillard, VERITAS Software Corp.
//                   (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Repository/InstanceIndexFile.h>
#include <Pegasus/Repository/InstanceDataFile.h>

#include <iostream>
#include <cstring>
#include <cassert>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const char* tmpDir;

void _Test01()
{
    String indexPath (tmpDir);
    indexPath.append("/X.idx");
    CString ipd = indexPath.getCString();
    const char* PATH = ipd;
    Uint32 index;
    Uint32 size;
    Uint32 freeCount = 0;

    // create 5 entries
    size = 1427;
    index = 0;
    Boolean result = InstanceIndexFile::createEntry(PATH, 
	CIMObjectPath("X.key1=1001,key2=\"Hello World 1\""), index, size);
    assert(result);

    size = 1433;
    index = 1427;
    result = InstanceIndexFile::createEntry(PATH, 
	CIMObjectPath("X.key1=1002,key2=\"Hello World 2\""), index, size);
    assert(result);

    size = 1428;
    index = 2860;
    result = InstanceIndexFile::createEntry(PATH, 
	CIMObjectPath("X.key1=1003,key2=\"Hello World 3\""), index, size);
    assert(result);

    size = 1433;
    index = 4288;
    result = InstanceIndexFile::createEntry(PATH, 
	CIMObjectPath("X.key1=1004,key2=\"Hello World 4\""), index, size);
    assert(result);

    size = 1431;
    index = 5721;
    result = InstanceIndexFile::createEntry(PATH, 
	CIMObjectPath("X.key1=1005,key2=\"Hello World 5\""), index, size);
    assert(result);

    // delete the 3rd entry
    result = InstanceIndexFile::deleteEntry(PATH, 
          CIMObjectPath("X.key2=\"Hello World 3\",key1=1003"), freeCount);
    assert(result);

    // create a new entry
    size = 1428;
    index = 2860;
    result = InstanceIndexFile::createEntry(
	PATH, CIMObjectPath("X.key1=1003,key2=\"Hello World 3\""), 
	index, size);
    assert(result);

    // delete the newly created entry
    result = InstanceIndexFile::deleteEntry(PATH, 
          CIMObjectPath("X.key2=\"Hello World 3\",key1=1003"), freeCount);
    assert(result);

    // delete the first entry
    result = InstanceIndexFile::deleteEntry(PATH, 
	CIMObjectPath("X.key1=1001,key2=\"Hello World 1\""), freeCount);
    assert(result);

    // modify the 5th entry
    size = 9999;
    index = 8888;
    result = InstanceIndexFile::modifyEntry(PATH, 
	CIMObjectPath("X.key1=1005,key2=\"Hello World 5\""), 
	index, size, freeCount);
    assert(result);

    //
    // Iterate the entries in the file:
    //   There should be 7 entries
    //   The 1st, 3rd, 5th and 6th entries should be marked as 'free'.
    //

    {
	Array<Uint32> freeFlags;
	Array<Uint32> indices;
	Array<Uint32> sizes;
	Array<CIMObjectPath> instanceNames;

	Boolean flag = InstanceIndexFile::enumerateEntries(
	    PATH, freeFlags, indices, sizes, instanceNames, true);

	assert(flag);

	assert(freeFlags.size() == indices.size());
	assert(indices.size() == sizes.size());
	assert(sizes.size() == instanceNames.size());

        assert( freeFlags[0] == 1 &&
                freeFlags[2] == 1 &&
                freeFlags[4] == 1 &&
                freeFlags[5] == 1); 
    }

    //
    // Now attempt to compact:
    //
    assert(InstanceIndexFile::compact(PATH));

    //
    // Verify the result:
    //   There should be 3 entries and no 'free' entries
    //
    {
	Array<Uint32> freeFlags;
	Array<Uint32> indices;
	Array<Uint32> sizes;
	Array<CIMObjectPath> instanceNames;

        Boolean flag = InstanceIndexFile::enumerateEntries(
            PATH, freeFlags, indices, sizes, instanceNames, true);

        assert(flag);

        assert(freeFlags.size() == 3);
        assert(freeFlags.size() == indices.size());
        assert(indices.size() == sizes.size());
        assert(sizes.size() == instanceNames.size());

        for (Uint32 i = 0; i < freeFlags.size(); i++)
        {
            assert(freeFlags[i] == 0);
        }
    }
}

void _Test02()
{
    String instancesPath (tmpDir);
    instancesPath.append("/X.instances");
    CString ipd = instancesPath.getCString();
    const char* PATH = ipd;

    //
    // Append some instances:
    //

    Array<char> data;
    Uint32 index;

    data.append("AAAAAAAA", 8);
    InstanceDataFile::appendInstance(PATH, data, index);
    data.clear();

    data.append("BBBBBBBB", 8);
    InstanceDataFile::appendInstance(PATH, data, index);
    data.clear();

    data.append("CCCCCCCC", 8);
    InstanceDataFile::appendInstance(PATH, data, index);
    data.clear();

    //
    // Load some instances:
    //

    InstanceDataFile::loadInstance(PATH, 8, 8, data);
    assert(memcmp(data.getData(), "BBBBBBBB", 8) == 0);
    data.clear();

    InstanceDataFile::loadInstance(PATH, 0, 8, data);
    assert(memcmp(data.getData(), "AAAAAAAA", 8) == 0);
    data.clear();

    InstanceDataFile::loadInstance(PATH, 16, 8, data);
    assert(memcmp(data.getData(), "CCCCCCCC", 8) == 0);
    data.clear();

    //
    // Load all instances:
    //

    InstanceDataFile::loadAllInstances(PATH, data);
    assert(memcmp(data.getData(), "AAAAAAAABBBBBBBBCCCCCCCC", 24) == 0);
    assert(data.size() == 3 * 8);
    data.clear();

    //
    // Now attempt rollback:
    //

    assert(InstanceDataFile::beginTransaction(PATH));

    data.append("ZZZZZZZZ", 8);
    InstanceDataFile::appendInstance(PATH, data, index);
    data.clear();

    assert(InstanceDataFile::rollbackTransaction(PATH));

    //
    // Now attempt commit:
    //

    assert(InstanceDataFile::beginTransaction(PATH));

    data.append("DDDDDDDD", 8);
    InstanceDataFile::appendInstance(PATH, data, index);
    data.clear();

    assert(InstanceDataFile::commitTransaction(PATH));

    //
    // Verify the result:
    //

    InstanceDataFile::loadAllInstances(PATH, data);
    assert(memcmp(data.getData(), "AAAAAAAABBBBBBBBCCCCCCCCDDDDDDDD", 32) == 0);
    assert(data.size() == 4 * 8);
    data.clear();

    //
    // Now attempt to compact:
    //

    Array<Uint32> freeFlags;
    Array<Uint32> indices;
    Array<Uint32> sizes;

    freeFlags.append(0);
    indices.append(0);
    sizes.append(8);

    freeFlags.append(0);
    indices.append(16);
    sizes.append(8);

    assert(InstanceDataFile::compact(PATH, freeFlags, indices, sizes));

    //
    // Verify the result:
    //

    InstanceDataFile::loadAllInstances(PATH, data);
    assert(memcmp(data.getData(), "AAAAAAAACCCCCCCC", 16) == 0);
    assert(data.size() == 2 * 8);
    data.clear();
}

int main(int argc, char** argv)
{
    tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        tmpDir = ".";
    }

    try
    {
	_Test01();
	_Test02();
    }

    catch (Exception& e)
    {
	cerr << "Error: " << e.getMessage() << endl;
	exit(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
