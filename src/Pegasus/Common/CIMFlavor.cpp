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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMFlavor.h"
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMFlavor::NONE = 0;
const Uint32 CIMFlavor::OVERRIDABLE = 1;
const Uint32 CIMFlavor::ENABLEOVERRIDE = 1;
const Uint32 CIMFlavor::TOSUBCLASS = 2;
const Uint32 CIMFlavor::TOINSTANCE = 4;
const Uint32 CIMFlavor::TRANSLATABLE = 8;
const Uint32 CIMFlavor::TOSUBELEMENTS = TOSUBCLASS | TOINSTANCE;
const Uint32 CIMFlavor::DISABLEOVERRIDE = 16;
const Uint32 CIMFlavor::RESTRICTED = 32;
const Uint32 CIMFlavor::DEFAULTS = OVERRIDABLE | TOSUBCLASS;
// ATTN: P1 KS 24 March 2002 Change here to make TOINSTANCE part of the defaults
//const Uint32 CIMFlavor::DEFAULTS = OVERRIDABLE | TOSUBCLASS| TOINSTANCE;
const Uint32 CIMFlavor::ALL = 
                 OVERRIDABLE | TOSUBCLASS | TOINSTANCE | TRANSLATABLE |
                 DISABLEOVERRIDE | RESTRICTED;


CIMFlavor::CIMFlavor ()
    : cimFlavor (CIMFlavor::NONE)
{
}

CIMFlavor::CIMFlavor (const CIMFlavor & flavor)
    : cimFlavor (flavor.cimFlavor)
{
}

CIMFlavor::CIMFlavor (const Uint32 flavor)
    : cimFlavor (flavor)
{
    _checkFlavor (flavor);
}

CIMFlavor & CIMFlavor::operator= (const CIMFlavor & flavor)
{
    this->cimFlavor = flavor.cimFlavor;
    return *this;
}

void CIMFlavor::addFlavor (const Uint32 flavor)
{
    _checkFlavor (flavor);
    this->cimFlavor |= flavor;
}

void CIMFlavor::addFlavor (const CIMFlavor & flavor)
{
    this->cimFlavor |= flavor.cimFlavor;
}

void CIMFlavor::removeFlavor (const Uint32 flavor)
{
    _checkFlavor (flavor);
    this->cimFlavor &= (~flavor);
}

Boolean CIMFlavor::hasFlavor (const Uint32 flavor) const
{
    return ((this->cimFlavor & flavor) == flavor) ? true : false;
}

Boolean CIMFlavor::hasFlavor (const CIMFlavor & flavor) const
{
    return ((this->cimFlavor & flavor.cimFlavor) == flavor.cimFlavor) ? 
        true : false;
}

Boolean CIMFlavor::equal (const CIMFlavor & flavor) const
{
    return (this->cimFlavor == flavor.cimFlavor) ? true : false;
}

String CIMFlavor::toString () const
{
    String tmp;

    if (this->hasFlavor (CIMFlavor::OVERRIDABLE))
	tmp += "OVERRIDABLE ";

    if (this->hasFlavor (CIMFlavor::TOSUBCLASS))
	tmp += "TOSUBCLASS ";

    if (this->hasFlavor (CIMFlavor::TOINSTANCE))
	tmp += "TOINSTANCE ";

    if (this->hasFlavor (CIMFlavor::TRANSLATABLE))
	tmp += "TRANSLATABLE ";

    if (this->hasFlavor (CIMFlavor::DISABLEOVERRIDE))
	tmp += "DISABLEOVERRIDE ";

    if (this->hasFlavor (CIMFlavor::RESTRICTED))
	tmp += "RESTRICTED ";

    if (tmp.size ())
	tmp.remove (tmp.size () - 1);

    return tmp;
}

void CIMFlavor::_checkFlavor (Uint32 flavor)
{
    //
    //  Test that no undefined bits are set 
    //
    //  Note that conflicting bits may be set in the Uint32 flavor
    //  For example, OVERRIDABLE and DISABLEOVERRIDE may both be set
    //  or TOSUBCLASS and RESTRICTED may both be set
    //  Currently, the flavor is not checked for these conflicts
    //  That is corrected later when a CIMQualifierDecl object is constructed 
    //  with the CIMFlavor object
    //
    if (flavor > CIMFlavor::ALL)
    {
        //
        //  Invalid flavor value
        //
        String flavorString;
        char buffer [32];
        sprintf (buffer, "%lu", (unsigned long) flavor);
        flavorString = buffer;
        throw InvalidFlavor (flavorString);
    }
}

PEGASUS_NAMESPACE_END
