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
// Author: Arthur Pichlkostner
//             (checked in: Markus Mueller sedgewick_de@yahoo.de)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IBM_CIMOMStatDataProvider_h
#define Pegasus_IBM_CIMOMStatDataProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Common/StatisticalData.h>

PEGASUS_NAMESPACE_BEGIN

class IBM_CIMOMStatDataProvider :
	public CIMInstanceProvider
{
public:
	IBM_CIMOMStatDataProvider(void);
	virtual ~IBM_CIMOMStatDataProvider(void);

	// CIMBaseProvider interface
	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	// CIMStatisticalDataProvider interface
	virtual void getInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Uint32 flags,
                const CIMPropertyList & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void enumerateInstances(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const Uint32 flags,
                const CIMPropertyList & propertyList,
		ResponseHandler<CIMInstance> & handler);

	virtual void enumerateInstanceNames(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ResponseHandler<CIMObjectPath> & handler);

	virtual void modifyInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		const Uint32 flags,
                const CIMPropertyList & propertyList,
		ResponseHandler<void> & handler);

	virtual void createInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		const CIMInstance & obj,
		ResponseHandler<CIMObjectPath> & handler);

	virtual void deleteInstance(
		const OperationContext & context,
		const CIMObjectPath & ref,
		ResponseHandler<void> & handler);

   CIMInstance getInstance(Uint16 type);

   protected:
      CIMObjectPath _references[StatisticalData::NUMBER_OF_TYPES];

};

PEGASUS_NAMESPACE_END

#endif
