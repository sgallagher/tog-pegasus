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

#ifndef _Pegasus_Common_Linkable_h
#define _Pegasus_Common_Linkable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <cstring>

#ifdef PEGASUS_DEBUG
# define PEGASUS_LINKABLE_SANITY
#endif

#define PEGASUS_LINKABLE_MAGIC 0xEB1C2781

PEGASUS_NAMESPACE_BEGIN

/** Non-virtual base class for anything that can be placed into a List.
*/
struct Linkable
{
    Linkable() : next(0), prev(0), list(0)
    {
#ifdef PEGASUS_LINKABLE_SANITY
	magic = PEGASUS_LINKABLE_MAGIC;
#endif
    }

    ~Linkable() 
    { 
#ifdef PEGASUS_LINKABLE_SANITY
	PEGASUS_ASSERT(magic == PEGASUS_LINKABLE_MAGIC);
	memset(this, 0xDD, sizeof(Linkable)); 
#endif
    }

    // ATTN: consider making private and fixing all places that have
    // copy constructors.
    Linkable(const Linkable&) : next(0), prev(0), list(0)
    {
#ifdef PEGASUS_LINKABLE_SANITY
	magic = PEGASUS_LINKABLE_MAGIC;
#endif
    }

    // Magic number (only used when PEGASUS_LINKABLE_SANITY defined).
    Uint32 magic;
    Linkable* next;
    Linkable* prev;
    // Backpointer to list that contains this element.
    class ListRep* list;
};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_Linkable_h */
