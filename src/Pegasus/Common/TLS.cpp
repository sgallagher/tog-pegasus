//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/SSLContextRep.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/FileSystem.h>    

#include "TLS.h"

// switch on if 'server needs certified client'
// this is for client certification that does not use PEGASUS_USE_232_CLIENT_CERTIFICATION (PEP#165)
//#define CLIENT_CERTIFY

//
// use the following definitions only if SSL is available
// 
#ifdef PEGASUS_HAS_SSL

PEGASUS_NAMESPACE_BEGIN

//
// Basic SSL socket
//

SSLSocket::SSLSocket(Sint32 socket, SSLContext * sslcontext)
   throw(SSLException) :
   _SSLCertificate(0),
   _SSLConnection(0),
   _socket(socket),
   _SSLContext(sslcontext)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::SSLSocket()");

    //
    // create the SSLConnection area
    //
    if (!( _SSLConnection = SSL_new(_SSLContext->_rep->getContext() )))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw( SSLException("Could not get SSL Connection Area"));
        MessageLoaderParms parms("Common.TLS.COULD_NOT_GET_SSL_CONNECTION_AREA",
                                 "Could not get SSL Connection Area");
        throw SSLException(parms);
    }

    //
    // and connect the active socket with the ssl operation
    //
    if (!(SSL_set_fd(_SSLConnection, _socket) ))
    {
        PEG_METHOD_EXIT();
        //l10n
        //throw( SSLException("Could not link socket to SSL Connection"));
        MessageLoaderParms parms("Common.TLS.COULD_NOT_LINK_SOCKET",
                                 "Could not link socket to SSL Connection");
        throw SSLException(parms);
    }

#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
    // test for client verification
    if (_SSLContext->isPeerVerificationEnabled()) 
    {
        _certificateStatus = NO_CERT;
    }
    else
    {
        _certificateStatus = VERIFICATION_DISABLED;
    } 
#endif

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "---> SSL: Created SSL socket");

    PEG_METHOD_EXIT();
}

SSLSocket::~SSLSocket()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::~SSLSocket()");

    SSL_free(_SSLConnection);

    if (_SSLCertificate) 
    {
        delete _SSLCertificate;
    }

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Deleted SSL socket");

    PEG_METHOD_EXIT();
}


Boolean SSLSocket::incompleteReadOccurred(Sint32 retCode)
{
    retCode = SSL_get_error(_SSLConnection, retCode);

    Tracer::trace(TRC_SSL, Tracer::LEVEL4,
        "SSLSocket::incompleteReadOccurred : retCode = %d", retCode);

    return(!(retCode == SSL_ERROR_WANT_READ ||
             retCode == SSL_ERROR_WANT_WRITE));
}

Sint32 SSLSocket::read(void* ptr, Uint32 size)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::read()");
    Sint32 rc;

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "---> SSL: (r) ");
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, SSL_state_string_long(_SSLConnection) );
    rc = SSL_read(_SSLConnection, (char *)ptr, size);

    PEG_METHOD_EXIT();
    return rc;
}

Sint32 SSLSocket::write( const void* ptr, Uint32 size)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::write()");
    Sint32 rc;

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "---> SSL: (w) ");
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, SSL_state_string_long(_SSLConnection) );
    rc = SSL_write(_SSLConnection, (char *)ptr, size);

    PEG_METHOD_EXIT();
    return rc;
}

void SSLSocket::close()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::close()");

    SSL_shutdown(_SSLConnection);
    Socket::close(_socket);

    PEG_METHOD_EXIT();
}

void SSLSocket::enableBlocking()
{
    Socket::enableBlocking(_socket);
}

void SSLSocket::disableBlocking()
{
    Socket::disableBlocking(_socket);
}

void SSLSocket::initializeInterface()
{
    Socket::initializeInterface();
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: initialized SSL");
}

void SSLSocket::uninitializeInterface()
{
    Socket::uninitializeInterface();
}

// SSL_accept returns 1 if successful, 0 if the handshake was shutdown, and < 0 if there is an error.
// Instead of returning 1 for success, send a more descriptive return value specifying the client's 
// certificate status when enableSSLClientVerification is set to true.
// modified -hns
Sint32 SSLSocket::accept()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::accept()");

    Sint32 ssl_rc,ssl_rsn;

    //these methods get implicitly done with the SSL_accept call
    //SSL_do_handshake(_SSLConnection);

    //SSL_set_accept_state(_SSLConnection);

