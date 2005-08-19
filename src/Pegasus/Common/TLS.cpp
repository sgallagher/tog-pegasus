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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
//         Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for PEP#101
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/SSLContextRep.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/MessageLoader.h> //l10n
#include <Pegasus/Common/FileSystem.h>    

#include "TLS.h"

//
// use the following definitions only if SSL is available
// 
#ifdef PEGASUS_HAS_SSL

PEGASUS_NAMESPACE_BEGIN

//
// Basic SSL socket
//

SSLSocket::SSLSocket(
    Sint32 socket,
    SSLContext * sslcontext,
    ReadWriteSem * sslContextObjectLock,
    Boolean exportConnection)
   :
   _SSLConnection(0),
   _socket(socket),
   _SSLContext(sslcontext),
   _sslContextObjectLock(sslContextObjectLock),
   _SSLCallbackInfo(0),
   _certificateVerified(false),
   _exportConnection(exportConnection)
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
    // set the verification callback data
    //
        
    //we are only storing one set of data, so we can just use index 0, this is defined in SSLContext.h
    //int index = SSL_get_ex_new_index(0, (void*)"pegasus", NULL, NULL, NULL);

    // 
    // Create a new callback info for each new connection
    // 
    _SSLCallbackInfo.reset(new SSLCallbackInfo(_SSLContext->getSSLCertificateVerifyFunction(),
										   _SSLContext->getCRLStore()));

    if (SSL_set_ex_data(_SSLConnection, SSLCallbackInfo::SSL_CALLBACK_INDEX, _SSLCallbackInfo.get())) 
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "--->SSL: Set callback info");
    }
    else
    {
		PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "--->SSL: Error setting callback info");
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

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "---> SSL: Created SSL socket");

    PEG_METHOD_EXIT();
}

SSLSocket::~SSLSocket()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::~SSLSocket()");

    SSL_free(_SSLConnection);

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Deleted SSL socket");

    PEG_METHOD_EXIT();
}


Boolean SSLSocket::incompleteReadOccurred(Sint32 retCode)
{
    Sint32 err = SSL_get_error(_SSLConnection, retCode);

    Tracer::trace(TRC_SSL, Tracer::LEVEL4,
        "SSLSocket::incompleteReadOccurred : err = %d", err);

    return((err == SSL_ERROR_WANT_READ ||
            err == SSL_ERROR_WANT_WRITE));
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

Sint32 SSLSocket::accept()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::accept()");

    Sint32 ssl_rc,ssl_rsn;

    //ATTN: these methods get implicitly done with the SSL_accept call
    //SSL_do_handshake(_SSLConnection);
    //SSL_set_accept_state(_SSLConnection);

    // Make sure the SSLContext object is not updated during this operation.
    ReadLock rlock(*_sslContextObjectLock);

    ssl_rc = SSL_accept(_SSLConnection);

    if (ssl_rc < 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
	   Tracer::trace(TRC_SSL, Tracer::LEVEL3, "---> SSL: Not accepted %d", ssl_rsn );

       if ((ssl_rsn == SSL_ERROR_WANT_READ) ||
           (ssl_rsn == SSL_ERROR_WANT_WRITE))
       {
           PEG_METHOD_EXIT();
           return 0;
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

    //
    // If peer certificate verification is enabled or request received on
    // export connection, get the peer certificate and verify the trust 
    // store validation result.
    //
    if (_SSLContext->isPeerVerificationEnabled() || _exportConnection)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to certify client");

        //
        // get client's certificate
        //
        X509 * client_cert = SSL_get_peer_certificate(_SSLConnection);
        if (client_cert != NULL)
        {
            //
            // get certificate verification result
            //
            int verifyResult = SSL_get_verify_result(_SSLConnection);
            Tracer::trace(TRC_SSL, Tracer::LEVEL3, "Verification Result:  %d", verifyResult );
            
            if (verifyResult == X509_V_OK)
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2, 
                    "---> SSL: Client Certificate verified.");
                //
                // set flag to indicate that the certificate was verified in
                // the trust store.
                //
                _certificateVerified = true;
            }
            else
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2, 
                     "---> SSL: Client Certificate not verified");    
                //
                // On export connection, do not continue if the
                // certificate is not verified.
                //
                if (_exportConnection)
                {
                    PEG_METHOD_EXIT();
                    return -1;
                }
            }

            X509_free(client_cert);
        }
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                "---> SSL: Client not certified, no certificate received");
            //
            // On export connection, do not continue if peer certificate 
            // is not received
            //
            if (_exportConnection)
            {
                PEG_METHOD_EXIT();
                return -1;
            }
        }
    }
    else
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Client certification disabled");
    }

    PEG_METHOD_EXIT();
    return 1;
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

    if (_SSLContext->isPeerVerificationEnabled())
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "Attempting to verify server certificate.");

        X509 * server_cert = SSL_get_peer_certificate(_SSLConnection);
        if (server_cert != NULL)
        {
            //
            // Do not check the verification result using SSL_get_verify_result here to see whether or not to continue.
            // The prepareForCallback does not reset the verification result, so it will still contain the original error.
            // If the client chose to override the default error in the callback and return true, we got here and 
            // should proceed with the transaction.  Otherwise, the handshake was already terminated.
            //
            
            if (SSL_get_verify_result(_SSLConnection) == X509_V_OK)
            {
                 PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "--->SSL: Server Certificate verified.");
            }
            else
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "--->SSL: Server Certificate not verified, but the callback overrode the default error.");
            }

            X509_free (server_cert);
        }
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "-->SSL: Server not certified, no certificate received.");
            PEG_METHOD_EXIT();
            return -1;
        }
	}
    else
    {
	    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Server certification disabled");
    }

    PEG_METHOD_EXIT();
    return ssl_rc;
}

