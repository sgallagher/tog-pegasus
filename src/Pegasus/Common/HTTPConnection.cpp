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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company(nagaraja_boranna@hp.com)
//         Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//         Dave Rosckes (rosckes@us.ibm.com)
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
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/CIMKerberosSecurityAssociation.h>
#endif


PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
/**
    Constant representing the Kerberos authentication challenge header.
*/
static const String KERBEROS_CHALLENGE_HEADER = "WWW-Authenticate: Negotiate ";
#endif

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
    MP_Socket* socket, 
    MessageQueue* ownerMessageQueue,
    MessageQueue* outputMessageQueue)
    : 
   Base(PEGASUS_QUEUENAME_HTTPCONNECTION), 
   _monitor(monitor),
   _socket(socket), 
   _ownerMessageQueue(ownerMessageQueue),
   _outputMessageQueue(outputMessageQueue),
   _contentOffset(-1),
   _contentLength(-1)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::HTTPConnection");

   //Socket::disableBlocking(_socket);
   _socket->disableBlocking();
   _authInfo = new AuthenticationInfo(true);

   PEG_METHOD_EXIT();
}

HTTPConnection::~HTTPConnection()
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::~HTTPConnection");

    _socket->close();
    delete _socket;
    delete _authInfo;

   PEG_METHOD_EXIT();
}


void HTTPConnection::handleEnqueue(Message *message)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection::handleEnqueue");

   if( ! message || _dying.value() > 0 )
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
         // TODO::KERBEROS complete and verify code
         CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();
         // Determine if message came from CIMOperationResponseEncoder and Kerberos is being used.
         if ((int)httpMessage->authInfo == 99 && sa)
         {
             char* outmessage = NULL;
             Uint64   outlength = 0;
             Array<Sint8> final_buffer;
             final_buffer.clear();
             Array<Sint8> header_buffer;
             header_buffer.clear();
             Array<Sint8> unwrapped_content_buffer;
             unwrapped_content_buffer.clear();
             if (sa->getClientAuthenticated())
             { 
                 // TODO::KERBEROS Question - will parse be able to distinguish headers from 
                 //     contents when the contents is wrapped??? I am thinking we are okay
                 //     because the code breaks out of the loop as soon as it finds the 
                 //     double separator that terminates the headers.
                 // Parse the HTTP message:
                 String startLine;
                 Array<HTTPHeader> headers;
                 Uint32 contentLength;
                 httpMessage->parse(startLine, headers, contentLength);

                 for (Uint64 i = 0; i < (httpMessage->message.size()-contentLength); i++)
                 {
                     header_buffer.append(httpMessage->message[i]);
                 }

                 for (Uint64 i = (httpMessage->message.size()-contentLength); i < httpMessage->message.size(); i++)
                 {
                      unwrapped_content_buffer.append(outmessage[i]);
                 }

                 if (sa->wrap_message((const char*)unwrapped_content_buffer.getData(),
                                      (Uint64)unwrapped_content_buffer.size(),
                                       outmessage,
                                       outlength))
                 {
                         // build a bad request
                         final_buffer = XmlWriter::formatHttpErrorRspMessage(HTTP_STATUS_BADREQUEST);
                 }
             }
             //  Note:  wrap_message can result in the client no longer being authenticated so the 
             //  flag needs to be checked.  
             if (!sa->getClientAuthenticated())
             {
                  if (final_buffer.size() == 0)
                  {
                      // set authenticated flag in _authInfo to not authenticated because the
                      // wrap resulted in an expired token or credential.
                      _authInfo->setAuthStatus(AuthenticationInfoRep::CHALLENGE_SENT);
                      // build a 401 response 
                      // do we need to add a token here or just restart the negotiate again???
                      // authResponse.append(sa->getServerToken());
                      XmlWriter::appendUnauthorizedResponseHeader(final_buffer, KERBEROS_CHALLENGE_HEADER);
                  }
             }
             else
             {
                  if (final_buffer.size() == 0 && outlength > 0)
                  {
                      Array<Sint8> wrapped_content_buffer;
                      wrapped_content_buffer.clear();
                      for (Uint64 i = 0; i < outlength; i++)
                      {
                          wrapped_content_buffer.append(outmessage[i]);
                      }
                      final_buffer.appendArray(header_buffer);
                      final_buffer.appendArray(wrapped_content_buffer);
                  }
             }

             if (outmessage)
                 delete [] outmessage;  // outmessage is no longer needed

             if (final_buffer.size())
             {
                 httpMessage->message.clear();
                 httpMessage->message = final_buffer;
             }
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
	 // decrement request count
	 //
	 _requestCount--;
	 _socket->disableBlocking();

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
        "HTTP/1.1 501",
        "HTTP/1.0 501"
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
   _dying = 1;
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

	_incomingBuffer.append(buffer, n);
	bytesRead += n;
    }

    
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
     "_socket->read bytesRead = %d", bytesRead);
   
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
	HTTPMessage* message = new HTTPMessage(_incomingBuffer, getQueueId());
        message->authInfo = _authInfo;

        //
        // increment request count 
        //
        _requestCount++;
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
	   _requestCount--;
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
   if( _dying.value() > 0)
      return false;
   
   Boolean handled_events = false;
   int events = 0;
   
   fd_set fdread; // , fdwrite;
   do 
   {
      struct timeval tv = { 0, 1 };
      FD_ZERO(&fdread);
      FD_SET(getSocket(), &fdread);
      events = select(FD_SETSIZE, &fdread, NULL, NULL, &tv);
#ifdef PEGASUS_OS_TYPE_WINDOWS
      if(events && events != SOCKET_ERROR && _dying.value() == 0 )
#else
      if(events && events != -1 && _dying.value() == 0 )
#endif
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
	       _monitor->_entries[_entry_index]._status = _MonitorEntry::IDLE;
	       return true;
	    }
	    handled_events = true;
	 }
	 else 
	    break;
      }
      else
	 break;
   } while(events != 0 && _dying.value() == 0);
   _monitor->_entries[_entry_index]._status = _MonitorEntry::IDLE;
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
   _contentLength(-1)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::HTTPConnection2");

   _authInfo = new AuthenticationInfo(true);

   PEG_METHOD_EXIT();
}

