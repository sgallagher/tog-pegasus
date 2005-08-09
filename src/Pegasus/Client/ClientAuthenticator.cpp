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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP-101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Base64.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Constants.h>
#include "ClientAuthenticator.h"

#include <ctype.h>

//
// Constants used to parse the authentication challenge header
//
#define CHAR_BLANK     ' '

#define CHAR_QUOTE     '"'


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
    clear();
}

ClientAuthenticator::~ClientAuthenticator()
{

}

void ClientAuthenticator::clear()
{
    _requestMessage.reset();
    _userName = String::EMPTY;
    _password = String::EMPTY;
    _realm = String::EMPTY;
    _challengeReceived = false;
    _authType = ClientAuthenticator::NONE;
}

Boolean ClientAuthenticator::checkResponseHeaderForChallenge(
    Array<HTTPHeader> headers)
{
    //
    // Search for "WWW-Authenticate" header:
    //
    String authHeader;
    String authType;
    String authRealm;

    if (!HTTPMessage::lookupHeader(
        headers, WWW_AUTHENTICATE, authHeader, false))
    {
        return false;
    }

    if (_challengeReceived)
    {
        // Do not respond to a challenge more than once
        return false;
    }
    else
    {
       _challengeReceived = true;

       //
       // Parse the authentication challenge header
       //
       if(!_parseAuthHeader(authHeader, authType, authRealm))
       {
           throw InvalidAuthHeader();
       }

       if ( String::equal(authType, "LocalPrivileged"))
       {
           _authType = ClientAuthenticator::LOCALPRIVILEGED;
       }
       else if ( String::equal(authType, "Local"))
       {
           _authType = ClientAuthenticator::LOCAL;
       }
       else if ( String::equal(authType, "Basic"))
       {
           _authType = ClientAuthenticator::BASIC;
       }
       else if ( String::equal(authType, "Digest"))
       {
           _authType = ClientAuthenticator::DIGEST;
       }
       else
       {
           throw InvalidAuthHeader();
       }

       if ( _authType == ClientAuthenticator::LOCAL ||
           _authType == ClientAuthenticator::LOCALPRIVILEGED )
       {
           String filePath = authRealm;
           FileSystem::translateSlashes(filePath);

           // Check whether the directory is a valid pre-defined directory.
           //
           Uint32 index = filePath.reverseFind('/');

           if (index != PEG_NOT_FOUND)
           {
               String dirName = filePath.subString(0,index);

               if (!String::equal(dirName, String(PEGASUS_LOCAL_AUTH_DIR)))
               {
                   // Refuse to respond to the challenge when the file is
                   // not in the expected directory
                   return false;
               }
           }
       }

       _realm = authRealm;

       return true;
   }
}


String ClientAuthenticator::buildRequestAuthHeader()
{
    String challengeResponse = String::EMPTY;

    switch (_authType)
    {
        case ClientAuthenticator::BASIC:

            if (_challengeReceived)
            {
                challengeResponse = BASIC_AUTH_HEADER;

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
                Array<char> userPassArray;

                Uint32 userPassLength = userPass.size();

                userPassArray.reserveCapacity( userPassLength );
                userPassArray.clear();

                for( Uint32 i = 0; i < userPassLength; i++ )
                {
                    userPassArray.append( (char)userPass[i] );
                }

                //
                // base64 encode the user name and password
                //
                Array<char> encodedArray;

                encodedArray = Base64::encode( userPassArray );

                challengeResponse.append(
                    String( encodedArray.getData(), encodedArray.size() ) );
            }
            break;

        //
        //ATTN: Implement Digest Auth challenge handling code here
        //
        case ClientAuthenticator::DIGEST:
        //    if (_challengeReceived)
        //    {
        //        challengeResponse = DIGEST_AUTH_HEADER;
        //
        //    }
            break;

        case ClientAuthenticator::LOCALPRIVILEGED:

            challengeResponse = LOCALPRIVILEGED_AUTH_HEADER;
            challengeResponse.append(" \"");

            if (_userName.size())
            {
                 challengeResponse.append(_userName);
            }
            else
            {
                //
                // Get the privileged user name on the system
                //
                challengeResponse.append(System::getPrivilegedUserName());
            }

            challengeResponse.append(_buildLocalAuthResponse());

            break;

        case ClientAuthenticator::LOCAL:

            challengeResponse = LOCAL_AUTH_HEADER;
            challengeResponse.append(" \"");

            if (_userName.size())
            {
                 challengeResponse.append(_userName);
            }
            else
            {
                //
                // Get the current login user name
                //
                challengeResponse.append(System::getEffectiveUserName());
            }

            challengeResponse.append(_buildLocalAuthResponse());

            break;

        case ClientAuthenticator::NONE:
            //
            // Gets here only when no authType was set.
            //
            challengeResponse.clear();
            break;

        default:
            PEGASUS_ASSERT(0);
            break;
    }

    return (challengeResponse);
}

