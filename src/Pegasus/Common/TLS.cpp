//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/SSLContextRep.h>

#include "TLS.h"

#define PEGASUS_CERT "/server.pem"
#define PEGASUS_KEY "/server.pem"


// debug flag
#define TLS_DEBUG(X) // X

// switch on 'server needs certified client'
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
   throw(SSL_Exception) :
   _SSLCertificate(0),
   _SSLConnection(0),
   _socket(socket),
   _SSLContext(sslcontext)
{
   //
   // create the SSLConnection area
   //
   if (!( _SSLConnection = SSL_new(_SSLContext->_rep->getContext() )))
      throw( SSL_Exception("Could not get SSL Connection Area"));

   //
   // and connect the active socket with the ssl operation
   //
   if (!(SSL_set_fd(_SSLConnection, _socket) ))
      throw( SSL_Exception("Could not link socket to SSL Connection"));

   TLS_DEBUG(cerr << "---> SSL: Created SSL socket\n";)
}

SSLSocket::~SSLSocket()
{
    SSL_free(_SSLConnection);
    TLS_DEBUG(cerr << "---> SSL: Deleted SSL socket\n";)
}

Sint32 SSLSocket::read(void* ptr, Uint32 size)
{
    Sint32 rc,rsn;
    TLS_DEBUG( cerr << "---> SSL: (r) ";)
    TLS_DEBUG( cerr << SSL_state_string_long(_SSLConnection) << endl;)
    rc = SSL_read(_SSLConnection, (char *)ptr, size);
    return rc;
}

Sint32 SSLSocket::write( const void* ptr, Uint32 size)
{
    Sint32 rc,rsn;
    TLS_DEBUG( cerr << "---> SSL: (w) ";)
    TLS_DEBUG( cerr << SSL_state_string_long(_SSLConnection) << endl;)
    rc = SSL_write(_SSLConnection, (char *)ptr, size);
    return rc;
}

void SSLSocket::close()
{
    SSL_shutdown(_SSLConnection);
    Socket::close(_socket);
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
    TLS_DEBUG(cerr << "---> SSL: initialized SSL\n";)
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
       TLS_DEBUG(cerr << "Not accepted " << ssl_rsn << endl;)

       if ((ssl_rsn == SSL_ERROR_WANT_READ) ||
           (ssl_rsn == SSL_ERROR_WANT_WRITE))
          goto redo_accept;
       else
          return -1;
    }
    else if (ssl_rc == 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       TLS_DEBUG(cerr << "Shutdown SSL_accept()\n";)
       TLS_DEBUG(cerr << "Error Code: " << ssl_rsn << " \n";)

       TLS_DEBUG(cerr << "Error string: " << ERR_error_string(ssl_rc, NULL) << " \n";)
       TLS_DEBUG(ERR_print_errors_fp(stderr));

       return -1;
    }
    TLS_DEBUG(cerr << "Accepted\n";)

#ifdef CLIENT_CERTIFY
    // get client's certificate
    // this is usually not needed 
    X509 * client_cert = SSL_get_peer_certificate(_SSLConnection);
    if (client_cert != NULL)
    {
       if (SSL_get_verify_result(_SSLConnection) == X509_V_OK)
       {
           TLS_DEBUG(cerr << "Client Certificate verified.\n";)
       }
       else
       {
           TLS_DEBUG(cerr << "Client Certificate not verified\n");    
           return -1;
       }

       X509_free (client_cert);
    }
    else
    {
       TLS_DEBUG(cerr << "Client not certified\n";)
       return -1;
    }
#endif

    PEG_METHOD_EXIT();
    return ssl_rc;
}

Sint32 SSLSocket::connect()
{
    Sint32 ssl_rc,ssl_rsn;

    SSL_set_connect_state(_SSLConnection);

redo_connect:

    ssl_rc = SSL_connect(_SSLConnection);

    if (ssl_rc < 0)
    {
       ssl_rsn = SSL_get_error(_SSLConnection, ssl_rc);
       TLS_DEBUG(cerr << "--->Not connected " << ssl_rsn << endl;)

       if ((ssl_rsn == SSL_ERROR_WANT_READ) ||
           (ssl_rsn == SSL_ERROR_WANT_WRITE))
          goto redo_connect;
       else
          return -1;
    }
    else if (ssl_rc == 0)
    {
       TLS_DEBUG(cerr << "--->Shutdown SSL_connect()\n";)
       TLS_DEBUG(ERR_print_errors_fp(stderr));
       return -1;
    }
    TLS_DEBUG(cerr << "--->Connected\n";)

    // get server's certificate
    X509 * server_cert = SSL_get_peer_certificate(_SSLConnection);
    if (server_cert != NULL)
    {
       if (SSL_get_verify_result(_SSLConnection) == X509_V_OK)
       {
           TLS_DEBUG(cerr << "Server Certificate verified.\n";)
       }
       else
       {
           TLS_DEBUG(cerr << "Server Certificate NOT verified\n");    
           return -1;
       }

       X509_free (server_cert);
    }
    else
    {
       TLS_DEBUG(cerr << "Server not certified\n";)
       return -1;
    }

    return ssl_rc;
}


//
// MP_Socket (Multi-purpose Socket class
//



MP_Socket::MP_Socket(Uint32 socket) 
 : _isSecure(false), _socket(socket) {}

MP_Socket::MP_Socket(Uint32 socket, SSLContext * sslcontext)
   throw(SSL_Exception)
{
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
}

MP_Socket::~MP_Socket()
{
    if (_isSecure)
        delete _sslsock;
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
   throw(SSL_Exception)
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
