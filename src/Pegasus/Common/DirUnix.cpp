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

#include "Destroyer.h"
#include "Dir.h"
#include "InternalException.h"

#include <dirent.h>

PEGASUS_NAMESPACE_BEGIN

// Clone the string to a plain old C-String and null out
// trailing slash (if any).

static CString _clonePath(const String& path)
{
    String clone = path;

    if (clone.size() && clone[clone.size()-1] == '/')
        clone.remove(clone.size()-1);

    return clone.getCString();
}

// ATTN-RK-20021111: Perhaps this should be enabled for all Unix platforms?
#if defined(PEGASUS_OS_OS400) || defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_LINUX_IA64_GNU)
#define PEGASUS_HAS_READDIR_R
#endif

struct DirRep
{
    DIR* dir;
    struct dirent* entry;
#ifdef PEGASUS_HAS_READDIR_R
    struct dirent buffer;
#endif
};

Dir::Dir(const String& path)
    : _path(path)
{
    _rep = new DirRep;
    _rep->dir = opendir(_clonePath(_path));

    if (_rep->dir)
    {
#ifdef PEGASUS_HAS_READDIR_R
	// Need to use readdir_r since we are multithreaded
	if (readdir_r(_rep->dir, &_rep->buffer, &_rep->entry) != 0)
        {
	    _more = false;
	    throw CannotOpenDirectory(_path);
        }
#else
	_rep->entry = readdir(_rep->dir);
#endif
	_more = _rep->entry != NULL;
    }
    else
    {
	_more = false;
	throw CannotOpenDirectory(_path);
    }
}

Dir::~Dir()
{
    if (_rep->dir)
	closedir(_rep->dir);

    delete _rep;
}

const char* Dir::getName() const
{
    return _more ? _rep->entry->d_name : "";
}

void Dir::next()
{
    if (_more)
    {
#ifdef PEGASUS_HAS_READDIR_R
	// Need to use readdir_r since we are multithreaded
	if (readdir_r(_rep->dir, &_rep->buffer, &_rep->entry) != 0)
        {
	    _more = false;
	    throw CannotOpenDirectory(_path);
        }
#else
	_rep->entry = readdir(_rep->dir);
#endif
	_more = _rep->entry != NULL;
    }
}

PEGASUS_NAMESPACE_END
