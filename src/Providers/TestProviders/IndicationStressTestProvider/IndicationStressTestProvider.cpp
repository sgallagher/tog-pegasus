//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Dave Sudlik, IBM (dsudlik@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
//#include <Pegasus/Common/OperationContext.h>
//#include <Pegasus/Common/System.h>
//#include <Pegasus/Query/QueryExpression/QueryExpression.h>
//#include <Pegasus/Provider/CIMOMHandleQueryContext.h>

#include "IndicationStressTestProvider.h"

PEGASUS_USING_STD;

PEGASUS_USING_PEGASUS;

static IndicationResponseHandler * _indication_handler = 0; 
static Boolean _enabled = false;
static Uint32 _nextUID = 0;
static Uint32 _numSubscriptions = 0;
static CIMOMHandle _cimom;

IndicationStressTestProvider::IndicationStressTestProvider (void) throw ()
{
}

IndicationStressTestProvider::~IndicationStressTestProvider (void) throw ()
{
}

void IndicationStressTestProvider::initialize (CIMOMHandle & cimom)
{
  _cimom = cimom;
}

void IndicationStressTestProvider::terminate (void)
{
    delete this;
}

void IndicationStressTestProvider::enableIndications (
    IndicationResponseHandler & handler)
{
    _enabled = true;
    _indication_handler = &handler;
}

void IndicationStressTestProvider::disableIndications (void)
{
    _enabled = false;
    _indication_handler->complete ();
}

void IndicationStressTestProvider::createSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    _numSubscriptions++;
}

void IndicationStressTestProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
}

void IndicationStressTestProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
    _numSubscriptions--;

    if (_numSubscriptions == 0)
        _enabled = false;
}

void IndicationStressTestProvider::invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler)
{
        Boolean sendIndication = false;
        Uint32 indicationSendCount;
        CIMIndication indicationInstance;

        handler.processing();

        inParameters[0].getValue().get(indicationSendCount);

        if (_enabled)
        {
            CIMInstance theIndication (CIMName ("IndicationStressTestClass"));
            indicationInstance = theIndication;

            CIMObjectPath path;
            path.setNameSpace("test/TestProvider");
            path.setClassName("IndicationStressTestClass");
            indicationInstance.setPath(path);
            
            char buffer[32];
            sprintf(buffer, "%d", _nextUID++);
            indicationInstance.addProperty
                (CIMProperty ("IndicationIdentifier",String(buffer)));

            Array <String> correlatedIndications; 
            indicationInstance.addProperty
                    (CIMProperty ("CorrelatedIndications", correlatedIndications));

            CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
            indicationInstance.addProperty
                    (CIMProperty ("IndicationTime", currentDateTime));

            handler.deliver( CIMValue( 0 ) );
        }
        handler.complete();
        
        if (indicationSendCount > 0)
            for (Uint32 i = 0; i < indicationSendCount; i++)
            {
                _indication_handler->deliver (indicationInstance);
            }
}

