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

/////////////////////////////////////////////////////////////////////////////
//  SSLContextManager
/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_HAS_SSL

#ifdef PEGASUS_PLATFORM_WIN32_IX86_MSVC
//Use the X509_NAME in wincrypt.h on Windows.  See X509.h for more info.
#include <windows.h>
#include <wincrypt.h>
#endif

#define OPENSSL_NO_KRB5 1
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#else
#define SSL_CTX void
#define X509_STORE void

#endif // end of PEGASUS_HAS_SSL

#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/FileSystem.h>

#include "SSLContext.h"
#include "SSLContextRep.h"
#include "SSLContextManager.h"


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// This method along with the relevent code is moved here
// from CIMServer.cpp
//
Boolean verifyClientOptionalCallback(SSLCertificateInfo &certInfo)
{
    // SSL callback for the "optional" client verification setting
    // By always returning true, we allow the handshake to continue
    // even if the client sent no certificate or sent an untrusted certificate.
    return true;
}



SSLContextManager::SSLContextManager()
    : _sslContext(0),
      _exportSSLContext(0)
{

}

SSLContextManager::~SSLContextManager()
{
    if (_sslContext)
    {
        delete _sslContext;
    }

    if (_exportSSLContext)
    {
        delete _exportSSLContext;
    }
}

//
// Part of this code logic comes from the CIMServer::_getSSLContext()
// and CIMServer::_getExportSSLContext() methods.
//
void SSLContextManager::createSSLContext(Uint32 contextType,
    const String& trustStore, const String& certPath, const String& keyPath, 
    const String& crlStore, Boolean callback, const String& randFile)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextManager::createSSLContext()");

    if ( contextType == SERVER_CONTEXT  && !_sslContext )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Creating the Server SSL Context.");

        //
        // Create the SSLContext object
        //
        if ( callback )
        {
            _sslContext = new SSLContext(trustStore, certPath, 
                keyPath, crlStore, 
                (SSLCertificateVerifyFunction*)verifyClientOptionalCallback,
                randFile);
        }
        else if ( trustStore != String::EMPTY )
        {
            _sslContext = new SSLContext(trustStore, certPath, 
                keyPath, crlStore, 0, randFile);
        }
        else 
        {
            _sslContext = new SSLContext(String::EMPTY, certPath, 
                keyPath, crlStore, 0, randFile);
        }
    }
    else if ( contextType == EXPORT_CONTEXT && !_exportSSLContext )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Creating the Export SSL Context.");

        //
        // Note: Trust store is used by default on Export connections,
        // verification callback function is not used.
        //
        _exportSSLContext = new SSLContext(trustStore, certPath, 
            keyPath, crlStore, 0, randFile);
    }

    PEG_METHOD_EXIT();
}

//
// use the following methods only if SSL is available
//
#ifdef PEGASUS_HAS_SSL

void SSLContextManager::reloadTrustStore(Uint32 contextType)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextManager::reloadTrustStore()");

    SSL_CTX* sslContext;
    String trustStore = String::EMPTY;

    if ( contextType == SERVER_CONTEXT && _sslContext )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Context Type is Server Context.");
        sslContext = _sslContext->_rep->getContext();
        trustStore = _sslContext->getTrustStore();
    }
    else if ( contextType == EXPORT_CONTEXT && _exportSSLContext )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Context Type is Export Context.");
        sslContext = _exportSSLContext->_rep->getContext();
        trustStore = _exportSSLContext->getTrustStore();
    }
    else
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
          "Could not reload the trust store, SSL Context is not initialized.");

        MessageLoaderParms parms(
         "Pegasus.Common.SSLContextManager.COULD_NOT_RELOAD_TRUSTSTORE_SSL_CONTEXT_NOT_INITIALIZED",
         "Could not reload the trust store, SSL Context is not initialized.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    if (trustStore == String::EMPTY)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Could not reload the trust store, the trust store is not configured.");

        MessageLoaderParms parms(
            "Pegasus.Common.SSLContextManager.TRUST_STORE_NOT_CONFIGURED",
            "Could not reload the trust store, the trust store is not configured.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    X509_STORE* newStore = _getNewX509Store(trustStore);

    //
    // acquire write lock to Context object and then overwrite the trust 
    // store cache
    //
    {
        WriteLock contextLock(_sslContextObjectLock);
        SSL_CTX_set_cert_store(sslContext, newStore);
    }
    PEG_METHOD_EXIT();
}