redo_accept:
    ssl_rc = SSL_accept(_SSLConnection);

    if (ssl_rc < 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       Tracer::trace(TRC_SSL, Tracer::LEVEL3, "---> SSL: Not accepted %d", ssl_rsn );

       if ((ssl_rsn == SSL_ERROR_WANT_READ) ||
           (ssl_rsn == SSL_ERROR_WANT_WRITE))
       {
           goto redo_accept;
       }
       else
       {
           PEG_METHOD_EXIT();
           return -1;
       }
    }
    else if (ssl_rc == 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Shutdown SSL_accept()");
       Tracer::trace(TRC_SSL, Tracer::LEVEL4, "Error Code:  %d", ssl_rsn );
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
           "Error string: " + String(ERR_error_string(ssl_rc, NULL)));

       PEG_METHOD_EXIT();
       return -1;
    }
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Accepted");

#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
    if (_SSLContext->isPeerVerificationEnabled()) 
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to certify client");

        // get client's certificate
        X509 * client_cert = SSL_get_peer_certificate(_SSLConnection);
        if (client_cert != NULL)
        {
            int verifyResult = SSL_get_verify_result(_SSLConnection);
            Tracer::trace(TRC_SSL, Tracer::LEVEL3, "Verification Result:  %d", verifyResult );
            
            if (verifyResult == X509_V_OK)
            {
                
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                    "---> SSL: Client Certificate verified.");
                _certificateStatus = CERT_SUCCESS;
            }
            else
            {
               PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                   "---> SSL: Client Certificate not verified");    
                _certificateStatus = CERT_FAILURE;
            }

            // create the certificate object
            // ATTN: enable when we need certificate
            /*
            char buf[256];

            X509_NAME_oneline(X509_get_subject_name(client_cert), buf, 256);
            String subjectName = String(buf);

            X509_NAME_oneline(X509_get_issuer_name(client_cert), buf, 256);
            String issuerName = String(buf);

            _SSLCertificate = new SSLCertificateInfo(subjectName, issuerName, 0, verifyResult, X509_V_OK);

            PEGASUS_STD(cout) << "Subject " << subjectName << "\n";
            PEGASUS_STD(cout) << "Issuer " << issuerName << "\n";
            */

            X509_free(client_cert);
    }
    else
    {
               PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Client not certified, no certificate received");
               _certificateStatus = NO_CERT;
        }
    }
    else
    {
        _certificateStatus = VERIFICATION_DISABLED;
    }

#else
// this is the old way of certifying clients
#ifdef CLIENT_CERTIFY
    // get client's certificate
    // this is usually not needed 
    X509 * client_cert = SSL_get_peer_certificate(_SSLConnection);
    if (client_cert != NULL)
    {
       if (SSL_get_verify_result(_SSLConnection) == X509_V_OK)
       {
           PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
               "---> SSL: Client Certificate verified.");
       }
       else
       {
           PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
               "---> SSL: Client Certificate not verified");    
           PEG_METHOD_EXIT();
           return -1;
       }

       X509_free (client_cert);
    }
    else
    {
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Client not certified");
       PEG_METHOD_EXIT();
       return -1;
    }
#endif // CLIENT_CERTIFY
#endif // PEGASUS_USE232_CLIENT_VERIFICATION

    PEG_METHOD_EXIT();
    return ssl_rc;
}

Sint32 SSLSocket::connect()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::connect()");

    Sint32 ssl_rc,ssl_rsn;

    SSL_set_connect_state(_SSLConnection);

redo_connect:

    ssl_rc = SSL_connect(_SSLConnection);

    if (ssl_rc < 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Not connected " + ssl_rsn );

       if ((ssl_rsn == SSL_ERROR_WANT_READ) ||
           (ssl_rsn == SSL_ERROR_WANT_WRITE))
       {
           goto redo_connect;
       }
       else
       {
           PEG_METHOD_EXIT();
           return -1;
       }
    }
    else if (ssl_rc == 0)
    {
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Shutdown SSL_connect()");
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
           "Error string: " + String(ERR_error_string(ssl_rc, NULL)));
       PEG_METHOD_EXIT();
       return -1;
    }
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Connected");

