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
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>

#include "SNMPIndicationProvider.h"

PEGASUS_USING_PEGASUS;

SNMPIndicationProvider::SNMPIndicationProvider(void) throw()
{
}

SNMPIndicationProvider::~SNMPIndicationProvider(void) throw()
{
}

void SNMPIndicationProvider::initialize(CIMOMHandle & cimom)
{
    // save cimom handle
    _cimom = cimom;
}

void SNMPIndicationProvider::terminate(void)
{
    delete this;
}

void SNMPIndicationProvider::enableIndications(IndicationResponseHandler & handler)
{
    handler.processing();

    //Indication
    CIMInstance cimInstance("root/PG_InterOp:cycHostAdapterDiscovered");

    cimInstance.addProperty(CIMProperty("cycHostAdapterNumber", Uint32(101)));
    cimInstance.addProperty(CIMProperty("cycHostAdapterID", String("HP-NU-123")));
    cimInstance.addProperty(CIMProperty("cycManagerID", String("HP-NU-456")));
    
    CIMIndication cimIndication(cimInstance);

    handler.deliver(cimIndication);
}

void SNMPIndicationProvider::disableIndications(void)
{
    //handler.complete();
}

void SNMPIndicationProvider::createSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void SNMPIndicationProvider::modifySubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void SNMPIndicationProvider::deleteSubscription(
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array<CIMObjectPath> & classNames)
{
}

