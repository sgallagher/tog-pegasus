//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
//             (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName SOURCENAMESPACE = 
    CIMNamespaceName ("root/SampleProvider");

void _createHandlerInstance 
    (CIMClient & client, 
     const String & name,
     const String & destination)
{
    CIMInstance handlerInstance (PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance.addProperty (CIMProperty (CIMName 
        ("SystemCreationClassName"), String ("CIM_UnitaryComputerSystem")));
    handlerInstance.addProperty (CIMProperty (CIMName ("SystemName"),
        String ("server001.acme.com")));
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
     const String & query)
{
    CIMInstance filterInstance (PEGASUS_CLASSNAME_INDFILTER);
    filterInstance.addProperty (CIMProperty (CIMName 
        ("SystemCreationClassName"), String ("CIM_UnitaryComputerSystem")));
    filterInstance.addProperty (CIMProperty (CIMName ("SystemName"),
        String ("server001.acme.com")));
    filterInstance.addProperty (CIMProperty (CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString ()));
    filterInstance.addProperty (CIMProperty (CIMName ("Name"), name));
    filterInstance.addProperty (CIMProperty (CIMName ("Query"), query));
    filterInstance.addProperty (CIMProperty (CIMName ("QueryLanguage"),
        String ("WQL")));
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

void _sendIndicationShouldSucceed 
    (CIMClient & client)
{
    Array <CIMParamValue> inParams;
    Array <CIMParamValue> outParams;
    Array <CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMName methodName ("SendTestIndication");
    CIMObjectPath className (String::EMPTY, CIMNamespaceName (), 
        CIMName ("RT_TestIndication"), keyBindings);

    CIMValue retValue = client.invokeMethod 
        (SOURCENAMESPACE,
        className,
        methodName,
        inParams,
        outParams);
    retValue.get (result);
    PEGASUS_ASSERT (result == 0);
}

void _sendIndicationShouldFail
    (CIMClient & client)
{
    Array <CIMParamValue> inParams;
    Array <CIMParamValue> outParams;
    Array <CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMName methodName ("SendTestIndication");
    CIMObjectPath className (String::EMPTY, CIMNamespaceName (), 
        CIMName ("RT_TestIndication"), keyBindings);

    CIMValue retValue = client.invokeMethod 
        (SOURCENAMESPACE,
        className,
        methodName,
        inParams,
        outParams);
    retValue.get (result);
    PEGASUS_ASSERT (result == 1);
}

void _sendIndicationShouldBeBlocked 
    (CIMClient & client)
{
    Array <CIMParamValue> inParams;
    Array <CIMParamValue> outParams;
    Array <CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMName methodName ("SendTestIndication");
    CIMObjectPath className (String::EMPTY, CIMNamespaceName (), 
        CIMName ("RT_TestIndication"), keyBindings);

    try
    {
        CIMValue retValue = client.invokeMethod 
            (SOURCENAMESPACE,
            className,
            methodName,
            inParams,
            outParams);
        retValue.get (result);
        PEGASUS_ASSERT (false);
    }
    catch (CIMException & e)
    {
        PEGASUS_ASSERT (e.getCode () == CIM_ERR_ACCESS_DENIED);
    }
}

void _deleteSubscriptionInstance 
    (CIMClient & client, 
     const String & filterName,
     const String & handlerName)
{
    Array<CIMKeyBinding> filterKeyBindings;
    filterKeyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("SystemName",
        "server001.acme.com", CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("CreationClassName",
        PEGASUS_CLASSNAME_INDFILTER.getString(), CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("Name", filterName,
        CIMKeyBinding::STRING));
    CIMObjectPath filterPath ("", CIMNamespaceName (),
        PEGASUS_CLASSNAME_INDFILTER, filterKeyBindings);

    Array<CIMKeyBinding> handlerKeyBindings;
    handlerKeyBindings.append (CIMKeyBinding ("SystemCreationClassName",
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("SystemName",
        "server001.acme.com", CIMKeyBinding::STRING));
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
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("SystemName",
        "server001.acme.com", CIMKeyBinding::STRING));
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
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    keyBindings.append (CIMKeyBinding ("SystemName",
        "server001.acme.com", CIMKeyBinding::STRING));
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
    PEGASUS_STD (cerr) 
        << "Usage: TestDisableEnable2 "
        << "{setup | create | sendSucceed | sendFail | sendBlock "
        << "| delete | cleanup}" 
        << PEGASUS_STD (endl);
}

void _setup (CIMClient & client)
{
    try
    {
        _createFilterInstance (client, String ("DEFilter01"),
            String ("SELECT MethodName FROM RT_TestIndication"));
        _createHandlerInstance (client, String ("DEHandler01"), 
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

void _create (CIMClient & client)
{
    try
    {

        _createSubscriptionInstance (client, CIMObjectPath ("CIM_IndicationFilter.CreationClassName=\"CIM_IndicationFilter\",Name=\"DEFilter01\",SystemCreationClassName=\"CIM_UnitaryComputerSystem\",SystemName=\"server001.acme.com\""), 
            CIMObjectPath ("CIM_IndicationHandlerCIMXML.CreationClassName=\"CIM_IndicationHandlerCIMXML\",Name=\"DEHandler01\",SystemCreationClassName=\"CIM_UnitaryComputerSystem\",SystemName=\"server001.acme.com\""));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "create failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ create completed successfully"
                       << PEGASUS_STD (endl);
}

void _sendSucceed (CIMClient & client)
{
    try
    {
        _sendIndicationShouldSucceed (client);
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "sendSucceed failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ sendSucceed completed successfully"
                       << PEGASUS_STD (endl);
}

void _sendFail (CIMClient & client)
{
    try
    {
        _sendIndicationShouldFail (client);
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "sendFail failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ sendFail completed successfully"
                       << PEGASUS_STD (endl);
}

void _sendBlock (CIMClient & client)
{
    try
    {
        _sendIndicationShouldBeBlocked (client);
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "sendBlock failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ sendBlock completed successfully"
                       << PEGASUS_STD (endl);
}

void _delete (CIMClient & client)
{
    try
    {
        _deleteSubscriptionInstance (client, String ("DEFilter01"),
            String ("DEHandler01"));
    }
    catch (Exception & e)
    {
        PEGASUS_STD (cerr) << "delete failed: " << e.getMessage ()
                           << PEGASUS_STD (endl);
        exit (-1);
    }

    PEGASUS_STD (cout) << "+++++ delete completed successfully"
                       << PEGASUS_STD (endl);
}

void _cleanup (CIMClient & client)
{
    try
    {
        _deleteHandlerInstance (client, String ("DEHandler01"));
        _deleteFilterInstance (client, String ("DEFilter01"));
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
        exit (-1);
    }

    if (argc != 2)
    {
        _usage ();
        exit (1);
    }

    else
    {
        const char * opt = argv [1];

        if (String::equalNoCase (opt, "setup"))
        {
            _setup (client);
        }
        else if (String::equalNoCase (opt, "create"))
        {
            _create (client);
        }
        else if (String::equalNoCase (opt, "sendSucceed"))
        {
            _sendSucceed (client);
        }
        else if (String::equalNoCase (opt, "sendFail"))
        {
            _sendFail (client);
        }
        else if (String::equalNoCase (opt, "sendBlock"))
        {
            _sendBlock (client);
        }
        else if (String::equalNoCase (opt, "delete"))
        {
            _delete (client);
        }
        else if (String::equalNoCase (opt, "cleanup"))
        {
            _cleanup (client);
        }
        else
        {
            PEGASUS_STD (cerr) << "Invalid option: " << opt 
                << PEGASUS_STD (endl);
            _usage ();
            exit (-1);
        }
    }

    exit (0);
}