HTTPConnection2::~HTTPConnection2()
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::~HTTPConnection2");

    delete _authInfo;

   PEG_METHOD_EXIT();
}


void HTTPConnection2::handleEnqueue(Message *message)
{
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::handleEnqueue");

   if( ! message || _dying.value() > 0 )
   {
      PEG_METHOD_EXIT();
      return;
   }

   
//    Boolean LockAcquired = false;

//   if (pegasus_thread_self() != _connection_mut.get_owner())
//   {
//      _connection_mut.lock(pegasus_thread_self());  // Use lock_connection() ?
//      LockAcquired = true;
//   }

   switch (message->getType())
   {
      case SOCKET_MESSAGE:
      {

         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::handleEnqueue - SOCKET_MESSAGE");
	 
	 SocketMessage* socketMessage = (SocketMessage*)message;

// 	 if (socketMessage->events & SocketMessage::READ)
// 	    _handleReadEvent();

	 break;
      }

      case HTTP_MESSAGE:
      {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
            "HTTPConnection2::handleEnqueue - HTTP_MESSAGE");
	 
	 HTTPMessage* httpMessage = (HTTPMessage*)message;

#ifdef PEGASUS_KERBEROS_AUTHENTICATION
         // TODO::KERBEROS complete and verify code
         CIMKerberosSecurityAssociation *sa = _authInfo->getSecurityAssociation();
         // Determine if message came from CIMOperationResponseEncoder and Kerberos is being used.
         if ((int)httpMessage->authInfo == 99 && sa)
         {
             char* outmessage = NULL;
             Uint64   outlength = 0;
             Array<Sint8> final_buffer;
             final_buffer.clear();
             Array<Sint8> header_buffer;
             header_buffer.clear();
             Array<Sint8> unwrapped_content_buffer;
             unwrapped_content_buffer.clear();
             if (sa->getClientAuthenticated())
             { 
                 // TODO::KERBEROS Question - will parse be able to distinguish headers from 
                 //     contents when the contents is wrapped??? I am thinking we are okay
                 //     because the code breaks out of the loop as soon as it finds the 
                 //     double separator that terminates the headers.
                 // Parse the HTTP message:
                 String startLine;
                 Array<HTTPHeader> headers;
                 Uint32 contentLength;
                 httpMessage->parse(startLine, headers, contentLength);

                 for (Uint64 i = 0; i < (httpMessage->message.size()-contentLength); i++)
                 {
                     header_buffer.append(httpMessage->message[i]);
                 }

                 for (Uint64 i = (httpMessage->message.size()-contentLength); i < httpMessage->message.size(); i++)
                 {
                      unwrapped_content_buffer.append(outmessage[i]);
                 }

                 if (sa->wrap_message((const char*)unwrapped_content_buffer.getData(),
                                      (Uint64)unwrapped_content_buffer.size(),
                                       outmessage,
                                       outlength))
                 {
                         // build a bad request
                         final_buffer = XmlWriter::formatHttpErrorRspMessage(HTTP_STATUS_BADREQUEST);
                 }
             }
             //  Note:  wrap_message can result in the client no longer being authenticated so the 
             //  flag needs to be checked.  
             if (!sa->getClientAuthenticated())
             {
                  if (final_buffer.size() == 0)
                  {
                      // set authenticated flag in _authInfo to not authenticated because the
                      // wrap resulted in an expired token or credential.
                      _authInfo->setAuthStatus(AuthenticationInfoRep::CHALLENGE_SENT);
                      // build a 401 response 
                      // do we need to add a token here or just restart the negotiate again???
                      // authResponse.append(sa->getServerToken());
                      XmlWriter::appendUnauthorizedResponseHeader(final_buffer, KERBEROS_CHALLENGE_HEADER);
                  }
             }
             else
             {
                  if (final_buffer.size() == 0 && outlength > 0)
                  {
                      Array<Sint8> wrapped_content_buffer;
                      wrapped_content_buffer.clear();
                      for (Uint64 i = 0; i < outlength; i++)
                      {
                          wrapped_content_buffer.append(outmessage[i]);
                      }
                      final_buffer.appendArray(header_buffer);
                      final_buffer.appendArray(wrapped_content_buffer);
                  }
             }

             if (outmessage)
                 delete [] outmessage;  // outmessage is no longer needed

             if (final_buffer.size())
             {
                 httpMessage->message.clear();
                 httpMessage->message = final_buffer;
             }
         }
#endif

	 // ATTN: convert over to asynchronous write scheme:

	 // Send response message to the client (use synchronous I/O for now:


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

	    Sint32 bytesWritten = _socket.write(
	       buffer.getData() + buffer.size() - bytesRemaining, 
	       bytesToWrite);

	    if (bytesWritten < 0)
	       break;
	    //throw ConnectionBroken();

            totalBytesWritten += bytesWritten;
	    bytesRemaining -= bytesWritten;
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

//    if (LockAcquired)
//    {
//       _connection_mut.unlock();  // Use unlock_connection() ?
//    }
   PEG_METHOD_EXIT();
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

void HTTPConnection2::_closeConnection()
{
   // return - don't send the close connection message. 
   // let the monitor dispatch function do the cleanup. 
   PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::_closeConnection");
   
   PEG_METHOD_EXIT();

}

void HTTPConnection2::_handleReadEvent(monitor_2_entry* entry)
{
    PEG_METHOD_ENTER(TRC_HTTP, "HTTPConnection2::_handleReadEvent");

    // -- Append all data waiting on socket to incoming buffer:

    lock_connection();
    _socket.disableBlocking();
    Sint32 bytesRead = 0;
    Boolean incompleteSecureReadOccurred = false;
    for (;;)
    {
	char buffer[4096];
	
        Sint32 n = _socket.read(buffer, sizeof(buffer));
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
	if(n < 0 )
	   continue;
	
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

        unlock_connection();

	if (bytesRead > 0)
        {
	   _outputMessageQueue->enqueue(message);
        }
        else if (bytesRead == 0)

	{
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL3,
			 "HTTPConnection2::_handleReadEvent - bytesRead == 0 - Conection being closed.");
	   _closeConnection(); 
	   entry->set_state(CLOSED);
	   
	   //
	   // decrement request count
	   //
	   _requestCount--;
	   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			 "_requestCount = %d", _requestCount.value());
	   
	   PEG_METHOD_EXIT();
	   return;
	}
    }
    PEG_METHOD_EXIT();
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
