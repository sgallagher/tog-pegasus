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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Dave Rosckes (rosckes@us.ibm.com)
//         Amit Arora, IBM (amita@in.ibm.com)
//         Heather Sterling, IBM (hsterl@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>

#include <iostream>
#include <cctype>
#include <cstdlib>
#include "Socket.h"
#include "TLS.h"
#include "HTTPConnection.h"
#include "MessageQueue.h"
#include "Monitor.h"
#include "HTTPMessage.h"
#include "Signal.h"
#include "Tracer.h"


#ifdef PEGASUS_KERBEROS_AUTHENTICATION
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif
#include <Pegasus/Common/XmlWriter.h>


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN


// initialize the request count

AtomicInt HTTPConnection::_requestCount = 0;

////////////////////////////////////////////////////////////////////////////////
//
// Local routines:
//
////////////////////////////////////////////////////////////////////////////////

static inline Uint32 _Min(Uint32 x, Uint32 y) 
{
    return x < y ? x : y; 
}

static char* _FindSeparator(const char* data, Uint32 size)
{
    const char* p = data;
    const char* end = p + size;

    while (p != end)
    {
	if (*p == '\r')
	{
	    Uint32 n = end - p;

	    if (n >= 2 && p[1] == '\n')
		return (char*)p;
	}
	else if (*p == '\n')
	    return (char*)p;

	p++;
    }

    return 0;
}

// Used to test signal handling
void * sigabrt_generator(void * parm)
{
    abort();
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
//
// HTTPConnection
//
////////////////////////////////////////////////////////////////////////////////

HTTPConnection::HTTPConnection(
    Monitor* monitor,
    //Sint32 socket, 
    AutoPtr<MP_Socket>& socket, 
    MessageQueue* ownerMessageQueue,
    MessageQueue* outputMessageQueue,
    Boolean exportConnection)
    : 
   Base(PEGASUS_QUEUENAME_HTTPCONNECTION), 
   _monitor(monitor),
   _socket(socket.get()), 
   _ownerMessageQueue(ownerMessageQueue),
   _outputMessageQueue(outputMessageQueue),
   _contentOffset(-1),
   _contentLength(-1),
   _connectionClosePending(false)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::HTTPConnection");

   //Socket::disableBlocking(_socket);
   _socket->disableBlocking();
   _authInfo = new AuthenticationInfo(true);

   // Add SSL verification information to the authentication information
   if (_socket->isSecure()) 
   {
       //
       // Set the flag to indicate that the request was received on
       // export Connection
       //
       if (exportConnection)
       {
          _authInfo->setExportConnection(exportConnection);
       }

       if (_socket->isPeerVerificationEnabled()) 
       {
           _authInfo->setPeerCertificate(_socket->getPeerCertificate());
           if (_socket->isCertificateVerified())
           {
               _authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);
           }
       }
   }

   _responsePending = false;
   _connectionRequestCount = 0;

   PEG_METHOD_EXIT();
}

HTTPConnection::~HTTPConnection()
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::~HTTPConnection");

    _socket->close();
    //delete _socket;
    delete _authInfo;

   PEG_METHOD_EXIT();
}


