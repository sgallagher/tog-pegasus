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
//      Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//      Chip Vincent (cvincent@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Query/QueryExpression/QueryExpression.h>
#include <Pegasus/Provider/CIMOMHandleQueryContext.h>

#include "RT_IndicationProvider.h"

PEGASUS_USING_STD;

PEGASUS_USING_PEGASUS;

static IndicationResponseHandler * _handler = 0;
static Boolean _enabled = false;
static Uint32 _nextUID = 0;
static Uint32 _numSubscriptions = 0;
static CIMOMHandle _cimom;

void _generateIndication (
    IndicationResponseHandler * handler,
    const CIMName methodName);

RT_IndicationProvider::RT_IndicationProvider (void) throw ()
{
	#if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
                _handler = 0;
                _enabled = false;
                _nextUID = 0;
                _numSubscriptions = 0;
        #endif
}

RT_IndicationProvider::~RT_IndicationProvider (void) throw ()
{
}

void RT_IndicationProvider::initialize (CIMOMHandle & cimom)
{
  _cimom = cimom;
}

void RT_IndicationProvider::terminate (void)
{
    delete this;
}

void RT_IndicationProvider::enableIndications (
    IndicationResponseHandler & handler)
{
    //
    //  enableIndications should not be called if indications have already been
    //  enabled
    //
    if (_enabled)
    {
        assert (false);
    }

    _enabled = true;
    _handler = &handler;
}

