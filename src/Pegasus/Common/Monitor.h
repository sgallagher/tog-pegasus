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
// Modified By:   Amit Arora (amita@in.ibm.com) for Bug#1170
//                Sushma Fernandes (sushma@hp.com) for Bug#2057
//              Josephine Eskaline Joyce (jojustin@in.ibm.com) for PEP#101
//                Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Monitor_h 
#define Pegasus_Monitor_h 

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ModuleController.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/DQueue.h>
#include <Pegasus/Common/Sharable.h>
#include <Pegasus/Common/Linkage.h> 
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE _MonitorEntry
{
public:
  Sint32 socket;
  Uint32 queueId;
  AtomicInt _status;
  int _type;
      
  _MonitorEntry(Sint32 sock, Uint32 q, int Type)
    : socket(sock), queueId(q), _status(EMPTY), _type(Type)
	   
  {
  }

  _MonitorEntry() : socket(0), queueId(0), _status(EMPTY), _type(0)
  {
  }
      
  Boolean operator ==(const void *key) const 
  {
    if(key != 0 && 
       (socket == (reinterpret_cast<_MonitorEntry *>(const_cast<void *>(key)))->socket))
      return true;
    return false;
  }
      
  Boolean operator ==(const _MonitorEntry & key) const
  {
    if(key.socket == socket)
      return true;
    return false;
  }

  _MonitorEntry & operator =(const _MonitorEntry & entry)
  {
    if( this != &entry )
      {
	this->socket = entry.socket;
	this->queueId = entry.queueId;
	this->_status = entry._status;
	this->_type = entry._type;
      }
	 
    return *this;
  }
      
  enum entry_status 
    {
      IDLE,
      BUSY,
      DYING,
      EMPTY
    };
};

/** This message occurs when there is activity on a socket. */
class SocketMessage : public Message
{
public:

  enum Events { READ = 1, WRITE = 2, EXCEPTION = 4 };
      

  SocketMessage(Sint32 socket_, Uint32 events_) :
    Message(SOCKET_MESSAGE), socket(socket_), events(events_)
  {
  }

  Sint32 socket;
  Uint32 events;
};

/** This class monitors system-level events and notifies its clients of these
    events by posting messages to their queues.

    The monitor generates following message types:

    <ul>
    <li> SocketMessage - occurs when activity on a socket </li>
    </ul>

    Clients solicit these messages by calling one of the following methods:

    <ul>
    <li> solicitSocketMessages() </li>
    </ul>

    The following example shows how to solicit socket messages:

    <pre>
    Monitor monitor;
    Sint32 socket;
    Uint32 queueId;


    ...

    monitor.solicitSocketMessages(
    socket, 
    SocketMessage::READ | SocketMessage::WRITE, 
    queueId);
    </pre>

    Each time activity occurs on the given socket, a SocketMessage is
    enqueued on the given queue.

    In order the monitor to generate messages, it must be run by calling
    the run() method as shown below:

    <pre>
    Monitor monitor;

    ...

    Uint32 milliseconds = 5000;
    monitor.run(milliseconds);
    </pre>

    In this example, the monitor is run for five seconds. The run method
    returns after the first message is occurs or five seconds has transpired
    (whichever occurs first).
*/
class PEGASUS_COMMON_LINKAGE Monitor
{
public:
  enum Type 
    {
      UNTYPED, ACCEPTOR, CONNECTOR, CONNECTION, INTERNAL
    };
      
      
  /** Default constructor. */
  Monitor();

  /** This destruct deletes all handlers which were installed. */
  ~Monitor();

  /** Sets the state of the monitor entry to the specified state.
      This is used to synchronize the monitor and the worker
      thread. Bug# 2057 */
  void setState( Uint32 index, _MonitorEntry::entry_status status );

  void initializeTickler();
  void tickle();
  /** Monitor system-level for the given number of milliseconds. Post a
      message to the corresponding queue when such an event occurs.
      Return after the time has elapsed or a single event has occurred,
      whichever occurs first.

      @param timeoutMsec the number of milliseconds to wait for an event.
      @return true if an event occured.
  */
  Boolean run(Uint32 timeoutMsec);

  /** Solicit interest in SocketMessages. Note that there may only 
      be one solicitor per socket.

      @param socket the socket to monitor for activity.
      @param events socket events to monitor (see the SocketMessage::Events
      enumeration for details).
      @param queueId of queue on which to post socket messages.
      @return false if messages have already been solicited on this socket.
  */
  int solicitSocketMessages(
			    Sint32 socket, 
			    Uint32 events,
			    Uint32 queueId,
			    int type);

  /** Unsolicit messages on the given socket.

  @param socket on which to unsolicit messages.
  @return false if no such solicitation has been made on the given socket.
  */
  void unsolicitSocketMessages(Sint32);

  /** dispatch a message to the cimom on an independent thread 
      Note: The Monitor class uses the MessageQueueService ThreadPool.
      This ThreadPool is only available if it has been initialized by
      the MessageQueueService.  Therefore, the Monitor class should
      only be used when the MessageQueueService is active in the
      system.
   */
  static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _dispatch(void *);
      
  /** stop listening for client connections 
   */
  void stopListeningForConnections(Boolean wait);

private:
      
  Array<_MonitorEntry> _entries;
  Mutex _entry_mut;
  AtomicInt _stopConnections;
  Semaphore _stopConnectionsSem;
  Uint32 _solicitSocketCount;  // tracks how many times solicitSocketCount() has been called
  friend class HTTPConnection;
  struct sockaddr_in _tickle_server_addr;
  struct sockaddr_in _tickle_client_addr;
  struct sockaddr_in _tickle_peer_addr;
  Sint32 _tickle_client_socket; 
  Sint32 _tickle_server_socket;      
  Sint32 _tickle_peer_socket;
  Mutex _tickle_mutex;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Monitor_h */
