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
// Author: Yi Zhou, Hewlett-Packard Company (Yi.Zhou@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/PegasusAssert.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/HandlerService/HandlerTable.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Config/ConfigManager.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE("TestSnmpHandler");

const CIMNamespaceName NS = CIMNamespaceName ("TestSnmpHandler");

const CIMName testClass1 = CIMName ("SnmpTestClass1");
const CIMName testClass2 = CIMName ("SnmpTestClass2");
const CIMName testClass3 = CIMName ("SnmpTestClass3");
const CIMName testClass4 = CIMName ("SnmpTestClass4");
const CIMName testClass5 = CIMName ("SnmpTestClass5");
const CIMName testClass6 = CIMName ("SnmpTestClass6");
const CIMName testClass7 = CIMName ("SnmpTestClass7");

String repositoryRoot;

static CIMInstance CreateHandlerInstance()
{
    CIMInstance handlerInstance(PEGASUS_CLASSNAME_INDHANDLER_SNMP);
    handlerInstance.addProperty (CIMProperty (CIMName
        ("SystemCreationClassName"), System::getSystemCreationClassName ()));
    handlerInstance.addProperty (CIMProperty (CIMName ("SystemName"),
        System::getFullyQualifiedHostName ()));
    handlerInstance.addProperty (CIMProperty (CIMName ("CreationClassName"),
        PEGASUS_CLASSNAME_INDHANDLER_SNMP.getString ()));
    handlerInstance.addProperty(CIMProperty(CIMName ("Name"),
        String("Handler1")));
    return (handlerInstance);
}

static void TestException(
    CIMHandler* handler,
    CIMInstance indicationHandlerInstance,
    CIMInstance indicationInstance,
    CIMStatusCode statusCode)
{
    OperationContext context;
    CIMInstance indicationSubscriptionInstance;
    ContentLanguageList contentLanguages;

    Boolean exceptionCaught = false;
    CIMException testException;

    try
    {
        handler->handleIndication(context, NAMESPACE, indicationInstance,
            indicationHandlerInstance, indicationSubscriptionInstance,
            contentLanguages);
    }
    catch (CIMException& e)
    {
        exceptionCaught = true;
        testException = e; 
    }

    PEGASUS_TEST_ASSERT(exceptionCaught && 
        testException.getCode() == statusCode);
}

// Snmp traps are sent, but, only partial data are in the trap since 
// there are errors in some data
static void TestError(
    CIMHandler* handler,
    CIMInstance indicationHandlerInstance,
    CIMInstance indicationInstance)
{
    OperationContext context;
    CIMInstance indicationSubscriptionInstance;
    ContentLanguageList contentLanguages;

    Boolean exceptionCaught = false;

    try
    {
        handler->handleIndication(context, NAMESPACE, indicationInstance,
            indicationHandlerInstance, indicationSubscriptionInstance,
            contentLanguages);
    }
    catch (CIMException& e)
    {
        exceptionCaught = true;
    }

    PEGASUS_TEST_ASSERT(!exceptionCaught);
}

