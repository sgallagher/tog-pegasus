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

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

static AtomicInt _connections(0);

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
    for (int i = 1; i < numberOfMonitorEntriesToAllocate; i++)
    {
       _MonitorEntry entry(0, 0, 0);
       _entries.append(entry);
    }
}

Monitor::~Monitor()
{
    uninitializeTickler();
    Socket::uninitializeInterface();
    PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4,
                  "returning from monitor destructor");
}
void Monitor::uninitializeTickler()
{
    PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4, "uninitializing interface");

    try
    {
        if (_tickle_peer_socket >= 0)
        {
            Socket::close(_tickle_peer_socket);
        }
        if (_tickle_client_socket >= 0)
        {
            Socket::close(_tickle_client_socket);
        }
        if (_tickle_server_socket >= 0)
        {
            Socket::close(_tickle_server_socket);
        }
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4,
                  "Failed to close tickle sockets");
    }

}

void Monitor::initializeTickler()
{
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
        if ((_tickle_server_socket =
                 Socket::createSocket(PF_INET, SOCK_STREAM, 0)) ==
             PEGASUS_INVALID_SOCKET)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_CREATE",
                "Received error number $0 while creating the internal socket.",
                getSocketError());
            throw Exception(parms);
        }

        // initialize the address
        memset(&_tickle_server_addr, 0, sizeof(_tickle_server_addr));

        _tickle_server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        _tickle_server_addr.sin_family = PF_INET;
        _tickle_server_addr.sin_port = 0;

        SocketLength _addr_size = sizeof(_tickle_server_addr);

        // bind server side to socket
        if ((::bind(_tickle_server_socket,
                 reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                 sizeof(_tickle_server_addr))) < 0)
        {
#ifdef PEGASUS_OS_ZOS
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_BIND_LONG",
                "Received error:$0 while binding the internal socket.",
                strerror(errno));
#else
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_BIND",
                "Received error number $0 while binding the internal socket.",
                getSocketError());
#endif
            throw Exception(parms);
        }

        // tell the kernel we are a server
        if ((::listen(_tickle_server_socket, 3)) < 0)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_LISTEN",
                "Received error number $0 while listening to the internal "
                    "socket.",
                getSocketError());
            throw Exception(parms);
        }

        // make sure we have the correct socket for our server
        int sock = ::getsockname(
            _tickle_server_socket,
            reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
            &_addr_size);
        if (sock < 0)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_SOCKNAME",
                "Received error number $0 while getting the internal socket "
                    "name.",
                getSocketError());
            throw Exception(parms);
        }

        /* set up the tickle client/connector */

        // get a socket for our tickle client
        if ((_tickle_client_socket =
                 Socket::createSocket(PF_INET, SOCK_STREAM, 0)) ==
            PEGASUS_INVALID_SOCKET)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_CLIENT_CREATE",
                "Received error number $0 while creating the internal client "
                    "socket.",
                getSocketError());
            throw Exception(parms);
        }

        // setup the address of the client
        memset(&_tickle_client_addr, 0, sizeof(_tickle_client_addr));

        _tickle_client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        _tickle_client_addr.sin_family = PF_INET;
        _tickle_client_addr.sin_port = 0;

        // bind socket to client side
        if ((::bind(_tickle_client_socket,
                 reinterpret_cast<struct sockaddr*>(&_tickle_client_addr),
                 sizeof(_tickle_client_addr))) < 0)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_CLIENT_BIND",
                "Received error number $0 while binding the internal client "
                    "socket.",
                getSocketError());
            throw Exception(parms);
        }

        // connect to server side
        if ((::connect(_tickle_client_socket,
                 reinterpret_cast<struct sockaddr*>(&_tickle_server_addr),
                 sizeof(_tickle_server_addr))) < 0)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_CLIENT_CONNECT",
                "Received error number $0 while connecting the internal "
                    "client socket.",
                getSocketError());
            throw Exception(parms);
        }

        /* set up the slave connection */
        memset(&_tickle_peer_addr, 0, sizeof(_tickle_peer_addr));
        SocketLength peer_size = sizeof(_tickle_peer_addr);
        Threads::sleep(1);

        // this call may fail, we will try a max of 20 times to establish
        // this peer connection
        if ((_tickle_peer_socket = ::accept(_tickle_server_socket,
                 reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
                 &peer_size)) < 0)
        {
            if (_tickle_peer_socket == PEGASUS_SOCKET_ERROR &&
                getSocketError() == PEGASUS_NETWORK_TRYAGAIN)
            {
                int retries = 0;
                do
                {
                    Threads::sleep(1);
                    _tickle_peer_socket = ::accept(
                        _tickle_server_socket,
                        reinterpret_cast<struct sockaddr*>(&_tickle_peer_addr),
                        &peer_size);
                    retries++;
                } while (_tickle_peer_socket == PEGASUS_SOCKET_ERROR &&
                         getSocketError() == PEGASUS_NETWORK_TRYAGAIN &&
                         retries < 20);
            }
            // TCP/IP is down, destroy sockets and retry again.
            if (_tickle_peer_socket == PEGASUS_SOCKET_ERROR &&
                getSocketError() == PEGASUS_NETWORK_TCPIP_STOPPED)
            {
                // destroy everything
                uninitializeTickler();
                // retry again.
                continue;
            }
        }
        if (_tickle_peer_socket == PEGASUS_SOCKET_ERROR)
        {
            MessageLoaderParms parms(
                "Common.Monitor.TICKLE_ACCEPT",
                "Received error number $0 while accepting the internal "
                    "socket connection.",
                getSocketError());
            throw Exception(parms);
        }
        else
        {
            // socket is ok
            break;
        }
    } while (1); // try until TCP/IP is restarted

    Socket::disableBlocking(_tickle_peer_socket);
    Socket::disableBlocking(_tickle_client_socket);

    // add the tickler to the list of entries to be monitored and set to
    // IDLE because Monitor only
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

