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
#include <Pegasus/Common/Base64.h>
#include "ClientAuthenticator.h"

//
// Privileged user that is used as default user for Local Privileged 
// authentication when the client does not specify the user name.
//
#if defined(PEGASUS_OS_TYPE_WINDOWS)
#define DEFAULT_PRIVILEGED_USER     "Administrator"
#else
#define DEFAULT_PRIVILEGED_USER     "root"
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

/**
    The constant represeting the authentication challenge header.
*/
static const String WWW_AUTHENTICATE            = "WWW-Authenticate";

/**
    Constant representing the Basic authentication header.
*/
static const String BASIC_AUTH_HEADER           = "Authorization: Basic ";

/**
    Constant representing the Digest authentication header.
*/
static const String DIGEST_AUTH_HEADER          = "Authorization: Digest ";

/**
    Constant representing the local authentication header.
*/
static const String LOCAL_AUTH_HEADER           = 
                             "PegasusAuthorization: Local";

/**
    Constant representing the local privileged authentication header.
*/
static const String LOCALPRIVILEGED_AUTH_HEADER = 
                             "PegasusAuthorization: LocalPrivileged";



ClientAuthenticator::ClientAuthenticator()
{
    //should the default authType be Local?
    //_authType = ClientAuthenticator::LOCAL;

    clearRequest(true);
}

ClientAuthenticator::~ClientAuthenticator()
{

}

void ClientAuthenticator::clearRequest(Boolean closeConnection)
{
    _requestMessage = 0;
    _challengeReceived = false;
 
    if (closeConnection)
    {
        _challengeResponse = String::EMPTY;
        _userName = String::EMPTY;
        _password = String::EMPTY;
        _realm = String::EMPTY;
    }
}

Boolean ClientAuthenticator::checkResponseHeaderForChallenge(
    Array<HTTPHeader> headers)
{
    //
    // Search for "WWW-Authenticate" header:
    //
    String authHeader;

    if (!HTTPMessage::lookupHeader(
        headers, WWW_AUTHENTICATE, authHeader, false))
    {
        _challengeReceived = false;
        return false;
    }

    _challengeReceived = true;

    Uint32 pos;

    //
    // Find the authentication type in the challenge
    //
    if ( (pos = authHeader.find("LocalPrivileged")) != PEG_NOT_FOUND )
    {
        _authType = ClientAuthenticator::LOCALPRIVILEGED;

        _challengeResponse = LOCALPRIVILEGED_AUTH_HEADER;
        _challengeResponse.append(" \"");

        if (_userName.size()) 
        {
             _challengeResponse.append(_userName);
        }
        else
        {
            //
            // Get the current login user name
            //
            _challengeResponse.append(DEFAULT_PRIVILEGED_USER);
        }
    }
    else if ( (pos = authHeader.find("Local")) != PEG_NOT_FOUND )
    {
        _authType = ClientAuthenticator::LOCAL;

        _challengeResponse = LOCAL_AUTH_HEADER;
        _challengeResponse.append(" \"");

        if (_userName.size()) 
        {
             _challengeResponse.append(_userName);
        }
        else
        {
            //
            // Get the current login user name
            //
            _challengeResponse.append(System::getCurrentLoginName());
        }
    }
    else if ( (pos = authHeader.find("Basic")) != PEG_NOT_FOUND )
    {
        _authType = ClientAuthenticator::BASIC;

        _challengeResponse = BASIC_AUTH_HEADER;
    }
    else if ( (pos = authHeader.find("Digest")) != PEG_NOT_FOUND )
    {
        _authType = ClientAuthenticator::DIGEST;

        _challengeResponse = DIGEST_AUTH_HEADER;
    }
    else
    {
        throw InvalidAuthHeader();
    }

    //
    // parse the authentication challenge header
    //
    Uint32 startQuote = authHeader.find(pos, '"');
    if (startQuote == PEG_NOT_FOUND)
    {
        throw InvalidAuthHeader();
    }

    Uint32 endQuote = authHeader.find(startQuote + 1, '"');
    if (endQuote == PEG_NOT_FOUND)
    {
        throw InvalidAuthHeader();
    }

    _realm = authHeader.subString(
        startQuote + 1, (endQuote - startQuote - 1));

    return true;
}


String ClientAuthenticator::buildRequestAuthHeader()
{
    switch (_authType)
    {
        case ClientAuthenticator::BASIC:

            if (_challengeReceived)
            {
                //
                // build the credentials string using the
                // user name and password
                //
                String userPass =  _userName;

                userPass.append(":");

                userPass.append(_password);

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

                _challengeResponse.append(
                    String( encodedArray.getData(), encodedArray.size() ) );
            }
            break;

        //    
        //ATTN: Implement Digest Auth challenge handling code here
        //    
        //case ClientAuthenticator::DIGEST:
        //    if (_challengeReceived) 
        //    {
        //    
        //    }
        //    break;

        case ClientAuthenticator::LOCALPRIVILEGED:
        case ClientAuthenticator::LOCAL:

            if (_challengeReceived) 
            {
                _challengeResponse.append(":");

                //
                // Append the file path that is in the realm sent by the server
                //
                _challengeResponse.append(_realm);

                _challengeResponse.append(":");

                //
                // Read and append the challenge file content 
                //
                String fileContent = String::EMPTY;
                try
                {
                    fileContent = _getFileContent(_realm);
                }
                catch(NoSuchFile& e)
                {
                    //ATTN: Log error message to log file
                }
                _challengeResponse.append(fileContent);
            }
            _challengeResponse.append("\"");

            break;

        default:
            // 
            // Gets here only when no authType was set.
            // 
            _challengeResponse.clear();
            break;
    }

    //
    // Reset the flag
    //
    if (_challengeReceived)
    {
        _challengeReceived = false;
    }

    return (_challengeResponse);
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

void ClientAuthenticator::setAuthType(ClientAuthenticator::AuthType type)
{
    if ( (type != ClientAuthenticator::BASIC) ||
         (type != ClientAuthenticator::DIGEST) ||
         (type != ClientAuthenticator::LOCAL) ||
         (type != ClientAuthenticator::LOCALPRIVILEGED) )
    {
        throw InvalidAuthType();
    } 
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
    // Check whether the file exists or not
    //
    if (!FileSystem::exists(filePath))
    {
        throw NoSuchFile(filePath);
    }

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
