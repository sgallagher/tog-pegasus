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
    Socket::uninitializeInterface();
}

Boolean Monitor::run(Uint32 milliseconds)
{
#ifdef PEGASUS_OS_TYPE_WINDOWS

    // Windows select() has a strange little bug. It returns immediately if
    // there are no descriptors in the set even if the timeout is non-zero.
    // To work around this, we call Sleep() for now:

    if (_entries.size() == 0)
	Sleep(milliseconds);

#endif

    // Check for events on the selected file descriptors. Only do this if
    // there were no undispatched events from last time.

    static int count = 0;

    if (count == 0)
    {
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
	    return false;
#ifdef PEGASUS_OS_TYPE_WINDOWS
	else if (count == SOCKET_ERROR)
#else
	else if (count == -1)
#endif
	{
	    count = 0;
	    return false;
	}
    }

    for (Uint32 i = 0, n = _entries.size(); i < n; i++)
    {
	Sint32 socket = _entries[i].socket;
	Uint32 events = 0;

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

	    MessageQueue* queue = MessageQueue::lookup(_entries[i].queueId);

	    if (!queue)
		unsolicitSocketMessages(_entries[i].queueId);


	    Message* message = new SocketMessage(socket, events);
	    queue->enqueue(message);

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

	    count--;
	    return true;
	}
    }

    return false;
}

Boolean Monitor::solicitSocketMessages(
    Sint32 socket, 
    Uint32 events,
    Uint32 queueId)
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

    _MonitorEntry entry = { socket, queueId };
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
            PEG_METHOD_EXIT();
	    return true;
	}
    }

    PEG_METHOD_EXIT();
    return false;
}

Uint32 Monitor::_findEntry(Sint32 socket) const
{
    for (Uint32 i = 0, n = _entries.size(); i < n; i++)
    {
	if (_entries[i].socket == socket)
	    return i;
    }

    return PEG_NOT_FOUND;
}

PEGASUS_NAMESPACE_END
