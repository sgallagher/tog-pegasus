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
// Authors: Alagaraja Ramasubramanian, IBM Corporation 
//          Seema Gupta, IBM Corporation
//          Subodh Soni, IBM Corporation
//
// Modified By: Karl Schopmeyer, (k.schopmeyer@opengroup.org) 
//
//%/////////////////////////////////////////////////////////////////////////////

/* Description: this provider manages the SLP advertisements for Pegasus.  It gets 
   information from other sources within the Pegasus environment (primarily the
   Interop Classes such as CIM_ObjectManager, CIM_ObjectManagerCommunicationMechanism,
   CIM_Namespaces, etc. and generates the corresponding registrations.
   It also maintains a single Class, PG_WBEMSlpTemplate that defines what is in the
   template.
   NOTE: This provider should NEVER be getting information itself but simply using
   other classes in the environment to generate information.
   The intrinistic operations that exist include:
   enumerateInstances
   enumerateInstanceNames
   getInstance
   
   This module provides an SLP service agent that is activited once the first
   registration is created.  This service agent provides either the normal SA functions
   or provides the registration to a DA if required.  That set of functions in in the
   Pegasus service agent code.
   
   To initiate the SA within pegasus, the system must make a first PEG_WBEMSLPTemplate
   enumerateInstances call to this provider.  That initiates the build of the registration
   and the startup of the SA.
*/

/* TODO
    We can make this more flexible by allowing the following:
    1. method to force reregistration.  Today, once you register it is finished.
    2. createInstance, deleteInstance to allow adding and deleting other
    registrations.
    3. Today this module assumes that we will populate the registration once at
    startup and then it will remain fixed.  It does not provide for dynamic
    re-registration when something changes in the system. KS Fix this. We need the
    following changes (1) dynamic reregistration within this module (2) dynamic
    call from other functions to get initiate the registration when something changes
    within the system (right now, interop changes).
*/

#include "SLPProvider.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include <Pegasus/Common/Constants.h>

#include <set>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/param.h>
//#include <unistd.h>
#include <time.h>
//#include <utmp.h>
//#include <regex.h>
//#include <dirent.h>

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Tracer.h>

//#define CDEBUG(X)
#define CDEBUG(X) PEGASUS_STD(cout) << "SLPProvider " << X << PEGASUS_STD(endl)

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;
//******************************************************************
//
// Constants
//
//*****************************************************************

const char *SlpProvider = "SlpProvider";
const char * SlpTemplateClassName = "PEG_WBEMSLPTemplate";
const char * CIMObjectManagerClassName = "CIM_ObjectManager";
const char * CIMObjectManagerCommMechName = "CIM_ObjectManagerCommunicationMechanism";
const char * CIMNamespaceClassName = "CIM_Namespace";

const char *serviceName = "service:wbem";

// Template attribute name constants

String serviceNameAttribute = serviceName;

String serviceHiDescription = "service_hi_description";
String serviceHiDescriptionAttribute = "service-hi-description";

String serviceHiName = "service_hi_name";
String serviceHiNameAttribute = "service-hi-name";

String serviceUrlSyntax = "template-url-syntax";
String serviceUrlSyntaxProperty = "template_url_syntax";

String serviceIDProperty = "service_id";
String serviceIDAttribute = "service-id";

String serviceLocationTCP = "service-location-tcp";
String serviceLocationTCPProperty = "service_location_tcp";
String templateType = "template-type";
String templateTypeProperty = "template_type";

String templateVersionAttribute = "template-version";
String templateVersionProperty = "template_version";

String templateVersion = "1.0";
String templateDescriptionAttribute = "template-description";
String templateDescriptionProperty = "template_description";
String templateDescription = 
    "This template describes the attributes used for advertising Pegasus CIM Servers.";