void SSLContextManager::reloadCRLStore()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextManager::reloadCRLStore()");

    if (!_sslContext && !_exportSSLContext)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
        "Could not reload the crl store, SSL Context is not initialized.");

        MessageLoaderParms parms(
         "Pegasus.Common.SSLContextManager.COULD_NOT_RELOAD_CRL_STORE_SSL_CONTEXT_NOT_INITIALIZED",
         "Could not reload the crl store, SSL Context is not initialized.");

        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    String crlPath = _sslContext->getCRLPath();

    if (crlPath == String::EMPTY)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Could not reload the crl store, the crl store is not configured.");

        MessageLoaderParms parms(
            "Pegasus.Common.SSLContextManager.CRL_STORE_NOT_CONFIGURED",
            "Could not reload the crl store, the crl store is not configured.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "CRL store path is " + crlPath);

    //update the CRL store for both the server and the export server since they share the same CRL store
    X509_STORE* crlStore;
    
    {
        WriteLock contextLock(_sslContextObjectLock);
        if (_sslContext)
        {
            _sslContext->_rep->setCRLStore(_getNewX509Store(crlPath));
        }
        if (_exportSSLContext)
        {
            _exportSSLContext->_rep->setCRLStore(_getNewX509Store(crlPath));
        }
    }

    PEG_METHOD_EXIT();
}

X509_STORE* SSLContextManager::_getNewX509Store(const String& storePath)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextManager::_getNewX509Store()");

    //
    // reload certificates from the specified store
    //
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
        "Reloading certificates from the store: " + storePath);

    X509_STORE* newStore = X509_STORE_new(); 

    //
    // Check if there is a CA certificate file or directory specified. 
    // If specified, load the certificates from the specified store path.
    //
    if (FileSystem::isDirectory(storePath))
    {
        X509_LOOKUP* storeLookup = X509_STORE_add_lookup(newStore, 
                                              X509_LOOKUP_hash_dir()); 
        if (storeLookup == NULL)
        {
            X509_STORE_free(newStore);

            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Could not reload the trust or crl store.");

            MessageLoaderParms parms(
             "Pegasus.Common.SSLContextManager.COULD_NOT_RELOAD_TRUST_OR_CRL_STORE",
             "Could not reload the trust or crl store.");
            PEG_METHOD_EXIT();
            throw SSLException(parms);
        }
        X509_LOOKUP_add_dir(storeLookup, 
            storePath.getCString(), X509_FILETYPE_PEM); 
    }
    else if (FileSystem::exists(storePath))
    {
        X509_LOOKUP* storeLookup = X509_STORE_add_lookup(newStore, 
                                                     X509_LOOKUP_file()); 
        if (storeLookup == NULL)
        {
            X509_STORE_free(newStore);

            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Could not reload the trust or crl store.");

            MessageLoaderParms parms(
             "Pegasus.Common.SSLContextManager.COULD_NOT_RELOAD_TRUST_OR_CRL_STORE",
             "Could not reload the trust or crl store.");
            PEG_METHOD_EXIT();
            throw SSLException(parms);
        }
        X509_LOOKUP_load_file(storeLookup, 
            storePath.getCString(), X509_FILETYPE_PEM); 
    }
    else
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Could not reload the trust or crl store, configured store not found.");

        MessageLoaderParms parms(
         "Pegasus.Common.SSLContextManager.CONFIGURED_TRUST_OR_CRL_STORE_NOT_FOUND",
         "Could not reload the trust or crl store, configured store not found.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    PEG_METHOD_EXIT();
    return newStore;
}

#else    //#ifdef PEGASUS_HAS_SSL

void SSLContextManager::reloadTrustStore(Uint32 contextType) { }

void SSLContextManager::reloadCRLStore() { }

X509_STORE* SSLContextManager::_getNewX509Store(const String& storePath) { return NULL; }

#endif   //#ifdef PEGASUS_HAS_SSL

/**
    Get a pointer to the sslContext object.
 */
SSLContext*  SSLContextManager::getSSLContext(Uint32 contextType) const
{
    if ( contextType == SERVER_CONTEXT )
    {
        return _sslContext;
    }
    else if ( contextType == EXPORT_CONTEXT )
    {
        return _exportSSLContext;
    }
    return 0;
}

/**
    Get a pointer to the sslContextObjectLock.
 */
ReadWriteSem* SSLContextManager::getSSLContextObjectLock()
{
    return &_sslContextObjectLock;
}

PEGASUS_NAMESPACE_END
