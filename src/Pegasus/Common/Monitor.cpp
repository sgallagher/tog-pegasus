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

#include "Network.h"
#include <Pegasus/Common/Config.h>
#include <cstring>
#include "Monitor.h"
#include "MessageQueue.h"
#include "Socket.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/HTTPConnection.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Exception.h>
#include "ArrayIterator.h"
#include <errno.h>
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
// Maximum iterations of Pipe processing in Monitor::run
const Uint32 maxIterations = 2;
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
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
     ,_solicitPipeCount(0)
#endif
{
    int numberOfMonitorEntriesToAllocate = MAX_NUMBER_OF_MONITOR_ENTRIES;
    Socket::initializeInterface();
    _entries.reserveCapacity(numberOfMonitorEntriesToAllocate);

#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
    _entries_pipe.reserveCapacity(numberOfMonitorEntriesToAllocate);
#endif
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
#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
    for( int i = 1; i < numberOfMonitorEntriesToAllocate; i++ )
    {
       _MonitorEntry entry(0, 0, 0);
       _entries_pipe.append(entry);
    }
#endif
}

Monitor::~Monitor()
{
    uninitializeTickler();
    Socket::uninitializeInterface();
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "returning from monitor destructor");
}
void Monitor::uninitializeTickler(){

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

}

void Monitor::initializeTickler(){
    /*
       NOTE: On any errors trying to
             setup out tickle connection,
             throw an exception/end the server
    */

    /* setup the tickle server/listener */
    // try until the tcpip is restarted
    do
    {

        // get a socket for the server side
        if((_tickle_server_socket = Socket::createSocket(PF_INET, SOCK_STREAM, 0)) == PEGASUS_INVALID_SOCKET)
        {
            //handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_CREATE",
                                     "Received error number $0 while creating the internal socket.",
                                     getSocketError());
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

        SocketLength _addr_size = sizeof(_tickle_server_addr);

        // bind server side to socket
        if((::bind(_tickle_server_socket,
                   reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                   sizeof(_tickle_server_addr))) < 0)
        {
            // handle error
#ifdef PEGASUS_OS_ZOS
            MessageLoaderParms parms("Common.Monitor.TICKLE_BIND_LONG",
                                "Received error:$0 while binding the internal socket."
                                ,strerror(errno));
#else
            MessageLoaderParms parms("Common.Monitor.TICKLE_BIND",
                                "Received error number $0 while binding the internal socket.",
                                getSocketError());
#endif
            throw Exception(parms);
        }

        // tell the kernel we are a server
        if((::listen(_tickle_server_socket,3)) < 0)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_LISTEN",
                                "Received error number $0 while listening to the internal socket.",
                                getSocketError());
            throw Exception(parms);
        }

        // make sure we have the correct socket for our server
        int sock = ::getsockname(_tickle_server_socket,
                            reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                            &_addr_size);
        if(sock < 0)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_SOCKNAME",
                                "Received error number $0 while getting the internal socket name.",
                                getSocketError());
            throw Exception(parms);
        }

        /* set up the tickle client/connector */

        // get a socket for our tickle client
        if((_tickle_client_socket = Socket::createSocket(PF_INET, SOCK_STREAM, 0))
           == PEGASUS_INVALID_SOCKET)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_CREATE",
                                "Received error number $0 while creating the internal client socket.",
                                getSocketError());
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
                   sizeof(_tickle_client_addr))) < 0)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_BIND",
                                     "Received error number $0 while binding the internal client socket.",
                                     getSocketError());
            throw Exception(parms);
        }

        // connect to server side
        if((::connect(_tickle_client_socket,
                      reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                      sizeof(_tickle_server_addr))) < 0)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_CLIENT_CONNECT",
                                     "Received error number $0 while connecting the internal client socket.",
                                     getSocketError());
            throw Exception(parms);
        }

        /* set up the slave connection */
        memset(&_tickle_peer_addr, 0, sizeof(_tickle_peer_addr));
        SocketLength peer_size = sizeof(_tickle_peer_addr);
        Threads::sleep(1);

        // this call may fail, we will try a max of 20 times to establish this peer connection
        if((_tickle_peer_socket = ::accept(_tickle_server_socket,
                                           reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
                                           &peer_size)) < 0)
        {
#if !defined(PEGASUS_OS_TYPE_WINDOWS)

            if(_tickle_peer_socket == PEGASUS_SOCKET_ERROR
               && getSocketError() == PEGASUS_NETWORK_TRYAGAIN)
            {
                int retries = 0;
                do
                {
                    Threads::sleep(1);
                    _tickle_peer_socket = ::accept(_tickle_server_socket,
                                                   reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
                                                   &peer_size);
                    retries++;
                } while(_tickle_peer_socket == PEGASUS_SOCKET_ERROR
                        && getSocketError() == PEGASUS_NETWORK_TRYAGAIN
                        && retries < 20);
            }
            // TCP/IP is down, destroy sockets and retry again.
            if(_tickle_peer_socket == PEGASUS_SOCKET_ERROR &&
               getSocketError() == PEGASUS_NETWORK_TCPIP_STOPPED )
            {
                // destroy everything
                uninitializeTickler();
                // retry again.
                continue;
            }
        }
