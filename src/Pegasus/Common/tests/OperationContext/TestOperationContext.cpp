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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/OperationContextInternal.h>

#include <iostream>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

int main(void)
{
    OperationContext context;

    //
    //  IdentityContainer
    //
    try
    {
        String userName("Yoda");

        context.insert(IdentityContainer(userName));

        IdentityContainer container = context.get(IdentityContainer::NAME);

        if(userName != container.getUserName())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Identity Container failed" << endl;

        exit(1);
    }

    try
    {
        context.clear();
        String userName("Yoda");

        context.insert(IdentityContainer(userName));

        //
        //  This test exercises the IdentityContainer copy constructor
        //
        IdentityContainer container = 
            (IdentityContainer)context.get(IdentityContainer::NAME);

        if(userName != container.getUserName())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Identity Container copy constructor failed" << endl;

        exit(1);
    }

    try
    {
        context.clear();

        String userName("Yoda");

        context.insert(IdentityContainer(userName));

        //
        //  This test exercises the IdentityContainer assignment operator
        //
        IdentityContainer container = IdentityContainer(" ");
        container =
            (IdentityContainer)context.get(IdentityContainer::NAME);

        if(userName != container.getUserName())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Identity Container assignment operator failed" << endl;

        exit(1);
    }

    //
    //  SubscriptionInstanceContainer
    //

    //
    //  Create a Subscription instance for testing
    //
    CIMInstance filterInstance(PEGASUS_CLASSNAME_INDFILTER);
    filterInstance.addProperty(CIMProperty (CIMName ("SystemCreationClassName"),
        String("CIM_UnitaryComputerSystem")));
    filterInstance.addProperty(CIMProperty(CIMName ("SystemName"),
        String("server001.acme.com")));
    filterInstance.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString()));
    filterInstance.addProperty(CIMProperty(CIMName ("Name"),
        String("Filter1")));
    filterInstance.addProperty (CIMProperty(CIMName ("Query"),
        String("SELECT * FROM CIM_AlertIndication WHERE AlertType = 5")));
    filterInstance.addProperty (CIMProperty(CIMName ("QueryLanguage"),
        String("WQL1")));
    filterInstance.addProperty (CIMProperty(CIMName ("SourceNamespace"),
        String("root/PG_InterOp")));
    Array<CIMKeyBinding> filterKeyBindings;
    filterKeyBindings.append (CIMKeyBinding ("SystemCreationClassName", 
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("SystemName", 
        "server001.acme.com", CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString(), CIMKeyBinding::STRING));
    filterKeyBindings.append (CIMKeyBinding ("Name", 
        "Filter1", CIMKeyBinding::STRING));
    CIMObjectPath filterRef ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDFILTER, filterKeyBindings);
    filterInstance.setPath (filterRef);

    CIMInstance handlerInstance(PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance.addProperty(CIMProperty (CIMName("SystemCreationClassName"),
        String("CIM_UnitaryComputerSystem")));
    handlerInstance.addProperty(CIMProperty(CIMName ("SystemName"),
        String("server001.acme.com")));
    handlerInstance.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString()));
    handlerInstance.addProperty(CIMProperty(CIMName ("Name"),
        String("Handler1")));
    handlerInstance.addProperty(CIMProperty(CIMName ("Destination"),
        String("localhost:5988/test1")));
    Array<CIMKeyBinding> handlerKeyBindings;
    handlerKeyBindings.append (CIMKeyBinding ("SystemCreationClassName", 
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("SystemName", 
        "server001.acme.com", CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString(), 
        CIMKeyBinding::STRING));
    handlerKeyBindings.append (CIMKeyBinding ("Name", 
        "Handler1", CIMKeyBinding::STRING));
    CIMObjectPath handlerRef ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, handlerKeyBindings);
    handlerInstance.setPath (handlerRef);

    CIMInstance subscriptionInstance (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    subscriptionInstance.addProperty(CIMProperty(CIMName ("Filter"),
        filterRef, 0, PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance.addProperty(CIMProperty(CIMName ("Handler"),
        handlerRef, 0, PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("OnFatalErrorPolicy"), CIMValue ((Uint16) 4)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("FailureTriggerTimeInterval"), CIMValue ((Uint64) 60)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionState"), CIMValue ((Uint16) 2)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("TimeOfLastStateChange"), CIMValue 
            (CIMDateTime::getCurrentDateTime())));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionDuration"), CIMValue ((Uint64) 86400)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionStartTime"), CIMValue 
            (CIMDateTime::getCurrentDateTime())));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionTimeRemaining"), CIMValue ((Uint64) 86400)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("RepeatNotificationPolicy"), CIMValue ((Uint16) 1)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("OtherRepeatNotificationPolicy"), CIMValue("AnotherPolicy")));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("RepeatNotificationInterval"), CIMValue ((Uint64) 60)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("RepeatNotificationGap"), CIMValue ((Uint64) 15)));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("RepeatNotificationCount"), CIMValue ((Uint16) 3)));

    try
    {
        context.insert(SubscriptionInstanceContainer(subscriptionInstance));

        SubscriptionInstanceContainer container = context.get
            (SubscriptionInstanceContainer::NAME);

        if(!subscriptionInstance.identical(container.getInstance()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Subscription Instance Container failed" << endl;

        exit(1);
    }

    try
    {
        context.clear();

        context.insert(SubscriptionInstanceContainer(subscriptionInstance));

        //
        //  This test exercises the SubscriptionInstanceContainer copy 
        //  constructor
        //
        SubscriptionInstanceContainer container = 
            (SubscriptionInstanceContainer)context.get
            (SubscriptionInstanceContainer::NAME);

        if(!subscriptionInstance.identical(container.getInstance()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Subscription Instance Container copy constructor failed"
             << endl;

        exit(1);
    }

    try
    {
        context.clear();

        context.insert(SubscriptionInstanceContainer(subscriptionInstance));

        //
        //  This test exercises the SubscriptionInstanceContainer assignment 
        //  operator
        //
        SubscriptionInstanceContainer container = SubscriptionInstanceContainer
            (CIMInstance());
        container = (SubscriptionInstanceContainer)context.get
            (SubscriptionInstanceContainer::NAME);

        if(!subscriptionInstance.identical(container.getInstance()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << 
            "----- Subscription Instance Container assignment operator failed" 
             << endl;

        exit(1);
    }

    //
    //  SubscriptionFilterConditionContainer
    //
    try
    {
        String filterCondition ("AlertType = 5");
        String queryLanguage ("WQL1");

        context.insert(SubscriptionFilterConditionContainer
            (filterCondition, queryLanguage));

        SubscriptionFilterConditionContainer container = context.get
            (SubscriptionFilterConditionContainer::NAME);

        if((filterCondition != container.getFilterCondition()) ||
           (queryLanguage != container.getQueryLanguage()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Subscription Filter Condition Container failed" << endl;

        exit(1);
    }

    try
    {
        context.clear();
        String filterCondition ("AlertType = 5");
        String queryLanguage ("WQL1");

        context.insert(SubscriptionFilterConditionContainer(filterCondition,
            queryLanguage));

        //
        //  This test exercises the SubscriptionFilterConditionContainer copy 
        //  constructor
        //
        SubscriptionFilterConditionContainer container = 
            (SubscriptionFilterConditionContainer)context.get
            (SubscriptionFilterConditionContainer::NAME);

        if((filterCondition != container.getFilterCondition()) ||
           (queryLanguage != container.getQueryLanguage()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << 
          "----- SubscriptionFilterCondition Container copy constructor failed" 
             << endl;

        exit(1);
    }

    try
    {
        context.clear();
        String filterCondition ("AlertType = 5");
        String queryLanguage ("WQL1");

        context.insert(SubscriptionFilterConditionContainer(filterCondition,
            queryLanguage));

        //
        //  This test exercises the SubscriptionFilterConditionContainer 
        //  assignment operator
        //
        SubscriptionFilterConditionContainer container = 
            SubscriptionFilterConditionContainer(" ", " ");
        container =
            (SubscriptionFilterConditionContainer)context.get
            (SubscriptionFilterConditionContainer::NAME);

        if((filterCondition != container.getFilterCondition()) ||
           (queryLanguage != container.getQueryLanguage()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << 
        "----- SubscriptionFilterCondition Container assignment operator failed"
             << endl;

        exit(1);
    }

    //
    //  SubscriptionInstanceNamesContainer
    //

    //
    //  Create a second Subscription instance for testing
    //
    CIMInstance filterInstance2(PEGASUS_CLASSNAME_INDFILTER);
    filterInstance2.addProperty(CIMProperty (CIMName 
        ("SystemCreationClassName"), String("CIM_UnitaryComputerSystem")));
    filterInstance2.addProperty(CIMProperty(CIMName ("SystemName"),
        String("server001.acme.com")));
    filterInstance2.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString()));
    filterInstance2.addProperty(CIMProperty(CIMName ("Name"),
        String("Filter2")));
    filterInstance2.addProperty (CIMProperty(CIMName ("Query"),
        String("SELECT * FROM CIM_AlertIndication WHERE AlertType = 8")));
    filterInstance2.addProperty (CIMProperty(CIMName ("QueryLanguage"),
        String("WQL1")));
    filterInstance2.addProperty (CIMProperty(CIMName ("SourceNamespace"),
        String("root/PG_InterOp")));
    Array<CIMKeyBinding> filterKeyBindings2;
    filterKeyBindings2.append (CIMKeyBinding ("SystemCreationClassName", 
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    filterKeyBindings2.append (CIMKeyBinding ("SystemName", 
        "server001.acme.com", CIMKeyBinding::STRING));
    filterKeyBindings2.append (CIMKeyBinding ("CreationClassName", 
        PEGASUS_CLASSNAME_INDFILTER.getString(), CIMKeyBinding::STRING));
    filterKeyBindings2.append (CIMKeyBinding ("Name", 
        "Filter2", CIMKeyBinding::STRING));
    CIMObjectPath filterRef2 ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDFILTER, filterKeyBindings2);
    filterInstance2.setPath (filterRef2);

    CIMInstance handlerInstance2(PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance2.addProperty(CIMProperty (CIMName
        ("SystemCreationClassName"), String("CIM_UnitaryComputerSystem")));
    handlerInstance2.addProperty(CIMProperty(CIMName ("SystemName"),
        String("server001.acme.com")));
    handlerInstance2.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString()));
    handlerInstance2.addProperty(CIMProperty(CIMName ("Name"),
        String("Handler2")));
    handlerInstance2.addProperty(CIMProperty(CIMName ("Destination"),
        String("localhost:5988/test2")));
    Array<CIMKeyBinding> handlerKeyBindings2;
    handlerKeyBindings2.append (CIMKeyBinding ("SystemCreationClassName", 
        "CIM_UnitaryComputerSystem", CIMKeyBinding::STRING));
    handlerKeyBindings2.append (CIMKeyBinding ("SystemName", 
        "server001.acme.com", CIMKeyBinding::STRING));
    handlerKeyBindings2.append (CIMKeyBinding ("CreationClassName", 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString(), 
        CIMKeyBinding::STRING));
    handlerKeyBindings2.append (CIMKeyBinding ("Name", 
        "Handler2", CIMKeyBinding::STRING));
    CIMObjectPath handlerRef2 ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML, handlerKeyBindings2);
    handlerInstance2.setPath (handlerRef2);

    CIMInstance subscriptionInstance2 (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    subscriptionInstance2.addProperty(CIMProperty(CIMName ("Filter"),
        filterRef2, 0, PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance2.addProperty(CIMProperty(CIMName ("Handler"),
        handlerRef2, 0, PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("OnFatalErrorPolicy"), CIMValue ((Uint16) 2)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("FailureTriggerTimeInterval"), CIMValue ((Uint64) 120)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("SubscriptionState"), CIMValue ((Uint16) 2)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("TimeOfLastStateChange"), CIMValue 
            (CIMDateTime::getCurrentDateTime())));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("SubscriptionDuration"), CIMValue ((Uint64) 172800)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("SubscriptionStartTime"), CIMValue 
            (CIMDateTime::getCurrentDateTime())));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("SubscriptionTimeRemaining"), CIMValue ((Uint64) 172800)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("RepeatNotificationPolicy"), CIMValue ((Uint16) 1)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("OtherRepeatNotificationPolicy"), 
            CIMValue("AnotherPolicy2")));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("RepeatNotificationInterval"), CIMValue ((Uint64) 120)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("RepeatNotificationGap"), CIMValue ((Uint64) 30)));
    subscriptionInstance2.addProperty (CIMProperty
        (CIMName ("RepeatNotificationCount"), CIMValue ((Uint16) 6)));

    //
    //  Create two Subscription instance names for testing
    //
    Array<CIMObjectPath> subscriptionInstanceNames;
    Array<CIMKeyBinding> subscriptionKeyBindings;
    subscriptionKeyBindings.append (CIMKeyBinding ("Filter", 
        filterRef.toString(), CIMKeyBinding::REFERENCE));
    subscriptionKeyBindings.append (CIMKeyBinding ("Handler", 
        handlerRef.toString(), CIMKeyBinding::REFERENCE));
    CIMObjectPath subscriptionRef ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDSUBSCRIPTION, subscriptionKeyBindings);
    subscriptionInstance.setPath (subscriptionRef);
    Array<CIMKeyBinding> subscriptionKeyBindings2;
    subscriptionKeyBindings2.append (CIMKeyBinding ("Filter", 
        filterRef2.toString(), CIMKeyBinding::REFERENCE));
    subscriptionKeyBindings2.append (CIMKeyBinding ("Handler", 
        handlerRef2.toString(), CIMKeyBinding::REFERENCE));
    CIMObjectPath subscriptionRef2 ("", CIMNamespaceName (), 
        PEGASUS_CLASSNAME_INDSUBSCRIPTION, subscriptionKeyBindings2);
    subscriptionInstance2.setPath (subscriptionRef2);
    subscriptionInstanceNames.append (subscriptionRef);
    subscriptionInstanceNames.append (subscriptionRef2);

    try
    {
        context.insert(SubscriptionInstanceNamesContainer
            (subscriptionInstanceNames));

        SubscriptionInstanceNamesContainer container = context.get
            (SubscriptionInstanceNamesContainer::NAME);

        Array<CIMObjectPath> returnedInstanceNames = 
            container.getInstanceNames();
        for (Uint8 i = 0; i < subscriptionInstanceNames.size(); i++)
        {
            if (!subscriptionInstanceNames[i].identical
                (returnedInstanceNames[i]))
            {
                throw 0;
            }
        }
    }
    catch(...)
    {
        cout << "----- Subscription Instance Names Container failed" << endl;

        exit(1);
    }

    try
    {
        context.clear();

        context.insert(SubscriptionInstanceNamesContainer
            (subscriptionInstanceNames));

        //
        //  This test exercises the SubscriptionInstanceNamesContainer copy 
        //  constructor
        //
        SubscriptionInstanceNamesContainer container = 
            (SubscriptionInstanceNamesContainer)context.get
            (SubscriptionInstanceNamesContainer::NAME);

        Array<CIMObjectPath> returnedInstanceNames = 
            container.getInstanceNames();
        for (Uint8 i = 0; i < subscriptionInstanceNames.size(); i++)
        {
            if (!subscriptionInstanceNames[i].identical
                (returnedInstanceNames[i]))
            {
                throw 0;
            }
        }
    }
    catch(...)
    {
        cout << 
          "----- Subscription Instance Names Container copy constructor failed"
             << endl;

        exit(1);
    }

    try
    {
        context.clear();

        context.insert(SubscriptionInstanceNamesContainer
            (subscriptionInstanceNames));

        //
        //  This test exercises the SubscriptionInstanceNamesContainer 
        //  assignment operator
        //
        Array<CIMObjectPath> returnedInstanceNames;
        SubscriptionInstanceNamesContainer container = 
            SubscriptionInstanceNamesContainer (returnedInstanceNames);
        container = (SubscriptionInstanceNamesContainer)context.get
            (SubscriptionInstanceNamesContainer::NAME);

        returnedInstanceNames = container.getInstanceNames();
        for (Uint8 i = 0; i < subscriptionInstanceNames.size(); i++)
        {
            if (!subscriptionInstanceNames[i].identical
                (returnedInstanceNames[i]))
            {
                throw 0;
            }
        }
    }
    catch(...)
    {
        cout << 
        "----- Subscription Instance Names Container assignment operator failed"
             << endl;

        exit(1);
    }

    try
    {
        String languageId("en-US");

        context.insert(LocaleContainer(languageId));

        LocaleContainer container = context.get(LocaleContainer::NAME);

        if(languageId != container.getLanguageId())
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Locale Container failed" << endl;

        exit(1);
    }

    try
    {
        CIMInstance module(CIMName ("PG_ProviderModule"));
        CIMInstance provider(CIMName ("PG_Provider"));

        context.insert(ProviderIdContainer(module, provider));

        ProviderIdContainer container = context.get(ProviderIdContainer::NAME);

        if(!module.identical(container.getModule()) || !provider.identical(container.getProvider()))
        {
            throw 0;
        }
    }
    catch(...)
    {
        cout << "----- Provider Id Container failed" << endl;

        exit(1);
    }

    try
    {
        OperationContext scopeContext;

        scopeContext = context;

        scopeContext.remove(IdentityContainer::NAME);
        scopeContext.remove(SubscriptionInstanceContainer::NAME);
        scopeContext.remove(SubscriptionFilterConditionContainer::NAME);
        scopeContext.remove(LocaleContainer::NAME);
        scopeContext.remove(ProviderIdContainer::NAME);
    }
    catch(...)
    {
    }

    cout << "+++++ passed all tests." << endl;

    exit(0);
}
