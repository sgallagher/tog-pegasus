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
// $Log: CIMQualifierNames.h,v $
// Revision 1.2  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.1  2001/02/16 02:07:06  mike
// Renamed many classes and headers (using new CIM prefixes).
//
// Revision 1.1.1.1  2001/01/14 19:53:09  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// CIMQualifierNames.h
//
//	This file declares several predefined qualifier names. They are
// 	scoped int the CIMQualifierNames namespace. These qualifiers have
//	special meaning in CIM.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QualifierNames_h
#define Pegasus_QualifierNames_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

namespace CIMQualifierNames
{
    PEGASUS_COMMON_LINKAGE extern const char KEY[];
    PEGASUS_COMMON_LINKAGE extern const char ABSTRACT[];
    PEGASUS_COMMON_LINKAGE extern const char ASSOCIATION[];
    PEGASUS_COMMON_LINKAGE extern const char INDICATION[];
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_QualifierNames_h */