#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
    // this will be true if the client specified a callback function and/or a truststore
    if (_SSLContext->isPeerVerificationEnabled())
    {
#endif
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to verify server certificate.");

        X509 * server_cert = SSL_get_peer_certificate(_SSLConnection);
        if (server_cert != NULL)
        {
            if (SSL_get_verify_result(_SSLConnection) == X509_V_OK)
            {
               PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Server Certificate verified.");
            }
            else
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Server Certificate NOT verified.");    
                PEG_METHOD_EXIT();
                return -1;
            }

            X509_free (server_cert);
        }
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Server not certified.");
           PEG_METHOD_EXIT();
           return -1;
        }
#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
    }
#endif

    PEG_METHOD_EXIT();
    return ssl_rc;
    }

#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
SSLCertificateInfo* SSLSocket::getPeerCertificate() 
{ 
    return _SSLCertificate; 
}

SSLSocket::certificateStatusFlag SSLSocket::getCertificateStatus() 
{ 
    return _certificateStatus; 
}

Boolean SSLSocket::addTrustedClient() 
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::addTrustedClient()");

    // check whether we have authority to automatically update truststore
    if (!_SSLContext->isTrustStoreAutoUpdateEnabled())
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Cannot add client certificate -- TrustStoreAutoUpdate is disabled.");
        return false;
    }

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to add client certificate to truststore.");

    X509 *client_cert = SSL_get_peer_certificate(_SSLConnection);

    if (client_cert != NULL)
    {
        unsigned long hashVal = X509_subject_name_hash(client_cert);

        String trustStore = _SSLContext->getTrustStore();
    
        if (trustStore == String::EMPTY || String::equal(trustStore, "none"))
        {
            // bail if we cannot find the truststore directory
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Cannot add client -- truststore directory invalid.");
            return false;
        }
        
        Uint32 index = 0;

        //The files are looked up by the CA subject name hash value. 
        //If more than one CA certificate with the same name hash value exist, 
        //the extension must be different (e.g. 9d66eef0.0, 9d66eef0.1 etc)
        char hashBuffer[32];
        sprintf(hashBuffer, "%x", hashVal);

        String hashString = "";
        for (int j = 0; j < 32; j++) 
        {
            if (hashBuffer[j] != '\0') 
            {
               hashString.append(hashBuffer[j]);
            } 
            else
            {
                break; // end of hash string
            }
        }
        
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Searching for files like " + hashString);

        FileSystem::translateSlashes(trustStore); 
        if (FileSystem::isDirectory(trustStore)) 
        {
            Array<String> trustedCerts;
            if (FileSystem::getDirectoryContents(trustStore, trustedCerts)) 
            {
                Uint32 count = trustedCerts.size();
                for (Uint32 i = 0; i < count; i++)
                {
                    if (String::compare(trustedCerts[i], hashString, hashString.size()) == 0) 
                    {
                       index++;
                    }
                }
            } 
            else
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Could not open truststore directory.");
                return false;
            }
        } 
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Trust path not directory.");
            return false;
        }
        
        //create new file with subject_hash.index in trust directory
        //copy client certificate into this file
        //now we trust this client and no longer need to check local OS credentials
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to add trusted client to " + trustStore);
        
        char filename[1024];
        sprintf(filename, "%s/%s.%d", 
                (const char*)trustStore.getCString(),
                (const char*)hashString.getCString(), 
                index);
        
        //use the ssl functions to write out the client x509 certificate
        //TODO: add some error checking here
        BIO* outFile = BIO_new(BIO_s_file());
        BIO_write_filename(outFile, filename);
        int i = PEM_write_bio_X509(outFile, client_cert);
        BIO_free_all(outFile);

        X509_free(client_cert);

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
			    "SSLSocket - Client certificate added to truststore.");
    }

    PEG_METHOD_EXIT();
    return true;
}
#endif // PEGASUS_USE_232_CLIENT_VERIFICATION

//
// MP_Socket (Multi-purpose Socket class
//



MP_Socket::MP_Socket(Uint32 socket) 
 : _isSecure(false), _socket(socket) {}

