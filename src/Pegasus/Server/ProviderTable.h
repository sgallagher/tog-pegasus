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

////////////////////////////////////////////////////////////////////////////////
//
// ProviderTable.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderTable_h
#define Pegasus_ProviderTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Provider/CIMProvider.h>

// Mon Oct  1 16:44:11 2001 mdday
// commented out to allow building until provider2 interface
// is fully integrated into the main build. 
//#include <Pegasus/Provider2/OperationContext.h>
//#include <Pegasus/Provider2/OperationFlag.h>

PEGASUS_NAMESPACE_BEGIN

class CreateProviderReturnedNull : public Exception
{
public:

    CreateProviderReturnedNull(
	const String& libName, 
	const String& funcName)
	: Exception(funcName + " returned null in library " + libName) { }
};

/** 
    The provider table maintains a list of providers which have been 
    dynamically loaded. It maintains a mapping between string 
    provider identifiers and providers. The server uses the provider 
    table to find providers for the purposes of request dispatching.
    The CIM server maintains one provider table instance.
*/

class PEGASUS_SERVER_LINKAGE ProviderTable
{
public:

    /** 
	Default constructor. Initializeds the provider table.
    */

    ProviderTable();

    /** 
	Lookup the provider with the given identifier
    */

    CIMProvider* lookupProvider(const String& providerId);

    /** 
	Dynamically load the provider with the given identifier.
    */

    CIMProvider* loadProvider(const String& providerId);

private:

    struct Entry
    {
	String providerId;
	CIMProvider* provider;
    };

    Array<Entry> _providers;

public:

    friend int operator==(const Entry& x, const Entry& y)
    {
	return 0;
    }

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ProviderTable_h */
