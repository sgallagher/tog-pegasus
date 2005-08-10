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
// Author: Dave Sudlik, IBM (dsudlik@us.ibm.com)
//
// Modified By: Jim Wunderlich (Jim_Wunderlich@prodigy.net)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/OptionManager.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/Stopwatch.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/AutoPtr.h>

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Listener/CIMListener.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName INTEROP_NAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName SOURCE_NAMESPACE = CIMNamespaceName ("test/TestProvider");

const String INDICATION_NAME = String ("IndicationStressTestClass");
AtomicInt receivedIndicationCount = 0;

#define MAX_UNIQUE_IDS 10000
Uint64 seqNumPrevious[MAX_UNIQUE_IDS];
AtomicInt seqNumberErrors = 0;
Uint32 seqNumberErrorsDisplay = 0;
Uint32 indicationSendCountTotal = 0;
Uint64 sendRecvDeltaTimeTotal = 0;
int sendRecvDeltaTimeCnt = 0;
int sendRecvDeltaTimeMax = 0;
int sendRecvDeltaTimeMin = 0x7fffffff;

AtomicInt errorsEncountered = 0;


////////////////////////////////////////////////////////////////////////////////
//
// Thread Parameters Class
//
////////////////////////////////////////////////////////////////////////////////

class T_Parms{
   public:
    AutoPtr<CIMClient> client;
    Uint32 indicationSendCount;
    Uint32 uniqueID;
};

////////////////////////////////////////////////////////////////////////////////
//
// IndicationConsumer Class
//
////////////////////////////////////////////////////////////////////////////////

class MyIndicationConsumer : public CIMIndicationConsumer
{
public:
    MyIndicationConsumer(String name);
    ~MyIndicationConsumer();

    void consumeIndication(const OperationContext& context,
        const String & url,
        const CIMInstance& indicationInstance);

private:
    String name;

};

MyIndicationConsumer::MyIndicationConsumer(String name)
{
    this->name = name;
    for (Uint32 i=0; i < MAX_UNIQUE_IDS; i++)
      seqNumPrevious[i] = 1;

//  cout << "Constructing MyIndicationConsumer" << endl;
}

MyIndicationConsumer::~MyIndicationConsumer()
{
//  cout << "Destructing MyIndicationConsumer" << endl;
}

Boolean maxUniqueIDMsgIssued = false;

