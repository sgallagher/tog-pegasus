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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>

#include <cstring>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/ArrayIterator.h>

//const static DWORD MAX_BUFFER_SIZE = 4096;  // 4 kilobytes

#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
// Maximum iterations of Pipe processing in Monitor::run
const Uint32 maxIterations = 3;

#endif

#ifdef PEGASUS_OS_TYPE_WINDOWS
# if defined(FD_SETSIZE) && FD_SETSIZE != 1024
#  error "FD_SETSIZE was not set to 1024 prior to the last inclusion \
of <winsock.h>. It may have been indirectly included (e.g., by including \
<windows.h>). Find inclusion of that header which is visible to this \
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

static AtomicInt _connections(0);

Mutex Monitor::_cout_mut;


// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
 #define PIPE_INCREMENT 1
#endif

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
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::Monitor(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
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
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::Monitor(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}

Monitor::~Monitor()
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::~Monitor(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
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
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::~Monitor(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}

void Monitor::initializeTickler(){
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::initializeTickler(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
    /*
       NOTE: On any errors trying to
             setup out tickle connection,
             throw an exception/end the server
    */

    /* setup the tickle server/listener */

    // get a socket for the server side
    if((_tickle_server_socket = ::socket(PF_INET, SOCK_STREAM, 0)) == PEGASUS_INVALID_SOCKET){
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
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
#endif
    _tickle_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(0)
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
    if((_tickle_client_socket = ::socket(PF_INET, SOCK_STREAM, 0)) == PEGASUS_INVALID_SOCKET){
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
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
#endif
    _tickle_client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(0)
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
	Tracer::trace(TRC_HTTP,Tracer::LEVEL2,"!!!!!!!! TICKLE SOCKET-ID = %u",_tickle_peer_socket);
    entry._status = _MonitorEntry::IDLE;
    _entries.append(entry);
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::initializeTickler(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}

void Monitor::tickle(void)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::tickle(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
    static char _buffer[] =
    {
      '0','0'
    };
               Tracer::trace (TRC_HTTP, Tracer::LEVEL2,
				   "Now Monitor::Tickle ");
    AutoMutex autoMutex(_tickle_mutex);
    Socket::disableBlocking(_tickle_client_socket);
	               Tracer::trace (TRC_HTTP, Tracer::LEVEL2,
					   "Now Monitor::Tickle::Write() ");

    Socket::write(_tickle_client_socket,&_buffer, 2);
    Socket::enableBlocking(_tickle_client_socket);
	               Tracer::trace (TRC_HTTP, Tracer::LEVEL2,
				   "Now Monitor::Tickled ");

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::tickle(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
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


    fd_set fdread;
    FD_ZERO(&fdread);

    AutoMutex autoEntryMutex(_entry_mut);

    ArrayIterator<_MonitorEntry> entries(_entries);

    // Check the stopConnections flag.  If set, clear the Acceptor monitor entries
    if (_stopConnections.get() == 1)
    {
        for ( int indx = 0; indx < (int)entries.size(); indx++)
        {
            if (entries[indx]._type == Monitor::ACCEPTOR)
            {
                if ( entries[indx]._status.get() != _MonitorEntry::EMPTY)
                {
					if ( entries[indx]._status.get() == _MonitorEntry::IDLE ||
                        entries[indx]._status.get() == _MonitorEntry::DYING )
                   {
                       // remove the entry
                       entries[indx]._status = _MonitorEntry::EMPTY;
                   }
                   else
                   {
                       // set status to DYING
                      entries[indx]._status = _MonitorEntry::DYING;
                   }
               }
           }
        }
        _stopConnections = 0;
        _stopConnectionsSem.signal();
    }

    for( int indx = 0; indx < (int)entries.size(); indx++)
    {
        const _MonitorEntry &entry = entries[indx];
        if ((entry._status.get() == _MonitorEntry::DYING) &&
           (entry._type == Monitor::CONNECTION))
        {

            MessageQueue *q = MessageQueue::lookup(entry.queueId);
            PEGASUS_ASSERT(q != 0);
            HTTPConnection &h = *static_cast<HTTPConnection *>(q);

            if (h._connectionClosePending == false)
			{
			    continue;
			}


            // NOTE: do not attempt to delete while there are pending responses
            // coming thru. The last response to come thru after a
            // _connectionClosePending will reset _responsePending to false
            // and then cause the monitor to rerun this code and clean up.
            // (see HTTPConnection.cpp)

            if (h._responsePending == true)
            {
// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
                if  (!entry.namedPipeConnection)
                {
#endif
                    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "Monitor::run - "
                        "Ignoring connection delete request because "
                        "responses are still pending. "
                        "connection=0x%p, socket=%d\n",
                        (void *)&h, h.getSocket());

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
                }
                else
                {
                    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "Monitor::run - "
                        "Ignoring connection delete request because "
                        "responses are still pending. "
                        "connection=0x%p, NamedPipe=%d\n",
                        (void *)&h, h.getNamedPipe().getPipe());
                }
#endif
                continue;
            }
            h._connectionClosePending = false;
            MessageQueue &o = h.get_owner();
		    Message* message = 0;

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
            if (!entry.namedPipeConnection)
            {
#endif
                message= new CloseConnectionMessage(entry.socket);

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
            }
            else
            {

			    message= new CloseConnectionMessage(entry.namedPipe);

            }
#endif
            message->dest = o.getQueueId();

            // HTTPAcceptor is responsible for closing the connection.
            // The lock is released to allow HTTPAcceptor to call
            // unsolicitSocketMessages to free the entry.
            // Once HTTPAcceptor completes processing of the close
            // connection, the lock is re-requested and processing of
            // the for loop continues.  This is safe with the current
            // implementation of the entries object.  Note that the
            // loop condition accesses the entries.size() on each
            // iteration, so that a change in size while the mutex is
            // unlocked will not result in an ArrayIndexOutOfBounds
            // exception.

            autoEntryMutex.unlock();
            o.enqueue(message);
            autoEntryMutex.lock();
            // After enqueue a message and the autoEntryMutex has been released and locked again,
            // the array of _entries can be changed. The ArrayIterator has be reset with the original _entries.

            entries.reset(_entries);
        }
    }

    Uint32 _idleEntries = 0;

    /*
        We will keep track of the maximum socket number and pass this value
        to the kernel as a parameter to SELECT.  This loop seems like a good
        place to calculate the max file descriptor (maximum socket number)
        because we have to traverse the entire array.
    */

    PEGASUS_SOCKET maxSocketCurrentPass = 0;
    int indx;

	// Record the indexes at which Sockets are available
	Array <Uint32> socketCountAssociator;
    int socketEntryCount=0;

     // Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
    //This array associates named pipe connections to their place in [indx]
    //in the entries array. The value in portion zero of the array is the
    //index of the fist named pipe connection in the entries array

	// Record the indexes at which Pipes are available
	Array <Uint32> indexPipeCountAssociator;
    int pipeEntryCount=0;
    int MaxPipes = PIPE_INCREMENT;
    // List of Pipe Handlers
    HANDLE * hPipeList = new HANDLE[PIPE_INCREMENT];
#endif

    // This loop takes care of setting the namedpipe which has to be used from the list....
    for ( indx = 0,socketEntryCount=0 ;
		             indx < (int)entries.size(); indx++)
    {

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
		if (!entries[indx].namedPipeConnection)
        {
#endif
            if (maxSocketCurrentPass < entries[indx].socket)
			{
				maxSocketCurrentPass = entries[indx].socket;
			}
            if(entries[indx]._status.get() == _MonitorEntry::IDLE)
            {
                _idleEntries++;
                FD_SET(entries[indx].socket, &fdread);
                socketCountAssociator.append(indx);
				socketEntryCount++;
            }

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
        }
		else
		{
		    entries[indx].pipeSet = false;
			if (pipeEntryCount >= MaxPipes)
			{
			    MaxPipes += PIPE_INCREMENT;
				HANDLE* temp_pList = new HANDLE[MaxPipes];
				for (Uint32 i =0;i<pipeEntryCount;i++)
				{
				    temp_pList[i] = hPipeList[i];
				}
				delete [] hPipeList;
				hPipeList = temp_pList;
		    }
			hPipeList[pipeEntryCount] = entries[indx].namedPipe.getPipe();
			indexPipeCountAssociator.append(indx);
			pipeEntryCount++;
		}

#endif
    }

    /*
        Add 1 then assign maxSocket accordingly. We add 1 to account for
        descriptors starting at 0.
    */
    maxSocketCurrentPass++;

    autoEntryMutex.unlock();

    int events = -1;
	// Since the pipes have been introduced, the ratio of procesing
	// time Socket:Pipe :: 3/4:1/4 respectively

	Uint32 newMilliseconds = milliseconds;
	#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)

	newMilliseconds = (milliseconds * 3)/4 ;

    #endif

	struct timeval tv = {newMilliseconds/1000, newMilliseconds%1000*1000};


	#ifdef PEGASUS_OS_TYPE_WINDOWS
		events = select(0, &fdread, NULL, NULL, &tv);
	#else
		events = select(maxSocketCurrentPass, &fdread, NULL, NULL, &tv);
	#endif

    autoEntryMutex.lock();
    // After enqueue a message and the autoEntryMutex has been released and locked again,
    // the array of _entries can be changed. The ArrayIterator has be reset with the original _entries
    entries.reset(_entries);

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
       // the entries structure has been corrupted or that
       // we have a synchronization error.

         PEGASUS_ASSERT(errno != EBADF);
    }
    else if (events)
    {
         Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
         "Monitor::run select event received events = %d, monitoring %d idle entries",
         events, _idleEntries);
         for ( int sindx = 0; sindx < socketEntryCount; sindx++)
         {
             // The Monitor should only look at entries in the table that are IDLE (i.e.,
             // owned by the Monitor).
		     indx = socketCountAssociator[sindx];

             if ((entries[indx]._status.get() == _MonitorEntry::IDLE) &&
                  (FD_ISSET(entries[indx].socket, &fdread)))
             {
                 MessageQueue *q = MessageQueue::lookup(entries[indx].queueId);
                 Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                               "Monitor::run indx = %d, queueId =  %d, q = %p",
                               indx, entries[indx].queueId, q);

                 PEGASUS_ASSERT(q !=0);

                 try
                 {
                     if (entries[indx]._type == Monitor::CONNECTION)
                     {
                         static_cast<HTTPConnection *>(q)->_entry_index = indx;

						 // Do not update the entry just yet. The entry gets updated once
						 // the request has been read.
						 //entries[indx]._status = _MonitorEntry::BUSY;

						 // If allocate_and_awaken failure, retry on next iteration
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
                            "Monitor::_dispatch: exited run() for index %d",
						    dst->_entry_index);

					}
					else if (entries[indx]._type == Monitor::INTERNAL)
 					{
						// set ourself to BUSY,
						// read the data
					    // and set ourself back to IDLE
					    static char buffer[2];
      					Socket::disableBlocking(entries[indx].socket);

      					Sint32 amt = Socket::read(entries[indx].socket,&buffer, 2);
						Socket::enableBlocking(entries[indx].socket);
						entries[indx]._status = _MonitorEntry::IDLE;
				    }
					else
					{
					    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
						 "Non-connection entry, indx = %d, has been received.", indx);

						int events = 0;
						events |= SocketMessage::READ;
						Message *msg = new SocketMessage(entries[indx].socket, events);
						entries[indx]._status = _MonitorEntry::BUSY;
						autoEntryMutex.unlock();
						q->enqueue(msg);
						autoEntryMutex.lock();
						// After enqueue a message and the autoEntryMutex has been released and locked again,
						// the array of entries can be changed. The ArrayIterator has be reset with the original _entries
						entries.reset(_entries);
						entries[indx]._status = _MonitorEntry::IDLE;
						handled_events = true;

					}
				}
				catch(...)
				{
				}
			  	handled_events = true;
			}
        }
	    return(handled_events);
    }


#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)

	//if no pipes are registered return immediately

	int pEvents = -1;
	int pCount = -1;
	BOOL bPeekPipe = 0;
	DWORD dwBytesAvail=0;
	// The pipe is sniffed and check if there are any data. If available, the
	// message is picked from the Queue and appropriate methods are invoked.


	// pipeProcessCount records the number of requests that are processed.
	// At the end of loop this is verified against the count of request
	// on local connection . If there are any pipes which needs to be
	// processed we would apply delay and then proceed to iterate.

    Uint32 pipeProcessCount =0;

    for (int counter = 1; counter < maxIterations ; counter ++)
    {


		// pipeIndex is used to index into indexPipeCountAssociator to fetch 
		// index of the _MonitorEntry of Monitor
        for (int pipeIndex = 0; pipeIndex < pipeEntryCount; pipeIndex++)
	    {
            dwBytesAvail = 0;
		    Tracer::trace(TRC_HTTP,Tracer::LEVEL4," PIPE_PEEKING for PIPE = %u ", hPipeList[pipeIndex]);
		    bPeekPipe = ::PeekNamedPipe(hPipeList[pipeIndex],
			                            NULL,
							            NULL,
								        NULL,
                                        &dwBytesAvail,
								        NULL
								       );

			// If peek on NamedPipe was successfull and data is available
            if (bPeekPipe && dwBytesAvail)
	        {

			    Tracer::trace(TRC_HTTP,Tracer::LEVEL4," PIPE_PEEKING FOUND = %u BYTES", dwBytesAvail);

			    pEvents = 1;
			    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "EVENT TRIGGERED in Pipe = %u ",entries[indexPipeCountAssociator[pipeIndex]].namedPipe.getPipe());
	            entries[indexPipeCountAssociator[pipeIndex]].pipeSet = true;
			    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                    "Monitor::run select event received events = %d, \
					monitoring %d idle entries",
                    pEvents,
					_idleEntries);

				int pIndx = indexPipeCountAssociator[pipeIndex];

				if ((entries[pIndx]._status.get() == _MonitorEntry::IDLE) &&
					 entries[pIndx].namedPipe.isConnected() &&
					 (pEvents))
		        {

			        MessageQueue *q = 0;

                    try
					{

				        q = MessageQueue::lookup (entries[pIndx].queueId);
                    }
                    catch (Exception e)
                    {
				        e.getMessage();
			        }
                    catch(...)
                    {
			        }

					Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                                  "Monitor::run indx = %d, queueId =  %d,\
								  q = %p",pIndx, entries[pIndx].queueId, q);
                    try
                    {
				        if (entries[pIndx]._type == Monitor::CONNECTION)
                        {

						    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
							              "entries[indx].type for indx = \
									      %d is Monitor::CONNECTION",
										  pIndx);
						    static_cast<HTTPConnection *>(q)->_entry_index = pIndx;
					        HTTPConnection *dst = reinterpret_cast \
									                   <HTTPConnection *>(q);
					        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
							              "Monitor::_dispatch: entering run() \
										  for indx  = %d, queueId = %d, \
										  q = %p",\
					                      dst->_entry_index,
										  dst->_monitor->_entries\
										  [dst->_entry_index].queueId, dst);

					        try
					        {

						        dst->run(1);

						        // Record that the requested data is read/Written
						        pipeProcessCount++;

					        }
					        catch (...)
					        {
						        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
								              "Monitor::_dispatch: \
											   exception received");
					        }

					        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
					                      "Monitor::_dispatch: exited \
							               \run() index %d",
										   dst->_entry_index);


				        }
				        else
				        {
					        /* The condition
							   entries[indx]._type == Monitor::INTERNAL can be
							   ignored for pipes as the tickler is of
							   Monitor::INTERNAL type. The tickler is
							   a socket.
					        */

					        Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
									          "Non-connection entry, indx = %d,\
											  has been received.", pIndx);
					        int events = 0;
					        Message *msg = 0;

						    pEvents |= NamedPipeMessage::READ;
						    msg = new NamedPipeMessage(entries[pIndx].namedPipe, pEvents);
		                    entries[pIndx]._status = _MonitorEntry::BUSY;
		                    autoEntryMutex.unlock();
				            q->enqueue(msg);
					        autoEntryMutex.lock();
		                    entries.reset(_entries);
		                    entries[pIndx]._status = _MonitorEntry::IDLE;
					        return true;
				        }


			        }
			        catch(...)
			        {

	                }
	            }

	        }
        }

		//Check if all the pipes had recieved the data, If no then try again
        if (pipeEntryCount == pipeProcessCount)
		{
		    break;
		}


    }

	delete [] hPipeList;

