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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/MessageQueue.h>
#include <Pegasus/Common/XmlWriter.h>

#include "IndicationProvider.h"

PEGASUS_NAMESPACE_BEGIN

IndicationProvider::IndicationProvider(void) throw() : pThread(0)
{
}

IndicationProvider::~IndicationProvider(void) throw()
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
	ResponseHandler<CIMInstance> & handler)
{
	// ensure the provider supports the type of indication requested.
	if(!String::equalNoCase(classReference.getClassName(), "sample_helloworldindication"))
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
	ResponseHandler<CIMInstance> & handler)
{
	// acknowledge request
	handler.processing();
	
	// do nothing
	
	// do not complete request until cancelIndication()
}

void IndicationProvider::cancelIndication(
	const OperationContext & context,
	const CIMReference & classReference,
	ResponseHandler<CIMInstance> & handler)
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
	ResponseHandler<CIMInstance> & handler)
{
	throw NotSupported("IndicationProvider::checkIndication");
}

IndicationProvider::IndicationThread::IndicationThread(
	ResponseHandler<CIMInstance> & handler) throw() :
	Thread(run, this, false)
{
	_pHandler = &handler;
	
	Thread::run();
}

IndicationProvider::IndicationThread::~IndicationThread(void) throw()
{
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL
        IndicationProvider::IndicationThread::run(void * pv) throw()
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
		CIMInstance indication("Sample_HelloWorldIndication");

		// send indication
		pThread->_pHandler->deliver(indication);
		
		pThread->sleep(30000);
	}

#ifdef PEGASUS_OS_WINDOWS
	pThread->exit_self(1);
#else
	pThread->exit_self(NULL);
#endif
}

void IndicationProvider::enableIndication(
        const OperationContext & context,
        const String & nameSpace,
        const Array<String> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy,
        const String & otherRepeatNotificationPolicy,
        const CIMDateTime & repeatNotificationInterval,
        const CIMDateTime & repeatNotificationGap,
        const Uint16 repeatNotificationCount,
        const String & condition,
        const String & queryLanguage,
        const CIMInstance & subscription,
        ResponseHandler<CIMInstance> & handler)
{
    // acknowledge request
    handler.processing();

    pThread = new IndicationThread(handler);
}


void IndicationProvider::disableIndication(
        const OperationContext & context,
        const String & nameSpace,
        const Array<String> & classNames,
        const CIMInstance & subscription,
        ResponseHandler<CIMInstance> & handler)
{
}

void IndicationProvider::modifyIndication(
        const OperationContext & context,
        const String & nameSpace,
        const Array<String> & classNames,
        const CIMPropertyList & propertyList,
        const Uint16 repeatNotificationPolicy,
        const String & otherRepeatNotificationPolicy,
        const CIMDateTime & repeatNotificationInterval,
        const CIMDateTime & repeatNotificationGap,
        const Uint16 repeatNotificationCount,
        const String & condition,
        const String & queryLanguage,
        const CIMInstance & subscription,
        ResponseHandler<CIMInstance> & handler)
{
}

PEGASUS_NAMESPACE_END
