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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Mike Day (monitor_2) mdday@us.ibm.com
//              Amit K Arora (Bug#1153) amita@in.ibm.com
//              Alagaraja Ramasubramanian (alags_raj@in.ibm.com) for Bug#1090
//              Sushma Fernandes (sushma@hp.com) for Bug#2057
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <cstring>
#include "Monitor.h"
#include "MessageQueue.h"
#include "Socket.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Exception.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# if defined(FD_SETSIZE) && FD_SETSIZE != 1024
#  error "FD_SETSIZE was not set to 1024 prior to the last inclusion \
of <winsock.h>. It may have been indirectly included (e.g., by including \
<windows.h>). Finthe inclusion of that header which is visible to this \
compilation unit and #define FD_SETZIE to 1024 prior to that inclusion; \
otherwise, less than 64 clients (the default) will be able to connect to the \
CIMOM. PLEASE DO NOT SUPPRESS THIS WARNING; PLEASE FIX THE PROBLEM."

# endif
# define FD_SETSIZE 1024
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// Define a platform-neutral socket length type
#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_OS_VMS)
typedef size_t PEGASUS_SOCKLEN_T;
#elif defined(PEGASUS_PLATFORM_AIX_RS_IBMCXX) || defined(PEGASUS_OS_LINUX) || (defined(PEGASUS_OS_SOLARIS) && !defined(SUNOS_5_6))
typedef socklen_t PEGASUS_SOCKLEN_T;
#else
typedef int PEGASUS_SOCKLEN_T;
#endif

static AtomicInt _connections = 0;

////////////////////////////////////////////////////////////////////////////////
//
// Monitor
//
////////////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_MONITOR_ENTRIES  32
Monitor::Monitor()
   : _stopConnections(0),
     _stopConnectionsSem(0),
     _solicitSocketCount(0),
     _tickle_client_socket(-1),
     _tickle_server_socket(-1),
     _tickle_peer_socket(-1)
{
    int numberOfMonitorEntriesToAllocate = MAX_NUMBER_OF_MONITOR_ENTRIES;
    Socket::initializeInterface();
    _entries.reserveCapacity(numberOfMonitorEntriesToAllocate);

    // setup the tickler
    initializeTickler();

    // Start the count at 1 because initilizeTickler()
    // has added an entry in the first position of the
    // _entries array
    for( int i = 1; i < numberOfMonitorEntriesToAllocate; i++ )
    {
       _MonitorEntry entry(0, 0, 0);
       _entries.append(entry);
    }
}

Monitor::~Monitor()
{
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "uninitializing interface");

    try{
        if(_tickle_peer_socket >= 0)
        {
            Socket::close(_tickle_peer_socket);
        }
        if(_tickle_client_socket >= 0)
        {
            Socket::close(_tickle_client_socket);
        }
        if(_tickle_server_socket >= 0)
        {
            Socket::close(_tickle_server_socket);
        }
    }
    catch(...)
    {
        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "Failed to close tickle sockets");
    }

    Socket::uninitializeInterface();
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "returning from monitor destructor");
}