void Monitor::tickle()
{
    static char _buffer[] =
    {
      '0','0'
    };

    AutoMutex autoMutex(_tickle_mutex);
    Socket::write(_tickle_client_socket,&_buffer, 2);
}

void Monitor::setState(
    Uint32 index,
    _MonitorEntry::entry_status status)
{
    AutoMutex autoEntryMutex(_entry_mut);
    // Set the state to requested state
    _entries[index]._status = status;
}

void Monitor::run(Uint32 milliseconds)
{


    struct timeval tv = {milliseconds/1000, milliseconds%1000*1000};

    fd_set fdread;
    FD_ZERO(&fdread);

    AutoMutex autoEntryMutex(_entry_mut);

    ArrayIterator<_MonitorEntry> entries(_entries);

    // Check the stopConnections flag.  If set, clear the Acceptor monitor
    // entries
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

    for (int indx = 0; indx < (int)entries.size(); indx++)
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
                PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                    "Monitor::run - Ignoring connection delete request "
                        "because responses are still pending. "
                        "connection=0x%p, socket=%d\n",
                    (void *)&h, h.getSocket()));
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

            // After enqueue a message and the autoEntryMutex has been
            // released and locked again, the array of _entries can be
            // changed. The ArrayIterator has be reset with the original
            // _entries.
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
    for (int indx = 0; indx < (int)entries.size(); indx++)
    {
       if (maxSocketCurrentPass < entries[indx].socket)
           maxSocketCurrentPass = entries[indx].socket;

       if (entries[indx]._status.get() == _MonitorEntry::IDLE)
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

    // After enqueue a message and the autoEntryMutex has been released and
    // locked again, the array of _entries can be changed. The ArrayIterator
    // has be reset with the original _entries
    entries.reset(_entries);

    if (events == PEGASUS_SOCKET_ERROR)
    {
        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
            "Monitor::run - errorno = %d has occurred on select.", errno));
        // The EBADF error indicates that one or more or the file
        // descriptions was not valid. This could indicate that
        // the entries structure has been corrupted or that
        // we have a synchronization error.

        PEGASUS_ASSERT(errno != EBADF);
    }
    else if (events)
    {
        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
            "Monitor::run select event received events = %d, monitoring %d "
                "idle entries",
            events, _idleEntries));
        for (int indx = 0; indx < (int)entries.size(); indx++)
        {
            // The Monitor should only look at entries in the table that are
            // IDLE (i.e., owned by the Monitor).
            if ((entries[indx]._status.get() == _MonitorEntry::IDLE) &&
                (FD_ISSET(entries[indx].socket, &fdread)))
            {
                MessageQueue *q = MessageQueue::lookup(entries[indx].queueId);
                PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                    "Monitor::run indx = %d, queueId =  %d, q = %p",
                    indx, entries[indx].queueId, q));
                PEGASUS_ASSERT(q !=0);

                try
                {
                    if (entries[indx]._type == Monitor::CONNECTION)
                    {
                        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                            "entries[indx].type for indx = %d is "
                                "Monitor::CONNECTION",
                            indx));
                        static_cast<HTTPConnection *>(q)->_entry_index = indx;

                        // Do not update the entry just yet. The entry gets
                        // updated once the request has been read.
                        //entries[indx]._status = _MonitorEntry::BUSY;

                        // If allocate_and_awaken failure, retry on next
                        // iteration
