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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CGIQueryString_h
#define Pegasus_CGIQueryString_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_NAMESPACE_BEGIN

struct CGIQueryStringEntry
{
    char *name;
    char *value;

    ~CGIQueryStringEntry() { }
};

/** The CGIQueryString class is used to parse and extract the fields from
    the CGI QUERY_STRING environment variable which is set by the Web Server
    and passed to the CGI program. The value of that environment variable
    may be passed to the constructor. Methods are provided for getting the
    names and values of each field.

    For the format of a query string, see a book on CGI.
*/
class CGIQueryString
{
public:

    /// Constructs from the value of the QUERY_STRING environment variable.
    CGIQueryString(char* queryString);

    /// Returns the number of fields.
    Uint32 getCount() const { return _entries.size(); }

    /// Returns the name of the ith field.
    const char* getName(Uint32 i) const { return _entries[i].name; }

    /// Returns the value of the ith field.
    const char* getValue(Uint32 i) const { return _entries[i].value; }

    /// Returns the value of the field with the given name.
    const char* findValue(const char* name) const;

private:

    Array<CGIQueryStringEntry> _entries;

    static void _parseCGIQueryString(
	char* queryString, 
	Array<CGIQueryStringEntry>& entries);

public:


};

inline int operator==(
    const CGIQueryStringEntry& x, 
    const CGIQueryStringEntry& y)
{
    return 0;
}

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CGIQueryString_h */