void Monitor::initializeTickler(){
    /*
       NOTE: On any errors trying to
             setup out tickle connection,
             throw an exception/end the server
    */

    /* setup the tickle server/listener */

    // get a socket for the server side
    if((_tickle_server_socket = ::socket(PF_INET, SOCK_STREAM, 0)) < 0){
	//handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_CREATE",
				 "Received error number $0 while creating the internal socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    // initialize the address
    memset(&_tickle_server_addr, 0, sizeof(_tickle_server_addr));
#ifdef PEGASUS_OS_ZOS
    _tickle_server_addr.sin_addr.s_addr = inet_addr_ebcdic("127.0.0.1");
#else
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
#endif
    _tickle_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(0)
#endif
#endif
    _tickle_server_addr.sin_family = PF_INET;
    _tickle_server_addr.sin_port = 0;

    PEGASUS_SOCKLEN_T _addr_size = sizeof(_tickle_server_addr);

    // bind server side to socket
    if((::bind(_tickle_server_socket,
               reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
	       sizeof(_tickle_server_addr))) < 0){
	// handle error
#ifdef PEGASUS_OS_ZOS
    MessageLoaderParms parms("Common.Monitor.TICKLE_BIND_LONG",
				 "Received error:$0 while binding the internal socket.",strerror(errno));
#else
	MessageLoaderParms parms("Common.Monitor.TICKLE_BIND",
				 "Received error number $0 while binding the internal socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
#endif
        throw Exception(parms);
    }

    // tell the kernel we are a server
    if((::listen(_tickle_server_socket,3)) < 0){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_LISTEN",
			 "Received error number $0 while listening to the internal socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    // make sure we have the correct socket for our server
    int sock = ::getsockname(_tickle_server_socket,
                   reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                   &_addr_size);
    if(sock < 0){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_SOCKNAME",
			 "Received error number $0 while getting the internal socket name.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    /* set up the tickle client/connector */

    // get a socket for our tickle client
    if((_tickle_client_socket = ::socket(PF_INET, SOCK_STREAM, 0)) < 0){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_CREATE",
			 "Received error number $0 while creating the internal client socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    // setup the address of the client
    memset(&_tickle_client_addr, 0, sizeof(_tickle_client_addr));
#ifdef PEGASUS_OS_ZOS
    _tickle_client_addr.sin_addr.s_addr = inet_addr_ebcdic("127.0.0.1");
#else
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
#endif
    _tickle_client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(0)
#endif
#endif
    _tickle_client_addr.sin_family = PF_INET;
    _tickle_client_addr.sin_port = 0;

    // bind socket to client side
    if((::bind(_tickle_client_socket,
               reinterpret_cast<struct sockaddr*>(&_tickle_client_addr),
	       sizeof(_tickle_client_addr))) < 0){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_BIND",
			 "Received error number $0 while binding the internal client socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    // connect to server side
    if((::connect(_tickle_client_socket,
                  reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
		  sizeof(_tickle_server_addr))) < 0){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_CONNECT",
			 "Received error number $0 while connecting the internal client socket.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }

    /* set up the slave connection */
    memset(&_tickle_peer_addr, 0, sizeof(_tickle_peer_addr));
    PEGASUS_SOCKLEN_T peer_size = sizeof(_tickle_peer_addr);
    pegasus_sleep(1);

    // this call may fail, we will try a max of 20 times to establish this peer connection
    if((_tickle_peer_socket = ::accept(_tickle_server_socket,
            reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
            &peer_size)) < 0){
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
        // Only retry on non-windows platforms.
        if(_tickle_peer_socket == -1 && errno == EAGAIN)
        {
          int retries = 0;
          do
          {
            pegasus_sleep(1);
            _tickle_peer_socket = ::accept(_tickle_server_socket,
                reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
                &peer_size);
            retries++;
          } while(_tickle_peer_socket == -1 && errno == EAGAIN && retries < 20);
        }
#endif
    }
    if(_tickle_peer_socket == -1){
	// handle error
	MessageLoaderParms parms("Common.Monitor.TICKLE_ACCEPT",
			 "Received error number $0 while accepting the internal socket connection.",
#if !defined(PEGASUS_OS_TYPE_WINDOWS)
				 errno);
#else
				 WSAGetLastError());
#endif
	throw Exception(parms);
    }
    // add the tickler to the list of entries to be monitored and set to IDLE because Monitor only
    // checks entries with IDLE state for events
    _MonitorEntry entry(_tickle_peer_socket, 1, INTERNAL);
    entry._status = _MonitorEntry::IDLE;
    _entries.append(entry);
}

void Monitor::tickle(void)
{
    static char _buffer[] =
    {
      '0','0'
    };

    AutoMutex autoMutex(_tickle_mutex);
    Socket::disableBlocking(_tickle_client_socket);
    Socket::write(_tickle_client_socket,&_buffer, 2);
    Socket::enableBlocking(_tickle_client_socket);
}

void Monitor::setState( Uint32 index, _MonitorEntry::entry_status status )
{
    // Set the state to requested state
    _entries[index]._status = status;
}

Boolean Monitor::run(Uint32 milliseconds)
{

    Boolean handled_events = false;
    int i = 0;

    struct timeval tv = {milliseconds/1000, milliseconds%1000*1000};

    fd_set fdread;
    FD_ZERO(&fdread);

    AutoMutex autoEntryMutex(_entry_mut);

    // Check the stopConnections flag.  If set, clear the Acceptor monitor entries
    if (_stopConnections == 1)
    {
        for ( int indx = 0; indx < (int)_entries.size(); indx++)
        {
            if (_entries[indx]._type == Monitor::ACCEPTOR)
            {
                if ( _entries[indx]._status.value() != _MonitorEntry::EMPTY)
                {
                   if ( _entries[indx]._status.value() == _MonitorEntry::IDLE ||
                        _entries[indx]._status.value() == _MonitorEntry::DYING )
                   {
                       // remove the entry
		       _entries[indx]._status = _MonitorEntry::EMPTY;
                   }
                   else
                   {
                       // set status to DYING
                      _entries[indx]._status = _MonitorEntry::DYING;
                   }
               }
           }
        }
        _stopConnections = 0;
	_stopConnectionsSem.signal();
    }

    for( int indx = 0; indx < (int)_entries.size(); indx++)
    {
			 const _MonitorEntry &entry = _entries[indx];
       if ((entry._status.value() == _MonitorEntry::DYING) &&
					 (entry._type == Monitor::CONNECTION))
       {
          MessageQueue *q = MessageQueue::lookup(entry.queueId);
          PEGASUS_ASSERT(q != 0);
          HTTPConnection &h = *static_cast<HTTPConnection *>(q);

					if (h._connectionClosePending == false)
						continue;

					// NOTE: do not attempt to delete while there are pending responses
					// coming thru. The last response to come thru after a
					// _connectionClosePending will reset _responsePending to false
					// and then cause the monitor to rerun this code and clean up.
					// (see HTTPConnection.cpp)

					if (h._responsePending == true)
					{
						Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "Monitor::run - "
													"Ignoring connection delete request because "
													"responses are still pending. "
													"connection=0x%p, socket=%d\n",
													(void *)&h, h.getSocket());
						continue;
					}
					h._connectionClosePending = false;
          MessageQueue &o = h.get_owner();
          Message* message= new CloseConnectionMessage(entry.socket);
          message->dest = o.getQueueId();

          // HTTPAcceptor is responsible for closing the connection.
          // The lock is released to allow HTTPAcceptor to call
          // unsolicitSocketMessages to free the entry.
          // Once HTTPAcceptor completes processing of the close
          // connection, the lock is re-requested and processing of
          // the for loop continues.  This is safe with the current
          // implementation of the _entries object.  Note that the
          // loop condition accesses the _entries.size() on each
          // iteration, so that a change in size while the mutex is
          // unlocked will not result in an ArrayIndexOutOfBounds
          // exception.

          autoEntryMutex.unlock();
          o.enqueue(message);
          autoEntryMutex.lock();
       }
    }

    Uint32 _idleEntries = 0;

    /*
	We will keep track of the maximum socket number and pass this value
	to the kernel as a parameter to SELECT.  This loop seems like a good
	place to calculate the max file descriptor (maximum socket number)
	because we have to traverse the entire array.
    */
    int maxSocketCurrentPass = 0;
    for( int indx = 0; indx < (int)_entries.size(); indx++)
    {
       if(maxSocketCurrentPass < _entries[indx].socket)
	  maxSocketCurrentPass = _entries[indx].socket;

       if(_entries[indx]._status.value() == _MonitorEntry::IDLE)
       {
	  _idleEntries++;
	  FD_SET(_entries[indx].socket, &fdread);
       }
    }

    /*
	Add 1 then assign maxSocket accordingly. We add 1 to account for
	descriptors starting at 0.
    */
    maxSocketCurrentPass++;

    autoEntryMutex.unlock();
    int events = select(maxSocketCurrentPass, &fdread, NULL, NULL, &tv);
    autoEntryMutex.lock();

#ifdef PEGASUS_OS_TYPE_WINDOWS
    if(events == SOCKET_ERROR)
#else
    if(events == -1)
#endif
    {
       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          "Monitor::run - errorno = %d has occurred on select.", errno);
       // The EBADF error indicates that one or more or the file
       // descriptions was not valid. This could indicate that
       // the _entries structure has been corrupted or that
       // we have a synchronization error.

       PEGASUS_ASSERT(errno != EBADF);
    }
    else if (events)
    {
       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          "Monitor::run select event received events = %d, monitoring %d idle entries",
	   events, _idleEntries);
       for( int indx = 0; indx < (int)_entries.size(); indx++)
       {
          // The Monitor should only look at entries in the table that are IDLE (i.e.,
          // owned by the Monitor).
	  if((_entries[indx]._status.value() == _MonitorEntry::IDLE) &&
	     (FD_ISSET(_entries[indx].socket, &fdread)))
	  {
	     MessageQueue *q = MessageQueue::lookup(_entries[indx].queueId);
             Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "Monitor::run indx = %d, queueId =  %d, q = %p",
                  indx, _entries[indx].queueId, q);
             PEGASUS_ASSERT(q !=0);

	     try
	     {
		if(_entries[indx]._type == Monitor::CONNECTION)
		{
                   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                     "_entries[indx].type for indx = %d is Monitor::CONNECTION", indx);
		   static_cast<HTTPConnection *>(q)->_entry_index = indx;

                   // Do not update the entry just yet. The entry gets updated once
                   // the request has been read.
		   //_entries[indx]._status = _MonitorEntry::BUSY;

                   // If allocate_and_awaken failure, retry on next iteration
/* Removed for PEP 183.
                   if (!MessageQueueService::get_thread_pool()->allocate_and_awaken(
                           (void *)q, _dispatch))
                   {
                      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                          "Monitor::run: Insufficient resources to process request.");
                      _entries[indx]._status = _MonitorEntry::IDLE;
                      return true;
                   }
*/
// Added for PEP 183
		   HTTPConnection *dst = reinterpret_cast<HTTPConnection *>(q);
  			 Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                         "Monitor::_dispatch: entering run() for indx  = %d, queueId = %d, q = %p",
                   dst->_entry_index, dst->_monitor->_entries[dst->_entry_index].queueId, dst);
                   try
                   {
                       dst->run(1);
                   }
   		   catch (...)
   		   {
      			Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          		"Monitor::_dispatch: exception received");
   		   }
   		   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                   "Monitor::_dispatch: exited run() for index %d", dst->_entry_index);

                   // It is possible the entry status may not be set to busy.
                   // The following will fail in that case.
   		   // PEGASUS_ASSERT(dst->_monitor->_entries[dst->_entry_index]._status.value() == _MonitorEntry::BUSY);
		   // Once the HTTPConnection thread has set the status value to either
		   // Monitor::DYING or Monitor::IDLE, it has returned control of the connection
		   // to the Monitor.  It is no longer permissible to access the connection
		   // or the entry in the _entries table.

                   // The following is not relevant as the worker thread or the
                   // reader thread will update the status of the entry.
		   //if (dst->_connectionClosePending)
		   //{
		   //  dst->_monitor->_entries[dst->_entry_index]._status = _MonitorEntry::DYING;
		   //}
		   //else
		   //{
		   //  dst->_monitor->_entries[dst->_entry_index]._status = _MonitorEntry::IDLE;
		   //}
// end Added for PEP 183
		}
	        else if( _entries[indx]._type == Monitor::INTERNAL){
			// set ourself to BUSY,
                        // read the data
                        // and set ourself back to IDLE

		   	_entries[indx]._status == _MonitorEntry::BUSY;
			static char buffer[2];
      			Socket::disableBlocking(_entries[indx].socket);
      			Sint32 amt = Socket::read(_entries[indx].socket,&buffer, 2);
      			Socket::enableBlocking(_entries[indx].socket);
			_entries[indx]._status == _MonitorEntry::IDLE;
		}
		else
		{
                   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                     "Non-connection entry, indx = %d, has been received.", indx);
		   int events = 0;
		   events |= SocketMessage::READ;
		   Message *msg = new SocketMessage(_entries[indx].socket, events);
		   _entries[indx]._status = _MonitorEntry::BUSY;
                   autoEntryMutex.unlock();
		   q->enqueue(msg);
                   autoEntryMutex.lock();
		   _entries[indx]._status = _MonitorEntry::IDLE;

		   return true;
		}
	     }
	     catch(...)
	     {
	     }
	     handled_events = true;
	  }
       }
    }

    return(handled_events);
}