void HTTPConnection::handleEnqueue(Message *message)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::handleEnqueue");

   if( ! message )
   {
      PEG_METHOD_EXIT();
      return;
   }

   
   Boolean LockAcquired = false;

  if (pegasus_thread_self() != _connection_mut.get_owner())
  {
     _connection_mut.lock(pegasus_thread_self());  // Use lock_connection() ?
     LockAcquired = true;
  }

   switch (message->getType())
   {
      case SOCKET_MESSAGE:
      {

         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::handleEnqueue - SOCKET_MESSAGE");
	 
	 SocketMessage* socketMessage = (SocketMessage*)message;

	 if (socketMessage->events & SocketMessage::READ)
	    _handleReadEvent();

	 break;
      }

      case HTTP_MESSAGE:
      {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection::handleEnqueue - HTTP_MESSAGE");
	 
	 HTTPMessage* httpMessage = (HTTPMessage*)message;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
         // The following is processing to wrap (encrypt) the response from the
	 // server when using kerberos authentications.

	 // If the security association does not exist then kerberos authentication
	 // is not being used.
	 CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();
	 if (sa)
	 {
	     // The message needs to be parsed in order to distinguish between the
	     // headers and content. When parsing, the code breaks out
	     // of the loop as soon as it finds the double separator that terminates
	     // the headers so the headers and content can be easily separated.
	     // Parse the HTTP message:
	     String startLine;
	     Array<HTTPHeader> headers;
	     Uint32 contentLength = 0;
	     httpMessage->parse(startLine, headers, contentLength);

	     Boolean authrecExists = false;
	     String authorization = String::EMPTY;
	     if (HTTPMessage::lookupHeader(headers, "WWW-Authenticate", authorization, false))
	     {
		 authrecExists = true;
	     }

	     // The following is processing to wrap (encrypt) the response from the
	     // server when using kerberos authentications.
	     sa->wrapResponseMessage(httpMessage->message, contentLength, authrecExists);
	 }
#endif

	 // ATTN: convert over to asynchronous write scheme:

	 // Send response message to the client (use synchronous I/O for now:

	 _socket->enableBlocking();

	 const Array<Sint8>& buffer = httpMessage->message;
 
	 const Uint32 CHUNK_SIZE = 16 * 1024;

	 SignalHandler::ignore(PEGASUS_SIGPIPE);

	 // use the next four lines to test the SIGABRT handler
	 //getSigHandle()->registerHandler(PEGASUS_SIGABRT, sig_act);
	 //getSigHandle()->activate(PEGASUS_SIGABRT);
	 //Thread t(sigabrt_generator, NULL, false);
	 //t.run();

         Uint32 totalBytesWritten = 0;
	 for (Uint32 bytesRemaining = buffer.size(); bytesRemaining > 0; )
	 {
	    Uint32 bytesToWrite = _Min(bytesRemaining, CHUNK_SIZE);

	    Sint32 bytesWritten = _socket->write(
	       buffer.getData() + buffer.size() - bytesRemaining, 
	       bytesToWrite);

	    if (bytesWritten < 0)
	       break;
	    //throw ConnectionBroken();

            totalBytesWritten += bytesWritten;
	    bytesRemaining -= bytesWritten;
	 }

     //
     // handle automatic truststore update, if enabled
     //
     if (_socket->isSecure() && _socket->isPeerVerificationEnabled())
     {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
                       "Authenticated = %d; Username = %s",
                       _authInfo->isAuthenticated(), 
                       (const char*)_authInfo->getAuthenticatedUser().getCString());

         // If the client sent an untrusted certificate along with valid credentials
         // for the SSL truststore, add the certificate to the server's truststore.
         // This will fail in the addTrustedClient function if enableSSLTrustStoreAutoUpdate is 
         // not enabled.
         if (_authInfo->isAuthenticated() &&
             _authInfo->getPeerCertificate() && 
             _authInfo->getPeerCertificate()->getErrorCode() != SSLCertificateInfo::V_OK)
         {
             _socket->addTrustedClient(_authInfo->getAuthenticatedUser().getCString());
         }
     }

	 //
	 // decrement request count
	 //
	 _requestCount--;
         _responsePending = false;
	 _socket->disableBlocking();

         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "A response has been sent (%d of %d bytes have been writtten).\n"
            "There are %d requests pending within the CIM Server.\n" 
            "A total of %d requests have been processed on this connection.",
            totalBytesWritten,  buffer.size(), _requestCount.value(),
            _connectionRequestCount);

	 break;
      }

      default:
	 // ATTN: need unexpected message error!
	 break;
   }

   delete message;

   if (LockAcquired)
   {
      _connection_mut.unlock();  // Use unlock_connection() ?
   }
   PEG_METHOD_EXIT();
}


void HTTPConnection::handleEnqueue()
{
   Message* message = dequeue();

    if (!message)
        return;
    handleEnqueue(message);
}