String InteropSchemaNamespaceAttribute = "InteropSchemaNamespace";
String InteropSchemaNamespace = "root/PG_Interop";
String otherCommunicationMechanismDescriptionAttribute = "OtherCommunicationMechanismDescription";
String functionalProfilesSupportedAttribute = "FunctionalProfilesSupported";
String functionalProfileDescriptionsAttribute = "FunctionalProfileDescriptions";
String otherProfileDescriptionAttribute = "OtherProfileDescription";
String communicationMechanismAttribute = "CommunicationMechanism";
String otherCommunicationMechanismAttribute = "otherCommunicationMechanism";
String multipleOperationsSupportedAttribute = "MultipleOperationsSupported";
String authenticationMechanismsSupportedAttribute =  "AuthenticationMechanismsSupported";
String otherAuthenticationDescriptionsAttribute = "OtherAuthenticationDescription";
String authenticationMechanismDescriptionsAttribute = "AuthenticationMechanismDescriptions";
String namespaceAttribute = "Namespace";
String classinfoAttribute =  "classinfo";

String protocolVersionAttribute = "ProtocolVersion";


String registeredProfilesSupportedAttribute = "RegisteredProfilesSupported";
String registeredProfilesList = 
"SNIA:Array:Cluster:Access Points:Disk Drive:Location:LUN Mapping \
and Masking:Pool Manipulation Capabilities and Settings:Extent Mapping:LUN Creation:Software";

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "SampleFamilyProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderModule to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
	const int MAXPATHLEN=2000;
#endif

extern "C" PEGASUS_EXPORT CIMProvider * PegasusCreateProvider(const String & name)
{

    CDEBUG("SLPProvider " << name);
    if(String::equalNoCase(name, "SLPProvider") ||
        String::equalNoCase(name, "SLPProvider(PROVIDER)"))
 {
     return(new SLPProvider());
 }

 return(0);
}

//********************************************************************
//
//     Support functions
//
//********************************************************************

String arrayToString(const Array<String>& s)
{
String output;    
for (Uint32 i = 0 ; i < s.size() ; i++)
    {
        if (i > 0)
            output.append(",");
        output.append(s[i]);
    }
    return(output);
}

void appendCSV(String& s, const String& s1)
{
    if (s.size() != 0)
    {
        s.append(",");
    }
    s.append(s1);
}

//*******************************************************************
//
// SLPProvider Protected and private methods
//
//*******************************************************************

/** get Host IP address from host name. If the
    host name is not provided, uses internal function.
    If everything fails, gets the definition normally
    used for localhost (127.0.0.1).
    
    @param hostName String with the name of the host
    @return String with the IP address to be used
    NOTE: This code should not be in slpprovider. This
    should be in the Interop classes but for the moment
    it is not.
*/
String SLPProvider::getHostAddress(String hostName)
{
  PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::getHostAddress()");
  String ipAddress("127.0.0.1");
  
  if (hostName == String::EMPTY)
    {
    	hostName = getHostName();
  	}

  struct hostent * phostent;
  struct in_addr   inaddr;
  
  if ((phostent = ::gethostbyname((const char *)hostName.getCString())) != NULL)
    {
      ::memcpy( &inaddr, phostent->h_addr,4);
      ipAddress = ::inet_ntoa( inaddr );
    }
  PEG_METHOD_EXIT();
  return ipAddress;
} 

/** gets the host name. Note that we really do not need this
    since it is already a system function in Pegasus.
    ATTN: Drop this function
*/

String SLPProvider::getHostName()
{
  PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::getHostName()");
  static char hostname[64];

  if (!*hostname)
    {
      ::gethostname(hostname, sizeof(hostname));
		}

  PEG_METHOD_EXIT();
  return hostname;
}

/** gets the list of registered profiles for the SLP template.
    Until we start using CIM 28, there are no template classes
    and no templates.  We are doing a temporary hack here of
    (1) getting from an environment variable or (2) if this does
    not exist, simply getting from a fixed field in this provider
    @return String containing the list of registered profiles.
    
    ATTN: KS 8 Feb 2004.  Replace this with function that gets
    the registered profiles from the correct classes when those
    classes begin to exist in the environment.
*/

String SLPProvider::getRegisteredProfileList()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::getRegisteredProfileList()");

    char * regList;
    regList = getenv("PEGASUS_REGISTERED_PROFILES");

    PEG_METHOD_EXIT();
    return((regList)? String(regList) : registeredProfilesList);
}

