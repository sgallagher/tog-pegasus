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
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMIndicationConsumer_h
#define Pegasus_CIMIndicationConsumer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

/**
This class defines the set of methods implemented by an indication consumer provider.
A providers that derives from this class must implement all methods. The minimal method
implementation simply throw the NotSupported exception.
*/
class PEGASUS_PROVIDER_LINKAGE CIMIndicationConsumer : public virtual CIMBaseProvider
{
public:
    CIMIndicationConsumer(void);
    virtual ~CIMIndicationConsumer(void);

    /**
    @param contex contains security and locale information relevant for the lifetime
    of this operation.

    @param indication

    @param handler asynchronusly processes the results of this operation.

    @exception NotSupported
    @exception InvalidParameter
    */
    virtual void handleIndication(
	const OperationContext & context,
	const CIMInstance & indication,
	ResponseHandler<CIMInstance> & handler) = 0;

    // ATTN: The following method is only for testing purposes.
    virtual void handleIndication(
	const OperationContext & context,
	const String & url,
	const CIMInstance& indicationInstance)
    {
    }
};

PEGASUS_NAMESPACE_END

#endif
