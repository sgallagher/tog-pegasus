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

#ifndef Pegasus_IndicationProvider_h
#define Pegasus_IndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Common/Thread.h>

PEGASUS_NAMESPACE_BEGIN

class IndicationProvider :
	public CIMIndicationProvider
{
public:
	IndicationProvider(void) throw();
	virtual ~IndicationProvider(void) throw();

	// CIMBaseProvider interface
	virtual void initialize(CIMOMHandle & cimom);
	virtual void terminate(void);

	// CIMInstanceProvider interface
	virtual void provideIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const Array<String> & propertyList,
		ResponseHandler<CIMIndication> & handler);
	
	virtual void updateIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const CIMDateTime & minimumInterval,
		const CIMDateTime & maximumInterval,
		const Array<String> & propertyList,
		ResponseHandler<CIMIndication> & handler);

	virtual void cancelIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		ResponseHandler<CIMIndication> & handler);

	virtual void checkIndication(
		const OperationContext & context,
		const CIMReference & classReference,
		const Array<String> & propertyList,
		ResponseHandler<CIMIndication> & handler);

protected:
	class IndicationThread : public Thread
	{
	public:
		IndicationThread(ResponseHandler<CIMIndication> & handler) throw();
		virtual ~IndicationThread(void) throw();
	
		static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL run(void *) throw();
	
	private:
		ResponseHandler<CIMIndication> * _pHandler;

	};

protected:
	CIMOMHandle _cimom;
	IndicationThread * pThread;

};

PEGASUS_NAMESPACE_END

#endif