/** get the list of valid namespaces and supporting info.
    Function builds an array of namespace names and a parallel
    array of classinfo information
    @param nameSpace in which to find CIM_namespace class
    @param, classInfo, anArray of strings for classinfo on return
    @return array of namespaces names
    ATTN: KS 5 Feb 2004.Today we get complete namespace list.  We need to modify
    to use the CIM_ObjectManager to CIM_Namespace association to get
    the information.  KS will do this when association added to interop
*/
String SLPProvider::getNameSpaceInfo(const CIMNamespaceName& nameSpace, String& classInfo )
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::getNameSpaceInfo()");

    String names;
    Array<CIMInstance> CIMNamespaceInstances;
    try
    {
    // ATTN: KS In the future get only the fields we want based on property list.
        CIMNamespaceInstances = _ch.enumerateInstances(
                                         OperationContext(),
                                         _interopNamespace,
                                         CIMName(CIMNamespaceClassName),
                                         true, true, true, true,
                                         CIMPropertyList());
}
    catch (exception& e)
    {
        //... catch if we get error here. In particular unsupported class
        return(names);
    }
    // Now extract the namespace names and class info from the objects.
    for (Uint32 i = 0 ; i < CIMNamespaceInstances.size() ; i++)
    {
        Uint32 pos;
        if ((pos = CIMNamespaceInstances[i].findProperty(CIMName("Name"))) != PEG_NOT_FOUND) 
        {
            CIMProperty p1= CIMNamespaceInstances[i].getProperty(pos);
            CIMValue v1=p1.getValue();
            appendCSV(names,v1.toString());

            if ((pos = CIMNamespaceInstances[i].findProperty(CIMName("ClassInfo"))) != PEG_NOT_FOUND)
            {
                CIMProperty p1=CIMNamespaceInstances[i].getProperty(pos);
                CIMValue v1=p1.getValue();
                appendCSV(classInfo, v1.toString());
            }
            else
            {
                appendCSV(classInfo, v1.toString());
            }
        }
        else
            CDEBUG("Must log error here if property not found");
    }
    
    PEG_METHOD_EXIT();
    return(names);
}

/** populate a single field in the template and the corresponding template instance.
    This function assumes that both the instance object and the template object have been
    created already. There is no error test for this at this point.
    It adds a property to the instance with the defined fieldname and value and also
    adds a line to the slp template with the field name and value.
    NOTE: There are two fieldName parameters because there may be different names for 
    these fields in the instance and registration (this is due to the fact that the
    - used in slp templates is illegal in CIMNames.
    
    @param instance CIMInstance being created. This is an instance of WBEMSlpTemplate.
    @param insstanceFieldName String defining the name of the field to populate in the instance.
    @param regfieldName String defining the name of the field to populate in the registration
    @param value String defining the value with which to populate the field
*/

void SLPProvider::populateTemplateField(CIMInstance& instance, 
                                        const String& instanceFieldName, 
                                        const String& regFieldName,
                                        const String& value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::populateTemplateField()");

    // instance1.addProperty(CIMProperty("classinfo",classInfoList[i]));
    // slpTemplateInstance.append("(classinfo=").append(classInfoList[i]).append("),");
    CDEBUG("Populate TemplateField name= " << instanceFieldName << ", " << regFieldName <<". Value= " << value);
    instance.addProperty(CIMProperty(CIMName(instanceFieldName), value));
    if (slpTemplateInstance.size() != 0)
    {
        slpTemplateInstance.append(",\n");
    }
    slpTemplateInstance.append("(");
    slpTemplateInstance.append(regFieldName);
    slpTemplateInstance.append("=");
    slpTemplateInstance.append(value);
    slpTemplateInstance.append(")");
    PEG_METHOD_EXIT();
}