/* Removed for PEP 183.
                        if (!MessageQueueService::get_thread_pool()->
                                allocate_and_awaken((void *)q, _dispatch))
                        {
                            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA,
                                Tracer::LEVEL2,
                                "Monitor::run: Insufficient resources to "
                                    "process request.");
                            entries[indx]._status = _MonitorEntry::IDLE;
                            return true;
                        }
*/
// Added for PEP 183
                        HTTPConnection *dst =
                            reinterpret_cast<HTTPConnection *>(q);
                        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                            "Monitor::_dispatch: entering run() for "
                                "indx = %d, queueId = %d, q = %p",
                            dst->_entry_index,
                            dst->_monitor->_entries[dst->_entry_index].queueId,
                            dst));

                        try
                        {
                            dst->run(1);
                        }
                        catch (...)
                        {
                            PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4,
                                "Monitor::_dispatch: exception received");
                        }
                        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                            "Monitor::_dispatch: exited run() for index %d",
                            dst->_entry_index));

                        // It is possible the entry status may not be set to
                        // busy.  The following will fail in that case.
                        // PEGASUS_ASSERT(dst->_monitor->_entries[
                        //     dst->_entry_index]._status.get() ==
                        //    _MonitorEntry::BUSY);
                        // Once the HTTPConnection thread has set the status
                        // value to either Monitor::DYING or Monitor::IDLE,
                        // it has returned control of the connection to the
                        // Monitor.  It is no longer permissible to access
                        // the connection or the entry in the _entries table.

                        // The following is not relevant as the worker thread
                        // or the reader thread will update the status of the
                        // entry.
                        //if (dst->_connectionClosePending)
                        //{
                        //  dst->_monitor->_entries[dst->_entry_index]._status =
                        //    _MonitorEntry::DYING;
                        //}
                        //else
                        //{
                        //  dst->_monitor->_entries[dst->_entry_index]._status =
                        //    _MonitorEntry::IDLE;
                        //}
// end Added for PEP 183
                    }
                    else if (entries[indx]._type == Monitor::INTERNAL)
                    {
                        // set ourself to BUSY,
                        // read the data
                        // and set ourself back to IDLE

                        entries[indx]._status = _MonitorEntry::BUSY;
                        static char buffer[2];
                        Sint32 amt =
                            Socket::read(entries[indx].socket,&buffer, 2);

                        if (amt == PEGASUS_SOCKET_ERROR &&
                            getSocketError() == PEGASUS_NETWORK_TCPIP_STOPPED)
                        {
                            PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4,
                                "Monitor::run: Tickler socket got an IO error. "
                                    "Going to re-create Socket and wait for "
                                    "TCP/IP restart.");
                            uninitializeTickler();
                            initializeTickler();
                        }
                        else
                        {
                            entries[indx]._status = _MonitorEntry::IDLE;
                        }
                    }
                    else
                    {
                        PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
                            "Non-connection entry, indx = %d, has been "
                                "received.",
                            indx));
                        int events = 0;
                        events |= SocketMessage::READ;
                        Message* msg = new SocketMessage(
                            entries[indx].socket, events);
                        entries[indx]._status = _MonitorEntry::BUSY;
                        SocketHandle sock = entries[indx].socket;
                        _entry_mut.unlock();
                        q->enqueue(msg);
                        _entry_mut.lock();

                        // If _entries[indx] no longer refers to the same 
                        // socket, then another thread has changed _entries[].
                        // We must return now and skip the code below that
                        // updates _entries[indx].
                        if (Uint32(indx) >= _entries.size() ||
                            _entries[indx].socket != sock)
                        {
                            return;
                        }

                        // After enqueue a message and the autoEntryMutex has
                        // been released and locked again, the array of
                        // entries can be changed. The ArrayIterator has be
                        // reset with the original _entries
                        entries.reset(_entries);
                        entries[indx]._status = _MonitorEntry::IDLE;
                    }
                }
                catch (...)
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


