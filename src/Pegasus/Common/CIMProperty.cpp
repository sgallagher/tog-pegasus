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
// $Log: CIMProperty.cpp,v $
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:53:03  mike
// Pegasus import
//
//
//END_HISTORY

#include "CIMProperty.h"

PEGASUS_NAMESPACE_BEGIN

CIMProperty::CIMProperty(const ConstCIMProperty& x)
{
    Inc(_rep = x._rep);
}

Boolean CIMProperty::identical(const ConstCIMProperty& x) const
{
    x._checkRep();
    _checkRep();
    return _rep->identical(x._rep);
}

PEGASUS_NAMESPACE_END
