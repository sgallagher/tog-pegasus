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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>

#include "ProcessIndicationProvider.h"

PEGASUS_NAMESPACE_BEGIN

ProcessIndicationProvider::ProcessIndicationProvider (void) throw ()
{
}

ProcessIndicationProvider::~ProcessIndicationProvider (void) throw ()
{
}

void ProcessIndicationProvider::initialize (CIMOMHandle & cimom)
{
    // save cimom handle
    _cimom = cimom;
}

void ProcessIndicationProvider::terminate (void)
{
}

void ProcessIndicationProvider::enableIndications (
    ResponseHandler <CIMIndication> & handler)
{
//cout << "enable ProcessIndicationProvider" << endl;
    handler.processing ();

    CIMInstance indicationInstance ("root/PG_Interop:CIM_ProcessIndication");

    indicationInstance.addProperty
        (CIMProperty ("IndicationTime", CIMValue (CIMDateTime ())));

    indicationInstance.addProperty
        (CIMProperty ("IndicationIdentifier", "ProcessIndication01"));

    Array <String> correlatedIndications;
    indicationInstance.addProperty (CIMProperty ("CorrelatedIndications", 
         CIMValue (correlatedIndications)));

    CIMIndication cimIndication (indicationInstance);

    handler.deliver (cimIndication);
}

void ProcessIndicationProvider::disableIndications (void)
{
    //handler.complete ();
}

void ProcessIndicationProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
//cout << "create ProcessIndicationProvider" << endl;
}

void ProcessIndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void ProcessIndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
}

PEGASUS_NAMESPACE_END
