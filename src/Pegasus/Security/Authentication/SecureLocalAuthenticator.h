//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SecureLocalAuthenticator_h
#define Pegasus_SecureLocalAuthenticator_h

#include "LocalAuthenticator.h"
#include "Linkage.h"


PEGASUS_NAMESPACE_BEGIN

/** This class implements file based secure local authentication mechanism. 
    It extends the LocalAuthenticator and provides implementation.
*/   
class PEGASUS_SECURITY_LINKAGE SecureLocalAuthenticator : public LocalAuthenticator
{
public:

    /** constructor. */ 
    SecureLocalAuthenticator();

    /** destructor. */ 
    ~SecureLocalAuthenticator();

    /** Verify the authentication of the requesting user.
        @param userName String containing the user name
        @param secretReceived String containing the authentication secret
        sent by the client.
        @param secretKept String containing the authentication secret that
        was sent to client as part of the challenge.
        @return true on successful authentication, false otherwise
    */
    Boolean authenticate(
        const String& userName, 
        const String& secretReceived, 
        const String& secretKept);

    /** Construct and return the Peagaus Local authentication challenge header
        @param authType String containing the authentication type
        @param userName String containing the user name
        @param challenge String to store the authentication challenge secret
        @return A string containing the authentication challenge header.
    */
    String getAuthResponseHeader(
        const String& authType, 
        const String& userName, 
        String& challenge);
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_SecureLocalAuthenticator_h */
