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

#include <cassert>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/FileSystem.h>

using namespace std;
using namespace Pegasus;

static const char* _nameSpaceNames[] =
{
    "aa",
    "aa/bb",
    "aa/bb/cc",
    "lmnop/qrstuv",
    "xx",
    "xx/yy"
};

static const Uint32 NUM_NAMSPACE_NAMES = 
    sizeof(_nameSpaceNames) / sizeof(_nameSpaceNames[0]);

void test01()
{
    FileSystem::removeDirectoryHier("./repository/lmnop#qrstuv");

    NameSpaceManager nsm("./repository");
    // nsm.print(cout);

    // Create a namespace called "lmnop/qrstuv":

    nsm.createNameSpace("lmnop/qrstuv");

    Array<String> nameSpaceNames;
    nsm.getNameSpaceNames(nameSpaceNames);
    assert(nameSpaceNames.size() == NUM_NAMSPACE_NAMES);
    BubbleSort(nameSpaceNames);

    for (Uint32 i = 0; i < NUM_NAMSPACE_NAMES; i++)
    {
	assert(_nameSpaceNames[i] == nameSpaceNames[i]);
	assert(nsm.nameSpaceExists(nameSpaceNames[i]));
    }

    nsm.deleteNameSpace("lmnop/qrstuv");

    nsm.getNameSpaceNames(nameSpaceNames);
    assert(nameSpaceNames.size() == NUM_NAMSPACE_NAMES - 1);

    String classFilePath = nsm.getClassFilePath("aa/bb", "MyClass");
}

int main()
{
    try 
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
