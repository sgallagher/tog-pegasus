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

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <iostream>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Base64.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Security/UserManager/UserManager.h>
#include <Pegasus/Security/Authentication/BasicAuthenticationHandler.h>

//
// Enable debug messages
//
//#define DEBUG 1


PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

String authType = "Basic";

String testUser = System::getEffectiveUserName();

String guestUser = "guest";

String guestPassword = "guest";

String invalidUser = "xyz123ww";

String invalidPassword = "xyz123ww";

String encodeUserPass(const String& userPass)
{
    //
    // copy userPass string content to Uint8 array for encoding
    //
    Array <Uint8>  userPassArray;

    Uint32 userPassLength = userPass.size();

    userPassArray.reserve( userPassLength );
    userPassArray.clear();

    for( Uint32 i = 0; i < userPassLength; i++ )
    {
        userPassArray.append( (Uint8)userPass[i] );
    }

    //
    // base64 encode the user name and password
    //
    Array <Sint8>  encodedArray;

    encodedArray = Base64::encode( userPassArray );

    String encodedStr =
        String( encodedArray.getData(), encodedArray.size() );

#ifdef DEBUG
    cout << "userPass: " << userPass << endl;
    cout << "Encoded userPass: " << encodedStr << endl;
#endif

    return (encodedStr);
}

void testAuthHeader()
{
    BasicAuthenticationHandler  basicAuthHandler;

    String respHeader = basicAuthHandler.getAuthResponseHeader();

#ifdef DEBUG
    cout << "realm = " << respHeader << endl;
#endif

    PEGASUS_ASSERT(respHeader.size() != 0);
}

//
// Test with invalid userPass (with no ':' separator)
//
void testAuthenticationFailure_1()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    BasicAuthenticationHandler  basicAuthHandler;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    //
    // Test with invalid user password string
    //
    String userPass = testUser;
    userPass.append(guestPassword);

    authHeader.append(encodeUserPass(userPass));

    authenticated = basicAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed." << endl;
#endif

    delete authInfo;

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid system user
//
void testAuthenticationFailure_2()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    BasicAuthenticationHandler  basicAuthHandler;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    //
    // Test with invalid password
    //
    String userPass = invalidUser;
    userPass.append(":");
    userPass.append(guestPassword);

    authHeader.append(encodeUserPass(userPass));

    authenticated = basicAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    if (authenticated)
        cout << "User " + invalidUser + " authenticated successfully." << endl;
    else
        cout << "User " + invalidUser + " authentication failed." << endl;
#endif

    delete authInfo;

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid password 
//
void testAuthenticationFailure_3()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    BasicAuthenticationHandler  basicAuthHandler;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    String userPass = testUser;
    userPass.append(":");
    userPass.append(guestPassword);

    authHeader.append(encodeUserPass(userPass));

    authenticated = basicAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed." << endl;
#endif

    delete authInfo;

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid CIM user or invalid password
//
void testAuthenticationFailure_4()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    BasicAuthenticationHandler  basicAuthHandler;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    String userPass = testUser;
    userPass.append(":");
    userPass.append(invalidPassword);

    authHeader.append(encodeUserPass(userPass));

    authenticated = basicAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed." << endl;
#endif

    delete authInfo;

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with valid user name and password 
// (Assuming there is a valid CIM user 'guest' with password 'guest')
//
void testAuthenticationSuccess()
{
    String authHeader = String::EMPTY;

    BasicAuthenticationHandler  basicAuthHandler;

    AuthenticationInfo* authInfo = new AuthenticationInfo(true);

    String userPass = guestUser;
    userPass.append(":");
    userPass.append(guestPassword);

    authHeader.append(encodeUserPass(userPass));

    Boolean authenticated;

    authenticated = basicAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    if (authenticated)
        cout << "User " + guestUser + " authenticated successfully." << endl;
    else
        cout << "User " + guestUser + " authentication failed." << endl;
#endif

    delete authInfo;

    //PEGASUS_ASSERT(authenticated);
}

////////////////////////////////////////////////////////////////////

int main()
{
#if defined(PEGASUS_OS_TYPE_UNIX)

    try
    {
#ifdef DEBUG
        Tracer::setTraceFile("/tmp/trace");
        Tracer::setTraceComponents("all");
#endif

        ConfigManager* configManager = ConfigManager::getInstance();

        const char* path = getenv("PEGASUS_HOME");
        String pegHome = path;

        if(pegHome.size())
            ConfigManager::setPegasusHome(pegHome);

#ifdef DEBUG
        cout << "Peg Home : " << ConfigManager::getPegasusHome() << endl;
#endif

#ifdef DEBUG
        cout << "Doing testAuthHeader()...." << endl;
#endif

        // -- Create a test repository:

        String repositoryPath = "./repository";

        PEGASUS_ASSERT(FileSystem::isDirectory(repositoryPath));

        CIMRepository* repository = new CIMRepository(repositoryPath);

        // -- Create a UserManager object:

        UserManager* userManager = UserManager::getInstance(repository);

        testAuthHeader();

#ifdef DEBUG
        cout << "Doing testAuthenticationFailure_1()...." << endl;
#endif

        testAuthenticationFailure_1();

#ifdef DEBUG
        cout << "Doing testAuthenticationFailure_2()...." << endl;
#endif

        testAuthenticationFailure_2();

#ifdef DEBUG
        cout << "Doing testAuthenticationFailure_3()...." << endl;
#endif

        testAuthenticationFailure_3();

#ifdef DEBUG
        cout << "Doing testAuthenticationFailure_4()...." << endl;
#endif

        testAuthenticationFailure_4();

#ifdef DEBUG
        cout << "Doing testAuthenticationSuccess()...." << endl;
#endif

        testAuthenticationSuccess();
    }
    catch(Exception& e)
    {
        cout << "Exception: " << e.getMessage() << endl;
        PEGASUS_ASSERT(0);
    }

#endif

    cout << "+++++ passed all tests" << endl;

    return 0;
}
