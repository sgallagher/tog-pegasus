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
// Author: Nag Boranna, Hewlett-Packard Company (nagaraja.boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SSLContextManager_h
#define Pegasus_SSLContextManager_h

#ifdef PEGASUS_HAS_SSL
#define OPENSSL_NO_KRB5 1
#include <openssl/ssl.h>
#else
#define SSL_CTX void
#define X509_STORE void
#endif

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Linkage.h>


PEGASUS_NAMESPACE_BEGIN


/** 
    This class provides the functionality necessary to manage SSLContext objects.
 */
class PEGASUS_COMMON_LINKAGE SSLContextManager
{
public:
    enum SSLContextType
    {
        SERVER_CONTEXT = 1, EXPORT_CONTEXT = 2
    };

    /** Constructor. */

    SSLContextManager();

    ~SSLContextManager();

    void createSSLContext(
        Uint32 contextType,
        const String& trustStore, 
        const String& certPath,
        const String& keyPath, 
        const String& crlStore, 
        Boolean callback, 
        const String& randFile);

    /**
        Reload the trust store used by either the CIM Server or 
        Indication Server based on the context type.

        @param contextType specifies the SSLContext type for which
               the trust store is to be reloaded
     */
    void reloadTrustStore(Uint32 contextType);

    /**
        Reload the CRL store.
     */
    void reloadCRLStore();

    /**
        Get a pointer to the sslContext object.
     */
    SSLContext*  getSSLContext(Uint32 contextType) const;

    /**
        Get a pointer to the sslContextObjectLock.
     */
    ReadWriteSem*  getSSLContextObjectLock();

private:
    /**
        Create a new store object and load the specified store and return
        a pointer to the new store object.
    */
    X509_STORE* _getNewX509Store(const String& store);

    /**
        A lock to control access to the _sslContext and _exportSSLContext 
        objects.  Before read accessing the _sslContext and _exportSSLContext 
        objects, one must first lock this for read access.  Before write 
        accessing the _sslContext and _exportSSLContext objects, one must 
        first lock this for write access.
     */
    ReadWriteSem _sslContextObjectLock;
    SSLContext* _sslContext; 
    SSLContext* _exportSSLContext;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContextManager_h */
