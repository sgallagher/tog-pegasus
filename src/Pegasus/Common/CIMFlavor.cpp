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
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMFlavor.h"

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
const Uint32 CIMFlavor::ALL = OVERRIDABLE | DEFAULTS
						 | TRANSLATABLE | DISABLEOVERRIDE | RESTRICTED;

PEGASUS_NAMESPACE_END