Boolean _IsBodylessMessage(const char* line)
{
    //ATTN: Make sure this is the right place to check for HTTP/1.1 and
    //      HTTP/1.0 that is part of the authentication challenge header.
    // ATTN-RK-P2-20020305: How do we make sure we have the complete list?
    const char* METHOD_NAMES[] =
    {
        "GET",
        "HTTP/1.1 400",
        "HTTP/1.0 400",
        "HTTP/1.1 401",
        "HTTP/1.0 401",
        "HTTP/1.1 413",
        "HTTP/1.0 413",
        "HTTP/1.1 500",
        "HTTP/1.0 500",
        "HTTP/1.1 501",
        "HTTP/1.0 501",
        "HTTP/1.1 503",
        "HTTP/1.0 503"
    };

    const Uint32 METHOD_NAMES_SIZE = sizeof(METHOD_NAMES) / sizeof(char*);

    for (Uint32 i = 0; i < METHOD_NAMES_SIZE; i++)
    {
	Uint32 n = strlen(METHOD_NAMES[i]);

	if (strncmp(line, METHOD_NAMES[i], n) == 0 && isspace(line[n]))
	    return true;
    }

    return false;
}

void HTTPConnection::_getContentLengthAndContentOffset()
{
    char* data = (char*)_incomingBuffer.getData();
    Uint32 size = _incomingBuffer.size();
    char* line = (char*)data;
    char* sep;
    Uint32 lineNum = 0;
    Boolean bodylessMessage = false;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
	char save = *sep;
	*sep = '\0';

	// Did we find the double separator which terminates the headers?

	if (line == sep)
	{
	    *sep = save;
	    line = sep + ((save == '\r') ? 2 : 1);
	    _contentOffset = line - _incomingBuffer.getData();
	    break;
	}

	// If this is one of the bodyless methods, then we can assume the
	// message is complete when the "\r\n\r\n" is encountered.

	if (lineNum == 0 && _IsBodylessMessage(line))
	    bodylessMessage = true;

	// Look for the content-length if not already found:

	char* colon = strchr(line, ':');

	if (colon)
	{
	    *colon  = '\0';

	    if (System::strcasecmp(line, "content-length") == 0)
		_contentLength = atoi(colon + 1);

	    *colon = ':';
	}

	*sep = save;
	line = sep + ((save == '\r') ? 2 : 1);
	lineNum++;
    }

    if (_contentOffset != -1 && bodylessMessage)
	_contentLength = 0;
}

void HTTPConnection::_clearIncoming()
{
    _contentOffset = -1;
    _contentLength = -1;
    _incomingBuffer.clear();
}

void HTTPConnection::_closeConnection()
{
   // return - don't send the close connection message. 
   // let the monitor dispatch function do the cleanup. 
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::_closeConnection");
   _connectionClosePending = true; 

   if (_responsePending)
   {
       Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
         "HTTPConnection::_closeConnection - Connection being closed with response still pending.");
   }

   if (_connectionRequestCount == 0)
   {
       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
         "HTTPConnection::_closeConnection - Connection being closed without receiving any requests.");
   }

   PEG_METHOD_EXIT();

//     Message* message= new CloseConnectionMessage(_socket->getSocket));
//     message->dest = _ownerMessageQueue->getQueueId();
//    SendForget(message);
//    _ownerMessageQueue->enqueue(message);
}

void HTTPConnection::_handleReadEvent()
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::_handleReadEvent");

    // -- Append all data waiting on socket to incoming buffer:

#ifdef LOCK_CONNECTION_ENABLED
    lock_connection();
