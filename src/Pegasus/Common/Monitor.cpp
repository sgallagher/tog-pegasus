//%/////////////////////////////////////////////////////////////////////////////
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

#include <Pegasus/Common/Config.h>
#include <cstring>
#include "Monitor.h"
#include "MessageQueue.h"
#include "Socket.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/HTTPConnection.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# if defined(FD_SETSIZE) && FD_SETSIZE != 1024
#  error "FD_SETSIZE was not set to 1024 prior to the last inclusion \
of <winsock.h>. It may have been indirectly included (e.g., by including \
<windows.h>). Find the inclusion of that header which is visible to this \
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
# include <unistd.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// MonitorRep
//
////////////////////////////////////////////////////////////////////////////////

struct MonitorRep
{
    fd_set rd_fd_set;
    fd_set wr_fd_set;
    fd_set ex_fd_set;
    fd_set active_rd_fd_set;
    fd_set active_wr_fd_set;
    fd_set active_ex_fd_set;
};

////////////////////////////////////////////////////////////////////////////////
//
// Monitor
//
////////////////////////////////////////////////////////////////////////////////

Monitor::Monitor()
   : _module_handle(0), _controller(0), _async(false)
{
    Socket::initializeInterface();
    _rep = new MonitorRep;
    FD_ZERO(&_rep->rd_fd_set);
    FD_ZERO(&_rep->wr_fd_set);
    FD_ZERO(&_rep->ex_fd_set);
    FD_ZERO(&_rep->active_rd_fd_set);
    FD_ZERO(&_rep->active_wr_fd_set);
    FD_ZERO(&_rep->active_ex_fd_set);
}

Monitor::~Monitor()
{
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "deregistering with module controller");

    if(_module_handle != NULL)
    {
       _controller->deregister_module(PEGASUS_MODULENAME_MONITOR);
       _controller = 0;
       delete _module_handle;
    }
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "deleting rep");
   
    delete _rep;
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "uninitializing interface");
    Socket::uninitializeInterface();
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "returning from monitor destructor");
}



//<<< Tue May 14 20:38:26 2002 mdd >>>
//  register with module controller
//  when it is time to enqueue the message, 
// use an async_thread_exec call to 
// isolate the entire if(events) { enqueue -> fd_clear } block
//  let the thread pool grow and shrink according to load. 

Boolean Monitor::run(Uint32 milliseconds)
{
   // register the monitor as a module to gain access to the cimserver's thread pool 
   // <<< Wed May 15 09:52:16 2002 mdd >>>
   while( _module_handle == NULL)
   {
      try 
      {
	 _controller = &(ModuleController::register_module(PEGASUS_QUEUENAME_CONTROLSERVICE, 
							   PEGASUS_MODULENAME_MONITOR,
							   (void *)this, 
							   0, 
							   0,
							   0,
							   &_module_handle));
	 
      }
      catch(IncompatibleTypes &)
      {
	 ModuleController* controlService =
	    new ModuleController(PEGASUS_QUEUENAME_CONTROLSERVICE);
      }
      catch( AlreadyExists & )
      {
	 break;
      }
   }

#ifdef PEGASUS_OS_TYPE_WINDOWS

    // Windows select() has a strange little bug. It returns immediately if
    // there are no descriptors in the set even if the timeout is non-zero.
    // To work around this, we call Sleep() for now:

    if (_entries.size() == 0)
	Sleep(milliseconds);

#endif

    // Check for events on the selected file descriptors. Only do this if
    // there were no undispatched events from last time.

    int count = 0;


    memcpy(&_rep->active_rd_fd_set, &_rep->rd_fd_set, sizeof(fd_set));
    memcpy(&_rep->active_wr_fd_set, &_rep->wr_fd_set, sizeof(fd_set));
    memcpy(&_rep->active_ex_fd_set, &_rep->ex_fd_set, sizeof(fd_set));
    
    const Uint32 SECONDS = milliseconds / 1000;
    const Uint32 MICROSECONDS = (milliseconds % 1000) * 1000;
    struct timeval tv = { SECONDS, MICROSECONDS };
    
    count = select(
       FD_SETSIZE,
       &_rep->active_rd_fd_set,
       &_rep->active_wr_fd_set,
       &_rep->active_ex_fd_set,
       &tv);
    if (count == 0)
    {
       return false;
    }
    
#ifdef PEGASUS_OS_TYPE_WINDOWS
    else if (count == SOCKET_ERROR)
#else
    else if (count == -1)
#endif
    {
       return false;
    }
    
    Boolean handled_events = false;
    for (Uint32 i = 0, n = _entries.size(); i < _entries.size(); i++)
    {
	Sint32 socket = _entries[i].socket;
	Uint32 events = 0;

	if(_entries[i].dying.value() > 0 )
	{
	   if(_entries[i]._type == Monitor::CONNECTION)
	   {
	      
	      MessageQueue *q = MessageQueue::lookup(_entries[i].queueId);
	      if(q && static_cast<HTTPConnection *>(q)->is_dying() && 
		 (0 == static_cast<HTTPConnection *>(q)->refcount.value()))
	      {
		 static_cast<HTTPConnection *>(q)->lock_connection();
		 static_cast<HTTPConnection *>(q)->unlock_connection();
		 
		 MessageQueue & o = static_cast<HTTPConnection *>(q)->get_owner();
		 Message* message= new CloseConnectionMessage(static_cast<HTTPConnection *>(q)->getSocket());
		 message->dest = o.getQueueId();
		 o.enqueue(message);
		 i--;
		 n = _entries.size();
	      }
	   }
	}

	if (FD_ISSET(socket, &_rep->active_rd_fd_set))
	    events |= SocketMessage::READ;

	if (FD_ISSET(socket, &_rep->active_wr_fd_set))
	    events |= SocketMessage::WRITE;

	if (FD_ISSET(socket, &_rep->active_ex_fd_set))
	    events |= SocketMessage::EXCEPTION;
 
	if (events)
	{
            Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
               "Monitor::run - Socket Event Detected events = %d", events);
	    if (events & SocketMessage::WRITE)
	    {
	       FD_CLR(socket, &_rep->active_wr_fd_set);
	       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			     "Monitor::run FD_CLR WRITE");
	    }
	    if (events & SocketMessage::EXCEPTION)
	    {
  	       FD_CLR(socket, &_rep->active_ex_fd_set);
	       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			     "Monitor::run FD_CLR EXECEPTION");
	    }
	    if (events & SocketMessage::READ)
	    {
	       FD_CLR(socket, &_rep->active_rd_fd_set);
	       Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
			     "Monitor::run FD_CLR READ");
	    }
	    MessageQueue* queue = MessageQueue::lookup(_entries[i].queueId);
	    if( ! queue )
	    {
	       unsolicitSocketMessages(socket);
	       break;
	    }
	    
	    if(_entries[i]._type == Monitor::CONNECTION)
	    {
	       if( static_cast<HTTPConnection *>(queue)->refcount.value() == 0 )
	       {
		  static_cast<HTTPConnection *>(queue)->refcount++;
		  if( false == static_cast<HTTPConnection *>(queue)->is_dying())
		     _controller->async_thread_exec(*_module_handle, _dispatch, (void *)queue);
		  else
		     static_cast<HTTPConnection *>(queue)->refcount--;
	       }
	    }
	    else 
	    {
	       Message* message = new SocketMessage(socket, events);
	       queue->enqueue(message);
	    }
	    count--;
	    pegasus_yield();
	}
	handled_events = true;
    }

    return(handled_events);
}