/** populates the SLP template and its corresponding instance. Creates one
    instance of slp registration data, tests the registration and registers
    the information.  The corresponding instance of PG_WBEMSLPTemplate represents
    the data in the registration. It uses largely information from the
    CIM_ObjectManager and its corresponding communication classes plus
    information for namespaces from CIM_Namespace.
    @param protocol String defining http or https
    @param instance_ObgMgr CIMInstance of CIM_ObjectManager
    @param instance-ObjMgrComm CIMInstance of Interop communication class.
    ATTN: In the future, we should populate these separately
*/
void SLPProvider::populateRegistrationData(const String &protocol,
                        const CIMInstance& instance_ObjMgr,
                        const CIMInstance& instance_ObjMgrComm)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::populateRegistrationData()");

   String mynamespace = "root/cimv2";
   _interopNamespace = mynamespace;
   slpTemplateInstance = "";
   Uint32 index=10;
   CDEBUG("populateData from namespace= " << _interopNamespace.getString() );
   CIMInstance instance1(SlpTemplateClassName);

    // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")
    String IPAddress = getHostAddress(getHostName());
      // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")
     // Need to tie these two together.
    Uint32 portNumber;
    if (protocol.find("https") != PEG_NOT_FOUND)
    {
         portNumber = System::lookupPort(WBEM_HTTPS_SERVICE_NAME,
            WBEM_DEFAULT_HTTPS_PORT); 
    }
    else
    {
        portNumber = System::lookupPort(WBEM_HTTP_SERVICE_NAME,
            WBEM_DEFAULT_HTTP_PORT);
    }
    IPAddress.append(":");
    
    char buffer[32];
    sprintf(buffer, "%u", portNumber);
    IPAddress.append(buffer);
    CDEBUG("portNumber" << buffer);

    // template-url-syntax=string
    // #The template-url-syntax MUST be the WBEM URI Mapping of
    // #the location of one service access point offered by the WBEM Server 
    // #over TCP transport. This attribute must provide sufficient addressing 
    // #information so that the WBEM Server can be addressed directly using 
    // #the URL.  The WBEM URI Mapping is defined in the WBEM URI Mapping 
    // #Specification 1.0.0 (DSP0207).
    // # Example: (template-url-syntax=https://localhost:5989)
    populateTemplateField(instance1, serviceUrlSyntaxProperty, serviceUrlSyntax, protocol + "://" + IPAddress);


    populateTemplateField(instance1, serviceLocationTCPProperty, serviceLocationTCP,IPAddress);


    // now fillout the serviceIDAttribute.  Is this correct??????
    // get the name property from the object manager instance.
    String strUUID;

    {
        Uint32 pos;
        CIMInstance instanceObjMgr = instance_ObjMgr;
        if ((pos = instanceObjMgr.findProperty("Name")) != PEG_NOT_FOUND)
        {
            CIMConstProperty p1 = instanceObjMgr.getProperty(pos);
            //XmlWriter::printPropertyElement(p1, PEGASUS_STD(cout));
            CIMValue v1  = p1.getValue();

            XmlWriter::printValueElement(v1, PEGASUS_STD(cout));
            if (!v1.isNull())
            {
                v1.get(strUUID);
            }
            else
                strUUID = "CIMOMNameNULL VALUE";
        }
        else
        {
            strUUID = "NoNameInObjectManagerInstance";
        }

    }
    // Fill out the CIMObjectpath for the slp instance.
    // ATTN: We have problem here now because using the serviceID as key and this
    // means non-unique key.
    Array<CIMKeyBinding> keyBindings;
    keyBindings.append(CIMKeyBinding(serviceIDProperty, strUUID, CIMKeyBinding::STRING));
    CIMObjectPath reference1  = CIMObjectPath("localhost",mynamespace,
                                              CIMName(SlpTemplateClassName),
                                              keyBindings);
    //service-id=string L
    //# The ID of this WBEM Server. The value MUST be the 
    //# CIM_ObjectManager.Name property value.
    populateTemplateField(instance1, serviceIDProperty, serviceIDAttribute, strUUID);

    // Enumerating Instances for the CIM_ObjectManager class .......
    // Note that the CIM_OBjectManager is a singleton class.  It is an error if there is
    // more than one instance of this class.
    for(Uint32 j=0; j < instance_ObjMgr.getPropertyCount(); j++)
    {
        CIMConstProperty p1=instance_ObjMgr.getProperty(j);
        CIMValue v1=p1.getValue();
        CIMName n1=p1.getName();
        /*
        if (n1.equal("Name"))
        {
            populateTemplateField(instance1, serviceUrlSyntax, protocol + "://" + IPAddress);

            //  populateTemplateField(instance1, serviceUrlSyntax,v1.toString());
        }
        */
        // service-hi-name=string O
        // # This string is used as a name of the CIM service for human
        // # interfaces. This attribute MUST be the
        // # CIM_ObjectManager.ElementName property value.
        if (n1.equal("ElementName"))
        {
            populateTemplateField(instance1, serviceHiName, serviceHiNameAttribute, v1.toString());
        }

        // service-hi-description=string O
        // # This string is used as a description of the CIM service for
        // # human interfaces.This attribute MUST be the 
        // # CIM_ObjectManager.Description property value.
        else if (n1.equal("Description"))
        {
          populateTemplateField(instance1, serviceHiDescription, serviceHiDescriptionAttribute, v1.toString());
        }
    }

    // Default properties for PG_SLPWBEMClass
    populateTemplateField(instance1, templateTypeProperty, templateType, String("wbem"));
    
    populateTemplateField(instance1, templateVersionProperty, templateVersionAttribute, String(templateVersion));
    
    populateTemplateField(instance1, templateDescriptionProperty, templateDescriptionAttribute,String(templateDescription));
    
    // InterOp Schema
    populateTemplateField(instance1, InteropSchemaNamespaceAttribute, InteropSchemaNamespaceAttribute, InteropSchemaNamespace);

    //Getting values from CIM_ObjectManagerCommunicationMechanism Class .......
    //// for (Uint32 i = 0; i < instances_ObjMgrComm.size(); i++)
    // ATTN: Delete this{
    // ATTN: Delete thisCIMInstance i1 = instance_ObjMgrComm;

    // ATTN: KS Loop through all properties Note: This does not make it easy to
    // distinguish requied vs. optional
    for(Uint32 j=0;  j < instance_ObjMgrComm.getPropertyCount(); j++)
    {
        CIMConstProperty p1=instance_ObjMgrComm.getProperty(j);
        CIMName n1 = p1.getName();
        CIMValue v1= p1.getValue();

        if (n1.equal(otherCommunicationMechanismAttribute))
        {
    
            populateTemplateField(instance1, communicationMechanismAttribute, communicationMechanismAttribute,v1.toString());
            
            if (String::equalNoCase(v1.toString(),"1"))
            {
                 index = instance_ObjMgrComm.findProperty(CIMName(otherCommunicationMechanismDescriptionAttribute));
                 CIMConstProperty temppr = instance_ObjMgrComm.getProperty(index);
                 populateTemplateField(instance1, otherCommunicationMechanismDescriptionAttribute, otherCommunicationMechanismDescriptionAttribute,(temppr.getValue()).toString());
            }
        }
        else if (n1.equal("Version"))
        {  
          populateTemplateField(instance1, protocolVersionAttribute,  protocolVersionAttribute,v1.toString());
        }
        else if (n1.equal("FunctionalProfileDescriptions"))
        {  
            Array<String> descriptions;
            v1.get(descriptions);

            String desList = arrayToString(descriptions);
            populateTemplateField(instance1,functionalProfilesSupportedAttribute, functionalProfilesSupportedAttribute, desList);
            if (String::equalNoCase(v1.toString(),"Other"))
            {
              Uint32 pos = instance_ObjMgrComm.findProperty(CIMName(otherProfileDescriptionAttribute));
              CIMConstProperty temppr = instance_ObjMgrComm.getProperty(pos);
              populateTemplateField(instance1, otherProfileDescriptionAttribute, otherProfileDescriptionAttribute, temppr.getValue().toString());
            }
        }

        else if (n1.equal(multipleOperationsSupportedAttribute))
        {  
            populateTemplateField(instance1, multipleOperationsSupportedAttribute, multipleOperationsSupportedAttribute,v1.toString());
        }
        
        else if (n1.equal(authenticationMechanismDescriptionsAttribute))
        {
            Array<String> authenticationDescriptions;
            v1.get(authenticationDescriptions);
            String authList = arrayToString(authenticationDescriptions);

            populateTemplateField(instance1, authenticationMechanismsSupportedAttribute, authenticationMechanismsSupportedAttribute, authList);
        }
    }
        
    // fill in the classname information (namespace and classinfo).
    String classInfoList;
    String nameSpaceList;

    nameSpaceList =  getNameSpaceInfo( CIMNamespaceName(mynamespace), classInfoList);
    CDEBUG("Return from getNameSpaceInfo");
    populateTemplateField(instance1, namespaceAttribute, namespaceAttribute, nameSpaceList);
    populateTemplateField(instance1, classinfoAttribute, classinfoAttribute, classInfoList);
    

    // populate the RegisteredProfiles Supported attribute.

    populateTemplateField(instance1,registeredProfilesSupportedAttribute, registeredProfilesSupportedAttribute,
        getRegisteredProfileList());

    //Create the WBEMSLPTemplate instance from all the data gathered above

     _instances.append(instance1);
     _instanceNames.append(reference1);

   //Begin registering the service.
   CDEBUG("Template:\n" << slpTemplateInstance);

   // Add the template to the instance as a diagnostic for the moment.
   instance1.addProperty(CIMProperty(CIMName("RegisteredTemplate"), slpTemplateInstance));

   // Create the service ID from the serviceName and UUID for this system
   
   // ATTN: All of this will be moved to issueSLPRegistrations()
   String ServiceID = serviceName;
   ServiceID = ServiceID + strUUID;
   CString CServiceID = ServiceID.getCString();

   // Make a Cstring form the registration information
   CString CstrUUID = slpTemplateInstance.getCString();
   
   // Test the registration
   slp_agent.test_registration((const char *)CServiceID , 
                        (const char *)CstrUUID,
                        serviceName,
                        "DEFAULT"); 

   // register this information.
   slp_agent.srv_register((const char *)CServiceID ,
                        (const char *)CstrUUID,
                        serviceName,
                        "DEFAULT", 
                        0xffff);
   PEG_METHOD_EXIT();
}

