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
#include <Pegasus/Common/SSLContextRep.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Config/ConfigManager.h>

#include "SSLContextManager.h"

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
    : _trustStore(String::EMPTY),
      _crlStore(String::EMPTY),
      _exportTrustStore(String::EMPTY),
      _sslContext(0),
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
// Original code for this method comes from the former _getSSLContext()
// and _getExportSSLContext() methods in CIMServer.cpp
//
SSLContext* SSLContextManager::getSSLContext(Uint32 contextType)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextManager::getSSLContext()");

    if ( (contextType == SERVER_CONTEXT && !_sslContext ) ||
         (contextType == EXPORT_CONTEXT && !_exportSSLContext ) )
    {
        static const String PROPERTY_NAME__SSL_CERT_FILEPATH = 
                                               "sslCertificateFilePath";
        static const String PROPERTY_NAME__SSL_KEY_FILEPATH  = "sslKeyFilePath";
        static const String PROPERTY_NAME__SSL_TRUST_STORE  = "sslTrustStore";
        static const String PROPERTY_NAME__EXPORT_SSL_TRUST_STORE = 
                                               "exportSSLTrustStore";
        static const String PROPERTY_NAME__SSL_CRL_STORE  = "crlStore";
        static const String PROPERTY_NAME__SSL_CLIENT_VERIFICATION = 
                                               "sslClientVerificationMode";
        static const String PROPERTY_NAME__SSL_AUTO_TRUST_STORE_UPDATE = 
                                               "enableSSLTrustStoreAutoUpdate";
        static const String PROPERTY_NAME__SSL_TRUST_STORE_USERNAME = 
                                               "sslTrustStoreUserName";
        static const String PROPERTY_NAME__HTTP_ENABLED = 
                                               "enableHttpConnection";

        //
        // Get a config manager instance
        //
        ConfigManager* configManager = ConfigManager::getInstance();

        String verifyClient = String::EMPTY;

        if ( contextType == SERVER_CONTEXT )
        {
            // Note that if invalid values were set for either sslKeyFilePath, 
            // sslCertificateFilePath, crlStore or sslTrustStore, the invalid 
            // paths would have been detected in SecurityPropertyOwner and 
            // terminated the server startup. This happens regardless of whether
            // or not HTTPS is enabled (not a great design, but that seems to 
            // be how other properties are validated as well)
            //
            // Get the sslClientVerificationMode property from the Config 
            // Manager.
            //
            verifyClient = configManager->getCurrentValue(
                                      PROPERTY_NAME__SSL_CLIENT_VERIFICATION);

            //
            // Get the sslTrustStore property from the Config Manager.
            //
            _trustStore = configManager->getCurrentValue(
                                     PROPERTY_NAME__SSL_TRUST_STORE);

            if (_trustStore != String::EMPTY)
            {
                _trustStore = ConfigManager::getHomedPath(_trustStore);
            }

            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Server trust store name: " + _trustStore);
            //
            // Get the sslTrustStoreUserName property from the Config Manager.
            //
            String trustStoreUserName = String::EMPTY;
            trustStoreUserName = configManager->getCurrentValue(
                                      PROPERTY_NAME__SSL_TRUST_STORE_USERNAME);

            if (!String::equal(verifyClient, "disabled"))
            {
                //
                // 'required' setting must have a valid truststore
                // 'optional' setting can be used with or without a truststore; 
                // log a warning if a truststore is not specified
                //
                if (_trustStore == String::EMPTY)
                {
                    if (String::equal(verifyClient, "required"))
                    {
                        MessageLoaderParms parms(
                            "Pegasus.Server.SSLContextManager.SSL_CLIENT_VERIFICATION_EMPTY_TRUSTSTORE",
                            "The \"sslTrustStore\" configuration property must be set if \"sslClientVerificationMode\" is 'required'. cimserver not started.");
                        PEG_METHOD_EXIT();
                        throw SSLException(parms);
                    }
                    else if (String::equal(verifyClient, "optional"))
                    {
                        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, 
                            Logger::WARNING, 
                            "SSL client verification is enabled but no truststore was specified.");
                    }
                }

                //
                // ATTN: 'required' setting must have http port enabled.
                // If only https is enabled, and a call to shutdown the 
                // cimserver is given, the call will hang and a forced shutdown
                // will ensue. This is because the CIMClient::connectLocal call
                // cannot (and should not) specify a truststore to validate the
                // local server against.  This limitation is being investigated.
                //
                if (String::equal(verifyClient, "required"))
                {
                    String httpEnabled = configManager->getCurrentValue(
                                                  PROPERTY_NAME__HTTP_ENABLED);

                    if (!String::equal(httpEnabled, "true"))
                    {
                        MessageLoaderParms parms(
                            "Pegasus.Server.SSLContextManager.SSL_CLIENT_VERIFICATION_HTTP_NOT_ENABLED_WITH_REQUIRED",
                            "The HTTP port must be enabled if \"sslClientVerificationMode\" is 'required' in order for the cimserver to properly shutdown. cimserver not started.");
                        PEG_METHOD_EXIT();
                        throw SSLException(parms);
                    }
                }

                //
                // A truststore username must be specified if 
                // sslClientVerificationMode is enabled and the truststore is a
                // single CA file.  If the truststore is a directory, then the 
                // CertificateProvider should be used to register users with 
                // certificates.
                //
                if ((_trustStore != String::EMPTY) && 
                    (!FileSystem::isDirectory(_trustStore)))
                {
                    if (trustStoreUserName == String::EMPTY)
                    {
                        MessageLoaderParms parms(
                            "Pegasus.Server.SSLContextManager.SSL_CLIENT_VERIFICATION_EMPTY_USERNAME",
                            "The \"sslTrustStoreUserName\" property must specify a valid username if \"sslClientVerificationMode\" is 'required' or 'optional' and the truststore is a single CA file. To register individual certificates to users, you must use a truststore directory along with the CertificateProvider.  cimserver not started.");
                        PEG_METHOD_EXIT();
                        throw SSLException(parms);
                    }
                }
            }
        }
        else if ( contextType == EXPORT_CONTEXT )
        {
            //
            // Get the exportSSLTrustStore property from the Config Manager.
            //
            _exportTrustStore = configManager->getCurrentValue(
                                      PROPERTY_NAME__EXPORT_SSL_TRUST_STORE);

            if (_exportTrustStore == String::EMPTY)
            {
                MessageLoaderParms parms(
                    "Pegasus.Server.SSLContextManager.EXPORT_TRUST_EMPTY",
                    "The \"exportSSLTrustStore\" configuration property must be set when \"enableSSLExportClientVerification\" is true. cimserver not started.");

                PEG_METHOD_EXIT();
                throw Exception(parms);
            }

            _exportTrustStore = ConfigManager::getHomedPath(_exportTrustStore);

            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Export trust store name: " + _exportTrustStore);
        }

        //
        // Get the crlStore property from the Config Manager.
        //
        _crlStore = configManager->getCurrentValue(
                                   PROPERTY_NAME__SSL_CRL_STORE);

        if (_crlStore != String::EMPTY)
        {
            _crlStore = ConfigManager::getHomedPath(_crlStore);
        }

        //
        // Get the sslCertificateFilePath property from the Config Manager.
        //
        String certPath;
        certPath = ConfigManager::getHomedPath(
            configManager->getCurrentValue(PROPERTY_NAME__SSL_CERT_FILEPATH));

        //
        // Get the sslKeyFilePath property from the Config Manager.
        //
        String keyPath;
        keyPath = ConfigManager::getHomedPath(
            configManager->getCurrentValue(PROPERTY_NAME__SSL_KEY_FILEPATH));

        String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
        // NOTE: It is technically not necessary to set up a random file on
        // the server side, but it is easier to use a consistent interface
        // on the client and server than to optimize out the random file on
        // the server side.
        randFile = ConfigManager::getHomedPath(PEGASUS_SSLSERVER_RANDOMFILE);
