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
// Modified By: Sean Keenan (sean.keenan@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Dir.h"
#include "InternalException.h"

#include <iostream>


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

Dir::Dir(const String& path)
    : _path(path)
{
    _dirRep.dir = opendir(_clonePath(_path));
    if (_dirRep.dir)
    {
	_dirRep.entry = readdir(_dirRep.dir);
	_more = _dirRep.entry != NULL;
    }
    else
    {
	_more = false;
	throw CannotOpenDirectory(_path);
    }
}

Dir::~Dir()
{
    if (_dirRep.dir)
	closedir(_dirRep.dir);
}

const char* Dir::getName() const
{
    return _more ? _dirRep.entry->d_name : "";
}

void Dir::next()
{
    if (_more)
    {
	_dirRep.entry = readdir(_dirRep.dir);
	_more = _dirRep.entry != NULL;
    }
}

PEGASUS_NAMESPACE_END
