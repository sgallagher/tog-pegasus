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
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Common/Base64.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/Authentication/AuthenticationManager.h>

//
// Enable debug messages
//
//#define DEBUG 1


PEGASUS_USING_STD;

PEGASUS_USING_PEGASUS;

Boolean verbose = false;

String testUser = System::getEffectiveUserName();

String guestUser = "guest";

String guestPassword = "guest";

String localHeader = "Local ";

String basicHeader = "Basic ";



String encodeUserPass(const String& userPass)
{
    //
    // copy userPass string content to Uint8 array for encoding
    //
    Array<char> userPassArray;

    Uint32 userPassLength = userPass.size();

    userPassArray.reserveCapacity( userPassLength );
    userPassArray.clear();

    for( Uint32 i = 0; i < userPassLength; i++ )
    {
        userPassArray.append( (Uint8)userPass[i] );
    }

    //
    // base64 encode the user name and password
    //
    Array<char> encodedArray;

    encodedArray = Base64::encode( userPassArray );

    String encodedStr =
        String( encodedArray.getData(), encodedArray.size() );

    if (verbose) cout << "userPass: " << userPass << endl;
    if (verbose) cout << "Encoded userPass: " << encodedStr << endl;

    return (encodedStr);
}

//
// Test HTTP Auth header creation
//
void testHttpAuthHeader()
{
    AuthenticationManager  authManager;

    String respHeader = authManager.getHttpAuthResponseHeader();

    if (verbose) cout << "realm = " << respHeader << endl;

    PEGASUS_ASSERT(respHeader.size() != 0);
}

//
// Test Local Auth header creation
//
void testLocalAuthHeader_1()
{
    String authHeader = String::EMPTY;

    AuthenticationManager  authManager;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    // Test invalid header
    String respHeader = authManager.getPegasusAuthResponseHeader(testUser, authInfo);

    if (verbose) cout << "RespHeader: " << respHeader << endl;

    delete authInfo;

    Uint32 startQuote = respHeader.find(0, '"');
    PEGASUS_ASSERT(startQuote == PEG_NOT_FOUND);
}

//
// Test Local Auth header creation
//
void testLocalAuthHeader_2()
{
    String authHeader = String::EMPTY;

    AuthenticationManager  authManager;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    // Test invalid header
    authHeader.append(localHeader);

    String respHeader = authManager.getPegasusAuthResponseHeader(authHeader, authInfo);

    if (verbose) cout << "RespHeader: " << respHeader << endl;

    delete authInfo;

    Uint32 startQuote = respHeader.find(0, '"');
    PEGASUS_ASSERT(startQuote == PEG_NOT_FOUND);
}

//
// Test Local Auth header creation
//
void testLocalAuthHeader_3()
{
    String authHeader = String::EMPTY;

    AuthenticationManager  authManager;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    // Test invalid header
    authHeader.append(localHeader);
    authHeader.append("\"\"");

    String respHeader = authManager.getPegasusAuthResponseHeader(authHeader, authInfo);

    if (verbose) cout << "RespHeader: " << respHeader << endl;

    delete authInfo;

    Uint32 startQuote = respHeader.find(0, '"');
    PEGASUS_ASSERT(startQuote == PEG_NOT_FOUND);
}