#endif
    Boolean bufferException = false;
    Sint32 bytesRead = 0;
    Boolean incompleteSecureReadOccurred = false;
    for (;;)
    {
	char buffer[4096];
        Sint32 n = _socket->read(buffer, sizeof(buffer));
	if (n <= 0)
	{
	    if (_socket->isSecure() && bytesRead == 0)
            {
	       // It is possible that SSL_read was not able to 
	       // read the entire SSL record.  This could happen
	       // if the record was send in multiple packets
	       // over the network and only some of the packets
	       // are available.  Since SSL requires the entire
	       // record to successfully decrypt, the SSL_read
	       // operation will return "0 bytes" read.
	       // Once all the bytes of the SSL record have been read,
	       // SSL_read will return the entire record.
	       // The following test was added to allow
	       // handleReadEvent to distinguish between a 
	       // disconnect and partial read of an SSL record.
	       //
               incompleteSecureReadOccurred = !_socket->incompleteReadOccurred(n);
            }
	    break;
	}

        try
        {
	    _incomingBuffer.append(buffer, n);
        }
        catch(...)
        {
            bufferException = true;
            _clearIncoming();
        }
        if (bufferException)
        {
            PEG_TRACE_STRING(TRC_HTTP, Tracer::LEVEL2, "HTTPConnection::_handleReadEvent: Unable to append the request to the input buffer");
            _requestCount++;
            Array<Sint8> message;
            message = XmlWriter::formatHttpErrorRspMessage( HTTP_STATUS_REQUEST_TOO_LARGE );
            HTTPMessage* httpMessage = new HTTPMessage( message );
            Tracer::traceBuffer(TRC_XML_IO, Tracer::LEVEL2,
	                        httpMessage->message.getData(),
                                httpMessage->message.size());
            handleEnqueue( httpMessage );
#ifdef LOCK_CONNECTION_ENABLED
            unlock_connection();
#endif
            _closeConnection(); 
	    return;
        }

	bytesRead += n;
    }

    
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
     "Total bytesRead = %d; Bytes read this iteration = %d", 
                _incomingBuffer.size(), bytesRead);

    if (_contentOffset == -1)
	_getContentLengthAndContentOffset();

    // -- See if the end of the message was reached (some peers signal end of 
    // -- the message by closing the connection; others use the content length
    // -- HTTP header and then there are those messages which have no bodies
    // -- at all).

    if ((bytesRead == 0 && !incompleteSecureReadOccurred) ||  
	_contentLength != -1 && 
	(Sint32(_incomingBuffer.size()) >= _contentLength + _contentOffset))
    {
	HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
        message->authInfo = _authInfo;

        //
        // increment request count 
        //
        if (bytesRead > 0)
        {
           _requestCount++;
           _connectionRequestCount++;
           _responsePending = true;
        }
        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          "_requestCount = %d", _requestCount.value());
	message->dest = _outputMessageQueue->getQueueId();
//	SendForget(message);
	
#ifndef LOCK_CONNECTION_ENABLED
	_outputMessageQueue->enqueue(message);
#endif
	_clearIncoming();

#ifdef LOCK_CONNECTION_ENABLED
        unlock_connection();

	if (bytesRead > 0)
        {
	   _outputMessageQueue->enqueue(message);
        }
        else 
#else
	if (bytesRead == 0)
#endif
	{
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
			 "HTTPConnection::_handleReadEvent - bytesRead == 0 - Conection being closed.");
	   _closeConnection();
	   
	   //
	   // decrement request count
	   //
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			 "_requestCount = %d", _requestCount.value());
	   
	   PEG_METHOD_EXIT();
	   return;
	}
    }
    PEG_METHOD_EXIT();
}

Uint32 HTTPConnection::getRequestCount()
{
    return(_requestCount.value());
}


Boolean HTTPConnection::run(Uint32 milliseconds)
{
   Boolean handled_events = false;
   int events = 0;
   
   fd_set fdread; // , fdwrite;
   struct timeval tv = { 0, 1 };
   do 
   {
      FD_ZERO(&fdread);
      FD_SET(getSocket(), &fdread);
      events = select(FD_SETSIZE, &fdread, NULL, NULL, &tv);
#ifdef PEGASUS_OS_TYPE_WINDOWS
      if(events == SOCKET_ERROR)
#else
      if(events == -1)
#endif
      {
	 return false;
      }

      if (events)
      {
	 events = 0;
	 if( FD_ISSET(getSocket(), &fdread))
	 {
	    events |= SocketMessage::READ;
	    Message *msg = new SocketMessage(getSocket(), events);
	    try 
	    {
	       handleEnqueue(msg);
	    }
	    catch(...)
	    {
              Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                  "HTTPConnection::run handleEnqueue(msg) failure");
	       return true;
	    }
	    handled_events = true;
	 }
      }
   } while(events != 0 && !_connectionClosePending);
   return handled_events;
}

