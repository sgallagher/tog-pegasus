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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName INTEROPNAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName 
    SOURCENAMESPACE = CIMNamespaceName ("root/SampleProvider");
static const CIMName 
    FILTER_CLASSNAME = CIMName ("CIM_IndicationFilter");
static const CIMName 
    HANDLER_CLASSNAME = CIMName ("CIM_IndicationHandlerCIMXML");
static const CIMName 
    SUBSCRIPTION_CLASSNAME = CIMName ("CIM_IndicationSubscription");

void _usage ()
{
    cerr << "Usage: SendTestIndications "
        << "[subscribe | sendTestIndications | unsubscribe]" << endl;
}

CIMObjectPath _createFilter
   (CIMClient & client,
    const String & name,
    const String & query,
    const String & queryLanguage,
    const String & sourceNamespace)
{
    CIMInstance filter (FILTER_CLASSNAME);

    filter.addProperty (CIMProperty (CIMName ("Name"), name));
    filter.addProperty (CIMProperty (CIMName ("Query"), query));
    filter.addProperty (CIMProperty (CIMName ("QueryLanguage"), queryLanguage));
    filter.addProperty (CIMProperty (CIMName ("SourceNamespace"), 
        sourceNamespace));

    CIMObjectPath path = client.createInstance (INTEROPNAMESPACE, filter);
    return path;
}

CIMObjectPath _createHandler
   (CIMClient & client,
    const String & name,
    const String & destination)
{
    CIMInstance handler (HANDLER_CLASSNAME);

    handler.addProperty (CIMProperty (CIMName ("Name"), name));
    handler.addProperty (CIMProperty (CIMName ("Destination"), destination));

    CIMObjectPath path = client.createInstance (INTEROPNAMESPACE, handler);
    return path;
}

CIMObjectPath _createSubscription
   (CIMClient & client,
    const CIMObjectPath & filterPath,
    const CIMObjectPath & handlerPath)
{
    CIMInstance subscription (SUBSCRIPTION_CLASSNAME);

    subscription.addProperty (CIMProperty (CIMName ("Filter"),
        filterPath, 0, FILTER_CLASSNAME));
    subscription.addProperty (CIMProperty (CIMName ("Handler"),
        handlerPath, 0, HANDLER_CLASSNAME));

    CIMObjectPath path = client.createInstance (INTEROPNAMESPACE, subscription);
    return path;
}

CIMObjectPath _findFilterOrHandlerPath
   (CIMClient & client,
    const CIMName & className,
    const String & name)
{
    CIMObjectPath path;
    Array <CIMObjectPath> paths;
    paths = client.enumerateInstanceNames (INTEROPNAMESPACE, className);

    for (Uint32 i = 0; i < paths.size (); i++)
    {
        Array <CIMKeyBinding> keyBindings = paths [i].getKeyBindings ();
        for (Uint32 j = 0; j < keyBindings.size (); j++)
        {
            if (keyBindings [j].getName ().equal ("Name"))
            {
                if (keyBindings [j].getValue () == name)
                {
                    path = paths [i];
                    break;
                }
            }
        }
    }

    return path;
}

CIMObjectPath _buildSubscriptionPath
   (const CIMObjectPath & filterPath,
    const CIMObjectPath & handlerPath)
{
    CIMObjectPath path;

    Array <CIMKeyBinding> keyBindings;
    keyBindings.append (CIMKeyBinding ("Filter",
        filterPath.toString (), CIMKeyBinding::REFERENCE));
    keyBindings.append (CIMKeyBinding ("Handler",
        handlerPath.toString (), CIMKeyBinding::REFERENCE));

    path.setClassName (SUBSCRIPTION_CLASSNAME);
    path.setKeyBindings (keyBindings);

    return path;
}

//
//  Create subscription to receive indications of the RT_TestIndication
//  class and have them sent to the SimpleDisplayConsumer
//  The SimpleDisplayConsumer logs received indications to the 
//  indicationLog file
//
void _subscribe
    (CIMClient & client)
{
    //
    //  Create filter
    //
    CIMObjectPath filterPath = _createFilter 
       (client, 
        "TestFilter",
        "SELECT MethodName FROM RT_TestIndication",
        "WQL",
         SOURCENAMESPACE.getString ());

    //
    //  Create handler
    //
    CIMObjectPath handlerPath = _createHandler 
       (client,
        "TestHandler",
        "localhost/CIMListener/Pegasus_SimpleDisplayConsumer");

    //
    //  Create subscription to receive indications of the RT_TestIndication
    //  class and have them sent to the SimpleDisplayConsumer
    //  The SimpleDisplayConsumer logs received indications to the 
    //  indicationLog file
    //
    CIMObjectPath subscriptionPath = _createSubscription 
       (client,
        filterPath, 
        handlerPath);
}