/** issue all necessary SLP registrations. Gets the objects that are required to
    to provide information for registration and calls populate function to create
    a registration for each communication adapter represented by a communication
    object.
*/
void SLPProvider::issueSLPRegistrations()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::issueSLPREgistrations()");

    // ATTN: Get the correct namespace. Right now we do this from root/CIMV2
    // This should be the interop namespace.
    String mynamespace = "root/cimv2";
    _interopNamespace = mynamespace;
    // ATTN: Protect against exceptions here.
    // Get the CIM_ObjectManager instance
    Array<CIMInstance> instances_ObjMgr = _ch.enumerateInstances(
                                             OperationContext(),
                                             _interopNamespace,
                                             CIMName(CIMObjectManagerClassName),
                                             false, false, false,false, CIMPropertyList());
    
    //XmlWriter::printInstanceElement(instances_ObjMgr[0], PEGASUS_STD(cout));
    // get instances of CIM_ObjectManagerCommMechanism and subclasses
    Array<CIMInstance> instances_ObjMgrComm = _ch.enumerateInstances(
                                             OperationContext(),
                                             _interopNamespace,
                                             CIMName(CIMObjectManagerCommMechName),
                                             false, false, false,false, CIMPropertyList());
    
    // determine if we got one objmgr back and other tests.

    //Loop to create an SLP registration for each communication mechanism
    for (Uint32 i = 0; i < instances_ObjMgrComm.size(); i++)
    {
        populateRegistrationData("http", instances_ObjMgr[0], instances_ObjMgrComm[i]);
        // TTN: Hack for the moment until we expand the obejct itself. Correct approach
        // is another subclass that defines the ssl characteristics.
        populateRegistrationData("https", instances_ObjMgr[0], instances_ObjMgrComm[i]);
    }
    
    // Start the slp listener background thread - nothing is advertised until this function returns. 
    slp_agent.start_listener();

    Uint32 finish, now, msec;
    System::getCurrentTime(now, msec);
    finish = now + 10;

    // wait for 10 seconds. Earlier wait for 30 secs caused the client to timeout !
    while(finish > now) 
    {
      pegasus_sleep(1000);
      System::getCurrentTime(now, msec);
    }
    initFlag=true;
    PEG_METHOD_EXIT();
}

