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
// Author: Mike Brasher (m.brasher@inovadevelopment.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "TSD.h"

PEGASUS_NAMESPACE_BEGIN

struct TSD_Entry
{
    ThreadType self;
    void* data;
};

TSD::TSD() : _entries(0), _num_entries(0)
{
}

TSD::~TSD()
{
    free(_entries);
}

void TSD::set(void* data)
{
    AutoMutex auto_mutex(_mutex);
    ThreadType self = Threads::self();

    for (size_t i = 0; i < _num_entries; i++)
    {
        if (Threads::equal(self, _entries[i].self))
        {
            _entries[i].data = data;
            return;
        }
    }

    _entries = (TSD_Entry*)realloc(
        _entries, (_num_entries + 1) * sizeof(TSD_Entry));
    _entries[_num_entries].self = self;
    _entries[_num_entries].data = data;
    _num_entries++;
}

void* TSD::get()
{
    AutoMutex auto_mutex(_mutex);
    ThreadType self = Threads::self();

    for (size_t i = 0; i < _num_entries; i++)
    {
        if (Threads::equal(self, _entries[i].self))
        {
            void* data = _entries[i].data;
            return data;
        }
    }

    return 0;
}

PEGASUS_NAMESPACE_END