//
// Test local authentication
//
void testLocalAuthSuccess()
{
    String authHeader = String::EMPTY;

    AuthenticationManager  authManager;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    // Test valid header
    authHeader.append(localHeader);
    authHeader.append("\"");
    authHeader.append(testUser);
    authHeader.append("\"");

    String respHeader = authManager.getPegasusAuthResponseHeader(authHeader, authInfo);

    if (verbose) cout << "RespHeader: " << respHeader << endl;

    Uint32 startQuote = respHeader.find(0, '"');
    PEGASUS_ASSERT(startQuote != PEG_NOT_FOUND);

    Uint32 endQuote = respHeader.find(startQuote + 1, '"');
    PEGASUS_ASSERT(endQuote != PEG_NOT_FOUND);

    String filePath = respHeader.subString(
        startQuote + 1, (endQuote - startQuote - 1));
    PEGASUS_ASSERT(filePath.size() != 0);

    authHeader.clear();
    authHeader.append(localHeader);
    authHeader.append("\"");
    authHeader.append(testUser);
    authHeader.append(":");
    authHeader.append(filePath);
    authHeader.append(":");
    authHeader.append(authInfo->getAuthChallenge());
    authHeader.append("\"");

    if (verbose) cout << "Local Resp AuthHeader: " << authHeader << endl;

    Boolean authenticated;

    authenticated =
        authManager.performPegasusAuthentication(authHeader, authInfo);

    //
    // remove the auth file created for this user request
    //
    if (FileSystem::exists(filePath))
    {
        FileSystem::removeFile(filePath);
    }

    if (authenticated)
        if (verbose) cout << "User " + testUser + " local authenticated successfully." << endl;
    else
        if (verbose) cout << "User " + testUser + " local authentication failed." << endl;

    delete authInfo;

    PEGASUS_ASSERT(authenticated);
}


//
// Test HTTP Basic with valid user name and password 
//
void testBasicAuthSuccess()
{
    String authHeader = String::EMPTY;

    AuthenticationManager  authManager;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    String userPass = guestUser;
    userPass.append(":");
    userPass.append(guestPassword);

    authHeader.append(basicHeader);
    authHeader.append(encodeUserPass(userPass));

    Boolean authenticated;

    authenticated = 
        authManager.performHttpAuthentication(authHeader, authInfo);

    if (authenticated)
        if (verbose) cout << "User " + guestUser + " authenticated successfully." << endl;
    else
        if (verbose) cout << "User " + guestUser + " authentication failed." << endl;

    delete authInfo;

    //PEGASUS_ASSERT(authenticated);
}

////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    verbose = (getenv ("PEGASUS_TEST_VERBOSE")) ? true : false;
    if (verbose) cout << argv[0] << ": started" << endl;
  
#if defined(PEGASUS_OS_TYPE_UNIX)

    try
    {
#ifdef DEBUG
        Tracer::setTraceFile("/tmp/trace");
        Tracer::setTraceComponents("all");
	verbose = true;
#endif

        ConfigManager* configManager = ConfigManager::getInstance();

        const char* path = getenv("PEGASUS_HOME");
        String pegHome = path;

        if(pegHome.size())
            ConfigManager::setPegasusHome(pegHome);

        if (verbose)cout << "Peg Home : " << ConfigManager::getPegasusHome() << endl;

        if (verbose) cout << "Doing testHttpAuthHeader()...." << endl;

        // -- Create a repository:
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

        FileSystem::isDirectory(repositoryPath);

        CIMRepository* repository = new CIMRepository(repositoryPath);

        // -- Create a UserManager object:

        UserManager* userManager = UserManager::getInstance(repository
);

        testHttpAuthHeader();

        if (verbose) cout << "Doing testLocalAuthHeader_1()...." << endl;
        testLocalAuthHeader_1();

        if (verbose) cout << "Doing testLocalAuthHeader_2()...." << endl;
        testLocalAuthHeader_2();

        if (verbose) cout << "Doing testLocalAuthHeader_3()...." << endl;
        testLocalAuthHeader_3();

        if (verbose) cout << "Doing testLocalAuthSuccess()...." << endl;
        testLocalAuthSuccess();

        if (verbose) cout << "Doing testBasicAuthSuccess()...." << endl;
        testBasicAuthSuccess();
    }
    catch(Exception& e)
    {
      cout << argv[0] << " Exception: " << e.getMessage() << endl;
        PEGASUS_ASSERT(0);
    }

#endif

    cout << argv[0] << " +++++ passed all tests" << endl;

    return 0;
}
