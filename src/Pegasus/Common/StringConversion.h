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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_String_Conversion_h
#define Pegasus_String_Conversion_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

// The following functions convert the integer "x" to a string. The "buffer" 
// argument is a scratch area that may or may not be used in the conversion.
// These functions return a pointer to the converted string and set "size" to 
// the length of that string.

PEGASUS_COMMON_LINKAGE
const char* Uint8ToString(char buffer[22], Uint8 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Uint16ToString(char buffer[22], Uint16 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Uint32ToString(char buffer[22], Uint32 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Uint64ToString(char buffer[22], Uint64 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Sint8ToString(char buffer[22], Sint8 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Sint16ToString(char buffer[22], Sint16 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Sint32ToString(char buffer[22], Sint32 x, Uint32& size);

PEGASUS_COMMON_LINKAGE
const char* Sint64ToString(char buffer[22], Sint64 x, Uint32& size);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_String_Conversion_h */
