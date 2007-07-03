//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SSLContextManager_h
#define Pegasus_SSLContextManager_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/ReadWriteSem.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class provides the functionality necessary to manage SSLContext
    objects.
 */
class PEGASUS_COMMON_LINKAGE SSLContextManager
{
public:
    /** Constructor. */

    SSLContextManager();

    ~SSLContextManager();

    void createSSLContext(
        const String& trustStore,
        const String& certPath,
        const String& keyPath,
        const String& crlStore,
        Boolean callback,
        const String& randFile);

    /**
        Reload the trust store used by either the CIM Server or
        Indication Server based on the context type.
     */
    void reloadTrustStore();

    /**
        Reload the CRL store.
     */
    void reloadCRLStore();

    /**
        Get a pointer to the sslContext object.
     */
    SSLContext*  getSSLContext() const;

    /**
        Get a pointer to the sslContextObjectLock.
     */
    ReadWriteSem*  getSSLContextObjectLock();

private:
    /**
        A lock to control access to the _sslContext object.
        Before read accessing the _sslContext object, one must first
        lock this for read access.  Before write accessing the _sslContext
        object, one must first lock this for write access.
     */
    ReadWriteSem _sslContextObjectLock;
    SSLContext* _sslContext;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContextManager_h */