Boolean Monitor::solicitSocketMessages(
    Sint32 socket, 
    Uint32 events,
    Uint32 queueId, 
    int type)
{
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::solictSocketMessage");

    // See whether a handler is already registered for this one:
    Uint32 pos = _findEntry(socket);

    if (pos != PEGASUS_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
	return false;
    }

    // Set the events:

    if (events & SocketMessage::READ)
	FD_SET(socket, &_rep->rd_fd_set);

    if (events & SocketMessage::WRITE)
	FD_SET(socket, &_rep->wr_fd_set);

    if (events & SocketMessage::EXCEPTION)
	FD_SET(socket, &_rep->ex_fd_set);

    // Add the entry to the list:
    _MonitorEntry entry(socket, queueId, type);
    _entries.append(entry);
    
    // Success!

    PEG_METHOD_EXIT();
    return true;
}

Boolean Monitor::unsolicitSocketMessages(Sint32 socket)
{
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::unsolicitSocketMessage");

    // Look for the given entry and remove it:

    for (Uint32 i = 0, n = _entries.size(); i < n; i++)
    {
	if (_entries[i].socket == socket)
	{
	    Sint32 socket = _entries[i].socket;
	    FD_CLR(socket, &_rep->rd_fd_set);
	    FD_CLR(socket, &_rep->wr_fd_set);
	    FD_CLR(socket, &_rep->ex_fd_set);
	    _entries.remove(i);
            // ATTN-RK-P3-20020521: Need "Socket::close(socket);" here?
            PEG_METHOD_EXIT();
	    return true;
	}
    }
    PEG_METHOD_EXIT();
    return false;
}

Uint32 Monitor::_findEntry(Sint32 socket) 
{
   for (Uint32 i = 0, n = _entries.size(); i < n; i++)
    {
	if (_entries[i].socket == socket)
	    return i;
    }

    return PEG_NOT_FOUND;
}


PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL Monitor::_dispatch(void *parm)
{
   HTTPConnection *dst = reinterpret_cast<HTTPConnection *>(parm);
   if( true == dst->is_dying())
   {
      dst->refcount--;
      return 0;
   }
   if( false == dst->is_dying())
   {
      if(false == dst->run(1))
	 pegasus_sleep(1);
      
   }
   dst->refcount--;
   return 0;
}


PEGASUS_NAMESPACE_END
