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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMIndicationConsumer_h
#define Pegasus_CIMIndicationConsumer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMBaseProvider.h>

#include <Pegasus/Common/CIMIndication.h>

PEGASUS_NAMESPACE_BEGIN

/*
    REVIEW: change CIMIndication to CIMInstance in interface below.

    REVIEW: The URL used in the interface below only applies to the
	CIM/XML handler. It would seem that the handleIndication() method
	needs another way of passing the URL (if it is needed at all). Why
	is it needed?

    REVIEW: There needs to be an indication consumer which is independent of
	the provider directory. In this way, an indication consumer may be
	used in in a cim-client or stand-alone listener process. As it is,
	there is no way to place an indication consumer in process other than
	the CIMOM, unless of course, that process contains provider interfaces
	which is problematic. Probably, CIMIndicationConsumer belongs in the
	Common directory. The class in this header, should be renamed to
	CIMIndicationConsumerProvider and should implement the
	CIMIndicationConsumer interface (defined in Common) and the
	CIMBaseProvider interface (defined in Provider).

	class CIMIndicationConsumerProvider
	    : public CIMIndicationConsumer, public CIMBaseProvider
	{

	}
*/

/**
    This class defines the set of methods implemented by an indication consumer
    provider. A providers that derives from this class must implement all
    methods. The minimal method implementation simply throw the NotSupported
    exception.

*/
class PEGASUS_PROVIDER_LINKAGE CIMIndicationConsumer
    : public virtual CIMBaseProvider
{
public:

    CIMIndicationConsumer(void);

    virtual ~CIMIndicationConsumer(void);

    /** @param contex contains security and locale information relevant for
	    the lifetime of this operation.
	@param indication
	@param handler asynchronusly processes the results of this operation.
	@exception NotSupported
	@exception InvalidParameter
    */
    virtual void handleIndication(
	const OperationContext & context,
	const CIMIndication & indication,
	ResponseHandler<CIMIndication> & handler) = 0;

    // ATTN: The following method is only for testing purposes.
    virtual void handleIndication(
	const OperationContext & context,
	const String & url,
	const CIMInstance& indicationInstance) { }
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMIndicationConsumer_h */
