//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Michael E. Brasher
//
// $Log: WindowsSelector.cpp,v $
// Revision 1.2  2001/04/08 05:06:06  mike
// New Files for Channel Implementation
//
// Revision 1.1  2001/04/08 04:46:11  mike
// Added new selector class for windows
//
//
//END_HISTORY

#include "WindowsSelector.h"

PEGASUS_NAMESPACE_BEGIN

#define FD_SETSIZE 4096

#include <winsock.h>

// This wrapper routine is needed to allow the select() WINSOCK routine to be 
// called from the Selector::select() method. Using the global namespace
// qualifier failed since WINSOCK select() is not in the global namespace.

static inline int select_wrapper(
    int nfds,
    fd_set* rd_fd_set,
    fd_set* wr_fd_set,
    fd_set* ex_fd_set,
    const struct timeval* tv)
{
    return select(FD_SETSIZE, rd_fd_set, wr_fd_set, ex_fd_set, tv);
}

////////////////////////////////////////////////////////////////////////////////
//
// WindowsSelectorRep
//
////////////////////////////////////////////////////////////////////////////////

struct WindowsSelectorRep
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
// WindowsSelector
//
////////////////////////////////////////////////////////////////////////////////


WindowsSelector::WindowsSelector()
{
    _rep = new WindowsSelectorRep;
    FD_ZERO(&_rep->rd_fd_set);
    FD_ZERO(&_rep->wr_fd_set);
    FD_ZERO(&_rep->ex_fd_set);
    FD_ZERO(&_rep->active_rd_fd_set);
    FD_ZERO(&_rep->active_wr_fd_set);
    FD_ZERO(&_rep->active_ex_fd_set);
}

WindowsSelector::~WindowsSelector()
{
    for (Uint32 i = 0, n = _entries.getSize(); i < n; i++)
	delete _entries[i].handler;

    delete _rep;
}

Boolean WindowsSelector::select(Uint32 milliseconds)
{
    // Check for events on the selected file descriptors. Only do this if
    // there were no undispatched events from last time.

    static int count = 0;

    if (count == 0)
    {
	memcpy(&_rep->active_rd_fd_set, &_rep->rd_fd_set, sizeof(fd_set));
	memcpy(&_rep->active_wr_fd_set, &_rep->wr_fd_set, sizeof(fd_set));
	memcpy(&_rep->active_ex_fd_set, &_rep->ex_fd_set, sizeof(fd_set));

	const Uint32 SEC = milliseconds / 1000;
	const Uint32 USEC = (milliseconds % 1000) * 1000;
	struct timeval tv = { SEC, USEC };

	count = select_wrapper(
	    FD_SETSIZE, 
	    &_rep->active_rd_fd_set, 
	    &_rep->active_wr_fd_set, 
	    &_rep->active_ex_fd_set, 
	    &tv);

	if (count == 0)
	    return false;
	else if (count == SOCKET_ERROR)
	{
	    count = 0;
	    return false;
	}
    }

    // Dispatch any handler events:

    for (Uint32 i = 0, n = _entries.getSize(); i < n; i++)
    {
	Uint32 desc = _entries[i].desc;
	Uint32 reasons = 0;

	if (FD_ISSET(desc, &_rep->active_rd_fd_set))
	    reasons |= READ;

	if (FD_ISSET(desc, &_rep->active_wr_fd_set))
	    reasons |= WRITE;

	if (FD_ISSET(desc, &_rep->active_ex_fd_set))
	    reasons |= EXCEPTION;

	if (reasons)
	{
	    SelectorHandler* handler = _entries[i].handler;

	    if (!handler->handle(desc, reasons))
		removeHandler(handler);

	    if (reasons & WRITE)
		FD_CLR(desc, &_rep->wr_fd_set);

	    if (reasons & EXCEPTION)
		FD_CLR(desc, &_rep->ex_fd_set);

	    if (reasons & READ)
		FD_CLR(desc, &_rep->rd_fd_set);

	    count--;
	    return true;
	}
    }

    return false;
}

Boolean WindowsSelector::addHandler(
    Uint32 desc,
    Uint32 reasons,
    SelectorHandler* handler)
{
    // See whether a handler is already registered for this one:

    Uint32 pos = _findEntry(desc);

    if (pos != Uint32(-1))
	return false;

    // Set the reasons:

    if (reasons & READ)
	FD_SET(desc, &_rep->rd_fd_set);

    if (reasons & WRITE)
	FD_SET(desc, &_rep->wr_fd_set);

    if (reasons & EXCEPTION)
	FD_SET(desc, &_rep->ex_fd_set);

    // Add the entry to the list:

    Entry entry = { desc, handler };
    _entries.append(entry);

    // Success!

    return true;
}

Boolean WindowsSelector::removeHandler(SelectorHandler* handler)
{
    // Look for the given handler and remove it!

    for (Uint32 i = 0, n = _entries.getSize(); i < n; i++)
    {
	if (_entries[i].handler == handler)
	{
	    Uint32 desc = _entries[i].desc;
	    FD_CLR(desc, &_rep->rd_fd_set);
	    FD_CLR(desc, &_rep->wr_fd_set);
	    FD_CLR(desc, &_rep->ex_fd_set);
	    _entries.remove(i);
	    return true;
	}
    }

    // Not found:

    return false;
}

Uint32 WindowsSelector::_findEntry(Uint32 desc) const
{
    for (Uint32 i = 0, n = _entries.getSize(); i < n; i++)
    {
	if (_entries[i].desc == desc)
	    return i;
    }

    return Uint32(-1);
}

PEGASUS_NAMESPACE_END
