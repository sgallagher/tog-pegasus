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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>

#include "AlertIndicationProvider.h"

PEGASUS_NAMESPACE_BEGIN

AlertIndicationProvider::AlertIndicationProvider (void) throw ()
{
}

AlertIndicationProvider::~AlertIndicationProvider (void) throw ()
{
}

void AlertIndicationProvider::initialize (CIMOMHandle & cimom)
{
    // save cimom handle
    _cimom = cimom;
}

void AlertIndicationProvider::terminate (void)
{
}

void AlertIndicationProvider::enableIndications (
    ResponseHandler <CIMIndication> & handler)
{
//cout << "enable AlertIndicationProvider" << endl;
    handler.processing ();

    CIMInstance indicationInstance ("root/cimv2:CIM_AlertIndication");

    indicationInstance.addProperty
        (CIMProperty ("IndicationTime", CIMValue (CIMDateTime ())));

    indicationInstance.addProperty
        (CIMProperty ("IndicationIdentifier", "AlertIndication02"));

    Array <String> correlatedIndications;
    indicationInstance.addProperty (CIMProperty ("CorrelatedIndications",
         CIMValue (correlatedIndications)));

    indicationInstance.addProperty
        (CIMProperty ("Description", "An alert indication for testing"));

    indicationInstance.addProperty
        (CIMProperty ("AlertingManagedElement", "The instance path"));

    indicationInstance.addProperty
        (CIMProperty ("AlertType", CIMValue ((Uint16) 5)));

    indicationInstance.addProperty
        (CIMProperty ("PerceivedSeverity", CIMValue ((Uint16) 4)));

    indicationInstance.addProperty
        (CIMProperty ("ProbableCause", CIMValue ((Uint16) 0)));

    indicationInstance.addProperty
        (CIMProperty ("ProbableCauseDescription", "Unknown"));

    indicationInstance.addProperty
        (CIMProperty ("Trending", CIMValue ((Uint16) 0)));

    Array <String> recommendedActions;
    recommendedActions.append ("Take the following actions ");
    recommendedActions.append ("to fix the problem.");
    indicationInstance.addProperty (CIMProperty ("RecommendedActions",
         CIMValue (recommendedActions)));

    indicationInstance.addProperty
        (CIMProperty ("EventID", "AlertEvent02"));

    indicationInstance.addProperty
        (CIMProperty ("EventTime", CIMValue (CIMDateTime ())));

    indicationInstance.addProperty
        (CIMProperty ("SystemCreationClassName", "CIM_ComputerSystem"));

    indicationInstance.addProperty
        (CIMProperty ("SystemName", "server001.acme.com"));

    indicationInstance.addProperty
        (CIMProperty ("ProviderName", "AlertIndicationProvider"));

    CIMIndication cimIndication (indicationInstance);

    handler.deliver (cimIndication);
}

void AlertIndicationProvider::disableIndications (void)
{
    //handler.complete ();
}

void AlertIndicationProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
//cout << "create AlertIndicationProvider" << endl;
}

void AlertIndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void AlertIndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
}

PEGASUS_NAMESPACE_END
