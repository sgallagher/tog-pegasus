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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Security/Authentication/LocalAuthFile.h>
#include "SecureLocalAuthenticator.h"

PEGASUS_NAMESPACE_BEGIN


/* constructor. */
SecureLocalAuthenticator::SecureLocalAuthenticator() 
{ 

}

/* destructor. */
SecureLocalAuthenticator::~SecureLocalAuthenticator() 
{ 

}

Boolean SecureLocalAuthenticator::authenticate(String userName, String password) 
{
    // not supported for local authentication, so return false.
    return (false);
}

//
// Does local authentication
//
Boolean SecureLocalAuthenticator::authenticate
(
   String userName, 
   String secretReceived, 
   String secretKept
)
{
    Boolean authenticated = false;

    if ((!String::equal(secretReceived, String::EMPTY)) &&
        (!String::equal(secretKept, String::EMPTY)))
    {
        if (String::equal(secretKept, secretReceived))
        {
            authenticated = true;
        }
    }

    // ATTN: Delete the authentication secret file
    //if (!localAuthFile.deleteFile())
    //{
    //}

    return (authenticated);
}

//
// Create authentication response header
//
String SecureLocalAuthenticator::getAuthResponseHeader(
    String userName, 
    String& challenge)
{
    String responseHeader = String::EMPTY;

    //
    // create a file using user name and write a random number in it.
    //
    LocalAuthFile localAuthFile(userName);
    String filePath  = localAuthFile.create();

    //
    // get the challenge string
    //
    challenge = localAuthFile.getChallengeString();

    // 
    // build response header with file path and challenge string.
    //
    responseHeader.assign(filePath);

    return (responseHeader);
}


PEGASUS_NAMESPACE_END
