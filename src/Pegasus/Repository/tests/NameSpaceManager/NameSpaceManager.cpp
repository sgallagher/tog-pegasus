//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

String repositoryRoot;

static const char* _nameSpaceNames[] =
{
    "aa",
    "aa/bb",
    "aa/bb/cc",
    "lmnop/qrstuv",
    "root",
    "xx",
    "xx/yy"
};

static const Uint32 NUM_NAMSPACE_NAMES = 
    sizeof(_nameSpaceNames) / sizeof(_nameSpaceNames[0]);

void test01()
{
    NameSpaceManager nsm (repositoryRoot);
    //nsm.print (cout);

    for (Uint32 j = 0; j < NUM_NAMSPACE_NAMES; j++)
    {
        if (_nameSpaceNames[j] != "root")
        {
            String dir (repositoryRoot);
            dir.append("/");
            dir.append(_nameSpaceNames [j]);

            FileSystem::removeDirectoryHier (dir);

            // Create a namespace
            nsm.createNameSpace (_nameSpaceNames[j]);
        }
    }

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

    String outPath;
    nsm.createClass ("aa/bb", "MyClass", "", outPath);
    String classFilePath = nsm.getClassFilePath("aa/bb", "MyClass");
    String cfp (repositoryRoot);
    cfp.append("/aa#bb/classes/MyClass.#");
    assert (classFilePath == cfp);
}

int main()
{
    const char* tmpDir = getenv ("PEGASUS_TMP");
    if (tmpDir == NULL)
    {
        repositoryRoot = ".";
    }
    else
    {
        repositoryRoot = tmpDir;
    }
    repositoryRoot.append("/repository");

    try 
    {
	test01();
    }
    catch (Exception& e)
    {
	cout << e.getMessage() << endl;
        exit (1);
    }

    cout << "+++++ passed all tests" << endl;

    return 0;
}
