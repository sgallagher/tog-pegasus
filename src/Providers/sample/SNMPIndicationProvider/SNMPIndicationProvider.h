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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Vijay Eli, (vijayeli@in.ibm.com) for bug#2330
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SNMPIndicationProvider_h
#define Pegasus_SNMPIndicationProvider_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Provider/CIMIndicationProvider.h>

class SNMPIndicationProvider :
    public  PEGASUS_NAMESPACE(CIMIndicationProvider)
{
public:
    SNMPIndicationProvider(void) throw();
    virtual ~SNMPIndicationProvider(void) throw();

    // CIMProvider interface
    virtual void initialize( PEGASUS_NAMESPACE(CIMOMHandle) & cimom);
    virtual void terminate(void);

    // CIMIndicationProvider interface
    virtual void enableIndications( PEGASUS_NAMESPACE(IndicationResponseHandler) & handler);
    virtual void disableIndications(void);

    virtual void createSubscription(
	const  PEGASUS_NAMESPACE(OperationContext) & context,
	const  PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
	const  PEGASUS_NAMESPACE(Array)< PEGASUS_NAMESPACE(CIMObjectPath)> & classNames,
	const  PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
	const  PEGASUS_NAMESPACE(Uint16) repeatNotificationPolicy);

    virtual void modifySubscription(
	const  PEGASUS_NAMESPACE(OperationContext) & context,
	const  PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
	const  PEGASUS_NAMESPACE(Array)< PEGASUS_NAMESPACE(CIMObjectPath)> & classNames,
	const  PEGASUS_NAMESPACE(CIMPropertyList) & propertyList,
	const  PEGASUS_NAMESPACE(Uint16) repeatNotificationPolicy);

    virtual void deleteSubscription(
	const  PEGASUS_NAMESPACE(OperationContext) & context,
	const  PEGASUS_NAMESPACE(CIMObjectPath) & subscriptionName,
	const  PEGASUS_NAMESPACE(Array)< PEGASUS_NAMESPACE(CIMObjectPath)> & classNames);

protected:
    PEGASUS_NAMESPACE(CIMOMHandle) _cimom;

};

#endif

