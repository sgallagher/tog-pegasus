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
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/AuthenticationInfo.h>
#include <Pegasus/Security/Authentication/LocalAuthenticationHandler.h>

// Uncomment this if you want detailed messages to be printed.
//#define VERBOSE 1


PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;


//
//
//
int main()
{
    String testUser = String::EMPTY;
    String authHeader = String::EMPTY;
    String authType = "Local";
    Boolean authenticated;

    testUser.assign(System::getCurrentLoginName());

//---------------------- Test Case 1 ----------------------------------------
    LocalAuthenticationHandler  localAuthHandler;

    AuthenticationInfo  authInfo;

    String respHeader = 
        localAuthHandler.getAuthResponseHeader(authType, testUser, &authInfo);

    Uint32 startQuote = respHeader.find(0, '"');
    assert(startQuote != PEG_NOT_FOUND);

    Uint32 endQuote = respHeader.find(startQuote + 1, '"');
    assert(startQuote != PEG_NOT_FOUND);

//---------------------------------------------------------------------------


//---------------------- Test Case 2 ----------------------------------------
    authHeader.assign(testUser);

    authenticated = localAuthHandler.authenticate(authHeader, &authInfo);
#ifdef VERBOSE
    if (authenticated)
        cout << "Authenticated" << endl;
    else
        cout << "Not authenticated" << endl;
#endif
    assert(!authenticated);
//---------------------------------------------------------------------------


//---------------------- Test Case 3 ----------------------------------------
    String filePath = respHeader.subString(startQuote + 1, (endQuote - startQuote - 1));

    authHeader.append(":");
    authHeader.append(filePath);

    authenticated = localAuthHandler.authenticate(authHeader, &authInfo);
#ifdef VERBOSE
    if (authenticated)
        cout << "Authenticated" << endl;
    else
        cout << "Not authenticated" << endl;
#endif
    assert(!authenticated);
//---------------------------------------------------------------------------


//---------------------- Test Case 4 ----------------------------------------
    authHeader.append(":");
    authHeader.append(authInfo.getAuthChallenge());

    authenticated = localAuthHandler.authenticate(authHeader, &authInfo);
#ifdef VERBOSE
    if (authenticated)
        cout << "Authenticated" << endl;
    else
        cout << "Not authenticated" << endl;
#endif
    assert(authenticated);
//---------------------------------------------------------------------------


    cout << "+++++ passed all tests" << endl;

    return 0;
}
