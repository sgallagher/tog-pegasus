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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for Bug#1502
//
//%/////////////////////////////////////////////////////////////////////////////
/*
    This test exercies the methods in NameSapcemanager.h.  It defines a repository
    puts namespaces in the repository manipulates them and then deletes the
    namespaces through the namespacemanager interface.  The method it tests includes:
    constructor
    NamespaceAttributes - Incomplete testing at this point
    createNamespace
    get
    nameSpaceExists
    createClass
    deleteNameSpace
    getNameSpaceNames
    modifyNameSpace
    getClassFilePath

    The functions it DOES NOT test today includes:
    isRemoteNameSpace
    getQualifierFilePath
    getInstanceDataFileBase
    deleteClassName
    getQualifiersRoot
    getAssocClassPath
    getAssocInstancePath
    checkModify
    getSubClassNames
    GetSuperClassNames
    classHasInstances
    classExists
    Note: Many of these untested functions are adequately tested through their
    use by higher level functions in CIMRepository.
*/

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Repository/NameSpaceManager.h>
#include <Pegasus/Common/FileSystem.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

Boolean verbose = false;

String repositoryRoot;

Array<CIMNamespaceName> _nameSpaceNames;

#define NUM_NAMSPACE_NAMES _nameSpaceNames.size()

void test01()
{
    NameSpaceManager nsm (repositoryRoot);
    NameSpaceManager::NameSpaceAttributes nsa;
    nsa.insert("shareable","true");
    if (verbose)
        nsm.print (cout);

   _nameSpaceNames.append(CIMNamespaceName("aa"));
   _nameSpaceNames.append(CIMNamespaceName("aa/bb"));
   _nameSpaceNames.append(CIMNamespaceName("aa/bb/cc"));
   _nameSpaceNames.append(CIMNamespaceName("/lmnop/qrstuv"));
   _nameSpaceNames.append(CIMNamespaceName("root"));
   _nameSpaceNames.append(CIMNamespaceName("xx"));
   _nameSpaceNames.append(CIMNamespaceName("xx/yy"));

    for (Uint32 j = 0; j < _nameSpaceNames.size(); j++)
    {
        if (!_nameSpaceNames[j].equal(CIMNamespaceName("root")))
        {
            String dir (repositoryRoot);
            dir.append("/");
            dir.append((const char*)_nameSpaceNames [j].getString().getCString());

            FileSystem::removeDirectoryHier (dir);
            if (verbose)
                cout << "Directory Hiearchy= " << dir << endl;

            // Create a namespace
           nsm.createNameSpace (_nameSpaceNames[j], nsa);
        }
    }

    Array<CIMNamespaceName> nameSpaceNames;
    nsm.getNameSpaceNames(nameSpaceNames);
    if (verbose)
        nsm.print(cout);
    assert(nameSpaceNames.size() == NUM_NAMSPACE_NAMES);
    BubbleSort(nameSpaceNames);

    for (Uint32 i = 0; i < NUM_NAMSPACE_NAMES; i++)
    {
	assert(_nameSpaceNames[i] == nameSpaceNames[i]);
	assert(nsm.nameSpaceExists(nameSpaceNames[i]));
    }

    nsm.deleteNameSpace(CIMNamespaceName("lmnop/qrstuv"));
    nsm.getNameSpaceNames(nameSpaceNames);
    assert(nameSpaceNames.size() == NUM_NAMSPACE_NAMES - 1);

    // Create and delete a class to test these functions
    String outPath;
    nsm.createClass(CIMNamespaceName("aa/bb"), "MyClass", CIMName(), outPath);
    String classFilePath = nsm.getClassFilePath(CIMNamespaceName("aa/bb"),
        "MyClass",NameSpaceRead);
    String cfp (repositoryRoot);
    cfp.append("/aa#bb/classes/MyClass.#");
    assert (classFilePath == cfp);

    for (Uint32 j = 0; j < _nameSpaceNames.size(); j++)
    {
        if (!_nameSpaceNames[j].equal(CIMNamespaceName("root")))
        {
            if(nsm.nameSpaceExists(_nameSpaceNames[j]))
                nsm.deleteNameSpace(CIMNamespaceName(_nameSpaceNames[j]));
        }
    }
    nsm.getNameSpaceNames(nameSpaceNames);
    // Only the root namespace should be left.
    assert(nameSpaceNames.size() == 1);
    assert(nameSpaceNames[0].equal(CIMNamespaceName("root")));
    // confirm we can delete root
    nsm.deleteNameSpace(nameSpaceNames[0]);
    nsm.getNameSpaceNames(nameSpaceNames);
    assert(nameSpaceNames.size() == 0);
}

int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ": started" << endl;

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
      cout << argv[0] << e.getMessage() << endl;
        exit (1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
