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
#include <Pegasus/Common/Base64.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Security/Authentication/LocalAuthenticationHandler.h>

//
// Enable debug messages
//
//#define DEBUG 1


PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

String authType = "Local";

String testUser = System::getEffectiveUserName();

String challenge = String::EMPTY;

String filePath = String::EMPTY;

AuthenticationInfo* authInfo = 0;

void testAuthHeader()
{
    LocalAuthenticationHandler  localAuthHandler;

    String respHeader = 
        localAuthHandler.getAuthResponseHeader(authType, testUser, authInfo);

#ifdef DEBUG
    cout << "respHeader= " << respHeader << endl;
#endif
    
    challenge = authInfo->getAuthChallenge();

    PEGASUS_ASSERT(respHeader.size() != 0);

    Uint32 startQuote = respHeader.find(0, '"');
    assert(startQuote != PEG_NOT_FOUND);

    Uint32 endQuote = respHeader.find(startQuote + 1, '"');
    assert(startQuote != PEG_NOT_FOUND);

    filePath = respHeader.subString(startQuote + 1, (endQuote - startQuote - 1));

    PEGASUS_ASSERT(filePath.size() != 0);
}

//
// Test with invalid userPass
//
void testAuthenticationFailure_1()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    LocalAuthenticationHandler  localAuthHandler;

    //
    // Test with invalid auth header
    //
    authHeader = testUser;
    authHeader.append(filePath);
    authHeader.append(challenge);

    authenticated = localAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    cout << "authHeader: " << authHeader << endl;

    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed.." << endl;
#endif

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid system user
//
void testAuthenticationFailure_2()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    LocalAuthenticationHandler  localAuthHandler;

    //
    // Test with invalid auth header
    //
    authHeader = testUser;
    authHeader.append(filePath);

    authenticated = localAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    cout << "authHeader: " << authHeader << endl;

    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed.." << endl;
#endif

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid password 
//
void testAuthenticationFailure_3()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    LocalAuthenticationHandler  localAuthHandler;

    authHeader = testUser;
    authHeader.append(":");
    authHeader.append(filePath);
    authHeader.append(":");

    authenticated = localAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    cout << "authHeader: " << authHeader << endl;

    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed.." << endl;
#endif

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with invalid CIM user or invalid password
//
void testAuthenticationFailure_4()
{
    String authHeader = String::EMPTY;
    Boolean authenticated;

    LocalAuthenticationHandler  localAuthHandler;

    authHeader = testUser;
    authHeader.append(":");
    authHeader.append(filePath);
    authHeader.append(":");
    authHeader.append("asd442394asd");

    authenticated = localAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    cout << "authHeader: " << authHeader << endl;

    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed.." << endl;
#endif

    PEGASUS_ASSERT(!authenticated);
}

//
// Test with valid user name and password 
//
void testAuthenticationSuccess()
{
    String authHeader = String::EMPTY;

    LocalAuthenticationHandler  localAuthHandler;

    authHeader = testUser;
    authHeader.append(":");
    authHeader.append(filePath);
    authHeader.append(":");
    authHeader.append(challenge);

    Boolean authenticated =
        localAuthHandler.authenticate(authHeader, authInfo);

#ifdef DEBUG
    cout << "authHeader: " << authHeader << endl;

    if (authenticated)
        cout << "User " + testUser + " authenticated successfully." << endl;
    else
        cout << "User " + testUser + " authentication failed.." << endl;
#endif

    PEGASUS_ASSERT(authenticated);
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

        authInfo = new AuthenticationInfo(true);

#ifdef DEBUG
        cout << "Doing testAuthHeader()...." << endl;
#endif

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

    delete authInfo;
#endif

    cout << "+++++ passed all tests" << endl;

    return 0;
}
