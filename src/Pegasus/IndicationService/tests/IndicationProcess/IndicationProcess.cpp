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
// Author: Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/System.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const CIMNamespaceName NAMESPACE = CIMNamespaceName ("root/PG_InterOp");
const CIMNamespaceName NAMESPACE1 = CIMNamespaceName ("root/SampleProvider");
const CIMNamespaceName NAMESPACE2 = CIMNamespaceName ("root/cimv2");
const CIMNamespaceName NAMESPACE3 = CIMNamespaceName ("test/TestProvider");

CIMObjectPath CreateHandler1Instance (CIMClient& client,
    const CIMNamespaceName & handlerNS)
{
    CIMInstance handlerInstance(PEGASUS_CLASSNAME_INDHANDLER_CIMXML);
    handlerInstance.addProperty(CIMProperty (CIMName("SystemCreationClassName"),
        System::getSystemCreationClassName ()));
    handlerInstance.addProperty(CIMProperty(CIMName ("SystemName"),
        System::getFullyQualifiedHostName ()));
    handlerInstance.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_CIMXML.getString()));
    handlerInstance.addProperty(CIMProperty(CIMName ("Name"),
        String("Handler1")));
    handlerInstance.addProperty(CIMProperty(CIMName ("Destination"),
        String("localhost/CIMListener/Pegasus_SimpleDisplayConsumer")));

    CIMObjectPath Ref = client.createInstance(handlerNS, handlerInstance);
    Ref.setNameSpace (handlerNS);
    return (Ref);
}

CIMObjectPath CreateFilterInstance (CIMClient& client,
                                    const String query,
                                    const String qlang,
                                    const String name,
                                    const CIMNamespaceName & filterNS)
{
    CIMInstance filterInstance(PEGASUS_CLASSNAME_INDFILTER);
    filterInstance.addProperty(CIMProperty (CIMName ("SystemCreationClassName"),
        System::getSystemCreationClassName ()));
    filterInstance.addProperty(CIMProperty(CIMName ("SystemName"),
        System::getFullyQualifiedHostName ()));
    filterInstance.addProperty(CIMProperty(CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDFILTER.getString()));
    filterInstance.addProperty(CIMProperty(CIMName ("Name"),
        String(name)));
    filterInstance.addProperty (CIMProperty(CIMName ("Query"),
        String(query)));
    filterInstance.addProperty (CIMProperty(CIMName ("QueryLanguage"),
        String(qlang)));
    filterInstance.addProperty (CIMProperty(CIMName ("SourceNamespace"),
        String("root/SampleProvider")));

    CIMObjectPath Ref = client.createInstance(filterNS, filterInstance);
    Ref.setNameSpace (filterNS);
    return (Ref);
}

CIMObjectPath CreateSbscriptionInstance (CIMClient& client,
    const CIMObjectPath handlerRef, 
    const CIMObjectPath filterRef,
    const CIMNamespaceName & subscriptionNS)
{
    CIMInstance subscriptionInstance
        (PEGASUS_CLASSNAME_INDSUBSCRIPTION);
    subscriptionInstance.addProperty(CIMProperty(CIMName ("Filter"),
        filterRef, 0, PEGASUS_CLASSNAME_INDFILTER));
    subscriptionInstance.addProperty(CIMProperty(CIMName ("Handler"),
        handlerRef, 0, PEGASUS_CLASSNAME_INDHANDLER_CIMXML));
    subscriptionInstance.addProperty (CIMProperty
        (CIMName ("SubscriptionState"), CIMValue ((Uint16) 2)));

    CIMObjectPath Ref = client.createInstance(subscriptionNS,
        subscriptionInstance);
    Ref.setNameSpace (subscriptionNS);
    return (Ref);
}

void generateIndication(CIMClient& client)
{
    CIMInstance indicationInstance (CIMName("RT_TestIndication"));

    CIMObjectPath path ;
    path.setNameSpace("root/SampleProvider");
    path.setClassName("RT_TestIndication");

    indicationInstance.setPath(path);

    Array<CIMParamValue> inParams;
    Array<CIMParamValue> outParams;

    CIMValue ret_value = client.invokeMethod(
	"root/SampleProvider",
        path,
	"SendTestIndication",
        inParams,
	outParams);
}

void DeleteInstance (CIMClient& client, const CIMObjectPath Ref,
    const CIMNamespaceName & instanceNS)
{
    client.deleteInstance(instanceNS, Ref);
}

