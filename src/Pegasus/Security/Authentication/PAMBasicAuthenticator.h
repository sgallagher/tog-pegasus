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
// Modified By: Yi Zhou, Hewlett-Packard Company(yi_zhou@hp.com)
//              Sushma Fernandes, Hewlett-Packard Company
//                  (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_PAMBasicAuthenticator_h
#define Pegasus_PAMBasicAuthenticator_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/IPC.h>
#include "BasicAuthenticator.h"


#include <Pegasus/Security/Authentication/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

/** This class provides PAM basic authentication by communicating with a
    standalone process.
*/

#if defined(PEGASUS_USE_PAM_STANDALONE_PROC)

class PEGASUS_SECURITY_LINKAGE PAMBasicAuthenticatorStandAlone
{
public:

    /** constructor. */
    PAMBasicAuthenticatorStandAlone();

    /** destructor. */
    ~PAMBasicAuthenticatorStandAlone();

    /** Verify the authentication of the requesting user.
        @param userName String containing the user name
        @param password String containing the user password
        @return true on successful authentication, false otherwise
    */
    Boolean authenticate(
        const String& userName,
        const String& password);

    /** Verify PAM account management for the requesting user.
        @param userName String containing the user name
        @return true on successful authentication, false otherwise
    */
    Boolean validateUser(const String& userName);

private:
    String        _realm;

    // Indicates that an authentication operation must be performed.
    static const String OPERATION_PAM_AUTHENTICATION;

    // Indicates that an account management operation must be performed.
    static const String OPERATION_PAM_ACCT_MGMT;

    // Indicates that authentication/account management operation was successful.
    static const String PAM_OPERATION_SUCCESS;

#if defined(PEGASUS_HAS_SIGNALS)
    /**
        Process ID of the active Provider Agent.
     */
    pid_t _pid;
#endif

    Boolean _authenticateByPAM(
        const String& userName,
        const String& password);

    void _createPAMStandalone();

    //
    // Indicates the status of a write operation.
    //
    enum _Status 
    {
        BROKEN_CONNECTION, // If the conntection is broken
        OTHER_ERROR, // Any other error
        SUCCESS
    };

    //
    // Sends a text string to the Stand Alone PAM Process.
    // @param text String to be sent
    // @return the status of the write operation
    //         
    //
    _Status _writeString(const String& text);

    //
    // Read a response string from the Stand Alone Process. 
    // @return reply from the PAM process
    // 
    String _readString();

    //
    // Restarts PAM Stand Alone Process
    //
    void _restartProcess(void);
};

#endif /* if defined(PEGASUS_USE_PAM_STANDALONE_PROC) */

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

    /** Verify whether the user is valid.
        @param userName String containing the user name
        @return true on successful validation, false otherwise
    */
    Boolean validateUser( const String& userName);

    /** Construct and return the HTTP Basic authentication challenge header
        @return A string containing the authentication challenge header.
    */
    String getAuthResponseHeader();

    /** PAM Call back function, the pointer to this function is passed to the PAM module.
        @param num_msg int containing the message count
        @param msg pointer to a pam_message structure
        @param resp pointer to a pam_respone structure
        @param appdata_ptr application data pointer
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

   /**  PAM AcctMgmt Call back function, the pointer to this function
        is passed to the PAM module.

        @param num_msg int containing the message count
        @param msg pointer to a pam_message structure
        @param resp pointer to a pam_respone structure
        @param appdata_ptr application data pointer

        @return PAM_SUCCESS on successful execution, a PAM error code otherwise
    */
    static Sint32 pamValidateUserCallback(
        Sint32 num_msg,
#if defined (PEGASUS_OS_LINUX) 

        const struct pam_message **msg,
#else
        struct pam_message **msg,
#endif
        struct pam_response **resp,
        void *appdata_ptr);

private:
    /**
        A mutex to serialize authentication calls.
    */
    static Mutex  _authSerializeMutex; 

    String        _realm;

    Boolean _authenticateByPAM(
	const String& userName,
	const String& password);

#if defined(PEGASUS_USE_PAM_STANDALONE_PROC)
    PAMBasicAuthenticatorStandAlone _pamBasicAuthenticatorStandAlone;
#endif

};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_PAMBasicAuthenticator_h */
