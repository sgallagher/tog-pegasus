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
<windows.h>). Finthe inclusion of that header which is visible to this \
compilation unit and #define FD_SETZIE to 1024 prior to that inclusion; \
otherwise, less than 64 clients (the default) will be able to connect to the \
CIMOM. PLEASE DO NOT SUPPRESS THIS WARNING; PLEASE FIX THE PROBLEM."

# endif
# define FD_SETSIZE 1024
g# include <windows.h>
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


static AtomicInt _connections = 0;


static struct timeval create_time = {0, 1};
static struct timeval destroy_time = {300, 0};
static struct timeval deadlock_time = {0, 0};

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
    _rep = 0;
    _entries.reserveCapacity(32);
    for( int i = 0; i < 32; i++ )
    {
       _MonitorEntry entry(0, 0, 0);
       _entries.append(entry);
    }
}

Monitor::Monitor(Boolean async)
   : _module_handle(0), _controller(0), _async(async)
{
    Socket::initializeInterface();
    _rep = 0;
    _entries.reserveCapacity(32);
    for( int i = 0; i < 32; i++ )
    {
       _MonitorEntry entry(0, 0, 0);
       _entries.append(entry);
    }
    if( _async == true )
    {
       _thread_pool = new ThreadPool(0, 
				     "Monitor", 
				     0, 
				     0,
				     create_time, 
				     destroy_time, 
				     deadlock_time);
    }
    else 
       _thread_pool = 0;
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
   
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4, "uninitializing interface");
    Socket::uninitializeInterface();
    Tracer::trace(TRC_HTTP, Tracer::LEVEL4,
                  "returning from monitor destructor");
    if(_async == true)
       delete _thread_pool;
}


int Monitor::kill_idle_threads()
{
   static struct timeval now, last;
   gettimeofday(&now, NULL);
   int dead_threads = 0;
   
   if( now.tv_sec - last.tv_sec > 120 )
   {
      gettimeofday(&last, NULL);
      try 
      {
	 dead_threads =  _thread_pool->kill_dead_threads();
      }
      catch(IPCException& )
      {
      }
      
   }
   return dead_threads;
}


Boolean Monitor::run(Uint32 milliseconds)
{

    Boolean handled_events = false;
    int i = 0;
    #if defined(PEGASUS_OS_OS400) || defined(PEGASUS_OS_HPUX)
    struct timeval tv = {milliseconds/1000, milliseconds%1000*1000};
#else
    struct timeval tv = {0, 1};
#endif
    fd_set fdread;
    FD_ZERO(&fdread);
    _entry_mut.lock(pegasus_thread_self());
    
    for( int indx = 0; indx < (int)_entries.size(); indx++)
    {
       if(_entries[indx]._status.value() == _MonitorEntry::IDLE)
       {
	  FD_SET(_entries[indx].socket, &fdread);
       }
    }

    
    int events = select(FD_SETSIZE, &fdread, NULL, NULL, &tv);

#ifdef PEGASUS_OS_TYPE_WINDOWS
    if(events && events != SOCKET_ERROR )
#else
    if(events && events != -1 )
#endif
    {
       for( int indx = 0; indx < (int)_entries.size(); indx++)
       {
	  if(FD_ISSET(_entries[indx].socket, &fdread))
	  {
	     MessageQueue *q = MessageQueue::lookup(_entries[indx].queueId);
	     if(q == 0)
	     {
		try
		{
		   _entries[indx]._status = _MonitorEntry::EMPTY;
		}
		catch(...)
		{

		}
		continue;
	     }
	     try 
	     {
		if(_entries[indx]._type == Monitor::CONNECTION)
		{
		   static_cast<HTTPConnection *>(q)->_entry_index = indx;
		   if(static_cast<HTTPConnection *>(q)->_dying.value() > 0 )
		   {
		      _entries[indx]._status = _MonitorEntry::DYING;
		      MessageQueue & o = static_cast<HTTPConnection *>(q)->get_owner();
		      Message* message= new CloseConnectionMessage(_entries[indx].socket);
		      message->dest = o.getQueueId();
		      _entry_mut.unlock();
		      o.enqueue(message);
		      return true;
		   }
		   _entries[indx]._status = _MonitorEntry::BUSY;
		   _thread_pool->allocate_and_awaken((void *)q, _dispatch);
		}
		else
		{
		   int events = 0;
		   events |= SocketMessage::READ;
		   Message *msg = new SocketMessage(_entries[indx].socket, events);
		   _entries[indx]._status = _MonitorEntry::BUSY;
		   _entry_mut.unlock();

		   q->enqueue(msg);
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
    _entry_mut.unlock();
    return(handled_events);
}



int  Monitor::solicitSocketMessages(
    Sint32 socket, 
    Uint32 events,
    Uint32 queueId, 
    int type)
{

   PEG_METHOD_ENTER(TRC_HTTP, "Monitor::solicitSocketMessages");

   int index = -1;
   _entry_mut.lock(pegasus_thread_self());
   
   for(index = 0; index < (int)_entries.size(); index++)
   {
      try 
      {
	 if(_entries[index]._status.value() == _MonitorEntry::EMPTY)
	 {
	    _entries[index].socket = socket;
	    _entries[index].queueId  = queueId;
	    _entries[index]._type = type;
	    _entries[index]._status = _MonitorEntry::IDLE;
	    _entry_mut.unlock();
	    
	    return index;
	 }
      }
      catch(...)
      {
      }

   }
      _entry_mut.unlock();
   PEG_METHOD_EXIT();
   return index;
}

void Monitor::unsolicitSocketMessages(Sint32 socket)
{
    PEG_METHOD_ENTER(TRC_HTTP, "Monitor::unsolicitSocketMessages");
    _entry_mut.lock(pegasus_thread_self());
    
    for(int index = 0; index < (int)_entries.size(); index++)
    {
       if(_entries[index].socket == socket)
       {
	  _entries[index]._status = _MonitorEntry::EMPTY;
	  break;
       }
    }
    _entry_mut.unlock();
    PEG_METHOD_EXIT();
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL Monitor::_dispatch(void *parm)
{
   HTTPConnection *dst = reinterpret_cast<HTTPConnection *>(parm);
   
   dst->run(1);
   if(  dst->_monitor->_entries.size() > (Uint32)dst->_entry_index )
      dst->_monitor->_entries[dst->_entry_index]._status = _MonitorEntry::IDLE;
   
   return 0;
}



PEGASUS_NAMESPACE_END
