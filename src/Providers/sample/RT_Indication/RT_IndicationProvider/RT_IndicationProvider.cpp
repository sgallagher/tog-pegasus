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

#include "RT_IndicationProvider.h"

static ResponseHandler<CIMIndication> * _handler = 0; 
static Boolean _enabled = false;
static Uint32 _nextUID = 0;

void _generateIndication (
    ResponseHandler <CIMIndication> * handler,
    const String methodName);

PEGASUS_NAMESPACE_BEGIN

RT_IndicationProvider::RT_IndicationProvider (void) throw ()
{
}

RT_IndicationProvider::~RT_IndicationProvider (void) throw ()
{
}

void RT_IndicationProvider::initialize (CIMOMHandle & cimom)
{
}

void RT_IndicationProvider::terminate (void)
{
}

void RT_IndicationProvider::enableIndications (
    ResponseHandler <CIMIndication> & handler)
{
    _enabled = true;
    _handler = &handler;
cout << "enable RT_IndicationProvider" << " _handler = " << _handler << endl;
    _handler->processing ();
}

void _generateIndication (
    ResponseHandler <CIMIndication> * handler,
    const String methodName)
{
    if (_enabled)
    {
        cout << "_generateIndication RT_IndicationProvider" 
             << " handler = " << handler << endl;
        CIMInstance indicationInstance ("root/SampleProvider:PG_TestIndication");

        indicationInstance.addProperty
            (CIMProperty ("IndicationTime", CIMValue (CIMDateTime ())));

        indicationInstance.addProperty
            (CIMProperty ("IndicationID", _nextUID++ ));

        indicationInstance.addProperty
            (CIMProperty ("MethodName", methodName));
        
        CIMIndication cimIndication (indicationInstance);

        handler->deliver (indicationInstance);
    }
}

void RT_IndicationProvider::disableIndications (void)
{
cout << "disable RT_IndicationProvider" << " _handler = " << _handler << endl;
    _enabled = false;
    _handler->complete ();
}

void RT_IndicationProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
//cout << "create AlertIndicationProvider" << endl;
    _generateIndication(_handler, "createSubscription");
}

void RT_IndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    _generateIndication(_handler, "modifySubscription");
}

void RT_IndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
    _generateIndication(_handler, "deleteSubscription");
}

void RT_IndicationProvider::invokeMethod(
        const OperationContext & context,
        const CIMReference & objectReference,
        const String & methodName,
        const Array<CIMParamValue> & inParameters,
        Array<CIMParamValue> & outParameters,
        ResponseHandler<CIMValue> & handler)
{
cout << "invokeMethod RT_IndicationProvider" << " _handler = " << _handler << endl;
        Boolean sendIndication = false;
        handler.processing();

        if(String::equalNoCase(objectReference.getClassName(),
                               "RT_TestIndication"))
        {                
            if(String::equalNoCase(methodName, "SendTestIndication"))
            {
                sendIndication = true;
                handler.deliver( CIMValue( 0 ) );
             }
        }

        else
        {
             handler.deliver( CIMValue( 1 ) );
        }

        handler.complete();

        if (sendIndication)
           _generateIndication(_handler,"invokeMethod");
}


PEGASUS_NAMESPACE_END
