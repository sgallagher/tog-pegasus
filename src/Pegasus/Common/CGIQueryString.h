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
// Author:
//
// $Log: CGIQueryString.h,v $
// Revision 1.2  2001/02/05 03:40:28  mike
// new documentation
//
// Revision 1.1.1.1  2001/01/14 19:50:35  mike
// Pegasus import
//
//
//END_HISTORY


#ifndef Pegasus_CGIUtils_h
#define Pegasus_CGIUtils_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/** The CGIQueryString class is used to parse and extract the fields from
    the CGI QUERY_STRING environment variable which is set by the Web Server
    and passed to the CGI program. The value of that environment variable
    may be passed to the constructor. Methods are provided for getting the
    names and values of each field.
*/

class PEGASUS_COMMON_LINKAGE CGIQueryString
{
public:

    /// Constructs from the value of the QUERY_STRING environment variable.
    CGIQueryString(char* queryString);

    /// Returns the number of fields.
    Uint32 getCount() const { return _entries.getSize(); }

    /// Returns the name of the ith field.
    const char* getName(Uint32 i) const { return _entries[i].name; }

    /// Returns the value of the ith field.
    const char* getValue(Uint32 i) const { return _entries[i].value; }

    /// Returns the value of the field with the given name.
    const char* findValue(const char* name) const;

private:

    struct Entry
    {
	char *name;
	char *value;
    };

    Array<Entry> _entries;

    friend static void _ParseCGIQueryString(
	char* queryString, 
	Array<Entry>& entries);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CGIUtils_h */