static void CreateRepository(CIMRepository & repository)
{
    repository.createNameSpace(NS);

    CIMQualifierDecl q1(CIMName ("MappingStrings"), String(), 
        CIMScope::PROPERTY + CIMScope::CLASS);

    // Qualifier name must be "MappingStrings", test the qualifier
    // name is not "MappingStrings"
    CIMQualifierDecl q2(CIMName ("NotMappingStrings"), String(), 
        CIMScope::CLASS);

    repository.setQualifier(NS, q1);
    repository.setQualifier(NS, q2);

    String mappingStr = "OID.IETF | SNMP.1.3.6.1.4.1.892.2.3.9000.8600";

    CIMClass class1(testClass1);
    class1.addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
        String(mappingStr)));
    
    // create wrong format property mappingStrings value
    class1.addProperty(CIMProperty(CIMName ("OidDataType"), String("OctetString"))
        .addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
            String("Wrong format OID.IETF | SNMP.1.3.6.1.4.1.2.3.9000.8600, DataType.IETF | OctetString "))));

    repository.createClass(NS, class1);

    // create invalid mapping string value
    String mappingStr2 = "OID.IETF |Invalid Mapping String Value";

    CIMClass class2(testClass2);
    class2.addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
        String(mappingStr2)));
    
    class2.addProperty(CIMProperty(CIMName ("OidDataType"), String())
        .addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
            String("OID.IETF | SNMP.1.3.6.1.4.1.2.3.9000.8600, DataType.IETF OctetString "))));
    repository.createClass(NS, class2);

    // create non MappingStrings qualifier
    CIMClass class3(testClass3);
    class3.addQualifier(CIMQualifier(CIMName ("NotMappingStrings"), String(mappingStr)));
    
    repository.createClass(NS, class3);

    // error building ASN.1 representation
    String mappingStr4 = "OID.IETF | SNMP.1.204.6.1.6.3.1.330.5.1.0 ";

    CIMClass class4(testClass4);
    class4.addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
    String(mappingStr4)));
    
    repository.createClass(NS, class4);

    // create incorrect class mappingStrings value
    String mappingStr5 = "OID.IETF | SNMP.1.3.6.1.6.test.1.1.5.1.3 ";

    CIMClass class5(testClass5);
    class5.addQualifier(CIMQualifier(CIMName 
        ("MappingStrings"), String(mappingStr5)));

    // create incorrect property name
    class5.addProperty(
        CIMProperty(CIMName ("WrongPropertyName"), String("OctetString"))
            .addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
                String(mappingStr5))));
    
    repository.createClass(NS, class5);

    // create incorrect property mappingStrings value
    String mappingStr6 = 
       "OID.IETF | SNMP.1.3.6.1.6.test.1.1.5.1.3, DataType.IETF | OctetString ";

    CIMClass class6(testClass6);
    class6.addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
        String("OID.IETF | SNMP.1.3.6.1.6.3.1.1.0.1 ")));
    class6.addProperty(
        CIMProperty(CIMName ("OidDataType"), String("OctetString"))
            .addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
                String(mappingStr6))));
    
    repository.createClass(NS, class6);

    // create unsupportted SNMP Data Type for the CIM property
    String mappingStr7 = 
        "OID.IETF | SNMP.1.3.6.1.6.test.1.1.5.1.3, DataType.IETF | test ";

    CIMClass class7(testClass7);
    class7.addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
        String("OID.IETF | SNMP.1.3.6.1.6.3.1.1.5.1 ")));
    class7.addProperty(
        CIMProperty(CIMName ("OidDataType"), String("test"))
            .addQualifier(CIMQualifier(CIMName ("MappingStrings"), 
                String(mappingStr7))));
    
    repository.createClass(NS, class7);
}

