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

#ifndef Pegasus_Scope_h
#define Pegasus_Scope_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The CIMQualifier Scopes are as follows:
    NONE, Class, ASSOCIATION,INDICATION,PROPERTY, REFERENCE, METHOD, 
    PARAMETER, ANY
*/
struct PEGASUS_COMMON_LINKAGE CIMScope
{
    static const Uint32 NONE;
    static const Uint32 CLASS;
    static const Uint32 ASSOCIATION;
    static const Uint32 INDICATION;
    static const Uint32 PROPERTY;
    static const Uint32 REFERENCE;
    static const Uint32 METHOD;
    static const Uint32 PARAMETER;
    static const Uint32 ANY;
};

PEGASUS_COMMON_LINKAGE String ScopeToString(Uint32 scope);

PEGASUS_COMMON_LINKAGE void ScopeToXml(Array<Sint8>& out, Uint32 scope);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Scope_h */
