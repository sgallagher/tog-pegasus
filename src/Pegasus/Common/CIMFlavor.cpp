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
// $Log: CIMFlavor.cpp,v $
// Revision 1.2  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:51:52  mike
// Pegasus import
//
//
//END_HISTORY

#include <cstring>
#include "CIMFlavor.h"
#include "Exception.h"
#include "XmlWriter.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMFlavor::NONE = 0;
const Uint32 CIMFlavor::OVERRIDABLE = 1;
const Uint32 CIMFlavor::TOSUBCLASS = 2;
const Uint32 CIMFlavor::TOINSTANCE = 4;
const Uint32 CIMFlavor::TRANSLATABLE = 8;
const Uint32 CIMFlavor::DEFAULTS = OVERRIDABLE | TOSUBCLASS;

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

String FlavorToString(Uint32 flavor)
{
    Boolean overridable = (flavor & CIMFlavor::OVERRIDABLE) != 0;
    Boolean toSubClass = (flavor & CIMFlavor::TOSUBCLASS) != 0;
    Boolean toInstance = (flavor & CIMFlavor::TOINSTANCE) != 0;
    Boolean translatable = (flavor & CIMFlavor::TRANSLATABLE) != 0;

    String tmp;

    if (!overridable)
	tmp += "OVERRIDABLE ";

    if (!toSubClass)
	tmp += "TOSUBCLASS ";

    if (toInstance)
	tmp += "TOINSTANCE ";

    if (translatable)
	tmp += "TRANSLATABLE ";

    if (tmp.getLength())
	tmp.remove(tmp.getLength() - 1);

    return tmp;
}

void FlavorToXml(Array<Sint8>& out, Uint32 flavor)
{
    Boolean overridable = (flavor & CIMFlavor::OVERRIDABLE) != 0;
    Boolean toSubClass = (flavor & CIMFlavor::TOSUBCLASS) != 0;
    Boolean toInstance = (flavor & CIMFlavor::TOINSTANCE) != 0;
    Boolean translatable = (flavor & CIMFlavor::TRANSLATABLE) != 0;

    if (!overridable)
	out << " OVERRIDABLE=\"" << _toString(overridable) << "\"";

    if (!toSubClass)
	out << " TOSUBCLASS=\"" << _toString(toSubClass) << "\"";

    if (toInstance)
	out << " TOINSTANCE=\"" << _toString(toInstance) << "\"";

    if (translatable)
	out << " TRANSLATABLE=\"" << _toString(translatable) << "\"";
}

PEGASUS_NAMESPACE_END
