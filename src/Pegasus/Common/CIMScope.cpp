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
// $Log: CIMScope.cpp,v $
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:53:12  mike
// Pegasus import
//
//
//END_HISTORY

#include <cstring>
#include "CIMScope.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

namespace CIMScope
{
    const Uint32 NONE = 0;
    const Uint32 CLASS = 1;
    const Uint32 ASSOCIATION = 2;
    const Uint32 INDICATION = 4;
    const Uint32 PROPERTY = 8;
    const Uint32 REFERENCE = 16;
    const Uint32 METHOD = 32;
    const Uint32 PARAMETER = 64;
    const Uint32 ANY = (1 | 2 | 4 | 8 | 16 | 32 | 64 );
}

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

    if (tmp.getLength())
	tmp.remove(tmp.getLength() - 1);

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