#endif
    }
        if(_tickle_peer_socket == PEGASUS_SOCKET_ERROR)
        {
            // handle error
            MessageLoaderParms parms("Common.Monitor.TICKLE_ACCEPT",
                                     "Received error number $0 while accepting the internal socket connection.",
                                     getSocketError());
            throw Exception(parms);
        } else
        {
            // socket is ok
            break;
        }
    } while(1); // try until TCP/IP is restarted

    Socket::disableBlocking(_tickle_peer_socket);
    Socket::disableBlocking(_tickle_client_socket);

    // add the tickler to the list of entries to be monitored and set to IDLE because Monitor only
    // checks entries with IDLE state for events
    _MonitorEntry entry(_tickle_peer_socket, 1, INTERNAL);
    entry._status = _MonitorEntry::IDLE;

    // is the tickler initalized as first socket on startup ?
    if (_entries.size()==0)
    {
       // if yes, append a new entry
       _entries.append(entry);
    }
    else
    {
       // if not, overwrite the tickler entry with new socket
       _entries[0]=entry;
    }

}

void Monitor::tickle(void)
{
    static char _buffer[] =
    {
      '0','0'
    };

    AutoMutex autoMutex(_tickle_mutex);
    Socket::write(_tickle_client_socket,&_buffer, 2);
}

void Monitor::setState( Uint32 index, _MonitorEntry::entry_status status )
{
    // Set the state to requested state
    _entries[index]._status = status;
}

#if defined PEGASUS_OS_TYPE_WINDOWS && !defined(PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET)
void Monitor::setPipeState( Uint32 index, _MonitorEntry::entry_status status )
{
    // Set the state to requested state
    _entries_pipe[index]._status = status;
}

int  Monitor::handlePipe()
{

    AutoMutex autoEntryMutex(_entry_mut);

    ArrayIterator<_MonitorEntry> entries(_entries_pipe);

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
                        "connection=0x%p, NamedPipe=%d\n",
                        (void *)&h, h.getNamedPipe().getPipe());
                        continue;
                    }
                    h._connectionClosePending = false;
          MessageQueue &o = h.get_owner();
          Message* message = 0;

          message= new CloseConnectionMessage(entry.namedPipe);

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

          _entry_mut.unlock();
          o.enqueue(message);
          _entry_mut.lock();
          // After enqueue a message and the autoEntryMutex has been released and locked again,
          // the array of _entries can be changed. The ArrayIterator has be reset with the original _entries.
          entries.reset(_entries_pipe);
       }
    }

    Uint32 _idleEntries = 0;

    /*
        We will keep track of the maximum socket number and pass this value
        to the kernel as a parameter to SELECT.  This loop seems like a good
        place to calculate the max file descriptor (maximum socket number)
        because we have to traverse the entire array.
    */
    SocketHandle maxSocketCurrentPass = 0;
    int indx = 0;


    Array <Uint32> indexPipeCountAssociator;
    int pipeEntryCount=0;
    int MaxPipes = PIPE_INCREMENT;
    // List of Pipe Handlers
    HANDLE * hPipeList = new HANDLE[PIPE_INCREMENT];


    for( indx = 0; indx < (int)entries.size()  ; indx++)
    {
           if (!entries[indx].namedPipeConnection)
               continue;

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
    /*
        Add 1 then assign maxSocket accordingly. We add 1 to account for
        descriptors starting at 0.
    */
    maxSocketCurrentPass++;



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

    _entry_mut.unlock();


    // pipeIndex is used to index into indexPipeCountAssociator to fetch
    // index of the _MonitorEntry of Monitor
    for (int pipeIndex = 0; pipeIndex < pipeEntryCount; pipeIndex++)
    {
        dwBytesAvail = 0;

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

            // Tracer::trace(TRC_HTTP,Tracer::LEVEL4," PIPE_PEEKING FOUND = %u BYTES", dwBytesAvail);

            pEvents = 1;
            Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "EVENT TRIGGERED in Pipe = %u ",entries[indexPipeCountAssociator[pipeIndex]].namedPipe.getPipe());
            entries[indexPipeCountAssociator[pipeIndex]].pipeSet = true;
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
                        q->enqueue(msg);
                        _entry_mut.lock();
                        entries.reset(_entries_pipe);
                        entries[pIndx]._status = _MonitorEntry::IDLE;
                        delete [] hPipeList;
                        return 1;
                    }


                }
                catch(...)
                {

                }
            }

        }

		_entry_mut.lock();
    }
    delete [] hPipeList;

	//If no request received on pipe rest for 4 seconds and give chance to Socket Thread
    if (!pipeProcessCount)
	{
		Sleep(4000);
	}
    return 1;

}
#endif