void ClientAuthenticator::setRequestMessage(Message* message)
{
    _requestMessage.reset(message);
}

Message* ClientAuthenticator::getRequestMessage()
{
    return _requestMessage.get();
}

Message* ClientAuthenticator::releaseRequestMessage()
{
    return _requestMessage.release();
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
    PEGASUS_ASSERT( (type == ClientAuthenticator::BASIC) ||
         (type == ClientAuthenticator::DIGEST) ||
         (type == ClientAuthenticator::LOCAL) ||
         (type == ClientAuthenticator::LOCALPRIVILEGED) ||
         (type == ClientAuthenticator::NONE) );

    _authType = type;
}

ClientAuthenticator::AuthType ClientAuthenticator::getAuthType()
{
    return (_authType);
}

String ClientAuthenticator::_getFileContent(String filePath)
{
    String challenge = String::EMPTY;

    FileSystem::translateSlashes(filePath);

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
#if defined(PEGASUS_OS_OS400)
    ifstream ifs(filePath.getCString(), PEGASUS_STD(_CCSID_T(1208)) );
#else
    ifstream ifs(filePath.getCString());
#endif
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

String ClientAuthenticator::_buildLocalAuthResponse()
{
    String authResponse = String::EMPTY;

    if (_challengeReceived)
    {
        authResponse.append(":");

        //
        // Append the file path that is in the realm sent by the server
        //
        authResponse.append(_realm);

        authResponse.append(":");

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
            //ATTN-NB-04-20000305: Log error message to log file
        }
        authResponse.append(fileContent);
    }
    authResponse.append("\"");

    return (authResponse);
}

Boolean ClientAuthenticator::_parseAuthHeader(
    const String authHeader,
    String& authType,
    String& authRealm)
{
    CString header = authHeader.getCString();
    const char* pAuthHeader = header;

    //
    // Skip the white spaces in the begining of the header
    //
    while (*pAuthHeader && isspace(*pAuthHeader))
    {
        *pAuthHeader++;
    }

    //
    // Get the authentication type
    //
    String type = _getSubStringUptoMarker(&pAuthHeader, CHAR_BLANK);

    if (!type.size())
    {
        return false;
    }

    //
    // Ignore the start quote
    //
    _getSubStringUptoMarker(&pAuthHeader, CHAR_QUOTE);


    //
    // Get the realm ending with a quote
    //
    String realm = _getSubStringUptoMarker(&pAuthHeader, CHAR_QUOTE);

    if (!realm.size())
    {
        return false;
    }

    authType = type;

    authRealm = realm;

    return true;
}


String ClientAuthenticator::_getSubStringUptoMarker(
    const char** line,
    char marker)
{
    String result = String::EMPTY;

    //
    // Look for the marker
    //
    const char *pos = strchr(*line, marker);

    if (pos)
    {
        if (*line != NULL)
        {
            Uint32 length = pos - *line;

            result.assign(*line, length);
        }

        while (*pos == marker)
        {
            ++pos;
        }

        *line = pos;
    }
    else
    {
        result.assign(*line);

        *line += strlen(*line);
    }

    return result;
}

PEGASUS_NAMESPACE_END
