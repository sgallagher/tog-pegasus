//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, 
// Hewlett-Packard Company, IBM
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
// Author: Yi Zhou (yi_zhou@hp.com)
//
// Modified By:
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderRegistrationTable_h
#define Pegasus_ProviderRegistrationTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/Sharable.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_SERVER_LINKAGE ProviderRegistrationTable
{
public:
	ProviderRegistrationTable(const Array<CIMInstance>& instance); 
	virtual ~ProviderRegistrationTable(void);

	const Array<CIMInstance> & getInstances(void) const;

protected:
	Array<CIMInstance> _instances;

};

PEGASUS_NAMESPACE_END

#endif
