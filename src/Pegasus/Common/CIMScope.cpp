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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <Pegasus/Common/Exception.h>
#include "CIMScope.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 CIMScope::NONE = 0;
const Uint32 CIMScope::CLASS = 1;
const Uint32 CIMScope::ASSOCIATION = 2;
const Uint32 CIMScope::INDICATION = 4;
const Uint32 CIMScope::PROPERTY = 8;
const Uint32 CIMScope::REFERENCE = 16;
const Uint32 CIMScope::METHOD = 32;
const Uint32 CIMScope::PARAMETER = 64;
const Uint32 CIMScope::ANY = (1 | 2 | 4 | 8 | 16 | 32 | 64);

CIMScope::CIMScope ()
    : cimScope (CIMScope::NONE)
{
}

CIMScope::CIMScope (const CIMScope & scope)
    : cimScope (scope.cimScope)
{
}

CIMScope::CIMScope (const Uint32 scope)
    : cimScope (scope)
{
    if (scope > CIMScope::ANY)
    {
        //
        //  Invalid scope value
        //
        String scopeString;
        char buffer [32];
        sprintf (buffer, "%lu", (unsigned long) scope);
        scopeString = buffer;
        throw InvalidScope (scopeString);
    }
}

CIMScope & CIMScope::operator= (const CIMScope & scope)
{
    this->cimScope = scope.cimScope;
    return *this;
}

void CIMScope::addScope (const Uint32 scope)
{
    if (scope > CIMScope::ANY)
    {
        //
        //  Invalid scope value
        //
        String scopeString;
        char buffer [32];
        sprintf (buffer, "%lu", (unsigned long) scope);
        scopeString = buffer;
        throw InvalidScope (scopeString);
    }

    this->cimScope |= scope;
}

Boolean CIMScope::hasScope (const Uint32 scope) const
{
    if ((this->cimScope & scope) == scope)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Boolean CIMScope::hasScope (const CIMScope & scope) const
{
    if ((this->cimScope & scope.cimScope) == scope.cimScope)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Boolean CIMScope::equal (const CIMScope & scope) const
{
    if (this->cimScope == scope.cimScope)
    {
        return true;
    }
    else
    {
        return false;
    }
}

String CIMScope::toString () const
{
    String tmp;

    if (this->hasScope (CIMScope::CLASS))
	tmp += "CLASS ";

    if (this->hasScope (CIMScope::ASSOCIATION))
	tmp += "ASSOCIATION ";

    if (this->hasScope (CIMScope::INDICATION))
	tmp += "INDICATION ";

    if (this->hasScope (CIMScope::PROPERTY))
	tmp += "PROPERTY ";

    if (this->hasScope (CIMScope::REFERENCE))
	tmp += "REFERENCE ";

    if (this->hasScope (CIMScope::METHOD))
	tmp += "METHOD ";

    if (this->hasScope (CIMScope::PARAMETER))
	tmp += "PARAMETER ";

    if (tmp.size())
	tmp.remove(tmp.size() - 1);

    return tmp;
}

PEGASUS_NAMESPACE_END
