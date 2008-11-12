//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////


#ifndef _Pegasus_Common_CIMKeyBindingRep_h
#define _Pegasus_Common_CIMKeyBindingRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_NAMESPACE_BEGIN

class CIMKeyBindingRep
{
public:
    CIMKeyBindingRep()
    {
    }

    CIMKeyBindingRep(const CIMKeyBindingRep& x)
        : _name(x._name), _value(x._value), _type(x._type)
    {
    }

    CIMKeyBindingRep(
        const CIMName& name,
        const String& value,
        CIMKeyBinding::Type type)
        : _name(name), _value(value), _type(type)
    {
    }

    ~CIMKeyBindingRep()
    {
    }

    CIMKeyBindingRep& operator=(const CIMKeyBindingRep& x)
    {
        if (&x != this)
        {
            _name = x._name;
            _value = x._value;
            _type = x._type;
        }
        return *this;
    }

    CIMName _name;
    String _value;
    CIMKeyBinding::Type _type;
};

PEGASUS_NAMESPACE_END

#endif /* _Pegasus_Common_CIMKeyBindingRep_h */
