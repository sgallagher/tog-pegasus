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
// $Log: CIMFlavor.h,v $
// Revision 1.2  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.3  2001/01/30 23:39:00  karl
// Add doc++ Documentation to header files
//
// Revision 1.2  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.1.1.1  2001/01/14 19:51:53  mike
// Pegasus import
//
//
//END_HISTORY

/*

 CIMFlavor.h -- This file declares qualifier flavor constants.

*/

#ifndef Pegasus_Flavor_h
#define Pegasus_Flavor_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN
/**
CIMQualifier flavor constants
*/
struct PEGASUS_COMMON_LINKAGE CIMFlavor
{
    static const Uint32 NONE;
    static const Uint32 OVERRIDABLE;
    static const Uint32 TOSUBCLASS;
    static const Uint32 TOINSTANCE;
    static const Uint32 TRANSLATABLE;

    // These are the defaults according to the CIM DTD:
    // OVERRIDABLE, TOSUBCLASS

    static const Uint32 DEFAULTS;
};

PEGASUS_COMMON_LINKAGE String FlavorToString(Uint32 scope);

PEGASUS_COMMON_LINKAGE void FlavorToXml(
    Array<Sint8>& out, 
    Uint32 flavor);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Flavor_h */