#endif

    return(handled_events);
}

void Monitor::stopListeningForConnections(Boolean wait)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::stopListeningForConnections(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::stopListeningForConnections()");
    // set boolean then tickle the server to recognize _stopConnections
    _stopConnections = 1;
    tickle();

    if (wait)
    {
      // Wait for the monitor to notice _stopConnections.  Otherwise the
      // caller of this function may unbind the ports while the monitor
      // is still accepting connections on them.
      _stopConnectionsSem.wait();
    }

    PEG_METHOD_EXIT();
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::stopListeningForConnections(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}


int  Monitor::solicitSocketMessages(
    PEGASUS_SOCKET socket,
    Uint32 events,
    Uint32 queueId,
    int type)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::solicitSocketMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
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
         if(_entries[index]._status.get() == _MonitorEntry::EMPTY)
         {
            _entries[index].socket = socket;
            _entries[index].queueId  = queueId;
            _entries[index]._type = type;
            _entries[index]._status = _MonitorEntry::IDLE;

#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
            {
                AutoMutex automut(Monitor::_cout_mut);
                PEGASUS_STD(cout) << "Exiting:  Monitor::solicitSocketMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
            }
#endif
            return index;
         }
      }
      catch(...)
      {
      }
   }
   _solicitSocketCount--;  // decrease the count, if we are here we didnt do anything meaningful
   PEG_METHOD_EXIT();
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
   {
       AutoMutex automut(Monitor::_cout_mut);
       PEGASUS_STD(cout) << "Exiting:  Monitor::solicitSocketMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
   }