MP_Socket::MP_Socket(Uint32 socket, SSLContext * sslcontext)
   throw(SSLException)
{
    PEG_METHOD_ENTER(TRC_SSL, "MP_Socket::MP_Socket()");
    if (sslcontext != NULL)
    {
        _isSecure = true;
        _sslsock = new SSLSocket(socket, sslcontext);
    }
    else 
    {
        _isSecure = false;
        _socket = socket;
    }
    PEG_METHOD_EXIT();
}

MP_Socket::~MP_Socket()
{
    PEG_METHOD_ENTER(TRC_SSL, "MP_Socket::~MP_Socket()");
    if (_isSecure)
    {
        delete _sslsock;
    }
    PEG_METHOD_EXIT();
}   
   
Boolean MP_Socket::isSecure() {return _isSecure;}

Boolean MP_Socket::incompleteReadOccurred(Sint32 retCode)
{
    if (_isSecure)
        return(_sslsock->incompleteReadOccurred(retCode));
    return (retCode <=  0);
}

Sint32 MP_Socket::getSocket()
{
    if (_isSecure)
        return _sslsock->getSocket();
    else
        return _socket;
}

Sint32 MP_Socket::read(void * ptr, Uint32 size)
{
    if (_isSecure)
        return _sslsock->read(ptr,size);
    else
        return Socket::read(_socket,ptr,size);
}

Sint32 MP_Socket::write(const void * ptr, Uint32 size)
{
    if (_isSecure)
        return _sslsock->write(ptr,size);
    else
        return Socket::write(_socket,ptr,size);
}

void MP_Socket::close()
{
    if (_isSecure)

        _sslsock->close();

    else

        Socket::close(_socket);

}

void MP_Socket::enableBlocking()
{
    if (_isSecure)

        _sslsock->enableBlocking();

    else

        Socket::enableBlocking(_socket);

}

void MP_Socket::disableBlocking()
{
    if (_isSecure)

        _sslsock->disableBlocking();

    else

       Socket::disableBlocking(_socket);

}

Sint32 MP_Socket::accept()
{
    if (_isSecure)
        if (_sslsock->accept() < 0) return -1;
    return 0;
}

Sint32 MP_Socket::connect()
{
    if (_isSecure)
        if (_sslsock->connect() < 0) return -1;
    return 0;
}

#ifdef PEGASUS_USE_232_CLIENT_VERIFICATION
SSLCertificateInfo* MP_Socket::getPeerCertificate()
{
    if (_isSecure)
    {
        return (_sslsock->getPeerCertificate());
    }
    return NULL;
}

Sint32 MP_Socket::getCertificateStatus()
{
    if (_isSecure)
    {
        return (_sslsock->getCertificateStatus());
    }
    return 0;
}

Boolean MP_Socket::addTrustedClient()
{
    if (_isSecure)
    {
        return (_sslsock->addTrustedClient());
    }
    return false;
}
#endif

PEGASUS_NAMESPACE_END

#else

PEGASUS_NAMESPACE_BEGIN


MP_Socket::MP_Socket(Uint32 socket)
 : _socket(socket), _isSecure(false) {}

MP_Socket::MP_Socket(Uint32 socket, SSLContext * sslcontext)
   throw(SSLException)
 : _socket(socket), _isSecure(false) {}

MP_Socket::~MP_Socket() {}

Boolean MP_Socket::isSecure() {return _isSecure;}

Boolean MP_Socket::incompleteReadOccurred(Sint32 retCode) 
{
   return (retCode <= 0);
}

Sint32 MP_Socket::getSocket()
{
    return _socket;
}

Sint32 MP_Socket::read(void * ptr, Uint32 size)
{
    return Socket::read(_socket,ptr,size);
}

Sint32 MP_Socket::write(const void * ptr, Uint32 size)
{
    return Socket::write(_socket,ptr,size);
}

void MP_Socket::close()
{
    Socket::close(_socket);
}

void MP_Socket::enableBlocking()
{
    Socket::enableBlocking(_socket);
}

void MP_Socket::disableBlocking()
{
    Socket::disableBlocking(_socket);
}

Sint32 MP_Socket::accept() { return 0; }

Sint32 MP_Socket::connect() { return 0; }

PEGASUS_NAMESPACE_END

#endif // end of PEGASUS_HAS_SSL
