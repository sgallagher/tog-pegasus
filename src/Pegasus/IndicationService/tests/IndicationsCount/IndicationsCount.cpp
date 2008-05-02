//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

static const CIMNamespaceName SOURCENAMESPACE =
    CIMNamespaceName("test/TestProvider");

static const String FILTER_NAME = "ICFilter01";
static const String HANDLER_NAME = "ICHandler01";

static Uint32 _matchedIndicationsCount = 5;
static Uint32 _orphanIndicationCount = 1;

CIMObjectPath _createHandlerInstance(
    CIMClient& client,
    const String& name,
    const String& destination)
{
    CIMInstance handlerInstance(PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance.addProperty(CIMProperty(
        CIMName("SystemCreationClassName"),
        System::getSystemCreationClassName()));
    handlerInstance.addProperty(CIMProperty(
        CIMName("SystemName"),
        System::getFullyQualifiedHostName()));
    handlerInstance.addProperty(CIMProperty(
        CIMName("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString()));
    handlerInstance.addProperty(CIMProperty(CIMName("Name"), name));
    handlerInstance.addProperty(CIMProperty(
        CIMName("Destination"),
        destination));

    return client.createInstance(
        PEGASUS_NAMESPACENAME_INTEROP,
        handlerInstance);
}

CIMObjectPath _createFilterInstance(
    CIMClient& client,
    const String& name,
    const String& query,
    const String& qlang)
{
    CIMInstance filterInstance(PEGASUS_CLASSNAME_INDFILTER);
    filterInstance.addProperty(CIMProperty(
        CIMName("SystemCreationClassName"),
        System::getSystemCreationClassName()));
    filterInstance.addProperty(CIMProperty(
        CIMName("SystemName"),
        System::getFullyQualifiedHostName()));
    filterInstance.addProperty(CIMProperty(
        CIMName("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString()));
    filterInstance.addProperty(CIMProperty(CIMName("Name"), name));
    filterInstance.addProperty(CIMProperty(CIMName("Query"), query));
    filterInstance.addProperty(CIMProperty(CIMName("QueryLanguage"), qlang));
    filterInstance.addProperty(CIMProperty(
        CIMName("SourceNamespace"),
        SOURCENAMESPACE.getString()));

    return client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, filterInstance);
}

CIMObjectPath _getFilterObjectPath(const String& name)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append(CIMKeyBinding(
        "SystemCreationClassName",
        System::getSystemCreationClassName(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(
        "SystemName",
        System::getFullyQualifiedHostName(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(
        "CreationClassName",
        PEGASUS_CLASSNAME_INDFILTER.getString(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding("Name", name, CIMKeyBinding::STRING));

    return CIMObjectPath(
        String::EMPTY,
        CIMNamespaceName(),
        PEGASUS_CLASSNAME_INDFILTER,
        keyBindings);
}

CIMObjectPath _getHandlerObjectPath(const String& name)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append(CIMKeyBinding(
        "SystemCreationClassName",
        System::getSystemCreationClassName(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(
        "SystemName",
        System::getFullyQualifiedHostName(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding(
        "CreationClassName",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString(),
        CIMKeyBinding::STRING));
    keyBindings.append(CIMKeyBinding("Name", name, CIMKeyBinding::STRING));

    return CIMObjectPath(
        String::EMPTY,
        CIMNamespaceName(),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML,
        keyBindings);
}

void _createSubscriptionInstance(
    CIMClient& client,
    const CIMObjectPath& filterPath,
    const CIMObjectPath& handlerPath)
{
    CIMInstance subscriptionInstance(PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    subscriptionInstance.addProperty(CIMProperty(
        CIMName("Filter"),
        filterPath,
        0,
        PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance.addProperty(CIMProperty(
        CIMName("Handler"),
        handlerPath,
        0,
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
    subscriptionInstance.addProperty(CIMProperty(
        CIMName("SubscriptionState"),
        CIMValue(Uint16(2))));

    client.createInstance(PEGASUS_NAMESPACENAME_INTEROP, subscriptionInstance);
}

void _sendTestIndication(
    CIMClient& client,
    const CIMName& methodName,
    Uint32 indicationSendCount)
{
    //
    //  Invoke method to send test indication
    //
    Array<CIMParamValue> inParams;
    Array<CIMParamValue> outParams;
    Array<CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMObjectPath className(
        String::EMPTY,
        CIMNamespaceName(),
        CIMName("Test_IndicationProviderClass"),
        keyBindings);

    inParams.append(CIMParamValue(
        String("indicationSendCount"),
        CIMValue(indicationSendCount)));

    CIMValue retValue = client.invokeMethod(
        SOURCENAMESPACE,
        className,
        methodName,
        inParams,
        outParams);

    retValue.get(result);
    PEGASUS_TEST_ASSERT(result == 0);

    //
    //  Allow time for the indication to be received and forwarded
    //
    System::sleep(2);
}

void _deleteSubscriptionInstance(
    CIMClient& client,
    const String& filterName,
    const String& handlerName)
{
    CIMObjectPath filterPath = _getFilterObjectPath(filterName);
    CIMObjectPath handlerPath = _getHandlerObjectPath(handlerName);

    Array<CIMKeyBinding> subscriptionKeyBindings;
    subscriptionKeyBindings.append(CIMKeyBinding(
        "Filter", filterPath.toString(), CIMKeyBinding::REFERENCE));
    subscriptionKeyBindings.append(CIMKeyBinding(
        "Handler", handlerPath.toString(), CIMKeyBinding::REFERENCE));
    CIMObjectPath subscriptionPath(
        String::EMPTY,
        CIMNamespaceName(),
        PEGASUS_CLASSNAME_INDSUBSCRIPTION,
        subscriptionKeyBindings);
    client.deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, subscriptionPath);
}

void _deleteHandlerInstance(
    CIMClient& client,
    const String& name)
{
    CIMObjectPath path = _getHandlerObjectPath(name);
    client.deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, path);
}

void _deleteFilterInstance(
    CIMClient& client,
    const String& name)
{
    CIMObjectPath path = _getFilterObjectPath(name);
    client.deleteInstance(PEGASUS_NAMESPACENAME_INTEROP, path);
}

void _setup(CIMClient& client, const String& qlang)
{
    CIMObjectPath filterObjectPath;
    CIMObjectPath handlerObjectPath;

    try
    {
        filterObjectPath = _createFilterInstance(
            client,
            FILTER_NAME,
            String("SELECT * FROM Test_IndicationProviderClass"),
            qlang);
    }
    catch (CIMException& e)
    {
        if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
        {
            filterObjectPath = _getFilterObjectPath(FILTER_NAME);
            cerr << "----- Warning: Filter Instance Not Created: "
                << e.getMessage() << endl;
        }
        else
        {
            cerr << "----- Error: Filter Instance Not Created: " << endl;
            throw;
        }
    }

    try
    {
        handlerObjectPath = _createHandlerInstance(
            client,
            HANDLER_NAME,
            String("localhost/CIMListener/Pegasus_SimpleDisplayConsumer"));
    }
    catch (CIMException& e)
    {
        if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
        {
            handlerObjectPath = _getHandlerObjectPath(HANDLER_NAME);
            cerr << "----- Warning: Handler Instance Not Created: "
                << e.getMessage() << endl;
        }
        else
        {
            cerr << "----- Error: Handler Instance Not Created: " << endl;
            throw;
        }
    }

    try
    {
        _createSubscriptionInstance(
            client, filterObjectPath, handlerObjectPath);
    }
    catch (CIMException& e)
    {
        if (e.getCode() == CIM_ERR_ALREADY_EXISTS)
        {
            cerr << "----- Warning: Client Subscription Instance: "
                << e.getMessage () << endl;
        }
        else
        {
            cerr << "----- Error: Client Subscription Instance: " << endl;
            throw;
        }
    }
    cout << "+++++ setup completed successfully" << endl;
}

void _generateTestIndications(CIMClient& client)
{
    // Generate matched indications
    _sendTestIndication(
        client,
        "SendTestIndicationsCount",
        _matchedIndicationsCount);

    // Generate orphan indication
    _sendTestIndication(
        client,
        "SendTestIndicationUnmatchingClassName",
        _orphanIndicationCount);

    cout << "+++++ generate indications completed successfully" << endl;
}

Array<CIMInstance> _getProviderIndicationDataInstances()
{
    Array<CIMInstance> providerIndDataInstances;

    Uint32 indicationsCount =
        _matchedIndicationsCount + _orphanIndicationCount;

    CIMInstance providerIndDataInstance(PEGASUS_CLASSNAME_PROVIDERINDDATA);
    providerIndDataInstance.addProperty(CIMProperty(
        CIMName("ProviderModuleName"),
        String("IndicationTestProviderModule")));
    providerIndDataInstance.addProperty(CIMProperty(
        CIMName("ProviderName"),
        String("IndicationTestProvider")));
    providerIndDataInstance.addProperty(CIMProperty(
        CIMName("IndicationCount"),
        Uint32(indicationsCount)));
    providerIndDataInstance.addProperty(CIMProperty(
        CIMName("OrphanIndicationCount"),
        Uint32(_orphanIndicationCount)));

    providerIndDataInstances.append(providerIndDataInstance);
    return providerIndDataInstances;
}

Array<CIMInstance> _getSubscriptionIndicationDataInstances()
{
    Array<CIMInstance> subIndDataInstances;

    CIMInstance subIndDataInstance(PEGASUS_CLASSNAME_SUBSCRIPTIONINDDATA);
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("FilterName"),
        String("root/PG_InterOp:ICFilter01")));
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("HandlerName"),
        String("root/PG_InterOp:CIM_IndicationHandlerCIMXML.ICHandler01")));
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("SourceNamespace"),
        String("root/PG_InterOp")));
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("ProviderModuleName"),
        String("IndicationTestProviderModule")));
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("ProviderName"),
        String("IndicationTestProvider")));
    subIndDataInstance.addProperty(CIMProperty(
        CIMName("MatchedIndicationCount"),
        Uint32(_matchedIndicationsCount)));

    subIndDataInstances.append(subIndDataInstance);

    return subIndDataInstances;
}

void _checkResult(CIMClient& client)
{
    Array<CIMInstance> expectedProvIndDataInstances =
        _getProviderIndicationDataInstances();

    Array<CIMInstance> expectedSubIndDataInstances =
        _getSubscriptionIndicationDataInstances();

    Array<CIMInstance> returnedProvIndDataInstances =
        client.enumerateInstances(
            PEGASUS_NAMESPACENAME_INTERNAL,
            PEGASUS_CLASSNAME_PROVIDERINDDATA);

    Array<CIMInstance> returnedSubIndDataInstances =
        client.enumerateInstances(
            PEGASUS_NAMESPACENAME_INTERNAL,
            PEGASUS_CLASSNAME_SUBSCRIPTIONINDDATA);

    Array<CIMInstance> matchedProvIndDataInstances;
    for (Uint32 i = 0; i < returnedProvIndDataInstances.size(); i++)
    {
        for (Uint32 j = 0; j < expectedProvIndDataInstances.size(); j++)
        {
            if (returnedProvIndDataInstances[i].identical(
                expectedProvIndDataInstances[j]))
            {
                matchedProvIndDataInstances.append(
                    returnedProvIndDataInstances[i]);
                break;
            }
        }
    }

    PEGASUS_TEST_ASSERT(
        matchedProvIndDataInstances.size() ==
        expectedProvIndDataInstances.size());

    Array<CIMInstance> matchedSubIndDataInstances;
    for (Uint32 i = 0; i < returnedSubIndDataInstances.size(); i++)
    {
        for (Uint32 j = 0; j < expectedSubIndDataInstances.size(); j++)
        {
            if (returnedSubIndDataInstances[i].identical(
                expectedSubIndDataInstances[j]))
            {
                matchedSubIndDataInstances.append(
                    returnedSubIndDataInstances[i]);
            }
        }
    }

    PEGASUS_TEST_ASSERT(
        matchedSubIndDataInstances.size() ==
        expectedSubIndDataInstances.size());

    cout << "+++++ Tests completed successfully" << endl;
}

void _cleanup(CIMClient& client)
{
    try
    {
        _deleteSubscriptionInstance(client, FILTER_NAME, HANDLER_NAME);
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_NOT_FOUND)
        {
            cerr << "----- Error: deleteSubscriptionInstance failure: "
                 << endl;
            throw;
        }
    }

    try
    {
        _deleteFilterInstance(client, FILTER_NAME);
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_NOT_FOUND)
        {
            cerr << "----- Error: deleteFilterInstance failure: "
                 << endl;
            throw;
        }
    }

    try
    {
        _deleteHandlerInstance(client, HANDLER_NAME);
    }
    catch (CIMException& e)
    {
        if (e.getCode() != CIM_ERR_NOT_FOUND)
        {
            cerr << "----- Error: deleteHandlerInstance failure: " << endl;
            throw;
        }
    }

    cout << "+++++ cleanup completed successfully" << endl;
}

void _test(CIMClient& client)
{
    _setup(client, "WQL");
    _generateTestIndications(client);
    _checkResult(client);
    _cleanup(client);
}

int main(int argc, char** argv)
{
#ifdef PEGASUS_ENABLE_INDICATION_COUNT
    CIMClient client;
    try
    {
        client.connectLocal();

        if (argc == 1)
        {
            _test(client);
        }
        else if (argc == 2 && (String::equalNoCase(argv[1], "cleanup")))
        {
            _cleanup(client);
        }
        else
        {
            cerr << "Invalid option: " << argv[1] << endl;
            return -1;
        }
    }
    catch (Exception& e)
    {
        cerr << e.getMessage() << endl;
        return -1;
    }
#else
    cout << argv[0]
         << ": Tracking indications count is not enabled; test skipped"
         << endl;
#endif

    return 0;
}
