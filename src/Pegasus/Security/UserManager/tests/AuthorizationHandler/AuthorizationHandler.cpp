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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//              (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/UserManager/AuthorizationHandler.h>

// Uncomment this if you want detailed messages to be printed.
//#define DEBUG 1

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

Boolean verbose = false;

static const CIMNamespaceName GOOD_NAMESPACE = CIMNamespaceName ("root/cimv2");

static const CIMNamespaceName BAD_NAMESPACE = CIMNamespaceName ("root/cimvx99");


//
// main
//
int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ": started" << endl;

    CIMNamespaceName nameSpace = CIMNamespaceName ();

    String testUser = String::EMPTY;

    testUser.assign(System::getEffectiveUserName());

    // Create a test repository
    const char* tmpDir = getenv ("PEGASUS_TMP");
    String repositoryPath;
    if (tmpDir == NULL)
    {
        repositoryPath = ".";
    }
    else
    {
        repositoryPath = tmpDir;
    }
    repositoryPath.append("/repository");

    PEGASUS_ASSERT(FileSystem::isDirectory(repositoryPath));

    CIMRepository* repository = new CIMRepository(repositoryPath);

    // -- Create a UserManager object:

    UserManager*  userManager = UserManager::getInstance(repository);

    //
    // Test authorization
    //
    try
    {
        nameSpace = BAD_NAMESPACE;
        assert(!userManager->verifyNamespace(nameSpace));

        nameSpace = GOOD_NAMESPACE;
        assert(!userManager->verifyNamespace(nameSpace));

        userManager->setAuthorization(testUser, nameSpace, "rw");
        userManager->setAuthorization("root", nameSpace, "w");

        String temp = userManager->getAuthorization(testUser, nameSpace);

        if (testUser != "root")
           assert(String::equal(temp, "rw") || String::equal(temp, "wr"));

        temp = userManager->getAuthorization("root", nameSpace);
        assert(String::equal(temp, "w"));

        userManager->removeAuthorization("root", nameSpace);
        temp.clear();
        try
        {
            temp = userManager->getAuthorization("root", nameSpace);
            assert(temp.size() == 0);
        }catch(const Exception&) { }

        userManager->setAuthorization("root", nameSpace, "w");

        if (testUser != "root") 
           assert(userManager->verifyAuthorization(testUser, nameSpace, 
                   CIMName ("GetInstance")));
        assert(!userManager->verifyAuthorization("root", nameSpace, 
            CIMName ("GetInstance")));

        userManager->setAuthorization("root", nameSpace, "r");

        assert(!userManager->verifyAuthorization("root", nameSpace, 
            CIMName ("SetProperty")));

        userManager->removeAuthorization("root", nameSpace);
        if (testUser != "root")
           userManager->removeAuthorization(testUser, nameSpace);
    }
    catch(Exception& e)
    {
      cout << argv[0] << " Exception: " << e.getMessage() << endl;
        assert(0);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