void SLPProvider::initialize(CIMOMHandle & handle)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::initialize()");

   _ch = handle;
   initFlag = false;
   
   PEG_METHOD_EXIT();
}

//***************************************************************************
//
//   SLPProvider Public methods, intrinstic and extrinstic methods
//      This includes getInstance, enumerateInstance, enumerateInstanceNames
//      Today the other functions return NOT_SUPPORTED.
//
//***************************************************************************

SLPProvider::SLPProvider(void)
{
}

SLPProvider::~SLPProvider(void)
{
}

/** gets a single instance. Note that this function uses the initFlag
    to determine if the instance has to be built and registered before
    it can return the instance.
*/
void SLPProvider::getInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::getInstance()");

    // if this is the first call, create the registration.    
    if(initFlag == false)
        issueSLPRegistrations();
    
    // convert a potential fully qualified reference into a local reference
   // (class name and keys only).
     CIMObjectPath localReference = CIMObjectPath(
                                    String(),
                                    CIMNamespaceName(),
                                    instanceReference.getClassName(),
                                    instanceReference.getKeyBindings());
   // begin processing the request

   handler.processing();

   // instance index corresponds to reference index

   for(Uint32 i = 0, n = _instances.size(); i < n; i++)
   {
       CIMObjectPath localReference_frominstanceNames = CIMObjectPath(
                                    String(),
                                    CIMNamespaceName(),
                                    _instanceNames[i].getClassName(),
                                    _instanceNames[i].getKeyBindings());

       if(localReference == localReference_frominstanceNames )
       {
          // deliver requested instance
          handler.deliver(_instances[i]);
          break;
       }
                                                                                                                               }
         //complete processing the request
        handler.complete();
        PEG_METHOD_EXIT();
}

