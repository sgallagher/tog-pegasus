//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Dave Sudlik, IBM (dsudlik@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Union_h
#define Pegasus_Union_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/** This union is used to represent the values of properties, qualifiers,
    method return values, and method arguments. All of the types
    defined in CIMType.h are represented by a Union. The 
    Union is used as the the basis for the CIMValue implementation.
*/
union Union
{
    Uint8 _booleanValue;
    Uint8 _uint8Value;
    Sint8 _sint8Value;
    Uint16 _uint16Value;
    Sint16 _sint16Value;
    Uint32 _uint32Value;
    Sint32 _sint32Value;
    Uint64 _uint64Value;
    Sint64 _sint64Value;
    Real32 _real32Value;
    Real64 _real64Value;
    Uint16 _char16Value;
    String* _stringValue;
    CIMDateTime* _dateTimeValue;
    CIMObjectPath* _referenceValue;
    CIMObject* _objectValue;

    Array<Boolean>* _booleanArray;
    Array<Uint8>* _uint8Array;
    Array<Sint8>* _sint8Array;
    Array<Uint16>* _uint16Array;
    Array<Sint16>* _sint16Array;
    Array<Uint32>* _uint32Array;
    Array<Sint32>* _sint32Array;
    Array<Uint64>* _uint64Array;
    Array<Sint64>* _sint64Array;
    Array<Real32>* _real32Array;
    Array<Real64>* _real64Array;
    Array<Char16>* _char16Array;
    Array<String>* _stringArray;
    Array<CIMDateTime>* _dateTimeArray;
    Array<CIMObjectPath>* _referenceArray;
    Array<CIMObject>* _objectArray;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Union_h */
