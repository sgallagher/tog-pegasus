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

#include <cstring>
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMScope::NONE = 0;
const Uint32 CIMScope::CLASS = 1;
const Uint32 CIMScope::ASSOCIATION = 2;
const Uint32 CIMScope::INDICATION = 4;
const Uint32 CIMScope::PROPERTY = 8;
const Uint32 CIMScope::REFERENCE = 16;
const Uint32 CIMScope::METHOD = 32;
const Uint32 CIMScope::PARAMETER = 64;
const Uint32 CIMScope::ANY = (1 | 2 | 4 | 8 | 16 | 32 | 64 );

String ScopeToString(Uint32 scope)
{
    String tmp;

    if (scope & CIMScope::CLASS)
	tmp += "CLASS ";

    if (scope & CIMScope::ASSOCIATION)
	tmp += "ASSOCIATION ";

    if (scope & CIMScope::INDICATION)
	tmp += "INDICATION ";

    if (scope & CIMScope::PROPERTY)
	tmp += "PROPERTY ";

    if (scope & CIMScope::REFERENCE)
	tmp += "REFERENCE ";

    if (scope & CIMScope::METHOD)
	tmp += "METHOD ";

    if (scope & CIMScope::PARAMETER)
	tmp += "PARAMETER ";

    if (tmp.size())
	tmp.remove(tmp.size() - 1);

    return tmp;
}

void ScopeToXml(Array<Sint8>& out, Uint32 scope)
{
    if (scope)
    {
	out << "<SCOPE";

	if (scope & CIMScope::CLASS)
	    out << " CLASS=\"true\"";

	if (scope & CIMScope::ASSOCIATION)
	    out << " ASSOCIATION=\"true\"";

	if (scope & CIMScope::REFERENCE)
	    out << " REFERENCE=\"true\"";

	if (scope & CIMScope::PROPERTY)
	    out << " PROPERTY=\"true\"";

	if (scope & CIMScope::METHOD)
	    out << " METHOD=\"true\"";

	if (scope & CIMScope::PARAMETER)
	    out << " PARAMETER=\"true\"";

	if (scope & CIMScope::INDICATION)
	    out << " INDICATION=\"true\"";

	out << "/>";
    }
}

PEGASUS_NAMESPACE_END
