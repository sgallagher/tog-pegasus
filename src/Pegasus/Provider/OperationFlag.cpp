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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "OperationFlag.h"

PEGASUS_NAMESPACE_BEGIN

const Uint32 OperationFlag::NONE = 0x00000000;
const Uint32 OperationFlag::LOCAL_ONLY = 0x00000001;
const Uint32 OperationFlag::INCLUDE_QUALIFIERS = 0x00000002;
const Uint32 OperationFlag::INCLUDE_CLASS_ORIGIN = 0x00000004;
const Uint32 OperationFlag::DEEP_INHERITANCE = 0x00000008;
const Uint32 OperationFlag::REMOTE_ONLY = 0x00000010;

Uint32 OperationFlag::convert(
	const Boolean localOnly,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const Boolean deepInheritance,
	const Boolean remoteOnly)
{
	Uint32 flags = NONE;

	flags |= localOnly ? LOCAL_ONLY : 0;
	flags |= includeQualifiers ? INCLUDE_QUALIFIERS : 0;
	flags |= includeClassOrigin ? INCLUDE_CLASS_ORIGIN : 0;
	flags |= deepInheritance ? DEEP_INHERITANCE : 0;
	flags |= remoteOnly ? REMOTE_ONLY : 0;

	return(flags);
}

PEGASUS_NAMESPACE_END
