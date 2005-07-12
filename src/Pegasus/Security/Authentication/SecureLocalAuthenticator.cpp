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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Tracer.h>

#include "LocalAuthFile.h"
#include "SecureLocalAuthenticator.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/**
    Constant representing the pegasus authentication challenge header.
*/
static const String PEGASUS_CHALLENGE_HEADER = "WWW-Authenticate: ";


/* constructor. */
SecureLocalAuthenticator::SecureLocalAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureLocalAuthenticator::SecureLocalAuthenticator()");

    PEG_METHOD_EXIT();

}

/* destructor. */
SecureLocalAuthenticator::~SecureLocalAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureLocalAuthenticator::~SecureLocalAuthenticator()");

    PEG_METHOD_EXIT();

}

//
// Does local authentication
//
Boolean SecureLocalAuthenticator::authenticate
(
   const String& filePath, 
   const String& secretReceived, 
   const String& secretKept
)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureLocalAuthenticator::authenticate()");

    Boolean authenticated = false;


    if ((!String::equal(secretReceived, String::EMPTY)) &&
        (!String::equal(secretKept, String::EMPTY)))
    {
        if (String::equal(secretKept, secretReceived))
        {
            authenticated = true;
        }
    }

    //
    // remove the auth file created for this user request
    //
    if (filePath.size())
    {
        if (FileSystem::exists(filePath))
        {
            FileSystem::removeFile(filePath);
        }
    }

    PEG_METHOD_EXIT();

    return (authenticated);
}


Boolean SecureLocalAuthenticator::validateUser (const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureLocalAuthenticator::validateUser()");

    Boolean authenticated = false;

    if (System::isSystemUser(userName.getCString()))
    {
        authenticated = true;
    }

    PEG_METHOD_EXIT();
    return (authenticated);
}

//
// Create authentication response header
//
String SecureLocalAuthenticator::getAuthResponseHeader(
    const String& authType, 
    const String& userName, 
    String& challenge)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "SecureLocalAuthenticator::getAuthResponseHeader()");

    String responseHeader = PEGASUS_CHALLENGE_HEADER;
    responseHeader.append(authType);
    responseHeader.append(" \"");

    //
    // create a file using user name and write a random number in it.
    //
    LocalAuthFile localAuthFile(userName);
    String filePath  = localAuthFile.create();

    //
    // get the challenge string
    //
    String temp = localAuthFile.getChallengeString();
    challenge = temp;

    // 
    // build response header with file path and challenge string.
    //
    responseHeader.append(filePath);
    responseHeader.append("\"");

    PEG_METHOD_EXIT();

    return (responseHeader);
}


PEGASUS_NAMESPACE_END
