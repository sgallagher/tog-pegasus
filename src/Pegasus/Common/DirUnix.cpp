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
// Modified By: Amit K Arora, IBM (amita@in.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "Dir.h"
#include "InternalException.h"

#include <iostream>

#ifdef PEGASUS_OS_OS400
typedef struct os400_pnstruct
{
  Qlg_Path_Name_T qlg_struct;
  char * pn;
} OS400_PNSTRUCT;
#endif


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

#ifdef PEGASUS_OS_OS400
    CString tmpPathclone = _clonePath(_path);
    const char* tmpPath = tmpPathclone;
    OS400_PNSTRUCT pathname;
    memset((void*)&pathname, 0x00, sizeof(OS400_PNSTRUCT));
    pathname.qlg_struct.CCSID = 1208;
#pragma convert(37)
    memcpy(pathname.qlg_struct.Country_ID,"US",2);
    memcpy(pathname.qlg_struct.Language_ID,"ENU",3);
#pragma convert(0)
    pathname.qlg_struct.Path_Type = QLG_PTR_SINGLE;
    pathname.qlg_struct.Path_Length = strlen(tmpPath);
    pathname.qlg_struct.Path_Name_Delimiter[0] = '/';
    pathname.pn = (char *)tmpPath;
    _dirRep.dir = QlgOpendir((Qlg_Path_Name_T *)&pathname);
#else
    _dirRep.dir = opendir(_clonePath(_path));
#endif
    if (_dirRep.dir)
    {
#ifdef PEGASUS_HAS_READDIR_R
	// Need to use readdir_r since we are multithreaded
#ifdef PEGASUS_OS_OS400
	if (QlgReaddir_r(_dirRep.dir, &_dirRep.buffer, &_dirRep.entry) != 0)
#else
	if (readdir_r(_dirRep.dir, &_dirRep.buffer, &_dirRep.entry) != 0)
#endif
        {
	    _more = false;
            closedir(_dirRep.dir);
	    throw CannotOpenDirectory(_path);
        }
#else
	_dirRep.entry = readdir(_dirRep.dir);
#endif
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

#if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
const Uint32 Dir::getInode() const
{
        return Uint32(_dirRep.entry->d_fileno);
}
#endif

const char* Dir::getName() const
{
#ifdef PEGASUS_OS_OS400
    _dirRep.entry->d_lg_name[_dirRep.entry->d_lg_qlg.Path_Length] = 0x00;
    return _more ? _dirRep.entry->d_lg_name : "";
#else
    return _more ? _dirRep.entry->d_name : "";
#endif
}

void Dir::next()
{
    if (_more)
    {
#ifdef PEGASUS_HAS_READDIR_R
	// Need to use readdir_r since we are multithreaded
#ifdef PEGASUS_OS_OS400
	if (QlgReaddir_r(_dirRep.dir, &_dirRep.buffer, &_dirRep.entry) != 0)
#else
#ifdef PEGASUS_OS_ZOS
    errno=0;
#endif
    if (readdir_r(_dirRep.dir, &_dirRep.buffer, &_dirRep.entry) != 0)
#endif
        {
	    _more = false;
	    throw CannotOpenDirectory(_path);
        }
#else
	_dirRep.entry = readdir(_dirRep.dir);
#endif
	_more = _dirRep.entry != NULL;
    }
}

PEGASUS_NAMESPACE_END