void SLPProvider::enumerateInstances(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::enumerateInstances()");

    // if this is the first call, create the registration.    
    if(initFlag == false)
        issueSLPRegistrations();
    
    // begin processing the request
    handler.processing();
    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
       // deliver instance
       handler.deliver(_instances[i]);
    }
    
        // complete processing the request
        handler.complete();
        PEG_METHOD_EXIT();
}

void SLPProvider::enumerateInstanceNames(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::enumerateInstanceNames()");

    // if this is the first call, create the registration.    
    if(initFlag == false)
        issueSLPRegistrations();
    
    // begin processing the request
    handler.processing();
    
    for(Uint32 i = 0, n =_instances.size(); i < n; i++)
    {
    // deliver reference
    handler.deliver(_instanceNames[i]);
    }
    // complete processing the request
    handler.complete();
	                                                                                 	
    PEG_METHOD_EXIT();
}
 
void SLPProvider::modifyInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const CIMInstance & instanceObject,
                const Boolean includeQualifiers,
                const CIMPropertyList & propertyList,
                ResponseHandler & handler)
{
    throw CIMNotSupportedException("SLPServiceProvider "
                       "does not support modifyInstance");
}

void SLPProvider::createInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const CIMInstance & instanceObject,
                ObjectPathResponseHandler & handler)
{
    throw CIMNotSupportedException("SLPServiceProvider "
                       "does not support createInstance");
}

void SLPProvider::deleteInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                ResponseHandler & handler)
{
    throw CIMNotSupportedException("SLPServiceProvider "
                       "does not support deleteInstance");
}

Boolean SLPProvider::tryterminate(void)
{
   return false;
}

void SLPProvider::terminate(void)
{
    slp_agent.unregister();
    delete this;
}

PEGASUS_NAMESPACE_END