AtomicInt HTTPConnection2::_requestCount(0);


HTTPConnection2::HTTPConnection2(pegasus_socket socket,
				 MessageQueue* outputMessageQueue)
    : 
   Base(PEGASUS_QUEUENAME_HTTPCONNECTION), 
   _socket(socket), 
   _outputMessageQueue(outputMessageQueue),
   _contentOffset(-1),
   _contentLength(-1),
   _closed(0)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::HTTPConnection2");

   _authInfo = new AuthenticationInfo(true);

   // add SSL verification information to the authentication information
   if (_socket.is_secure() && _socket.isPeerVerificationEnabled()) 
   {
       _authInfo->setPeerCertificate(_socket.getPeerCertificate());
       if (_socket.isCertificateVerified())
       {
           _authInfo->setAuthStatus(AuthenticationInfoRep::AUTHENTICATED);
       }
   }

   PEG_METHOD_EXIT();
}

HTTPConnection2::~HTTPConnection2()
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::~HTTPConnection2");
   try 
   {
      _close_connection();
   }
   catch(...)
   {
   }
   
    delete _authInfo;

   PEG_METHOD_EXIT();
}


void HTTPConnection2::handleEnqueue(Message *message)
{

   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::handleEnqueue");
 

   switch (message->getType())
   {
      case HTTP_MESSAGE:
      {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::handleEnqueue - HTTP_MESSAGE");
	 
	 HTTPMessage* httpMessage = (HTTPMessage*)message;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
         // The following is processing to wrap (encrypt) the response from the
	 // server when using kerberos authentication.

	 // If the security association does not exist then kerberos authentication
	 // is not being used.
	 CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();
	 if (sa)
	 {
	     // The message needs to be parsed in order to distinguish between the
	     // headers and content. When parsing, the code breaks out
	     // of the loop as soon as it finds the double separator that terminates
	     // the headers so the headers and content can be easily separated.
	     // Parse the HTTP message:
	     String startLine;
	     Array<HTTPHeader> headers;
	     Uint32 contentLength = 0;
	     httpMessage->parse(startLine, headers, contentLength);

	     Boolean authrecExists = false;
	     String authorization = String::EMPTY;
	     if (HTTPMessage::lookupHeader(headers, "WWW-Authenticate", authorization, false))
	     {
		 authrecExists = true;
	     }

	     // The following is processing to wrap (encrypt) the response from the
	     // server when using kerberos authentications.
	     sa->wrapResponseMessage(httpMessage->message, contentLength, authrecExists);
	 }
#endif

	 //------------------------------------------------------------
	 // There is no need to convert the write calls to be asynchronous.
	 // The write is happening on a dedicated thread (not the monitor thread)
	 // so it is just fine if it blocks. << Tue Oct  7 09:48:06 2003 mdd >>
	 //------------------------------------------------------------

	 const Array<Sint8>& buffer = httpMessage->message;
 
	 const Uint32 CHUNK_SIZE = 16 * 1024;

	 SignalHandler::ignore(PEGASUS_SIGPIPE);

         Uint32 totalBytesWritten = 0;
	 for (Uint32 bytesRemaining = buffer.size(); bytesRemaining > 0; )
	 {
	    Uint32 bytesToWrite = _Min(bytesRemaining, CHUNK_SIZE);

	    Sint32 bytesWritten = _socket.write(
	       buffer.getData() + buffer.size() - bytesRemaining, 
	       bytesToWrite);

	    if (bytesWritten < 0)
	       break;

            totalBytesWritten += bytesWritten;
	    bytesRemaining -= bytesWritten;
	 }

     //
     // handle automatic truststore update, if enabled
     //
     if (_socket.is_secure() && _socket.isPeerVerificationEnabled())
     {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
                       "Authenticated = %d; Username = %s",
                       _authInfo->isAuthenticated(), 
                       (const char*)_authInfo->getAuthenticatedUser().getCString());

         // If the client sent an untrusted certificate along with valid credentials
         // for the SSL truststore, add the certificate to the server's truststore.
         // This will fail in the addTrustedClient function if enableSSLTrustStoreAutoUpdate is 
         // not enabled.
         if (_authInfo->isAuthenticated() &&
             _authInfo->getPeerCertificate() && 
             _authInfo->getPeerCertificate()->getErrorCode() != SSLCertificateInfo::V_OK)
         {
             _socket.addTrustedClient(_authInfo->getAuthenticatedUser().getCString());
         }
         
     }

	 //
	 // decrement request count
	 //
	 _requestCount--;

         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "Total bytes written = %d; Buffer Size = %d; _requestCount = %d",
             totalBytesWritten,  buffer.size(), _requestCount.value());

	 break;
      }

      default:
	 // ATTN: need unexpected message error!
	 break;
   }

   delete message;

   PEG_METHOD_EXIT();
}

