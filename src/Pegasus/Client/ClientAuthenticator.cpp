//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Destroyer.h>
#include "ClientAuthenticator.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

ClientAuthenticator::ClientAuthenticator()
{
    //authType is Local for now
    _authType = ClientAuthenticator::LOCAL;
    _userName = String::EMPTY;
    _password = String::EMPTY;

    clearRequest();
}

ClientAuthenticator::~ClientAuthenticator()
{

}

void ClientAuthenticator::clearRequest()
{
    _requestMessage = 0;
    _challengeReceived = false;
    _challengeResponse = String::EMPTY;
}

Boolean ClientAuthenticator::checkResponseHeaderForChallenge(Array<HTTPHeader> headers)
{
    //
    // Search for "WWW-Authenticate" header:
    //

    String authHeader;

    if (!HTTPMessage::lookupHeader(
        headers, "WWW-Authenticate", authHeader, false))
    {
        //
        // Challenge information not found in the reponse header
        //

        return false;
    }

    _challengeReceived = true;

    String filePath   = String::EMPTY;

    Uint32 pos;

    if ( (pos = authHeader.find("LocalPrivileged")) == PEG_NOT_FOUND )
    {
        if ( (pos = authHeader.find("Local")) == PEG_NOT_FOUND )
        {
            //
            //Invalid authorization header
            //
            //ATTN: throw exception
            return false;
        }
    }

    Uint32 startQuote = authHeader.find(pos, '"');
    if (startQuote == PEG_NOT_FOUND)
    {
        //
        //Invalid authorization header
        //
        //ATTN: throw exception
        return false;
    }

    Uint32 endQuote = authHeader.find(startQuote + 1, '"');
    if (endQuote == PEG_NOT_FOUND)
    {
        //
        //Invalid authorization header
        //
        //ATTN: throw exception
        return false;
    }

    filePath = authHeader.subString(startQuote + 1, (endQuote - startQuote - 1));

    //
    // Read the challenge file content and build the challenge response.
    //
    _challengeResponse.assign(filePath);

    _challengeResponse.append(":");

    _challengeResponse.append(_getFileContent(filePath));

    return true;
}


String ClientAuthenticator::buildRequestAuthHeader()
{
    String authHeader = String::EMPTY;

    switch (_authType)
    {
        //ATTN: Implement Basic and Digest
        //
        //case BASIC:
        //    authHeader = "Authorization: Basic";
        //    if (_challengeReceived) 
        //    {
        //    
        //    }
        //    break;
        //case DIGEST:
        //    authHeader = "Authorization: Digest";
        //    if (_challengeReceived) 
        //    {
        //    
        //    }
        //    break;

        case ClientAuthenticator::LOCALPRIVILEGED:
            authHeader = "PegasusAuthorization: LocalPrivileged";

            authHeader.append(" \"");
            if (_userName.size()) 
            {
                authHeader.append(_userName);
            }
            else
            {
                //ATTN: Get this from a common place.
                authHeader.append("root");
            }

            if (_challengeReceived) 
            {
                authHeader.append(":");
                authHeader.append(_challengeResponse);
            }
            authHeader.append("\"");
            break;

        case ClientAuthenticator::LOCAL:
            authHeader = "PegasusAuthorization: Local";

            authHeader.append(" \"");

            if (_userName.size()) 
            {
                authHeader.append(_userName);
            }
            else
            {
                //
                // Get the current login user name
                //
                authHeader.append(System::getCurrentLoginName());
            }

            if (_challengeReceived) 
            {
                authHeader.append(":");
                authHeader.append(_challengeResponse);
            }
            authHeader.append("\"");
            break;

        default:
            break;
    }

    //
    // Reset the flag
    //
    if (_challengeReceived) 
    {
        _challengeReceived = false;
    }

    return (authHeader);

}

void ClientAuthenticator::setRequestMessage(Message* message)
{
    _requestMessage = message;
}


Message* ClientAuthenticator::getRequestMessage()
{
   return _requestMessage;
   
}

void ClientAuthenticator::setUserName(const String& userName)
{
    _userName = userName;
}

String ClientAuthenticator::getUserName()
{
    return (_userName);
}

void ClientAuthenticator::setPassword(const String& password)
{
    _password = password;
}

String ClientAuthenticator::getPassword()
{
    return (_password);
}

void ClientAuthenticator::setAuthType(ClientAuthenticator::AuthType type)
{
    //ATTN: Validate the type before accepting.
    
    _authType = type;
}

ClientAuthenticator::AuthType ClientAuthenticator::getAuthType()
{
    return (_authType);
}

String ClientAuthenticator::_getFileContent(String filePath)
{
    String challenge = String::EMPTY;

    //
    //ATTN: Check whether the file exists or not
    //

    //
    // Open the challenge file and read the challenge data
    //
    ArrayDestroyer<char> p(filePath.allocateCString());
    ifstream ifs(p.getPointer());
    if (!ifs)
    {
       //ATTN: Log error message 
        return (challenge);
    }

    String line;

    while (GetLine(ifs, line))
    {
        challenge.append(line);
    }

    ifs.close();

    return (challenge);
}


PEGASUS_NAMESPACE_END