void MyIndicationConsumer::consumeIndication(
                         const OperationContext & context,
                         const String & url,
                         const CIMInstance& indicationInstance)
{
  //
  // Increment the count of indications received
  //
  receivedIndicationCount++;
  assert(indicationInstance.getClassName().getString() == INDICATION_NAME);
  if (receivedIndicationCount.value() % 200 == 0)
    cout << "+++++     received indications = " 
         << receivedIndicationCount.value() 
         << " of " << indicationSendCountTotal 
         << " sent, waiting for more ..." << endl;

  // cout << "IndicationStressTest consumer - recvd indication = " << ((CIMObject)indicationInstance).toString() << endl;

  //
  // Get the date and time from the indication
  // Compare it to the current date
  // calculate the time it took to be delivered.
  // add it to the total delivery time to calculate the average 
  //      indication delivery time for the test.
  // Update the min and max delta times.
  //

  // 
  // Calculate the time diference between when sent and received (now)
  //

  Uint32 indicationTimeIndex = indicationInstance.findProperty("IndicationTime");
  

  if (indicationTimeIndex == PEG_NOT_FOUND)
    {
      cout << "+++++ ERROR: Indication Stress Test Consumer - indicationTime NOT FOUND" << endl;
      errorsEncountered++;
      return;
    }


  CIMConstProperty indicationTime_property = indicationInstance.getProperty(indicationTimeIndex);
  // cout << "indicationTime = " << indicationTime_property.getValue().toString() << endl;

  CIMDateTime indicationTime;
  indicationTime_property.getValue().get(indicationTime);

  CIMDateTime currentDateTime = CIMDateTime::getCurrentDateTime ();
  Sint64 sendRecvDeltaTime = CIMDateTime::getDifference(indicationTime, currentDateTime);

  // cout << "sendRecvDeltaTime = " << (long)(sendRecvDeltaTime/1000) << " milli-seconds" << endl;

  sendRecvDeltaTimeTotal += sendRecvDeltaTime;
  sendRecvDeltaTimeCnt++;

  if (sendRecvDeltaTime > sendRecvDeltaTimeMax)
    sendRecvDeltaTimeMax = sendRecvDeltaTime;

  if (sendRecvDeltaTime < sendRecvDeltaTimeMin)
    sendRecvDeltaTimeMin = sendRecvDeltaTime;

  //
  // Get the unique ID
  // 
  // This is sort of a Thread ID except that the unique ID keeps incrementing
  // across tests runs as long as the server continues to run) 
  //

  Uint32 uniqueIDIndex = indicationInstance.findProperty("IndicationIdentifier");
  

  if (uniqueIDIndex == PEG_NOT_FOUND)
    {
      cout << "+++++ ERROR: Indication Stress Test Consumer - indication Unique id NOT FOUND" << endl;
      errorsEncountered++;
      return;
    }


  CIMConstProperty uniqueID_property = indicationInstance.getProperty(uniqueIDIndex);
  // cout << "uniqueID = " << uniqueID_property.getValue().toString() << endl;

  String uniqueID_string;
  Uint32 uniqueID = 0;
  uniqueID_property.getValue().get(uniqueID_string);
  uniqueID = atoi (uniqueID_string.getCString());
  
  // cout << "uniqueID = " << uniqueID << endl;

  //
  // Get the seq number
  //

  Uint32 seqNumIndex = indicationInstance.findProperty ("IndicationSequenceNumber");

  if (seqNumIndex == PEG_NOT_FOUND)
    {
      cout << "+++++ ERROR: Indication Stress Test Consumer - indication seq number NOT FOUND" << endl;
      errorsEncountered++;
    }
  else if ((long)uniqueID > MAX_UNIQUE_IDS)
    {
      if (!maxUniqueIDMsgIssued)
        {
          maxUniqueIDMsgIssued = true;
          cout << endl;
          cout << "+++++ ERROR: Indication Stress TestConsumer - recvd uniqueID ( "
               << (long)uniqueID << " ) GT MAX_UNIQUE_IDS ( " << MAX_UNIQUE_IDS << " )"
               << endl;
          cout << "+++++        To correct: Stop and start the server, this resets the uniqueID generated by the provider." 
               << endl;
          cout << "+++++        Sequence number checking is not completly enabled without this" << endl << endl;
          errorsEncountered++;
        }
      
    }
  else
    {
      CIMConstProperty seqNum_property = indicationInstance.getProperty(seqNumIndex);
      // cout << "seqNum = " << (seqNum_property.getValue()).toString() << endl;

      Uint64 seqNumRecvd;
      seqNum_property.getValue().get(seqNumRecvd);
      // cout << "seqNumRecvd = " << (long)seqNumRecvd << endl;

      //
      // See if seqNumRecvd less than previous received matches seqNumPrevious
      //
      // The method used to determine the out of order count is 
      // (received < previous) received. 
      //
      // The other choice would would be (received != expected) where expected
      // is the previous received +1. 
      // 
      // The (actual < previous) was chosen as giving results that  
      // are more representative of the ordering problems.
      // 
      // Consider these indication sequences:
      //
      //     A: 1,3,4,2,5
      //     B: 1,4,3,2,5
      //
      // The "out of sequence" counts for "(actual != expected)" are A=3, B=2, 
      // while "(actual < previous)" gives A=1, B=2.
      //
      // Thanks to Roger Kump at HP for suggesting the actual < previous method.
      //
      // JR Wunderlich 7/14/2005
      //

      if (seqNumRecvd < seqNumPrevious[uniqueID])
        {
          seqNumberErrors++;
          if (seqNumberErrorsDisplay)
            {
              cout << "+++++ ERROR: Indication Stress Test Consumer"
                   << "- Sequence error "
                   << " previous = " << (unsigned long) seqNumPrevious[uniqueID]
                   << " received = " << (unsigned long) seqNumRecvd << endl;
            }
        }
      seqNumPrevious[uniqueID] = seqNumRecvd;  
    }

  
  return;

}