void _generateIndication (
    IndicationResponseHandler * handler,
    const CIMName methodName)
{
    if (_enabled)
    {
        CIMInstance indicationInstance;

        if (methodName.equal ("SendTestIndicationSubclass"))
        {
            CIMInstance theIndication (CIMName ("RT_TestIndicationSubclass"));
            indicationInstance = theIndication;
        }
        else
        {
            CIMInstance theIndication (CIMName ("RT_TestIndication"));
            indicationInstance = theIndication;
        }

        CIMObjectPath path;
        if (methodName.equal ("SendTestIndicationUnmatchingNamespace"))
        {
            //
            //  For SendTestIndicationUnmatchingNamespace, generate an
            //  indication instance with namespace that does not match the
            //  subscription instance name included in the operation context
            //  (nor does it match the namespace for which provider has
            //  registered)
            //
            path.setNameSpace ("root/cimv2");
            path.setClassName ("RT_TestIndication");
        }
        else if (methodName.equal ("SendTestIndicationUnmatchingClassName"))
        {
            // the indication class name and object path class must match
            indicationInstance = CIMInstance("CIM_AlertIndication");

            //
            //  For SendTestIndicationUnmatchingClassName, generate an
            //  indication instance with classname that does not match the
            //  subscription instance name included in the operation context
            //  (nor does it match the classname for which provider has
            //  registered)
            //
            path.setNameSpace ("root/SampleProvider");
            path.setClassName ("CIM_AlertIndication");
        }
        else if (methodName.equal ("SendTestIndicationSubclass"))
        {
            //
            //  For SendTestIndicationSubclass, generate an indication instance
            //  of a the RT_TestIndicationSubclass subclass
            //
            path.setNameSpace ("root/SampleProvider");
            path.setClassName ("RT_TestIndicationSubclass");
        }
        else
        {
            path.setNameSpace("root/SampleProvider");
            path.setClassName("RT_TestIndication");
        }

        indicationInstance.setPath(path);

        char buffer[32];
        sprintf(buffer, "%d", _nextUID++);
        indicationInstance.addProperty
            (CIMProperty ("IndicationIdentifier",String(buffer)));

	CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
	indicationInstance.addProperty
            (CIMProperty ("IndicationTime", currentDateTime));

        //
        //  For SendTestIndicationMissingProperty, leave out the
        //  CorrelatedIndications property
        //
        if (!methodName.equal ("SendTestIndicationMissingProperty"))
        {
	    Array <String> correlatedIndications;
	    indicationInstance.addProperty
                (CIMProperty ("CorrelatedIndications", correlatedIndications));
        }

        if ((methodName.equal ("SendTestIndicationNormal")) ||
            (methodName.equal ("SendTestIndicationSubclass")) ||
            (methodName.equal ("SendTestIndicationMissingProperty")) ||
            (methodName.equal ("SendTestIndicationExtraProperty")) ||
            (methodName.equal ("SendTestIndicationMatchingInstance")) ||
            (methodName.equal ("SendTestIndicationUnmatchingNamespace")) ||
            (methodName.equal ("SendTestIndicationUnmatchingClassName")))
        {
            indicationInstance.addProperty
                (CIMProperty ("MethodName", CIMValue (methodName.getString())));
        }
        else
        {
            indicationInstance.addProperty
                (CIMProperty ("MethodName",
                    CIMValue (String ("generateIndication"))));
        }

        //
        //  For SendTestIndicationExtraProperty, add an extra property,
        //  ExtraProperty, that is not a member of the indication class
        //
        if (methodName.equal ("SendTestIndicationExtraProperty"))
        {
            indicationInstance.addProperty
                (CIMProperty ("ExtraProperty",
                    CIMValue (String ("extraProperty"))));
        }

        CIMIndication cimIndication (indicationInstance);

        //
        //  For SendTestIndicationSubclass,
        //  SendTestIndicationMatchingInstance,
        //  SendTestIndicationUnmatchingNamespace or
        //  SendTestIndicationUnmatchingClassName, include
        //  SubscriptionInstanceNamesContainer in operation context
        //
        if ((methodName.equal ("SendTestIndicationSubclass")) ||
            (methodName.equal ("SendTestIndicationMatchingInstance")) ||
            (methodName.equal ("SendTestIndicationUnmatchingNamespace")) ||
            (methodName.equal ("SendTestIndicationUnmatchingClassName")))
        {
            Array <CIMObjectPath> subscriptionInstanceNames;
            Array <CIMKeyBinding> subscriptionKeyBindings;

            String filterString;
            filterString.append ("CIM_IndicationFilter.CreationClassName=\"CIM_IndicationFilter\",Name=\"PIFilter01\",SystemCreationClassName=\"");
            filterString.append (System::getSystemCreationClassName ());
            filterString.append ("\",SystemName=\"");
            filterString.append (System::getFullyQualifiedHostName ());
            filterString.append ("\"");
            subscriptionKeyBindings.append (CIMKeyBinding ("Filter",
                filterString, CIMKeyBinding::REFERENCE));

            String handlerString;
            handlerString.append ("CIM_IndicationHandlerCIMXML.CreationClassName=\"CIM_IndicationHandlerCIMXML\",Name=\"PIHandler01\",SystemCreationClassName=\"");
            handlerString.append (System::getSystemCreationClassName ());
            handlerString.append ("\",SystemName=\"");
            handlerString.append (System::getFullyQualifiedHostName ());
            handlerString.append ("\"");
            subscriptionKeyBindings.append (CIMKeyBinding ("Handler",
                handlerString, CIMKeyBinding::REFERENCE));

            CIMObjectPath subscriptionPath ("",
                CIMNamespaceName ("root/PG_InterOp"),
                CIMName ("CIM_IndicationSubscription"),
                subscriptionKeyBindings);
            subscriptionInstanceNames.append (subscriptionPath);

	    OperationContext context;
            context.insert (SubscriptionInstanceNamesContainer
                (subscriptionInstanceNames));

            handler->deliver (context, indicationInstance);
        }
        else
        {
	    // deliver an indication without trapOid
            handler->deliver (indicationInstance);
        }

        //
        //  Only deliver extra indication with trapOid for SendTestIndication
        //
        if ((!methodName.equal ("SendTestIndicationNormal")) &&
            (!methodName.equal ("SendTestIndicationSubclass")) &&
            (!methodName.equal ("SendTestIndicationMissingProperty")) &&
            (!methodName.equal ("SendTestIndicationExtraProperty")) &&
            (!methodName.equal ("SendTestIndicationMatchingInstance")) &&
            (!methodName.equal ("SendTestIndicationUnmatchingNamespace")) &&
            (!methodName.equal ("SendTestIndicationUnmatchingClassName")))
        {
	    // deliver another indication with a trapOid which contains in the
	    // operationContext container
	    OperationContext context;

 	    // add trap OID to the context
	    context.insert
                (SnmpTrapOidContainer("1.3.6.1.4.1.900.2.3.9002.9600"));
            handler->deliver (context, indicationInstance);
        }
    }
}

void RT_IndicationProvider::disableIndications (void)
{
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
    String funcName("RT_IndicationProvider::createSubscription ");
    _checkOperationContext(context, funcName);

    _numSubscriptions++;
}