#endif
   return -1;

}

void Monitor::unsolicitSocketMessages(PEGASUS_SOCKET socket)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::unsolicitSocketMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif

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
          _entries[index].socket = PEGASUS_INVALID_SOCKET;
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
    while(_entries[index]._status.get() == _MonitorEntry::EMPTY){
	if(_entries.size() > MAX_NUMBER_OF_MONITOR_ENTRIES)
                _entries.remove(index);
	index--;
    }
    PEG_METHOD_EXIT();
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::unsolicitSocketMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}

// Note: this is no longer called with PEP 183.
PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL Monitor::_dispatch(void *parm)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::_dispatch(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
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

   PEGASUS_ASSERT(dst->_monitor->_entries[dst->_entry_index]._status.get() == _MonitorEntry::BUSY);

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

// Added for NamedPipe implementation for windows
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
//This method is anlogus to solicitSocketMessages. It does the same thing for named Pipes
int  Monitor::solicitPipeMessages(
    NamedPipe namedPipe,
    Uint32 events,  //not sure what has to change for this enum
    Uint32 queueId,
    int type)
{
   PEG_METHOD_ENTER(TRC_HTTP, "Monitor::solicitPipeMessages");

   AutoMutex autoMut(_entry_mut);
   // Check to see if we need to dynamically grow the _entries array
   // We always want the _entries array to 2 bigger than the
   // current connections requested
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
{
   AutoMutex automut(Monitor::_cout_mut);
   PEGASUS_STD(cout) << "In Monitor::solicitPipeMessages at the begining" << PEGASUS_STD(endl);
}
#endif

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
         if(_entries[index]._status.get() == _MonitorEntry::EMPTY)
         {
            _entries[index].socket = NULL;
            _entries[index].namedPipe = namedPipe;
            _entries[index].namedPipeConnection = true;
            _entries[index].queueId  = queueId;
            _entries[index]._type = type;
            _entries[index]._status = _MonitorEntry::IDLE;
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
  {
            AutoMutex automut(Monitor::_cout_mut);
            PEGASUS_STD(cout) << "In Monitor::solicitPipeMessages after seting up  _entries[index] index = " << index << PEGASUS_STD(endl);
  }
#endif

            return index;
         }
      }
      catch(...)
      {
      }

   }
   _solicitSocketCount--;  // decrease the count, if we are here we didnt do anything meaningful
   PEGASUS_STD(cout) << "In Monitor::solicitPipeMessages nothing happed - it didn't work" << PEGASUS_STD(endl);

   PEG_METHOD_EXIT();
   return -1;

}

