//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Ben Heilbronn, Hewlett-Packard Company(ben_heilbronn@hp.com)
//
//         Parts of this code originated within PAMBasicAuthenticator.
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Cimservera_h
#define Pegasus_Cimservera_h

PEGASUS_NAMESPACE_BEGIN

/** This class provides PAM basic authentication implementation on its own
*/
class Cimservera
{
public:

    /** constructor. */ 
    Cimservera();

    /** destructor. */ 
    ~Cimservera();

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

        
    static Boolean authenticateByPAM(
        const String& userName,
        const String& password);


private:

};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Cimservera_h */