#endif

        if ( contextType == SERVER_CONTEXT )
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Creating the Server SSL Context.");
            //
            // Create the SSLContext defined by the configuration properties
            //
            if (String::equal(verifyClient, "required"))
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "SSL Client verification REQUIRED.");

                _sslContext = new SSLContext(_trustStore, certPath, 
                    keyPath, _crlStore, 0, randFile);
            }
            else if (String::equal(verifyClient, "optional"))
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "SSL Client verification OPTIONAL.");

                _sslContext = new SSLContext(_trustStore, certPath, 
                    keyPath, _crlStore, 
                    (SSLCertificateVerifyFunction*)verifyClientOptionalCallback,
                    randFile);
            }
            else if (String::equal(verifyClient, "disabled") || 
                     verifyClient == String::EMPTY)
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "SSL Client verification DISABLED.");

                _sslContext = new SSLContext(String::EMPTY, certPath, 
                    keyPath, _crlStore, 0, randFile);
            }
            PEG_METHOD_EXIT();
            return _sslContext;
        }
        else if ( contextType == EXPORT_CONTEXT )
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Creating the Export SSL Context.");
            //
            // Note: Trust store is used by default on Export connections,
            // verification callback function is not used.
            //

            _exportSSLContext = new SSLContext(_exportTrustStore, certPath, 
                keyPath, _crlStore, 0, randFile);

            PEG_METHOD_EXIT();
            return _exportSSLContext;
        }
    }

    PEG_METHOD_EXIT();
    return 0;
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
        trustStore = _trustStore;
    }
    else if ( contextType == EXPORT_CONTEXT && _exportSSLContext )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Context Type is Export Context.");
        sslContext = _exportSSLContext->_rep->getContext();
        trustStore = _exportTrustStore;
    }
    else
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
          "Could not reload the trust store, SSL Context is not initialized.");

        MessageLoaderParms parms(
         "Pegasus.Server.SSLContextManager.COULD_NOT_RELOAD_TRUSTSTORE_SSL_CONTEXT_NOT_INITIALIZED",
         "Could not reload the trust store, SSL Context is not initialized.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    if (trustStore == String::EMPTY)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Could not reload the trust store, the trust store is not configured.");

        MessageLoaderParms parms(
            "Pegasus.Server.SSLContextManager.TRUST_STORE_NOT_CONFIGURED",
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
         "Pegasus.Server.SSLContextManager.COULD_NOT_RELOAD_CRL_STORE_SSL_CONTEXT_NOT_INITIALIZED",
         "Could not reload the crl store, SSL Context is not initialized.");

        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    if (_crlStore == String::EMPTY)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Could not reload the crl store, the crl store is not configured.");

        MessageLoaderParms parms(
            "Pegasus.Server.SSLContextManager.CRL_STORE_NOT_CONFIGURED",
            "Could not reload the crl store, the crl store is not configured.");
        PEG_METHOD_EXIT();
        throw SSLException(parms);
    }

    X509_STORE* newStore = _getNewX509Store(_crlStore);

    //
    // Acquire write lock to Context objects and then overwrite the
    // crl store cache for both the cimContext and exportContext
    //
    {
        WriteLock contextLock(_sslContextObjectLock);

        if (_sslContext)
        {
            SSL_CTX_set_cert_store(
                _sslContext->_rep->getContext(), newStore);
        }

        if (_exportSSLContext)
        {
            SSL_CTX_set_cert_store(
                _exportSSLContext->_rep->getContext(), newStore);
        }
    }

    PEG_METHOD_EXIT();
}

X509_STORE* SSLContextManager::_getNewX509Store(const String storePath)
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
             "Pegasus.Server.SSLContextManager.COULD_NOT_RELOAD_TRUST_OR_CRL_STORE",
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
             "Pegasus.Server.SSLContextManager.COULD_NOT_RELOAD_TRUST_OR_CRL_STORE",
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
         "Pegasus.Server.SSLContextManager.CONFIGURED_TRUST_OR_CRL_STORE_NOT_FOUND",
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

X509_STORE* SSLContextManager::_getNewX509Store(const String storePath) { return NULL; }

#endif   //#ifdef PEGASUS_HAS_SSL

/**
    Get a pointer to the sslContextObjectLock.
 */
ReadWriteSem* SSLContextManager::getSSLContextObjectLock()
{
    return &_sslContextObjectLock;
}

PEGASUS_NAMESPACE_END
