//%////-*-c++-*-////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Monitor_h 
#define Pegasus_Monitor_h 

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/ModuleController.h>

PEGASUS_NAMESPACE_BEGIN

class  _MonitorEntry
{
   public:
      Sint32 socket;
      Uint32 queueId;
      AtomicInt busy;
      _MonitorEntry(Sint32 sock, Uint32 q)
	 : socket(sock), queueId(q), busy(0)
      {
      }
      
};

struct MonitorRep;

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

    /** Default constructor. */
    Monitor();

    /** This destruct deletes all handlers which were installed. */
    ~Monitor();

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
    Boolean solicitSocketMessages(
	Sint32 socket, 
	Uint32 events,
	Uint32 queueId);

    /** Unsolicit messages on the given socket.

	@param socket on which to unsolicit messages.
	@return false if no such solicitation has been made on the given socket.
    */
      Boolean unsolicitSocketMessages(Sint32 socket);

      /** dispatch a message to the cimom on an independent thread 
       */
      static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _dispatch(void *);

      class monitor_dispatch 
      {
	 public:
	    Monitor *_myself;
	    MessageQueue *_decoder;
	    Uint32 _entry;
	    Sint32 _socket;
	    Uint32 _events;
	    
	    
	    monitor_dispatch(Monitor *myself, 
			     MessageQueue *decoder,
			     Uint32 entry, 
			     Sint32 socket,
			     Uint32 events)
	       : _myself(myself),  
		 _decoder(decoder),
		 _entry(entry),
		 _socket(socket),
		 _events(events)
	    {
	       
	    }
	    
	    ~monitor_dispatch(void) 
	    {
	    }
	    
	 private:
	    monitor_dispatch(void);
	    monitor_dispatch(const monitor_dispatch &);
	    monitor_dispatch & operator = (const monitor_dispatch &);
      };
      void set_async(Boolean async);

private:

    Uint32 _findEntry(Sint32 socket) const;

      Array<_MonitorEntry> _entries;
      MonitorRep* _rep;
      pegasus_module * _module_handle;
      ModuleController * _controller;
      Boolean _async;
      
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Monitor_h */