void HTTPConnection2::enqueue(Message* message) throw(IPCException)
{
   _reentry.lock(pegasus_thread_self());
   if(_closed.value())
   {
      delete message;
      
   }
   else
   {
      handleEnqueue(message);
   }
   _reentry.unlock();
}


void HTTPConnection2::handleEnqueue()
{
   Message* message = dequeue();

    if (!message)
        return;
    handleEnqueue(message);
}

Sint32 HTTPConnection2::getSocket(void)
{
   return (Sint32)_socket;
   
}


void HTTPConnection2::_getContentLengthAndContentOffset()
{
    char* data = (char*)_incomingBuffer.getData();
    Uint32 size = _incomingBuffer.size();
    char* line = (char*)data;
    char* sep;
    Uint32 lineNum = 0;
    Boolean bodylessMessage = false;

    while ((sep = _FindSeparator(line, size - (line - data))))
    {
	char save = *sep;
	*sep = '\0';

	// Did we find the double separator which terminates the headers?

	if (line == sep)
	{
	    *sep = save;
	    line = sep + ((save == '\r') ? 2 : 1);
	    _contentOffset = line - _incomingBuffer.getData();
	    break;
	}

	// If this is one of the bodyless methods, then we can assume the
	// message is complete when the "\r\n\r\n" is encountered.

	if (lineNum == 0 && _IsBodylessMessage(line))
	    bodylessMessage = true;

	// Look for the content-length if not already found:

	char* colon = strchr(line, ':');

	if (colon)
	{
	    *colon  = '\0';

	    if (System::strcasecmp(line, "content-length") == 0)
		_contentLength = atoi(colon + 1);

	    *colon = ':';
	}

	*sep = save;
	line = sep + ((save == '\r') ? 2 : 1);
	lineNum++;
    }

    if (_contentOffset != -1 && bodylessMessage)
	_contentLength = 0;
}

void HTTPConnection2::_clearIncoming()
{
    _contentOffset = -1;
    _contentLength = -1;
    _incomingBuffer.clear();
}

