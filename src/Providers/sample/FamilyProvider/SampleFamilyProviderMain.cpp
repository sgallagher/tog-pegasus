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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
// This is a sample provider that provides the instance and association operations
// associated with the TST_Family MOF for the dynamic versions of the classes.
// It provides the TST_PersonDynamic instances and the
// TST_LineageDynamic and TST_LabeledLineageDynamic Associations
// When started it builds the classes into an internal table.
// ISSUE: How do I create a multiple provider (instance and association)


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#include "SampleFamilyProvider.h"

PEGASUS_NAMESPACE_BEGIN

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "SampleFamilyProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderModule to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

extern "C" PEGASUS_EXPORT CIMProvider * PegasusCreateProvider(const String & name)
{
    CDEBUG("Create SampleFamilyProvider " << name);
	if(String::equalNoCase(name, "samplefamilyprovider") ||
		String::equalNoCase(name, "samplefamilyprovider (PROVIDER)"))
	{
        return(new SampleFamilyProvider());
	}

	return(0);
}

PEGASUS_NAMESPACE_END