void Monitor::stopListeningForConnections(Boolean wait)
{
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::stopListeningForConnections()");
    // set boolean then tickle the server to recognize _stopConnections
    _stopConnections = 1;
    tickle();

    if (wait)
    {
      // Wait for the monitor to notice _stopConnections.  Otherwise the
      // caller of this function may unbind the ports while the monitor
      // is still accepting connections on them.
      try
	{
	  _stopConnectionsSem.time_wait(10000);
	}
      catch (TimeOut &)
	{
	  // The monitor is probably busy processng a very long request, and is
	  // not accepting connections.  Let the caller unbind the ports.
	}
    }

    PEG_METHOD_EXIT();
}


int  Monitor::solicitSocketMessages(
    Sint32 socket,
    Uint32 events,
    Uint32 queueId,
    int type)
{
   PEG_METHOD_ENTER(TRC_HTTP, "Monitor::solicitSocketMessages");
   AutoMutex autoMut(_entry_mut);
   // Check to see if we need to dynamically grow the _entries array
   // We always want the _entries array to 2 bigger than the
   // current connections requested
   _solicitSocketCount++;  // bump the count
   int size = (int)_entries.size();
   if((int)_solicitSocketCount >= (size-1)){
        for(int i = 0; i < ((int)_solicitSocketCount - (size-1)); i++){
                _MonitorEntry entry(0, 0, 0);
                _entries.append(entry);
        }
   }

   int index;
   for(index = 1; index < (int)_entries.size(); index++)
   {
      try
      {
         if(_entries[index]._status.value() == _MonitorEntry::EMPTY)
         {
            _entries[index].socket = socket;
            _entries[index].queueId  = queueId;
            _entries[index]._type = type;
            _entries[index]._status = _MonitorEntry::IDLE;

            return index;
         }
      }
      catch(...)
      {
      }
   }
   _solicitSocketCount--;  // decrease the count, if we are here we didnt do anything meaningful
   PEG_METHOD_EXIT();
   return -1;

}

