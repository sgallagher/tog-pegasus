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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Bapu Patil, Hewlett-Packard Company ( bapu_patil@hp.com )
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//         Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/SSLContextRep.h>
#include <Pegasus/Common/IPC.h>

#include "TLS.h"

// switch on if 'server needs certified client'
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
        throw( SSLException("Could not get SSL Connection Area"));
    }

    //
    // and connect the active socket with the ssl operation
    //
    if (!(SSL_set_fd(_SSLConnection, _socket) ))
    {
        PEG_METHOD_EXIT();
        throw( SSLException("Could not link socket to SSL Connection"));
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

Sint32 SSLSocket::read(void* ptr, Uint32 size)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLSocket::read()");

    #define MAX_READ_RETRIES 5
    #define SLEEP_TIME_TO_RETRY 10

    Sint32 rc;
    Uint32 retryCount = 0;

    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "---> SSL: (r) ");
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, SSL_state_string_long(_SSLConnection) );

    while (retryCount++ < MAX_READ_RETRIES)
    {
	rc = SSL_read(_SSLConnection, (char *)ptr, size);
	if (rc > 0) return rc;
	switch (SSL_get_error (_SSLConnection, rc))
	{
           case SSL_ERROR_ZERO_RETURN:
	      return 0;
	      break;
	   case SSL_ERROR_WANT_READ:
	      pegasus_sleep(SLEEP_TIME_TO_RETRY);
	      break;
	   default:
	      return rc;
	}
    }

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

    SSL_do_handshake(_SSLConnection);

    SSL_set_accept_state(_SSLConnection);

redo_accept:
    ssl_rc = SSL_accept(_SSLConnection);

    if (ssl_rc < 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Not accepted " + ssl_rsn );

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
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "Error Code: " + ssl_rsn );
       PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
           "Error string: " + String(ERR_error_string(ssl_rc, NULL)));

       PEG_METHOD_EXIT();
       return -1;
    }
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Accepted");

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
#endif

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

    // get server's certificate
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

    PEG_METHOD_EXIT();
    return ssl_rc;
}


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
        delete _sslsock;
    PEG_METHOD_EXIT();
}   
   
Boolean MP_Socket::isSecure() {return _isSecure;}

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
        return _sslsock->close();
    else
        return Socket::close(_socket);
}

void MP_Socket::enableBlocking()
{
    if (_isSecure)
        return _sslsock->enableBlocking();
    else
        return Socket::enableBlocking(_socket);
}

void MP_Socket::disableBlocking()
{
    if (_isSecure)
        return _sslsock->disableBlocking();
    else
        return Socket::disableBlocking(_socket);
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