Boolean SSLSocket::isPeerVerificationEnabled()
{ 
    return (_SSLContext->isPeerVerificationEnabled()); 
}

SSLCertificateInfo* SSLSocket::getPeerCertificate() 
{ 
    if (_SSLCallbackInfo.get()) 
    {
        return (_SSLCallbackInfo->_rep->peerCertificate);
    }

    return NULL;
}

Boolean SSLSocket::isCertificateVerified()
{ 
    return _certificateVerified; 
}


//
// MP_Socket (Multi-purpose Socket class
//



MP_Socket::MP_Socket(Uint32 socket) 
 : _socket(socket), _isSecure(false) {}

MP_Socket::MP_Socket(
    Uint32 socket,
    SSLContext * sslcontext,
    ReadWriteSem * sslContextObjectLock,
    Boolean exportConnection)
{
    PEG_METHOD_ENTER(TRC_SSL, "MP_Socket::MP_Socket()");
    if (sslcontext != NULL)
    {
        _isSecure = true;
        _sslsock = new SSLSocket(
            socket, sslcontext, sslContextObjectLock, exportConnection);
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
    {
        return (_sslsock->accept());
    }
    return 1;
}

Sint32 MP_Socket::connect()
{
    if (_isSecure)
        if (_sslsock->connect() < 0) return -1;
    return 0;
}

Boolean MP_Socket::isPeerVerificationEnabled()
{
    if (_isSecure)
    {
        return (_sslsock->isPeerVerificationEnabled());
    }
    return false;
}

SSLCertificateInfo* MP_Socket::getPeerCertificate()
{
    if (_isSecure)
    {
        return (_sslsock->getPeerCertificate());
    }
    return NULL;
}

Boolean MP_Socket::isCertificateVerified()
{
    if (_isSecure)
    {
        return (_sslsock->isCertificateVerified());
    }
    return false;
}

PEGASUS_NAMESPACE_END

#else

PEGASUS_NAMESPACE_BEGIN


MP_Socket::MP_Socket(Uint32 socket)
 : _socket(socket), _isSecure(false) {}

MP_Socket::MP_Socket(
    Uint32 socket,
    SSLContext * sslcontext,
    ReadWriteSem * sslContextObjectLock,
    Boolean exportConnection)
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

Sint32 MP_Socket::accept() { return 1; }

Sint32 MP_Socket::connect() { return 0; }

Boolean MP_Socket::isPeerVerificationEnabled() { return false; }

SSLCertificateInfo* MP_Socket::getPeerCertificate() { return NULL; }

Boolean MP_Socket::isCertificateVerified() { return false; }

PEGASUS_NAMESPACE_END

#endif // end of PEGASUS_HAS_SSL
