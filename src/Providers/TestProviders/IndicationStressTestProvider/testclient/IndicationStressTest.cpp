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
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName SOURCENAMESPACE = 
    CIMNamespaceName ("test/TestProvider");

void _createHandlerInstance 
    (CIMClient & client, 
     const String & name,
     const String & destination)
{
    CIMInstance handlerInstance (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance.addProperty (CIMProperty (CIMName 
        ("SystemCreationClassName"), System::getSystemCreationClassName ()));
    handlerInstance.addProperty (CIMProperty (CIMName ("SystemName"),
        System::getFullyQualifiedHostName ()));
    handlerInstance.addProperty (CIMProperty (CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString ()));
    handlerInstance.addProperty (CIMProperty (CIMName ("Name"), name));
    handlerInstance.addProperty (CIMProperty (CIMName ("Destination"),
        destination));

    CIMObjectPath path = client.createInstance (NAMESPACE, handlerInstance);
}

void _createFilterInstance 
    (CIMClient & client, 
     const String & name,
     const String & query,
     const String & qlang)
{
    CIMInstance filterInstance (PEGASUS_CLASSNAME_INDFILTER);
    filterInstance.addProperty (CIMProperty (CIMName 
        ("SystemCreationClassName"), System::getSystemCreationClassName ()));
    filterInstance.addProperty (CIMProperty (CIMName ("SystemName"),
        System::getFullyQualifiedHostName ()));
    filterInstance.addProperty (CIMProperty (CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString ()));
    filterInstance.addProperty (CIMProperty (CIMName ("Name"), name));
    filterInstance.addProperty (CIMProperty (CIMName ("Query"), query));
    filterInstance.addProperty (CIMProperty (CIMName ("QueryLanguage"),
        String (qlang)));
    filterInstance.addProperty (CIMProperty (CIMName ("SourceNamespace"),
        SOURCENAMESPACE.getString ()));

    CIMObjectPath path = client.createInstance (NAMESPACE, filterInstance);
}

void _createSubscriptionInstance 
    (CIMClient & client,
     const CIMObjectPath & filterPath,
     const CIMObjectPath & handlerPath)
{
    CIMInstance subscriptionInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    subscriptionInstance.addProperty (CIMProperty (CIMName ("Filter"),
        filterPath, 0, PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance.addProperty (CIMProperty (CIMName ("Handler"),
        handlerPath, 0, PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionState"), CIMValue ((Uint16) 2)));

    CIMObjectPath path = client.createInstance (NAMESPACE, 
        subscriptionInstance);
}

void _sendTestIndication(CIMClient & client, const CIMName & methodName, Uint32 indicationSendCount)
{
    //
    //  Invoke method to send test indication
    //
    Array <CIMParamValue> inParams;
    Array <CIMParamValue> outParams;
    Array <CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMValue sendCountValue(indicationSendCount);
    inParams.append(CIMParamValue(String("indicationSendCount"), CIMValue(indicationSendCount)));

    CIMObjectPath className (String::EMPTY, CIMNamespaceName (), 
        CIMName ("IndicationStressTestClass"), keyBindings);

    CIMValue retValue = client.invokeMethod 
        (SOURCENAMESPACE,
        className,
        methodName,
        inParams,
        outParams);
    retValue.get (result);
    PEGASUS_ASSERT (result == 0);

    //
    //  Allow time for the indication to be received and forwarded
    //
    System::sleep (5);
}

void _sendTestIndicationNormal(CIMClient & client, Uint32 indicationSendCount)
{
    _sendTestIndication (client, CIMName ("SendTestIndicationNormal"), indicationSendCount);
}

void _deleteSubscriptionInstance 
    (CIMClient & client, 
     const String & filterName,
     const String & handlerName)
{
    Array<CIMKeyBinding> filterKeyBindings;
    filterKeyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        System::getSystemCreationClassName (), CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("SystemName",
        System::getFullyQualifiedHostName (), CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("CreationClassName",
        PEGASUS_CLASSNAME_INDFILTER.getString(), CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("Name", filterName,
        CIMKeyBinding::STRING));
    CIMObjectPath filterPath ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDFILTER, filterKeyBindings);

    Array<CIMKeyBinding> handlerKeyBindings;
    handlerKeyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        System::getSystemCreationClassName (), CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("SystemName",
        System::getFullyQualifiedHostName (), CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("CreationClassName",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString(),
        CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("Name", handlerName,
        CIMKeyBinding::STRING));
    CIMObjectPath handlerPath ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, handlerKeyBindings);

    Array<CIMKeyBinding> subscriptionKeyBindings;
    subscriptionKeyBindings.append (CIMKeyBinding ("Filter",
        filterPath.toString (), CIMKeyBinding::REFERENCE));
    subscriptionKeyBindings.append (CIMKeyBinding ("Handler",
        handlerPath.toString (), CIMKeyBinding::REFERENCE));
    CIMObjectPath subscriptionPath ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDSUBSCRIPTION, subscriptionKeyBindings);
    client.deleteInstance (NAMESPACE, subscriptionPath);
}

void _deleteHandlerInstance 
    (CIMClient & client, 
     const String & name)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        System::getSystemCreationClassName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("SystemName",
        System::getFullyQualifiedHostName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("CreationClassName",
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString(),
        CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("Name", name,
        CIMKeyBinding::STRING));
    CIMObjectPath path ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, keyBindings);
    client.deleteInstance (NAMESPACE, path);
}

void _deleteFilterInstance 
    (CIMClient & client, 
     const String & name)
{
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        System::getSystemCreationClassName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("SystemName",
        System::getFullyQualifiedHostName (), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("CreationClassName",
        PEGASUS_CLASSNAME_INDFILTER.getString(), CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("Name", name, 
        CIMKeyBinding::STRING));
    CIMObjectPath path ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDFILTER, keyBindings);
    client.deleteInstance (NAMESPACE, path);
}

void _usage ()
{
    PEGASUS_STD (cerr) << PEGASUS_STD(endl)
        << "Usage:" << PEGASUS_STD(endl) 
        << "\tTestIndicationStressTest setup [ wql | cim:cql ]" << PEGASUS_STD(endl)
        << "\tTestIndicationStressTest run <indicationSendCount>" << PEGASUS_STD(endl)
        << "\tTestIndicationStressTest cleanup" << PEGASUS_STD(endl)
        << "where: " << PEGASUS_STD(endl)
        << "\t<indicationSendCount> is the number of indications to generate," << PEGASUS_STD(endl)
        << "\t\tand can be zero to measure the overhead in calling the provider." << PEGASUS_STD(endl)
        << "\t\tThis parameter is only required for the \"run\" option." << PEGASUS_STD(endl) <<PEGASUS_STD(endl);
}

void _setup (CIMClient & client, String& qlang)
{
    try
    {
        _createFilterInstance (client, String ("IPFilter01"),
            String ("SELECT * FROM IndicationStressTestClass"),
            qlang);
        _createHandlerInstance (client, String ("IPHandler01"), 
            String ("localhost/CIMListener/Pegasus_SimpleDisplayConsumer"));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "setup failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ setup completed successfully"
                       << PEGASUS_STD (endl);
}

void _create1 (CIMClient & client)
{
    try
    {
        String filterPathString;
        filterPathString.append ("CIM_IndicationFilter.CreationClassName=\"CIM_IndicationFilter\",Name=\"IPFilter01\",SystemCreationClassName=\"");
        filterPathString.append (System::getSystemCreationClassName ());
        filterPathString.append ("\",SystemName=\"");
        filterPathString.append (System::getFullyQualifiedHostName ());
        filterPathString.append ("\"");
        String handlerPathString;
        handlerPathString.append ("CIM_IndicationHandlerCIMXML.CreationClassName=\"CIM_IndicationHandlerCIMXML\",Name=\"IPHandler01\",SystemCreationClassName=\"");
        handlerPathString.append (System::getSystemCreationClassName ());
        handlerPathString.append ("\",SystemName=\"");
        handlerPathString.append (System::getFullyQualifiedHostName ());
        handlerPathString.append ("\"");
        _createSubscriptionInstance (client, CIMObjectPath (filterPathString),
            CIMObjectPath (handlerPathString));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "create1 failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ create1 completed successfully"
                       << PEGASUS_STD (endl);
}

void _sendNormal (CIMClient & client, Uint32 indicationSendCount)
{
    try
    {
        _sendTestIndicationNormal (client, indicationSendCount);
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "sendNormal failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ sendNormal completed successfully"
                       << PEGASUS_STD (endl);
}

void _delete1 (CIMClient & client)
{
    try
    {
        _deleteSubscriptionInstance (client, String ("IPFilter01"),
            String ("IPHandler01"));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "delete1 failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ delete1 completed successfully"
                       << PEGASUS_STD (endl);
}

void _cleanup (CIMClient & client)
{
    try
    {
        _deleteHandlerInstance (client, String ("IPHandler01"));
        _deleteFilterInstance (client, String ("IPFilter01"));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "cleanup failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ cleanup completed successfully"
                       << PEGASUS_STD (endl);
}

int _test(CIMClient& client, const char* opt, const char* optTwo)
{
    if (String::equalNoCase (opt, "setup"))
    { 
        if ((optTwo == NULL) ||
            (!(String::equalNoCase (optTwo, "wql") ||
            String::equalNoCase (optTwo, "cim:cql"))))
        { 
            PEGASUS_STD (cerr) << "Invalid query language."
                               << PEGASUS_STD (endl);
            _usage ();
            return -1;
        }
        String qlang(optTwo);
        _setup (client, qlang);
        _create1 (client);
    }
    else if (String::equalNoCase (opt, "run"))
    {
        if (optTwo == NULL) 
        {
            PEGASUS_STD (cerr) << "Invalid indicationSendCount."
                               << PEGASUS_STD (endl);
            _usage ();
            return -1;
        }
        Uint32 indicationSendCount = atoi(optTwo);
        _sendNormal (client, indicationSendCount);
    }
    else if (String::equalNoCase (opt, "cleanup"))
    {
        _delete1 (client);
        _cleanup (client);
    }
    else
    {
      PEGASUS_STD (cerr) << "Invalid option: " << opt 
                         << PEGASUS_STD (endl);
      _usage ();
      return -1;
    }

  return 0;
}

int main (int argc, char** argv)
{
    CIMClient client;
    try
    {
        client.connectLocal ();
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << e.getMessage () << PEGASUS_STD (endl);
        return -1;
    }
    
    if (argc <= 1 || argc > 3)
    {
        PEGASUS_STD (cerr) << "Invalid argument count: " << argc
                           << PEGASUS_STD (endl);
        _usage ();
        return 1;
    }
    else
    {
        const char * opt = argv[1];
        const char * optTwo;

        if (argc == 3)
            optTwo = argv [2];
        else
            optTwo = NULL;
        
        return _test(client, opt, optTwo);
    }

    return 0;
}
