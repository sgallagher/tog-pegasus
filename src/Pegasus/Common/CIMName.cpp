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

#include <cctype>
#include "CIMName.h"

PEGASUS_NAMESPACE_BEGIN

Boolean CIMName::legal(const Char16* name) throw()
{
    const Char16* p = name;

    if (!p)
        return false;
	//throw NullPointer();  // No apparent need for an exception here

    if (!*p || !(isalpha(*p) || *p == '_'))
	return false;

    for (p++; *p; p++)
    {
#ifdef PEGASUS_HAS_EBCDIC
	if (*p > 255)
#else
	if (*p > 127)
#endif
	    return false;

	if (!(isalnum(*p) || *p == '_'))
	    return false;
    }

    return true;
}

Boolean CIMName::legal(const String& name) throw()
{
    return legal(name.getData());
}

Boolean CIMName::equal(const String& name1, const String& name2) throw()
{
    return String::equalNoCase(name1, name2);
}

PEGASUS_NAMESPACE_END
