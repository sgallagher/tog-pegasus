//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_RT_IndicationProvider_h
#define Pegasus_RT_IndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>
#include <Pegasus/Provider/CIMMethodProvider.h>

PEGASUS_USING_PEGASUS;

class RT_IndicationProvider :
    public CIMMethodProvider,
    public CIMIndicationProvider
{
public:
    RT_IndicationProvider (void) throw ();
    virtual ~RT_IndicationProvider (void) throw ();

    // CIMProvider interface
    virtual void initialize (CIMOMHandle & cimom);
    virtual void terminate (void);

    // CIMIndicationProvider interface
    virtual void enableIndications (IndicationResponseHandler & handler);
    virtual void disableIndications (void);

    virtual void createSubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames,
	const CIMPropertyList & propertyList,
	const Uint16 repeatNotificationPolicy);

    virtual void modifySubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames,
	const CIMPropertyList & propertyList,
	const Uint16 repeatNotificationPolicy);

    virtual void deleteSubscription (
	const OperationContext & context,
	const CIMObjectPath & subscriptionName,
	const Array <CIMObjectPath> & classNames);

    // CIMMethodProvider Interface
     virtual void invokeMethod(
         const OperationContext & context,
         const CIMObjectPath & objectReference,
         const CIMName & methodName,
         const Array<CIMParamValue> & inParameters,
         MethodResultResponseHandler & handler);

protected:

     void _checkOperationContext(const OperationContext& context,
                                 const String& funcName);

};

#endif