///////////////////////////////////////////////////////////////////////////

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

    CIMObjectPath path = client.createInstance (INTEROP_NAMESPACE, handlerInstance);
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
        SOURCE_NAMESPACE.getString ()));

    CIMObjectPath path = client.createInstance (INTEROP_NAMESPACE, filterInstance);
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

    CIMObjectPath path = client.createInstance (INTEROP_NAMESPACE,
        subscriptionInstance);
}

void _sendTestIndication(CIMClient* client, const CIMName & methodName, Uint32 indicationSendCount)
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

    CIMValue retValue = client->invokeMethod
        (SOURCE_NAMESPACE,
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


void _sendTestIndicationNormal(CIMClient* client, Uint32 indicationSendCount)
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
    client.deleteInstance (INTEROP_NAMESPACE, subscriptionPath);
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
    client.deleteInstance (INTEROP_NAMESPACE, path);
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
    client.deleteInstance (INTEROP_NAMESPACE, path);
}

void _usage ()
{
   cerr << endl
        << "Usage:" << endl
        << "\tTestIndicationStressTest setup [ WQL | CIM:CQL ]" << endl
        << "\tTestIndicationStressTest run <indicationSendCount> [<threads>]" << endl
        << "\tTestIndicationStressTest cleanup" << endl
        << "where: " << endl
        << "\t<indicationSendCount> is the number of indications to generate," << endl
        << "\t\tand can be zero to measure the overhead in calling the provider." << endl
        << "\t<threads> is an optional number of client threads to create, default is one," << endl
        << "\t\tThese parameters are only required for the \"run\" option." << endl <<endl;
}

void _setup (CIMClient & client, String& qlang)
{
    try
    {
        _createFilterInstance (client, String ("IPFilter01"),
            String ("SELECT * FROM IndicationStressTestClass"),
            qlang);
    }
    catch (Exception & e)
    {
        cerr << "----- setup 1 failed: " << e.getMessage () << endl;
    }

    try
    {
        // Create the handler for the internal consumer
        _createHandlerInstance (client, String ("IPHandler01"),
            String ("localhost/CIMListener/Pegasus_SimpleDisplayConsumer"));
    }
    catch (Exception & e)
    {
        cerr << "----- setup 2 failed: " << e.getMessage () << endl;
    }

    try
    {
        // Create the handler with this program as the CIMListener
        _createHandlerInstance (client, String ("IPHandler02"),
            String ("http://localhost:2005/TestIndicationStressTest"));
    }
    catch (Exception & e)
    {
        cerr << "----- setup 3 failed: " << e.getMessage () << endl;
    }

    String filterPathString;
    filterPathString.append ("CIM_IndicationFilter.CreationClassName=\"CIM_IndicationFilter\",Name=\"IPFilter01\",SystemCreationClassName=\"");
    filterPathString.append (System::getSystemCreationClassName ());
    filterPathString.append ("\",SystemName=\"");
    filterPathString.append (System::getFullyQualifiedHostName ());
    filterPathString.append ("\"");

    String handlerPathString01;
    handlerPathString01.append ("CIM_IndicationHandlerCIMXML.CreationClassName=\"CIM_IndicationHandlerCIMXML\",Name=\"IPHandler01\",SystemCreationClassName=\"");
    handlerPathString01.append (System::getSystemCreationClassName ());
    handlerPathString01.append ("\",SystemName=\"");
    handlerPathString01.append (System::getFullyQualifiedHostName ());
    handlerPathString01.append ("\"");

    String handlerPathString02;
    handlerPathString02.append ("CIM_IndicationHandlerCIMXML.CreationClassName=\"CIM_IndicationHandlerCIMXML\",Name=\"IPHandler02\",SystemCreationClassName=\"");
    handlerPathString02.append (System::getSystemCreationClassName ());
    handlerPathString02.append ("\",SystemName=\"");
    handlerPathString02.append (System::getFullyQualifiedHostName ());
    handlerPathString02.append ("\"");

    try
    {
        _createSubscriptionInstance (client, CIMObjectPath (filterPathString),
            CIMObjectPath (handlerPathString01));
    }
    catch (Exception & e)
    {
        cerr << "----- setup 4 failed: " << e.getMessage () << endl;
    }

    try
    {
        _createSubscriptionInstance (client, CIMObjectPath (filterPathString),
            CIMObjectPath (handlerPathString02));
    }
    catch (Exception & e)
    {
        cerr << "----- setup 5 failed: " << e.getMessage () << endl;
    }
}