int Monitor::solicitSocketMessages(
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
    if ((int)_solicitSocketCount >= (size-1))
    {
        for (int i = 0; i < ((int)_solicitSocketCount - (size-1)); i++)
        {
            _MonitorEntry entry(0, 0, 0);
            _entries.append(entry);
        }
    }

    for (int index = 1; index < (int)_entries.size(); index++)
    {
        try
        {
            if (_entries[index]._status.get() == _MonitorEntry::EMPTY)
            {
                _entries[index].socket = socket;
                _entries[index].queueId  = queueId;
                _entries[index]._type = type;
                _entries[index]._status = _MonitorEntry::IDLE;

                return index;
            }
        }
        catch (...)
        {
        }
    }
    // decrease the count, if we are here we didn't do anything meaningful
    _solicitSocketCount--;
    PEG_METHOD_EXIT();
    return -1;
}

void Monitor::unsolicitSocketMessages(SocketHandle socket)
{
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::unsolicitSocketMessages");
    AutoMutex autoMut(_entry_mut);

    /*
        Start at index = 1 because _entries[0] is the tickle entry which
        never needs to be EMPTY;
    */
    unsigned int index;
    for (index = 1; index < _entries.size(); index++)
    {
        if (_entries[index].socket == socket)
        {
            _entries[index]._status = _MonitorEntry::EMPTY;
            _entries[index].socket = PEGASUS_INVALID_SOCKET;
            _solicitSocketCount--;
            break;
        }
    }

    /*
        Dynamic Contraction:
        To remove excess entries we will start from the end of the _entries
        array and remove all entries with EMPTY status until we find the
        first NON EMPTY.  This prevents the positions, of the NON EMPTY
        entries, from being changed.
    */
    index = _entries.size() - 1;
    while (_entries[index]._status.get() == _MonitorEntry::EMPTY)
    {
        if (_entries.size() > MAX_NUMBER_OF_MONITOR_ENTRIES)
                _entries.remove(index);
        index--;
    }
    PEG_METHOD_EXIT();
}

// Note: this is no longer called with PEP 183.
ThreadReturnType PEGASUS_THREAD_CDECL Monitor::_dispatch(void* parm)
{
    HTTPConnection *dst = reinterpret_cast<HTTPConnection *>(parm);
    PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
        "Monitor::_dispatch: entering run() for indx  = %d, queueId = %d, "
            "q = %p",
        dst->_entry_index,
        dst->_monitor->_entries[dst->_entry_index].queueId,
        dst));

    try
    {
        dst->run(1);
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(TRC_HTTP, Tracer::LEVEL4,
            "Monitor::_dispatch: exception received");
    }
    PEG_TRACE((TRC_HTTP, Tracer::LEVEL4,
        "Monitor::_dispatch: exited run() for index %d", dst->_entry_index));

    PEGASUS_ASSERT(dst->_monitor->_entries[dst->_entry_index]._status.get() ==
        _MonitorEntry::BUSY);

    // Once the HTTPConnection thread has set the status value to either
    // Monitor::DYING or Monitor::IDLE, it has returned control of the
    // connection to the Monitor.  It is no longer permissible to access the
    // connection or the entry in the _entries table.
    if (dst->_connectionClosePending)
    {
        dst->_monitor->_entries[dst->_entry_index]._status =
            _MonitorEntry::DYING;
    }
    else
    {
        dst->_monitor->_entries[dst->_entry_index]._status =
            _MonitorEntry::IDLE;
    }
    return 0;
}

PEGASUS_NAMESPACE_END