void Monitor::unsolicitSocketMessages(Sint32 socket)
{

    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::unsolicitSocketMessages");
    AutoMutex autoMut(_entry_mut);

    /*
        Start at index = 1 because _entries[0] is the tickle entry which never needs
        to be EMPTY;
    */
    unsigned int index;
    for(index = 1; index < _entries.size(); index++)
    {
       if(_entries[index].socket == socket)
       {
          _entries[index]._status = _MonitorEntry::EMPTY;
          _entries[index].socket = -1;
          _solicitSocketCount--;
          break;
       }
    }

    /*
	Dynamic Contraction:
	To remove excess entries we will start from the end of the _entries array
	and remove all entries with EMPTY status until we find the first NON EMPTY.
	This prevents the positions, of the NON EMPTY entries, from being changed.
    */
    index = _entries.size() - 1;
    while(_entries[index]._status == _MonitorEntry::EMPTY){
	if(_entries.size() > MAX_NUMBER_OF_MONITOR_ENTRIES)
                _entries.remove(index);
	index--;
    }
    PEG_METHOD_EXIT();
}

// Note: this is no longer called with PEP 183.
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL Monitor::_dispatch(void *parm)
{
   HTTPConnection *dst = reinterpret_cast<HTTPConnection *>(parm);
   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
        "Monitor::_dispatch: entering run() for indx  = %d, queueId = %d, q = %p",
        dst->_entry_index, dst->_monitor->_entries[dst->_entry_index].queueId, dst);
   try
   {
      dst->run(1);
   }
   catch (...)
   {
      Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          "Monitor::_dispatch: exception received");
   }
   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
          "Monitor::_dispatch: exited run() for index %d", dst->_entry_index);

   PEGASUS_ASSERT(dst->_monitor->_entries[dst->_entry_index]._status.value() == _MonitorEntry::BUSY);

   // Once the HTTPConnection thread has set the status value to either
   // Monitor::DYING or Monitor::IDLE, it has returned control of the connection
   // to the Monitor.  It is no longer permissible to access the connection
   // or the entry in the _entries table.
   if (dst->_connectionClosePending)
   {
      dst->_monitor->_entries[dst->_entry_index]._status = _MonitorEntry::DYING;
   }
   else
   {
      dst->_monitor->_entries[dst->_entry_index]._status = _MonitorEntry::IDLE;
   }
   return 0;
}

PEGASUS_NAMESPACE_END
