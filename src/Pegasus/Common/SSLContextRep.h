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
// Author: Nag Boranna, Hewlett-Packard Company ( nagaraja_boranna@hp.com )
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_HAS_SSL
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#else
#define SSL_CTX void
#endif
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Linkage.h>

#ifndef Pegasus_SSLContextRep_h
#define Pegasus_SSLContextRep_h


PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMMON_LINKAGE SSLContextRep
{
public:

    /** Constructor for a SSLContextRep object.
    @param certPath  certificate file path
    @param certKeyPath  server certificate and key file path
    @param verifyCert  function pointer to a certificate verification
    call back function.
    @param randomFile  file path of a random file that is used as a seed
    for random number generation by OpenSSL.

    @exception SSLException  exception indicating failure to create a context.
    */
    SSLContextRep(
        const String& certPath,
        const String& certKeyPath = String::EMPTY,
        SSLCertificateVerifyFunction* verifyCert = NULL,
        const String& randomFile = String::EMPTY);

    SSLContextRep(const SSLContextRep& sslContextRep);

    ~SSLContextRep();

    SSL_CTX * getContext() const;

private:

    SSL_CTX * _makeSSLContext();
    void _randomInit(const String& randomFile);
    Boolean _verifyPrivateKey(SSL_CTX *ctx, const char *keyFilePath);

    CString _certPath;
    CString _certKeyPath;
    String _randomFile;
    SSL_CTX * _sslContext;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContextRep_h */