//
//  Send test indications
//  The RT_IndicationProvider SendTestIndication method is invoked
//  If the provider is enabled, it sends two test indications
//  (the second test indication includes SNMP trap OID)
//
Sint32 _sendTestIndications
   (CIMClient & client)
{
    //
    //  Invoke method to send test indication
    //
    Array <CIMParamValue> inParams;
    Array <CIMParamValue> outParams;
    Array <CIMKeyBinding> keyBindings;
    Sint32 result;

    CIMObjectPath className (String::EMPTY, CIMNamespaceName (),
        CIMName ("RT_TestIndication"), keyBindings);

    CIMValue retValue = client.invokeMethod
        (SOURCENAMESPACE,
        className,
        CIMName ("SendTestIndication"),
        inParams,
        outParams);
    retValue.get (result);

    return result;
}

//
//  Delete the subscription, handler, and filter instances
//
void _unsubscribe
    (CIMClient & client)
{
    CIMObjectPath filterPath = 
        _findFilterOrHandlerPath (client, FILTER_CLASSNAME, "TestFilter");
    CIMObjectPath handlerPath = 
        _findFilterOrHandlerPath (client, HANDLER_CLASSNAME, "TestHandler");
    CIMObjectPath subscriptionPath = 
        _buildSubscriptionPath (filterPath, handlerPath);

    //
    //  Delete subscription
    //
    client.deleteInstance (INTEROPNAMESPACE, subscriptionPath);

    //
    //  Delete handler
    //
    client.deleteInstance (INTEROPNAMESPACE, handlerPath);

    //
    //  Delete filter
    //
    client.deleteInstance (INTEROPNAMESPACE, filterPath);
}

int main (int argc, char ** argv)
{
    try
    {
        CIMClient client;

        //
        //  Connect to CIM Server
        //
        try
        {
            client.connectLocal ();
        }

        catch(Exception& e)
        {
            cerr << "Exception thown by client.connectLocal(): " 
                << e.getMessage() << endl;
            return -1;
        }


        if (argc > 2)
        {
            _usage ();
            return 1;
        }

        else if (argc == 1)
        {
            try
            {
                _subscribe (client);
            }
            catch(Exception& e)
            {
                cerr << "Exception thrown by _subscribe method: " 
                    << e.getMessage() << endl;
                return -1;
            }

            Sint32 result = _sendTestIndications (client);

            if (result == 0)
            {
                cout << "Successfully sent test indications" << endl;
            }
            else
            {
                cerr << "Failed to send test indications" << endl;
            }

            try
            {
                _unsubscribe (client);
            }
            catch(Exception& e)
            {
                cerr << "Exception thrown by _unsubscribe method: " 
                    << e.getMessage() << endl;
                return -1;
            }
        }

        else
        {
            const char * opt = argv [1];

            if (String::equalNoCase (opt, "subscribe"))
            {
                try
                {
                    _subscribe (client);
                }
                catch(Exception& e)
                {
                    cerr << "Exception thrown by _subscribe method: "
                         << e.getMessage() << endl;
                    return -1;
                }
            }
            else if (String::equalNoCase (opt, "sendTestIndications"))
            {
                Sint32 result = _sendTestIndications (client);

                if (result == 0)
                {
                    cout << "Successfully sent test indications" << endl;
                }
                else
                {
                    cerr << "Failed to send test indications" << endl;
                }
            }
            else if (String::equalNoCase (opt, "unsubscribe"))
            {
                try
                {
                    _unsubscribe (client);
                }
                catch(Exception& e)
                {
                    cerr << "Exception thrown by _unsubscribe method: "
                         << e.getMessage() << endl;
                    return -1;
                }
            }
            else
            {
                cerr << "Invalid option: " << opt << endl;
                _usage ();
                return -1;
            }
        }
    }
    catch (Exception & e)
    {
        cerr << "SendTestIndications failed: " << e.getMessage () << endl;
        return -1;
    }

    return 0;
}
