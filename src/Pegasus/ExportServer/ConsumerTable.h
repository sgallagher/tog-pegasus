//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ConsumerTable_h
#define Pegasus_ConsumerTable_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Provider/CIMIndicationConsumer.h>
#include <Pegasus/ExportServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class CreateIndicationConsumerReturnedNull : public Exception
{
public:

    CreateIndicationConsumerReturnedNull(
	const String& libName,
	const String& funcName)
	: Exception(funcName + " returned null in library " + libName) { }
};

// The consumer table maintains a list of consumers which have been
// dynamically loaded. It maintains a mapping between string
// consumer identifiers and consumers. CIMOM Listner will use the
// consumer table to find consumer for the purposes of request dispatching.

static char CONSUMER_LIST_FILE [] = "consumer_list.dat";

class PEGASUS_EXPORT_SERVER_LINKAGE ConsumerTable
{
public:

    ConsumerTable();

    ~ConsumerTable()
    {
    }

    void set(Boolean dynamicReg, Boolean staticConsumers, Boolean persistence);

    CIMIndicationConsumer* lookupConsumer(const String& consumerId);

    CIMIndicationConsumer* loadConsumer(const String& consumerId);

    CIMStatusCode registerConsumer(
	const String& consumerId,
	const String& consumerLocation,
	const String& action,
	String& errorDescription);

private:

    struct ConsumerList
    {
	String consumerId;
	String consumerLocation;
    };

    Array<struct ConsumerList> _consumerList;

    struct Entry
    {
	String consumerId;
	CIMIndicationConsumer* consumer;
    };

    Array<Entry> _consumers;

    Boolean _dynamicReg;
    Boolean _staticConsumers;
    Boolean _persistence;

    String _GetConsumerName(const String& consumerId);

public:

    friend int operator==(const Entry& x, const Entry& y)
    {
	return 0;
    }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ConsumerTable_h */