void RT_IndicationProvider::modifySubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames,
    const CIMPropertyList & propertyList,
    const Uint16 repeatNotificationPolicy)
{
    String funcName("RT_IndicationProvider::modifySubscription ");
    _checkOperationContext(context, funcName);

    _generateIndication(_handler, "modifySubscription");
}

void RT_IndicationProvider::deleteSubscription (
    const OperationContext & context,
    const CIMObjectPath & subscriptionName,
    const Array <CIMObjectPath> & classNames)
{
    _numSubscriptions--;

    if (_numSubscriptions == 0)
        _enabled = false;
}

void RT_IndicationProvider::invokeMethod(
        const OperationContext & context,
        const CIMObjectPath & objectReference,
        const CIMName & methodName,
        const Array<CIMParamValue> & inParameters,
        MethodResultResponseHandler & handler)
{
        Boolean sendIndication = false;
        handler.processing();

        if (objectReference.getClassName().equal ("RT_TestIndication") &&
	    _enabled)
        {
            if ((methodName.equal ("SendTestIndication")) ||
                (methodName.equal ("SendTestIndicationNormal")) ||
                (methodName.equal ("SendTestIndicationMissingProperty")) ||
                (methodName.equal ("SendTestIndicationExtraProperty")) ||
                (methodName.equal ("SendTestIndicationMatchingInstance")) ||
                (methodName.equal ("SendTestIndicationUnmatchingNamespace")) ||
                (methodName.equal ("SendTestIndicationUnmatchingClassName")))
            {
                sendIndication = true;
                handler.deliver( CIMValue( 0 ) );
            }
        }

        else if ((objectReference.getClassName ().equal
            ("RT_TestIndicationSubclass")) && _enabled)
        {
            if (methodName.equal ("SendTestIndicationSubclass"))
            {
                sendIndication = true;
                handler.deliver( CIMValue( 0 ) );
            }
        }

        else
        {
             handler.deliver( CIMValue( 1 ) );
	     PEGASUS_STD(cout) << "Provider is not enabled." << PEGASUS_STD(endl);
        }

        handler.complete();

        if (sendIndication)
           _generateIndication(_handler, methodName);
}

void RT_IndicationProvider::_checkOperationContext(const OperationContext& context,
                                                  const String &  funcName)
{
	 //
	 // Test the filter query container
	 //
	 SubscriptionFilterQueryContainer qContainer = context.get(SubscriptionFilterQueryContainer::NAME);
    if (qContainer.getFilterQuery() == String::EMPTY)
    {
      PEGASUS_STD(cout) << funcName << "- empty filter query" << PEGASUS_STD(endl);
      throw CIMOperationFailedException(funcName + "- empty filter query");
    }
    if (qContainer.getQueryLanguage() == String::EMPTY)
    {
      PEGASUS_STD(cout) << funcName << "- empty filter query lang" << PEGASUS_STD(endl);
      throw CIMOperationFailedException(funcName + "- empty filter query lang");
    }

    CIMNamespaceName tst("root/SampleProvider");
    if (!qContainer.getSourceNameSpace().equal(tst))
    {
      PEGASUS_STD(cout) << funcName << "- incorrect source namespace" << PEGASUS_STD(endl);
      throw CIMOperationFailedException(funcName + "- incorrect source namespace");
    }

    try
    {
      //
      // Try to parse the filter query from the filter query container
      //
      CIMOMHandleQueryContext ctx(qContainer.getSourceNameSpace(), _cimom);
      QueryExpression qe(qContainer.getQueryLanguage(),
                         qContainer.getFilterQuery(),
                         ctx);

      // Exercise the QueryExpression...this will cause repository access through
      // the CIMOMHandleQueryContext.
      qe.validate();
    }
    catch (Exception & e)
    {
      PEGASUS_STD(cout) << funcName << "- parse error: " << e.getMessage() << PEGASUS_STD(endl);
      throw CIMOperationFailedException(funcName + "- parse error: " + e.getMessage());
    }

    //
    // Test the filter condition container.
    // Note:  since this only contains the WHERE clause, the condition could be empty (and will
    // be for some testcases)
    //
    SubscriptionFilterConditionContainer cContainer = context.get(SubscriptionFilterConditionContainer::NAME);
	 if (cContainer.getQueryLanguage() == String::EMPTY)
    {
      PEGASUS_STD(cout) << funcName << "- empty filter condition lang" << PEGASUS_STD(endl);
      throw CIMOperationFailedException(funcName + "- empty filter condition lang");
    }
}