void Monitor::run(Uint32 milliseconds)
{

    int i = 0;

    struct timeval tv = {milliseconds/1000, milliseconds%1000*1000};

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
          // implementation of the entries object.  Note that the
          // loop condition accesses the entries.size() on each
          // iteration, so that a change in size while the mutex is
          // unlocked will not result in an ArrayIndexOutOfBounds
          // exception.

          _entry_mut.unlock();
          o.enqueue(message);
          _entry_mut.lock();
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
    SocketHandle maxSocketCurrentPass = 0;
    for( int indx = 0; indx < (int)entries.size(); indx++)
    {
       if(maxSocketCurrentPass < entries[indx].socket)
            maxSocketCurrentPass = entries[indx].socket;

       if(entries[indx]._status.get() == _MonitorEntry::IDLE)
       {
           _idleEntries++;
           FD_SET(entries[indx].socket, &fdread);
       }
    }

    /*
        Add 1 then assign maxSocket accordingly. We add 1 to account for
        descriptors starting at 0.
    */
    maxSocketCurrentPass++;

    _entry_mut.unlock();

    //
    // The first argument to select() is ignored on Windows and it is not
    // a socket value.  The original code assumed that the number of sockets
    // and a socket value have the same type.  On Windows they do not.
    //
#ifdef PEGASUS_OS_TYPE_WINDOWS
    int events = select(0, &fdread, NULL, NULL, &tv);
#else
    int events = select(maxSocketCurrentPass, &fdread, NULL, NULL, &tv);
#endif
    _entry_mut.lock();
    // After enqueue a message and the autoEntryMutex has been released and locked again,
    // the array of _entries can be changed. The ArrayIterator has be reset with the original _entries
    entries.reset(_entries);

    if (events == PEGASUS_SOCKET_ERROR)
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
       for( int indx = 0; indx < (int)entries.size(); indx++)
       {
          // The Monitor should only look at entries in the table that are IDLE (i.e.,
          // owned by the Monitor).
          if((entries[indx]._status.get() == _MonitorEntry::IDLE) &&
             (FD_ISSET(entries[indx].socket, &fdread)))
          {
             MessageQueue *q = MessageQueue::lookup(entries[indx].queueId);
             Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "Monitor::run indx = %d, queueId =  %d, q = %p",
                  indx, entries[indx].queueId, q);
             PEGASUS_ASSERT(q !=0);

             try
             {
                if(entries[indx]._type == Monitor::CONNECTION)
                {
                   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                     "entries[indx].type for indx = %d is Monitor::CONNECTION", indx);
                   static_cast<HTTPConnection *>(q)->_entry_index = indx;

                   // Do not update the entry just yet. The entry gets updated once
                   // the request has been read.
                   //entries[indx]._status = _MonitorEntry::BUSY;

                   // If allocate_and_awaken failure, retry on next iteration
/* Removed for PEP 183.
                   if (!MessageQueueService::get_thread_pool()->allocate_and_awaken(
                           (void *)q, _dispatch))
                   {
                      Tracer::trace(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                          "Monitor::run: Insufficient resources to process request.");
                      entries[indx]._status = _MonitorEntry::IDLE;
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
              // PEGASUS_ASSERT(dst->_monitor->_entries[dst->_entry_index]._status.get() == _MonitorEntry::BUSY);
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
            else if( entries[indx]._type == Monitor::INTERNAL){
            // set ourself to BUSY,
                        // read the data
                        // and set ourself back to IDLE

               entries[indx]._status = _MonitorEntry::BUSY;
            static char buffer[2];
            Sint32 amt = Socket::read(entries[indx].socket,&buffer, 2);

            if(amt == PEGASUS_SOCKET_ERROR &&
               getSocketError() == PEGASUS_NETWORK_TCPIP_STOPPED )
            {
                Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                      "Monitor::run: Tickler socket got an IO error. "
                      "Going to re-create Socket and wait for TCP/IP restart.");
                uninitializeTickler();
                initializeTickler();

            } else
            {
                entries[indx]._status = _MonitorEntry::IDLE;
            }

        }
        else
        {
                   Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                     "Non-connection entry, indx = %d, has been received.", indx);
           int events = 0;
           events |= SocketMessage::READ;
           Message *msg = new SocketMessage(entries[indx].socket, events);
           entries[indx]._status = _MonitorEntry::BUSY;
                   _entry_mut.unlock();
           q->enqueue(msg);
                   _entry_mut.lock();
           // After enqueue a message and the autoEntryMutex has been released and locked again,
           // the array of entries can be changed. The ArrayIterator has be reset with the original _entries
           entries.reset(_entries);
           entries[indx]._status = _MonitorEntry::IDLE;
        }
         }
         catch(...)
         {
         }
      }
       }
    }
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
      _stopConnectionsSem.wait();
    }

    PEG_METHOD_EXIT();
}



int  Monitor::solicitSocketMessages(
    SocketHandle socket,
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
         if(_entries[index]._status.get() == _MonitorEntry::EMPTY)
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

void Monitor::unsolicitSocketMessages(SocketHandle socket)
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
}

// Note: this is no longer called with PEP 183.
ThreadReturnType PEGASUS_THREAD_CDECL Monitor::_dispatch(void *parm)
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
   AutoMutex autoMut(_entry_pipe_mut);
   // Check to see if we need to dynamically grow the _entries array
   // We always want the _entries array to 2 bigger than the
   // current connections requested
#ifdef PEGASUS_LOCALDOMAINSOCKET_DEBUG
{
   AutoMutex automut(Monitor::_cout_mut);
   PEGASUS_STD(cout) << "In Monitor::solicitPipeMessages at the begining" << PEGASUS_STD(endl);
}
#endif

   _solicitPipeCount++;  // bump the count
   int size = (int)_entries_pipe.size();
   if((int)_solicitPipeCount >= (size-1)){
        for(int i = 0; i < ((int)_solicitPipeCount - (size-1)); i++){
                _MonitorEntry entry(0, 0, 0);
                _entries_pipe.append(entry);
        }
   }

   int index;
   for(index = 1; index < (int)_entries_pipe.size(); index++)
   {
      try
      {
         if(_entries_pipe[index]._status.get() == _MonitorEntry::EMPTY)
         {
            _entries_pipe[index].socket = NULL;
            _entries_pipe[index].namedPipe = namedPipe;
            _entries_pipe[index].namedPipeConnection = true;
            _entries_pipe[index].queueId  = queueId;
            _entries_pipe[index]._type = type;
            _entries_pipe[index]._status = _MonitorEntry::IDLE;
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
   _solicitPipeCount--;  // decrease the count, if we are here we didnt do anything meaningful
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
    AutoMutex autoMut(_entry_pipe_mut);

    /*
        Start at index = 1 because _entries[0] is the tickle entry which never needs
        to be EMPTY;
    */
    unsigned int index;
    for (index = 1; index < _entries_pipe.size(); index++)
    {
        if (_entries_pipe[index].namedPipe.getPipe() == namedPipe.getPipe())
        {
            _entries_pipe[index]._status = _MonitorEntry::EMPTY;
            // Ensure that the client has read the data
            ::FlushFileBuffers (namedPipe.getPipe());
            //Disconnect to release the pipe. This doesn't release Pipe Handle
            ::DisconnectNamedPipe (_entries_pipe[index].namedPipe.getPipe());
            // Must use CloseHandle to Close Pipe
            ::CloseHandle(_entries_pipe[index].namedPipe.getPipe());
            _entries_pipe[index].namedPipe.disconnect();
            _solicitPipeCount--;
            break;
        }
    }

    /*
        Dynamic Contraction:
        To remove excess entries we will start from the end of the _entries array
        and remove all entries with EMPTY status until we find the first NON EMPTY.
        This prevents the positions, of the NON EMPTY entries, from being changed.
    */
    index = _entries_pipe.size() - 1;
    while (_entries_pipe[index]._status.get() == _MonitorEntry::EMPTY
        && index > 0)
    {
        if ((_entries_pipe[index].namedPipe.getPipe() == namedPipe.getPipe()) ||
            (_entries_pipe.size() > MAX_NUMBER_OF_MONITOR_ENTRIES))
        {
            _entries_pipe.remove(index);
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