void _sendNormal(CIMClient* client, Uint32 indicationSendCount)
{
    try
    {
        _sendTestIndicationNormal(client, indicationSendCount);
    }
    catch (Exception & e)
    {
        cerr << "----- sendNormal failed: " << e.getMessage () << endl;
        exit (-1);
    }

//  cout << "+++++ sendNormal completed successfully" << endl;
}

void _cleanup (CIMClient & client)
{
    try
    {
        _deleteSubscriptionInstance (client, String ("IPFilter01"),
            String ("IPHandler01"));
    }
    catch (Exception & e)
    {
        cerr << "----- cleanup 1 failed: " << e.getMessage () << endl;
    }

    try
    {
        _deleteSubscriptionInstance (client, String ("IPFilter01"),
            String ("IPHandler02"));
    }
    catch (Exception & e)
    {
        cerr << "----- cleanup 2 failed: " << e.getMessage () << endl;
    }

    try
    {
        _deleteFilterInstance (client, String ("IPFilter01"));
    }
    catch (Exception & e)
    {
        cerr << "----- cleanup 3 failed: " << e.getMessage () << endl;
    }

    try
    {
        _deleteHandlerInstance (client, String ("IPHandler01"));
    }
    catch (Exception & e)
    {
        cerr << "----- cleanup 4 failed: " << e.getMessage () << endl;
    }

    try
    {
        _deleteHandlerInstance (client, String ("IPHandler02"));
    }
    catch (Exception & e)
    {
        cerr << "----- cleanup 5 failed: " << e.getMessage () << endl;
    }
}

/* Status display of the various steps.  Shows message of function and
time to execute.  Grow this to a class so we have start and stop and time
display with success/failure for each function.
*/
static void _testStart(const String& uniqueID, const String& message)
{
    cout << "+++++ thread" << uniqueID << ": " << message << endl;
}

