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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>

#include "IndicationProvider.h"

PEGASUS_NAMESPACE_BEGIN

IndicationProvider::IndicationProvider(void) : pThread(0)
{
}

IndicationProvider::~IndicationProvider(void)
{
}

void IndicationProvider::initialize(CIMOMHandle & cimom)
{
	// save cimom handle
	_cimom = cimom;
}

void IndicationProvider::terminate(void)
{
}

void IndicationProvider::provideIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	// ensure the provider supports the type of indication requested.
	if(!String::equalNoCase(classReference.getClassName(), "sample_indication"))
	{
		throw InvalidClass(classReference.getClassName());
	}
	
	// acknowledge request
	handler.processing();
	
	pThread = new IndicationThread(handler);
}

void IndicationProvider::updateIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const CIMDateTime & minimumInterval,
	const CIMDateTime & maximumInterval,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	// acknowledge request
	handler.processing();
	
	// do nothing
	
	// do not complete request until cancelIndication()
}

void IndicationProvider::cancelIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMIndication> & handler)
{
	// acknowledge request
	handler.processing();
		
	if(pThread != 0)
	{
		delete pThread;
		pThread = 0;
	}

	// complete request
	handler.complete();
}
	
void IndicationProvider::checkIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	const Array<String> & propertyList,
	ResponseHandler<CIMIndication> & handler)
{
	throw NotSupported("IndicationProvider::checkIndication");
}

IndicationProvider::IndicationThread::IndicationThread(
	ResponseHandler<CIMIndication> & handler) :
	Thread(run, this, false)
{
	_pHandler = &handler;
	
	Thread::run();
}

IndicationProvider::IndicationThread::~IndicationThread(void)
{
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL IndicationProvider::IndicationThread::run(
	void * pv)
{
	if(pv == 0)
	{
		return(0);
	}

	IndicationThread * pThread = reinterpret_cast<IndicationThread *>(pv);
	
	pThread->_pHandler->processing();

	while(true)
	{
		// create indication
		CIMIndication indication("Sample_Indication");

		// send indication
		pThread->_pHandler->deliver(indication);
		
		pThread->sleep(30000);
	}

	pThread->exit_self(1);
}

PEGASUS_NAMESPACE_END