//////////////////////////////////////////////////////////////////////////////
// Method Name      : unsolicitPipeMessages
// Input Parameter  : namedPipe  - type NamedPipe
// Return Type      : void
//============================================================================
// This method is invoked from HTTPAcceptor::handleEnqueue for server
// when the CLOSE_CONNECTION_MESSAGE is recieved. This method is also invoked
// from HTTPAcceptor::destroyConnections method when the CIMServer is shutdown.
// For the CIMClient, this is invoked from HTTPConnector::handleEnqueue when the
// CLOSE_CONNECTION_MESSAGE is recieved. This method is also invoked from
// HTTPConnector::disconnect when CIMClient requests a disconnect request.
// The list of _MonitorEntry is searched for the matching pipe.
// The Handle of the identified is closed and _MonitorEntry for the
// requested pipe is removed.
///////////////////////////////////////////////////////////////////////////////

void Monitor::unsolicitPipeMessages(NamedPipe namedPipe)
{
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Entering: Monitor::unsolicitPipeMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif

    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::unsolicitPipeMessages");
    AutoMutex autoMut(_entry_mut);

    /*
        Start at index = 1 because _entries[0] is the tickle entry which never needs
        to be EMPTY;
    */
    unsigned int index;
    for (index = 1; index < _entries.size(); index++)
    {
        if (_entries[index].namedPipe.getPipe() == namedPipe.getPipe())
        {
            _entries[index]._status = _MonitorEntry::EMPTY;
            // Ensure that the client has read the data
		    ::FlushFileBuffers (namedPipe.getPipe());
		    //Disconnect to release the pipe. This doesn't release Pipe Handle
		    ::DisconnectNamedPipe (_entries[index].namedPipe.getPipe());
            // Must use CloseHandle to Close Pipe
			::CloseHandle(_entries[index].namedPipe.getPipe());
		    _entries[index].namedPipe.disconnect();
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
    while (_entries[index]._status.get() == _MonitorEntry::EMPTY)
	{
        if ((_entries[index].namedPipe.getPipe() == namedPipe.getPipe()) ||
            (_entries.size() > MAX_NUMBER_OF_MONITOR_ENTRIES))
        {
		    _entries.remove(index);
        }
        index--;
    }
    PEG_METHOD_EXIT();
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
    {
        AutoMutex automut(Monitor::_cout_mut);
        PEGASUS_STD(cout) << "Exiting:  Monitor::unsolicitPipeMessages(): (tid:" << Uint32(pegasus_thread_self()) << ")" << PEGASUS_STD(endl);
    }
#endif
}

#endif

PEGASUS_NAMESPACE_END
