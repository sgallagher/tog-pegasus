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
// Modified By: Sushma Fernandes, Hewlett-Packard Company
//                  sushma_fernandes@hp.com
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
#include <Pegasus/Common/IPC.h>

#ifndef Pegasus_SSLContextRep_h
#define Pegasus_SSLContextRep_h


PEGASUS_NAMESPACE_BEGIN


class PEGASUS_COMMON_LINKAGE SSLContextRep
{
    /*
    SSL locking callback function. It is needed to perform locking on 
    shared data structures.

    This function needs access to variable ssl_locks.
    Declare it as a friend of class SSLContextRep.

    @param mode 	Specifies whether to lock/unlock.
    @param type	Type of lock.
    @param file      File name of the function setting the lock.
    @param line      Line number of the function setting the lock.
    */
    friend void pegasus_locking_callback(
                      int       mode,
                      int       type,
                      const     char* file,
                      int       line);

public:

    /** Constructor for a SSLContextRep object.
    @param trustPath  trust store file path
    @param certPath  server certificate file path
    @param keyPath  server key file path
    @param verifyCert  function pointer to a certificate verification
    call back function.
    @param randomFile  file path of a random file that is used as a seed
    for random number generation by OpenSSL.

    @exception SSLException  exception indicating failure to create a context.
    */
    SSLContextRep(
        const String& trustPath,
        const String& certPath = String::EMPTY,
        const String& keyPath = String::EMPTY,
        SSLCertificateVerifyFunction* verifyCert = NULL,
        const String& randomFile = String::EMPTY);

    SSLContextRep(const SSLContextRep& sslContextRep);

    ~SSLContextRep();

    SSL_CTX * getContext() const;

    /*
    Initialize the SSL locking environment. 
         
    This function sets the locking callback functions.
    */
    static void init_ssl();

    /*
    Cleanup the SSL locking environment.
    */
    static void free_ssl();

private:

    SSL_CTX * _makeSSLContext();
    void _randomInit(const String& randomFile);
    Boolean _verifyPrivateKey(SSL_CTX *ctx, const char *keyFilePath);

    CString _trustPath;
    CString _certPath;
    CString _keyPath;
    String _randomFile;
    SSL_CTX * _sslContext;

    /*
       Mutex containing the SSL locks.
    */
    static Mutex* _sslLocks;

    /*
       Count for instances of this class. This is used to initialize and free
       SSL locking objects.
    */
    static int _countRep;

    /*
       Mutex for countRep.
    */
    static Mutex _countRepMutex;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContextRep_h */

