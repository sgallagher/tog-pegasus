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
// Modified By: Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//            Sushma Fernandes, Hewlett-Packard Company(sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Config/ConfigManager.h>

#include "PAMBasicAuthenticator.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


/**
    Constant representing the Basic authentication challenge header.
*/
static const String BASIC_CHALLENGE_HEADER = "WWW-Authenticate: Basic \"";


/* constructor. */
PAMBasicAuthenticator::PAMBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMBasicAuthenticator()");

    //
    // get the local system name
    //
    _realm.assign(System::getHostName());

    //
    // get the configured port number
    //
    ConfigManager* configManager = ConfigManager::getInstance();

    String port = configManager->getCurrentValue("httpPort");

    //
    // Create realm that will be used for Basic challenges
    //
    _realm.append(":");
    _realm.append(port);

    PEG_METHOD_EXIT();
}

/* destructor. */
PAMBasicAuthenticator::~PAMBasicAuthenticator() 
{ 
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::~PAMBasicAuthenticator()");

    PEG_METHOD_EXIT();
}

Boolean PAMBasicAuthenticator::authenticate(
    const String& userName, 
    const String& password) 
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::authenticate()");

    //FUTURE: PAM authentication is not implemented on non Unix platforms,
    //hence this function return false as default indicating authentication failure.

    Boolean authenticated = false;

    PEG_METHOD_EXIT();

    return (authenticated);
}

Boolean PAMBasicAuthenticator::validateUser(
    const String& userName)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::validateUser()");

    //FUTURE: PAM authentication is not implemented on non Unix platforms,
    //hence this function return false as default indicating validation failure.

    Boolean authenticated = false;

    PEG_METHOD_EXIT();

    return (authenticated);
}

//
// Create authentication response header
//
String PAMBasicAuthenticator::getAuthResponseHeader()
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::getAuthResponseHeader()");

    // 
    // build response header using realm
    //
    String responseHeader = BASIC_CHALLENGE_HEADER;
    responseHeader.append(_realm);
    responseHeader.append("\"");

    PEG_METHOD_EXIT();

    return (responseHeader);
}

//
// Call back function, the pointer to this function is passed to the PAM module.
//
Sint32 PAMBasicAuthenticator::PAMCallback(Sint32 num_msg, struct pam_message **msg,
        struct pam_response **resp, void *appdata_ptr)
{
    PEG_METHOD_ENTER(TRC_AUTHENTICATION,
        "PAMBasicAuthenticator::PAMCallback()");

    //FUTURE: No implementation on non Unix platforms

    PEG_METHOD_EXIT();

    return 0;
}

PEGASUS_NAMESPACE_END