int _test(CIMClient& client, String& qlang, String& query1, String& query2)
{
    CIMObjectPath Handler1Ref, Handler2Ref; 
    CIMObjectPath Filter1Ref, Filter2Ref;
    CIMObjectPath Subscription1Ref, Subscription2Ref;

    try
    {
        Handler1Ref = CreateHandler1Instance (client, NAMESPACE);
        Handler2Ref = CreateHandler1Instance (client, NAMESPACE2);
    }
    catch (Exception& e)
    {
        PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                           << PEGASUS_STD (endl);
        PEGASUS_STD (cerr) << "create handler instance failed" 
                           << PEGASUS_STD (endl);
        return -1;
    }

    PEGASUS_STD (cout) << "+++++ handler instances created" 
                       << PEGASUS_STD (endl);
    try
    {
      String name1 = "TestFilter01";
      String name2 = "TestFilter02";
      Filter1Ref = CreateFilterInstance (client, query1, qlang, name1,
          NAMESPACE);
      Filter2Ref = CreateFilterInstance (client, query2, qlang, name2,
          NAMESPACE1);
    }
    catch (Exception& e)
    {
        PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                           << PEGASUS_STD (endl);
        PEGASUS_STD (cerr) << "create filter instances failed" 
                           << PEGASUS_STD (endl);
        return -1;
    }

    PEGASUS_STD (cout) << "+++++ filter instances created for "
                       << qlang 
                       << PEGASUS_STD (endl);
    try
    {
        Subscription1Ref = 
          CreateSbscriptionInstance (client, Handler1Ref, Filter1Ref,
              NAMESPACE);
        Subscription2Ref = 
          CreateSbscriptionInstance (client, Handler1Ref, Filter2Ref,
              NAMESPACE3);
    }
    catch (Exception& e)
    {
        PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                           << PEGASUS_STD (endl);
        PEGASUS_STD (cerr) << "create subscription instance failed" 
                           << PEGASUS_STD (endl);
        return -1;
    }

    PEGASUS_STD (cout) << "+++++ subscription instances created" 
                       << PEGASUS_STD (endl);

    // get display consumer
    String indicationFile2, oldIndicationFile;

    indicationFile2 = INDICATION_DIR;
    indicationFile2.append("/indicationLog"); 

    if (FileSystem::exists(indicationFile2))
    {
        oldIndicationFile = INDICATION_DIR;
        oldIndicationFile.append("/oldIndicationFile");
        if (FileSystem::exists(oldIndicationFile))
        {
          FileSystem::removeFile(oldIndicationFile);
        }
        if (!FileSystem::renameFile(indicationFile2, oldIndicationFile))
        {
          FileSystem::removeFile(indicationFile2);
        }
    }

    try
    {
      //
      // generate three indications
      //
      for(Uint8 i = 0; i < 3; i++)
      {
        generateIndication(client);
      }
    }
    catch (Exception& e)
    {
      PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                         << PEGASUS_STD (endl);
      PEGASUS_STD (cerr) << "generate indication failed" 
                         << PEGASUS_STD (endl);
      return -1;
    }

    PEGASUS_STD (cout) << "+++++ indications generated" 
                       << PEGASUS_STD (endl);

    System::sleep(5);

    try
    {
      DeleteInstance (client, Subscription1Ref, NAMESPACE);
      DeleteInstance (client, Subscription2Ref, NAMESPACE3);
      DeleteInstance (client, Filter1Ref, NAMESPACE);
      DeleteInstance (client, Filter2Ref, NAMESPACE1);
      DeleteInstance (client, Handler1Ref, NAMESPACE);
      DeleteInstance (client, Handler2Ref, NAMESPACE2);
    }
    catch (Exception& e)
    {
      PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                         << PEGASUS_STD (endl);
      PEGASUS_STD (cerr) << "delete instance failed" 
                         << PEGASUS_STD (endl);
      return -1;
    }

    PEGASUS_STD (cout) << "+++++ instances deleted" 
                       << PEGASUS_STD (endl);

    //
    // compare indications with the master output
    //
    String indicationFile, masterFile, indication_failed;

    indicationFile = INDICATION_DIR;
    indicationFile.append("/indicationLog"); 

    // Get environment variable:
    masterFile = getenv("PEGASUS_ROOT");
    masterFile.append("/src/Pegasus/IndicationService/tests/IndicationProcess/masterOutput");

    if (FileSystem::exists(indicationFile) && FileSystem::exists(masterFile))
    {
      if (FileSystem::compareFiles(indicationFile, masterFile))
      {
        PEGASUS_STD (cout) << "+++++ passed all tests." << PEGASUS_STD (endl);
        // remove indicationFile
        FileSystem::removeFile(indicationFile);
        return 0;
      }
      else
      {
        PEGASUS_STD (cerr) << "----- tests failed" << PEGASUS_STD (endl);
        // rename indicationFile to be indicationLog_FAILED 
        // and remove indicationFile
        indication_failed = INDICATION_DIR;
        indication_failed.append("/indicationLog_FAILED");
        FileSystem::renameFile(indicationFile, indication_failed);
        FileSystem::removeFile(indicationFile);
        return 1;
      }
    }
    else
    {
      PEGASUS_STD (cerr) << "----- tests failed" << PEGASUS_STD (endl);
      // rename indicationFile to be indicationFile_FAILED 
      // and remove indicationFile
      if (FileSystem::exists(indicationFile))
      {
        indication_failed = INDICATION_DIR;
        indication_failed.append("/indicationLog_FAILED");
        FileSystem::renameFile(indicationFile, indication_failed);
        FileSystem::removeFile(indicationFile);
      }
      return 1;
    }
}

int main(int argc, char** argv)
{
    CIMClient client;
    try
    {
      client.connectLocal();
    }
    catch (Exception& e)
    {
      PEGASUS_STD (cerr) << "Exception: " << e.getMessage () 
                         << PEGASUS_STD (endl);
      PEGASUS_STD (cerr) << "connectLocal failed" 
                         << PEGASUS_STD (endl);
      return -1;
    }

    String query1="SELECT MethodName FROM rt_testindication";

    // Note that CQL expects single quote around string literals, 
    // while WQL expects double quote.
    // Note that CQL use <> for the inequality operator.
    String query2wql="SELECT MethodName FROM RT_TestIndication WHERE IndicationIdentifier != \"x\"";
    String query2cql="SELECT MethodName FROM RT_TestIndication WHERE IndicationIdentifier <> 'x'";
    
    String wql("WQL");
    String cql("CIM:CQL");

    PEGASUS_STD (cout) << "+++++ start wql test" << PEGASUS_STD (endl);
    int rc;
    rc = _test(client, wql, query1, query2wql);
    if (rc != 0)
      return rc;

#ifndef PEGASUS_DISABLE_CQL  
    PEGASUS_STD (cout) << "+++++ start cql test" << PEGASUS_STD (endl);
    return _test(client, cql, query1, query2cql);      
#else
    PEGASUS_STD (cout) << "+++++ cql test disabled" << PEGASUS_STD (endl); 
    return 0;
#endif
}
