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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_PAMBasicAuthenticator_h
#define Pegasus_PAMBasicAuthenticator_h

#include <Pegasus/Common/Config.h>
#include "BasicAuthenticator.h"

#include <Pegasus/Security/Authentication/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

/** This class provides PAM basic authentication implementation by extending
    the BasicAuthenticator.
*/
class PEGASUS_SECURITY_LINKAGE PAMBasicAuthenticator : public BasicAuthenticator
{
public:

    /** constructor. */ 
    PAMBasicAuthenticator();

    /** destructor. */ 
    ~PAMBasicAuthenticator();

    /** Verify the authentication of the requesting user.
        @param userName String containing the user name
        @param password String containing the user password
        @return true on successful authentication, false otherwise
    */
    Boolean authenticate(
        const String& userName, 
        const String& password);

    /** Construct and return the HTTP Basic authentication challenge header
        @return A string containing the authentication challenge header.
    */
    String getAuthResponseHeader();

    /** PAM Call back function, the pointer to this function is passed to the PAM module.
        @param num_msg int containing the message count
        @param msg pointer to a pam_message structure
        @param resp pointer to a pam_respone structure
        @param appdata_prt application data pointer
        @return PAM_SUCCESS on successful execution, a PAM error code otherwise
    */
    static Sint32 PAMCallback(
        Sint32 num_msg, 
#if defined (PEGASUS_OS_LINUX) && defined(PEGASUS_PAM_AUTHENTICATION)

        const struct pam_message **msg,
#else
        struct pam_message **msg,
#endif
        struct pam_response **resp, 
        void *appdata_ptr);

private:

    String        _realm;

    Boolean _authenticateByPAM(
	const String& userName,
	const String& password);

#if defined (PEGASUS_OS_HPUX)
    Boolean	_usePAM;

    Boolean _authenticateByPwnam(
	const char* userName,
	const String& password);
#endif
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_PAMBasicAuthenticator_h */
