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

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMOMHandle.h>
#include <Pegasus/Provider2/CIMIndicationConsumer.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

class PEGASUS_PROVIDER_LINKAGE DisplayConsumer : public CIMIndicationConsumer
{
public:

    DisplayConsumer()
    {

    }

    virtual ~DisplayConsumer()
    {

    }

    void initialize(CIMOMHandle & cimom)
    {
	
    }

    void terminate()
    {

    }

    void handleIndication(
	const OperationContext & context,
	const CIMIndication & indication,
	ResponseHandler<CIMIndication> & handler)
    {
    }

	void handleIndication(
	const OperationContext & context,
	const String& url,
	const CIMInstance& indicationInstance)
    {
	Array<Sint8> buffer;
	indicationInstance.toXml(buffer);
	cout << buffer.getData() << endl;
    }
};

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "FlatFileConsumer" which is appened to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMIndicationConsumer*
    PegasusCreateIndicationConsumer_DisplayConsumer() {
    return new DisplayConsumer;
}

PEGASUS_NAMESPACE_END