static void _testEnd(const String& uniqueID, const double elapsedTime)
{
    cout << "+++++ thread" << uniqueID << ": passed in " << elapsedTime << " seconds" << endl;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _executeTests(void *parm)
{
    Thread *my_thread = (Thread *)parm;
    T_Parms *parms = (T_Parms *)my_thread->get_parm();
    CIMClient *client = parms->client.get();
    Uint32 indicationSendCount = parms->indicationSendCount;
    Uint32 id = parms->uniqueID;
    char id_[4];
    memset(id_,0x00,sizeof(id_));
    sprintf(id_,"%i",id);
    String uniqueID = "_";
    uniqueID.append(id_);

    try
    {
        Stopwatch elapsedTime;

        _testStart(uniqueID, "Calling client->invokeMethod to start indication generation");
        elapsedTime.reset();
        elapsedTime.start();
        _sendNormal(client, indicationSendCount);
        elapsedTime.stop();
        _testEnd(uniqueID, elapsedTime.getElapsed());
    }
    catch(Exception e)
    {
        cout << e.getMessage() << endl;
    }
    my_thread->exit_self((PEGASUS_THREAD_RETURN)5);
    return(0);
}

Thread * _runTestThreads(
    CIMClient* client,
    Uint32 indicationSendCount,
    Uint32 uniqueID)
{
    // package parameters, create thread and run...
    AutoPtr<T_Parms> parms(new T_Parms());
    parms->client.reset(client);
    parms->indicationSendCount = indicationSendCount;
    parms->uniqueID = uniqueID;
    AutoPtr<Thread> t(new Thread(_executeTests, (void*)parms.release(), false));

    // zzzzz... (1 second) zzzzz...
    pegasus_sleep(1000);
    t->run();
    return t.release();
}

int _beginTest(CIMClient& workClient, const char* opt, const char* optTwo, const char* optThree)
{
    if (String::equalNoCase(opt, "setup"))
    {
        if ((optTwo == NULL) ||
            (!(String::equal(optTwo, "WQL") ||
               String::equal(optTwo, "CIM:CQL"))))
        {
            cerr << "Invalid query language: '" << optTwo << "'" << endl;
            _usage();
            return -1;
        }
        String qlang(optTwo);
        _setup(workClient, qlang);
        cout << "+++++ setup completed successfully" << endl;
    }
    else if (String::equalNoCase(opt, "run"))
    {
        if (optTwo == NULL)
        {
            cerr << "Invalid indicationSendCount." << endl;
            _usage ();
            return -1;
        }
        Uint32 indicationSendCount = atoi(optTwo);

        Uint32 runClientThreadCount = 1;
        if (optThree != NULL)
        {
            runClientThreadCount = atoi(optThree);
        }

        //
        //  Remove previous indication log file, if there
        //
        String previousIndicationFile, oldIndicationFile;

        previousIndicationFile = INDICATION_DIR;
        previousIndicationFile.append ("/indicationLog");

        if (FileSystem::exists (previousIndicationFile))
        {
            oldIndicationFile = INDICATION_DIR;
            oldIndicationFile.append ("/oldIndicationFile");
            if (FileSystem::exists (oldIndicationFile))
            {
                FileSystem::removeFile (oldIndicationFile);
            }
            if (!FileSystem::renameFile (previousIndicationFile, oldIndicationFile))
            {
                FileSystem::removeFile (previousIndicationFile);
            }
        }

        // Construct our CIMListener

        Uint32 portNumber = 2005;

        CIMListener listener(portNumber);

        // Add our consumer
        MyIndicationConsumer* consumer1 = new MyIndicationConsumer("1");
        listener.addConsumer(consumer1);

        // Finish starting the CIMListener
        try
        {
            cout << "+++++ Starting the CIMListener at destination"
                 << " http://localhost:2005/TestIndicationStressTest" << endl;

            // Start the listener
            listener.start();
        }
        catch (BindFailedException & bfe)
        {
            // Got a bind error.  The port is probably already in use.
            // Put out a message and fail.
            cerr << endl << "==>WARNING: unable to bind to listener port 2005" << endl;
            cerr << "The listener port may be in use." << endl;
            throw;
        }

        Array<CIMClient *> clientConnections;

        CIMClient * tmpClient;
        for(Uint32 i = 0; i < runClientThreadCount; i++)
        {
            tmpClient = new CIMClient();
            clientConnections.append(tmpClient);
        }
        // determine total number of indication send count
        indicationSendCountTotal = indicationSendCount * runClientThreadCount;


        // calculate the timeout based on the total send count allowing
        // using the MSG_PER_SEC rate 
        // allow 20 seconds of test overhead for very small tests 

#define MSG_PER_SEC 4

        Uint32 testTimeout = 20000+(indicationSendCountTotal/MSG_PER_SEC)*1000;
        cout << "++++ Estimated test duration = " <<
          testTimeout/60000 << " minutes." << endl;

        // connect the clients
        for(Uint32 i = 0; i < runClientThreadCount; i++)
        {
            clientConnections[i]->setTimeout(testTimeout);
            clientConnections[i]->connectLocal();
        }

        // run tests
        Array<Thread *> clientThreads;

        Stopwatch elapsedTime;
        elapsedTime.reset();
        elapsedTime.start();

        for(Uint32 i = 0; i < clientConnections.size(); i++)
        {
            clientThreads.append(_runTestThreads(clientConnections[i], indicationSendCount, i));
        }

        for(Uint32 i=0; i< clientThreads.size(); i++)
        {
            clientThreads[i]->join();
        }

        // clean up
        for(Uint32 i=0; i< clientConnections.size(); i++)
        {
            if(clientConnections[i])
                delete clientConnections[i];
        }
        for(Uint32 i=0; i < clientThreads.size(); i++)
        {
            if(clientThreads[i])
                delete clientThreads[i];
        }

        //
        //  Allow time for the indication to be received and forwarded
        //  Wait in SLEEP_SEC second intervals.
        //  Put msg out every MSG_SEC intervals
        //

#define SLEEP_SEC 1
#define MSG_SEC 30

        Uint32 sleep_nbr = 30 +
	    indicationSendCountTotal/(MSG_PER_SEC*SLEEP_SEC);

        // cout << "+++++ sleep_iterations = " << sleep_nbr << endl;

        for (Uint32 i = 1; i <= sleep_nbr; i++)
        {
            System::sleep (SLEEP_SEC);
            if (indicationSendCountTotal == receivedIndicationCount.value())
                break;
            if (i % (MSG_SEC/SLEEP_SEC) == 1)
              cout << "+++++     received indications = " 
                   << receivedIndicationCount.value() 
                   << " of " << indicationSendCountTotal 
                   << " sent, waiting for more ...." << endl;
        }
        elapsedTime.stop();

        cout << "+++++ Stopping the listener"  << endl;
        listener.stop();
        listener.removeConsumer(consumer1);
        delete consumer1;
        cout << "+++++ TEST RESULTS: " << endl;
        cout << "+++++     Number of send threads =    "
             << runClientThreadCount << endl;
        cout << "+++++     Sent indications =          " 
             << indicationSendCountTotal << endl;
        cout << "+++++     Received indications =      " 
             << receivedIndicationCount.value() << endl;
        cout << "+++++     Out of Sequence =           "
             << seqNumberErrors.value() << endl;
        cout << "+++++     Avg. Send-Recv Delta time = "
             << (long)(sendRecvDeltaTimeTotal/1000)/sendRecvDeltaTimeCnt
             << " milli-seconds" << endl;
        cout << "+++++     Min. Send-Recv Delta time = "
             << sendRecvDeltaTimeMin/1000
             << " milli-seconds" << endl;
        cout << "+++++     Max. Send-Recv Delta time = "
             << sendRecvDeltaTimeMax/1000
             << " milli-seconds" << endl;
        cout << "+++++     Elapsed time =              "
             << elapsedTime.getElapsed()
             << " seconds, or  " 
             << elapsedTime.getElapsed()/60
             << " minutes." << endl;
        cout << "+++++     Rate =                      " 
             << receivedIndicationCount.value()/elapsedTime.getElapsed()
             << " indications per second." << endl;

        // assert that all indications sent have been received.
        assert((indicationSendCount * runClientThreadCount) == receivedIndicationCount.value());

        // if error encountered then fail the test.
        if (errorsEncountered.value())
          {
          cout << "+++++ test failed" << endl;
          return (-1);
          }
        else
          {
          cout << "+++++ passed all tests" << endl;
          }
        
    }
    else if (String::equalNoCase (opt, "cleanup"))
    {
        _cleanup (workClient);
        cout << "+++++ cleanup completed successfully" << endl;
    }
    else
    {
      cerr << "Invalid option: " << opt << endl;
      _usage ();
      return -1;
    }

  return 0;
}

int main (int argc, char** argv)
{
    // This client connection is used soley to create subscriptions.
    CIMClient workClient;
    try
    {
        workClient.connectLocal();
    }
    catch (Exception & e)
    {
        cerr << e.getMessage () << endl;
        return -1;
    }

    if (argc <= 1)
    {
        cerr << "Invalid argument count: " << argc << endl;
        _usage();
        return 1;
    }
    else
    {
        const char * opt = argv[1];
        const char * optTwo;
        const char * optThree;

        if (argc == 4) {
            optTwo = argv[2];
            optThree = argv[3];
        }
        else if (argc == 3) {
            optTwo = argv[2];
            optThree = NULL;
        }
        else {
            optTwo = NULL;
            optThree = NULL;
        }

        int rc = _beginTest(workClient, opt, optTwo, optThree);

	return rc;
    }

    PEGASUS_UNREACHABLE( return 0; )
}