void HTTPConnection2::_handleReadEvent(monitor_2_entry* entry)
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::_handleReadEvent");

    // -- Append all data waiting on socket to incoming buffer:

    _socket.disableBlocking();
    Sint32 bytesRead = 0;
    Boolean incompleteSecureReadOccurred = false;
    Boolean would_block = false;

   Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "Doing a read on %d.", (Sint32)entry->get_sock());

    for (;;)
    {
	char buffer[4096];
	
        Sint32 n = _socket.read(buffer, sizeof(buffer));

//------------------------------------------------------------
// Note on reading non-blocking sockets: 
// If there is no data to read from a non-blocking socket, the return
// code from _socket.read will be a -1 and errno will be EWOULDBLOCK.
// The correct thing to do in this case is nothing at all. The client on 
// the other end of the connection has not closed the socket, and the 
// connection will have data to read very soon. The connection needs to 
// be present in the monitor's select loop so it can be read as soon as the 
// data arrives. << Tue Oct  7 09:45:37 2003 mdd >>
//------------------------------------------------------------

#ifdef PEGASUS_OS_TYPE_WINDOWS
	if( n == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK )
#else
	if( n == -1 && errno == EWOULDBLOCK )
#endif
	{
	   would_block = true;
	}

	if (n <= 0)
	{
	    if (_socket.is_secure() && bytesRead == 0)
            {
	       // It is possible that SSL_read was not able to 
	       // read the entire SSL record.  This could happen
	       // if the record was send in multiple packets
	       // over the network and only some of the packets
	       // are available.  Since SSL requires the entire
	       // record to successfully decrypt, the SSL_read
	       // operation will return "0 bytes" read.
	       // Once all the bytes of the SSL record have been read,
	       // SSL_read will return the entire record.
	       // The following test was added to allow
	       // handleReadEvent to distinguish between a 
	       // disconnect and partial read of an SSL record.
	       //
               incompleteSecureReadOccurred = !_socket.incompleteReadOccurred(n);
            }

	    break;
	}

	
	_incomingBuffer.append(buffer, n);
	bytesRead += n;
    }
    _socket.enableBlocking();
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
     "_socket.read bytesRead = %d", bytesRead);
   
    // -- If still waiting for beginning of content!

    if (_contentOffset == -1)
	_getContentLengthAndContentOffset();

    // -- See if the end of the message was reached (some peers signal end of 
    // -- the message by closing the connection; others use the content length
    // -- HTTP header and then there are those messages which have no bodies
    // -- at all).

    if ((bytesRead == 0 && !incompleteSecureReadOccurred) ||  
	_contentLength != -1 && 
	(Sint32(_incomingBuffer.size()) >= _contentLength + _contentOffset))
    {
	if (bytesRead > 0)
        {
           // We are setting the state of entry as IDLE so that monitor_2::run 
           // does a select on this FD. And only when next time _handleReadEvent
           // gets called, bytesRead would be 0, and the state of the entry
           // would be changed to CLOSED.
           entry->set_state(IDLE);
	   delete entry;
	   HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
	   message->authInfo = _authInfo;
	   
	   //
	   // increment request count 
	   //
	   _requestCount++;
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			 "_requestCount = %d", _requestCount.value());
	   message->dest = _outputMessageQueue->getQueueId();
	   
	   _clearIncoming();
	   _outputMessageQueue->enqueue(message);
        }
        else if (bytesRead == 0)
	{
	   if(would_block == false)
	   {
	      
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
			 "HTTPConnection2::_handleReadEvent - bytesRead == 0 - Conection being closed.");
	   
	   //
	   // decrement request count
	   //

           // Commenting out below line, since the decrement is happening
           // twice, and increment only once for every connection. One
           // decrement was happening here and another by handleEnqueue()
           // after response has been written on the socket.

	   // _requestCount--;

	   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			 "_requestCount = %d", _requestCount.value());
	   
	   _close_connection();
       if(entry->get_type() != CLIENTSESSION)
	     entry->set_state(CLOSED);
	   delete entry;
	   
	   }
	}
    }
    PEG_METHOD_EXIT();
}


void HTTPConnection2::_close_connection(void)
{
   _reentry.lock(pegasus_thread_self());
   
   _closed = 1;
   remove_myself(_queueId);
   _reentry.unlock();
   

}


Uint32 HTTPConnection2::getRequestCount()
{
    return(_requestCount.value());
}


Boolean HTTPConnection2::operator ==(const HTTPConnection2& h2)
{
  if(this == &h2)
    return true;
  return false;
}

Boolean HTTPConnection2::operator ==(void* h2)
{
  if((void *)this == h2)
    return true;
  return false;
}


void HTTPConnection2::connection_dispatch(monitor_2_entry* entry)
{
  HTTPConnection2* myself = (HTTPConnection2*) entry->get_dispatch();
  myself->_socket = entry->get_sock();
  myself->_handleReadEvent(entry);
  
}


PEGASUS_NAMESPACE_END
