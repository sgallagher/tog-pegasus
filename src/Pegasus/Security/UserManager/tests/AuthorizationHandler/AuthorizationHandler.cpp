//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies of substantial portions of this software.
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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Security/UserManager/AuthorizationHandler.h>

// Uncomment this if you want detailed messages to be printed.
//#define VERBOSE 1

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;


static const String GOOD_NAMESPACE       = "root/cimv2";

static const String BAD_NAMESPACE        = "root/cimvx99";


void GetEnvironmentVariables(String& pegasusHome)
{
    // Get environment variables:

    const char* tmp = getenv("PEGASUS_HOME");

    if (!tmp)
    {
        cerr << " PEGASUS_HOME environment variable undefined" << endl;
        exit(1);
    }

    pegasusHome = tmp;

    FileSystem::translateSlashes(pegasusHome);
}

//
// main
//
int main()
{
    String pegasusHome = String::EMPTY;
    String nameSpace = String::EMPTY;

    static const char REPOSITORY[] = "/repository";

    String testUser = String::EMPTY;

    Boolean authorized;


    testUser.assign(System::getCurrentLoginName());

    GetEnvironmentVariables(pegasusHome);

    assert(FileSystem::isDirectory(pegasusHome));

    pegasusHome.append(REPOSITORY);

    assert(FileSystem::isDirectory(pegasusHome));
        

    // Create a repository
    CIMRepository* repository = new CIMRepository(pegasusHome);

    // Create auth handler
    AuthorizationHandler* authHandler = new AuthorizationHandler(repository);

    //
    // Test authorization
    //
    try
    {
        nameSpace.assign(BAD_NAMESPACE);
        assert(!authHandler->verifyNamespace(nameSpace));

        nameSpace.assign(GOOD_NAMESPACE);
        assert(authHandler->verifyNamespace(nameSpace));

        authHandler->setAuthorization(testUser, nameSpace, "rw");
        authHandler->setAuthorization("root", nameSpace, "w");

        String temp = authHandler->getAuthorization(testUser, nameSpace);

        if (testUser != "root")
           assert(String::equal(temp, "rw") || String::equal(temp, "wr"));

        temp = authHandler->getAuthorization("root", nameSpace);
        assert(String::equal(temp, "w"));

        authHandler->removeAuthorization("root", nameSpace);
        temp.clear();
        try
        {
            temp = authHandler->getAuthorization("root", nameSpace);
            assert(temp.size() == 0);
        }catch(Exception& e) { }

        authHandler->setAuthorization("root", nameSpace, "w");

        if (testUser != "root") 
           assert(authHandler->verifyAuthorization(testUser, nameSpace, 
                   "GetInstance"));
        assert(!authHandler->verifyAuthorization("root", nameSpace, "GetInstance"));

        authHandler->setAuthorization("root", nameSpace, "r");

        assert(!authHandler->verifyAuthorization("root", nameSpace, "SetProperty"));

        authHandler->removeAuthorization("root", nameSpace);
        if (testUser != "root")
           authHandler->removeAuthorization(testUser, nameSpace);
    }
    catch(Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
        assert(0);
    }


    cout << "+++++ passed all tests" << endl;

    return 0;
}
