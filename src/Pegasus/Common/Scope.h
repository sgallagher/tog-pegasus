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
// $Log: Scope.h,v $
// Revision 1.1.1.1  2001/01/14 19:53:12  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Scope.h
//
//     This file declares qualifier scope constants.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Scope_h
#define Pegasus_Scope_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

namespace Scope
{
    PEGASUS_COMMON_LINKAGE extern const Uint32 NONE;
    PEGASUS_COMMON_LINKAGE extern const Uint32 CLASS;
    PEGASUS_COMMON_LINKAGE extern const Uint32 ASSOCIATION;
    PEGASUS_COMMON_LINKAGE extern const Uint32 INDICATION;
    PEGASUS_COMMON_LINKAGE extern const Uint32 PROPERTY;
    PEGASUS_COMMON_LINKAGE extern const Uint32 REFERENCE;
    PEGASUS_COMMON_LINKAGE extern const Uint32 METHOD;
    PEGASUS_COMMON_LINKAGE extern const Uint32 PARAMETER;
    PEGASUS_COMMON_LINKAGE extern const Uint32 ANY;
}

PEGASUS_COMMON_LINKAGE String ScopeToString(Uint32 scope);

PEGASUS_COMMON_LINKAGE void ScopeToXml(Array<Sint8>& out, Uint32 scope);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Scope_h */