static void TestExceptionHandling(CIMHandler* handler)
{
    CIMInstance indicationHandlerInstance;

    // Test "invalid IndicationHandlerSNMPMapper instance" exception
    CIMInstance indicationInstance(testClass1);
    indicationHandlerInstance = CreateHandlerInstance();
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "no required property TargetHostFormat" exception
    indicationInstance = CIMInstance(testClass1);
    indicationInstance.addProperty(CIMProperty(
       CIMName ("OidDataType"), String("OctetString")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "no required property SNMPVersion" exception
    indicationInstance = CIMInstance(testClass1);
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "unsupported snmp Version" exception
    indicationInstance = CIMInstance(testClass1);
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(5)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "SNMP session open failed" exception
    indicationInstance = CIMInstance(testClass1);
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("OtherTargetHostFormat"), String("testOtherTargetHostFormat")));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("PortNumber"), Uint32(200)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "invalid MappingStrings value" exception
    indicationInstance = CIMInstance(testClass2);
    indicationInstance.addProperty(CIMProperty(
        CIMName ("OidDataType"), String("OctetString")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "no MappingStrings qualifier" exception
    indicationInstance = CIMInstance(testClass3);
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "send trap failed" exception
    indicationInstance = CIMInstance(testClass4);
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPSecurityName"), String("t")));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("OtherTargetHostFormat"), String("testOtherTargetHostFormat")));
    TestException(handler, indicationHandlerInstance, indicationInstance,
        CIM_ERR_FAILED);

    // Test "failed to add snmp variables to PDU", 
    // Both a DiscardedData message and an error message
    // are logged to log file 
    indicationInstance = CIMInstance(testClass5);
    indicationInstance.addProperty(CIMProperty(
        CIMName ("OidDataType"), String("OctetString")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPSecurityName"), String("t")));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("OtherTargetHostFormat"), String("testOtherTargetHostFormat")));
    TestError(handler, indicationHandlerInstance, indicationInstance);

    // Test "convert enterprise OID from numeric form to a list of"
    // "subidentifiers failed".
    // Both a DiscardedData message and an error message
    // are logged to log file 
    indicationInstance = CIMInstance(testClass5);
    indicationInstance.addProperty(CIMProperty(
        CIMName ("OidDataType"), String("OctetString")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPSecurityName"), String("t")));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("OtherTargetHostFormat"), String("testOtherTargetHostFormat")));
    TestError(handler, indicationHandlerInstance, indicationInstance);

    // Test "convert property OID from numeric form to a list of"
    // "subidentifiers failed".
    // Both a DiscardedData message and an error message
    // are logged to log file 
    indicationInstance = CIMInstance(testClass6);
    indicationInstance.addProperty(CIMProperty(
        CIMName ("OidDataType"), String("OctetString")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("OtherTargetHostFormat"), String("testOtherTargetHostFormat")));
    TestError(handler, indicationHandlerInstance, indicationInstance);

    // Test "unsupported SNMP data type for the CIM property"
    // Both a DiscardedData message and an error message
    // are logged to log file 
    indicationInstance = CIMInstance(testClass7);
    indicationInstance.addProperty(CIMProperty(
        CIMName ("OidDataType"), String("test")));
    indicationHandlerInstance = CreateHandlerInstance();
    indicationHandlerInstance.addProperty(CIMProperty(
       CIMName("TargetHost"), String("15.13.140.120")));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("TargetHostFormat"), Uint16(3)));
    indicationHandlerInstance.addProperty(CIMProperty(
        CIMName("SNMPVersion"), Uint16(2)));
    TestError(handler, indicationHandlerInstance, indicationInstance);
}


int main(int argc, char** argv)
{

    const char* pegasusHome = getenv("PEGASUS_HOME");
    if (!pegasusHome)
    {
        cerr << "PEGASUS_HOME environment variable not set" << endl;
        exit(1);
    }

    String repositoryRoot = pegasusHome;
    repositoryRoot.append("/repository");

    CIMRepository_Mode mode;
    mode.flag = CIMRepository_Mode::BIN;
    CIMRepository* repository = new CIMRepository(repositoryRoot, mode);

    ConfigManager::setPegasusHome(pegasusHome);

    // -- Create repository and namespaces:

    CreateRepository(*repository);

    try
    {
        HandlerTable handlerTable;
        String handlerId = "snmpIndicationHandler";
        CIMHandler* handler = handlerTable.getHandler(handlerId, repository);
	PEGASUS_TEST_ASSERT(handler != 0);

        handler->initialize(repository);
        TestExceptionHandling(handler);

        //
        // -- Clean up classes:
        //
        repository->deleteClass(NS, testClass1);
        repository->deleteClass(NS, testClass2);
        repository->deleteClass(NS, testClass3);
        repository->deleteClass(NS, testClass4);
        repository->deleteClass(NS, testClass5);
        repository->deleteClass(NS, testClass6);
        repository->deleteClass(NS, testClass7);

        //
        // -- Delete the qualifier:
        //
        repository->deleteQualifier(NS, CIMName ("MappingStrings"));
        repository->deleteQualifier(NS, CIMName ("NotMappingStrings"));

        repository->deleteNameSpace(NS);

	delete repository;
    }
    catch(Exception& e)
    {
	PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	exit(1);
    }

    cout << "+++++ passed all tests" << endl;
    return 0;
}
