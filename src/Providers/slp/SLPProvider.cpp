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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include "SLPProvider.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
//#include <string.h>

//The following include is needed for UUID Generator
// #include <uuid/uuid.h>

//#include <sys/utsname.h>

#include <set>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/param.h>
//#include <unistd.h>
#include <time.h>
//#include <utmp.h>
//#include <regex.h>
//#include <dirent.h>


//#define CDEBUG(X)
#define CDEBUG(X) PEGASUS_STD(cout) << "SLPProvider " << X << PEGASUS_STD(endl)

PEGASUS_NAMESPACE_BEGIN

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
const char *serviceName = "service:wbem.pegasus";

// Template attribute name constants

String serviceNameAttribute = "service:wbem.pegasus";
String serviceHiDescription = "service_hi_description";
String serviceHiName = "service_hi_name";
String serviceUrlSyntax = "template_url_syntax";
String serviceIDAttribute = "service_id";
String serviceLocationTCP = "service_location_tcp";
String templateType = "template_type";
String templateVersionAttribute = "template_version";
String templateVersion = "1.0";
String templateDescriptionAttribute = "template_description";
String templateDescription = 
    "This template describes the attributes used for advertising CIM Servers.";
String CIM_InteropSchemaNamespaceAttribute = "CIM_InteropSchemaNamespace";
String CIM_InteropSchemaNamespace = "/root/PG_Interop";
String otherCommunicationMechanismDescriptionAttribute = "OtherCommunicationMechanismDescription";
String functionalProfilesSupportedAttribute = "FunctionalProfilesSupported";
String functionalProfileDescriptionsAttribute = "FunctionalProfileDescriptions";
String otherProfileDescriptionAttribute = "OtherProfileDescription";
String communicationMechanismAttribute = "CommunicationMechanism";
String otherCommunicationMechanismAttribute = "otherCommunicationMechanism";
String multipleOperationsSupportedAttribute = "MultipleOperationsSupported";
String authenticationMechanismsSupportedAttribute =  "AuthenticationMechanismsSupported";
String otherAuthenticationDescriptionsAttribute = "OtherAuthenticationDescription";
String authenticationMechanismDescriptionsAttribute =  "AuthenticationMechanismDescriptions";
String namespaceAttribute = "Namespace";
String  classinfoAttribute =  "classinfo";


// This is the dynamic entry point into this dynamic module. The name of
// this provider is "SampleFamilyProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderModule to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

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

String getUUIDString()
{
    
   //UUID Generation code starts 
   //Uses "libuuid.a" in /usr/lib directory. Make sure it's there. Also Makefile has "-luuid" added to $EXTRA_LIBRARIES
   /*
   uuid_t outUUID;
   int j;
   char strUUID[37], tempstrUUID[3];
   
   uuid_generate_random(outUUID);
   strcpy(strUUID, "");
   //uuid(6B29FC40-CA47-1067-B31D-00DD010662DA) 
   for(j=0;j<16;j++)
   {
       sprintf(tempstrUUID, "%2.2x", outUUID[j]);
       tempstrUUID[2]='\0';
       strcat(strUUID,tempstrUUID);
       if(j==3 || j==7 || j==11)
           strcat(strUUID,"-");
   }
   strUUID[36]='\0';
   strUUID = "TESTUUID1234567890"
   */
   //UUID Generation code ends

   //uuid_t outUUID;
   /*
    int j;
   char strUUID[37], tempstrUUID[3];
   
   //uuid_generate_random(outUUID);
   strcpy(strUUID, "");
   
   for(j=0; j < 16; j++)
   {
       sprintf(tempstrUUID, "%2.2x", outUUID[j]);
       tempstrUUID[2]='\0';
       strcat(strUUID,tempstrUUID);
       if(j==3 || j==7 || j==11)
           strcat(strUUID,"-");
   }
   */
    //char strUUID[37], tempstrUUID[3];
    String strUUID = "6B29FC40-CA47-1067-B31D-00DD010662DA";
    //strUUID[36]='\0';
    return (String(strUUID));
}

void appendCSV(String& s, const String& s1)
{
    if (s.size() != 0)
    {
        s.append(",");
    }
    s.append(s1);
}

/** get the list of valid namespaces and supporting info.
    Function builds an array of namespace names and a parallel
    array of classinfo information
    @param nameSpace in which to find CIM_namespace class
    @param, classInfo, anArray of strings for classinfo on return
    @return array of namespaces names
    
*/
String SLPProvider::getNameSpaceInfo(const CIMNamespaceName& nameSpace, String& classInfo )
{
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
    return(names);
    
}

/** populate a single field in the template and the corresponding template instance.
    This function assumes that both the instance object and the template object have been
    created already. There is no error test for this at this point.
    It adds a property to the instance with the defined fieldname and value and also
    adds a line to the slp template with the field name and value.
    @param fieldName String defining the name of the field to populate. Note that this
    assumes that the name is the same in the class and the slp template.
    @param value String defining the value with which to populate the field
*/

void SLPProvider::populateTemplateField(CIMInstance& instance,const String& fieldName, const String& value)
{
    // instance1.addProperty(CIMProperty("classinfo",classInfoList[i]));
    // slpTemplateInstance.append("(classinfo=").append(classInfoList[i]).append("),");
    CDEBUG("Populate TemplateField name= " << fieldName << ". Value= " << value);
    instance.addProperty(CIMProperty(CIMName(fieldName), value));
    if (slpTemplateInstance.size() != 0)
    {
        slpTemplateInstance.append(",\n");
    }
    slpTemplateInstance.append("(");
    slpTemplateInstance.append(fieldName);
    slpTemplateInstance.append("=");
    slpTemplateInstance.append(value);
    slpTemplateInstance.append(")");
}

/** populates the SLP template and its corresponding instance
    ATTN: In the future, we should populate these separately
*/
void SLPProvider::populateData(void)
{
   String mynamespace = "root/cimv2";
   Uint32 index=10;
   slpTemplateInstance = "";
   _interopNamespace = mynamespace;
   CDEBUG("populateData from namespace= " << _interopNamespace.getString() );
   CIMInstance instance1(SlpTemplateClassName);

   // get the uuid for this device.
   String strUUID = getUUIDString();
   
   Array<CIMKeyBinding> keyBindings;
   keyBindings.append(CIMKeyBinding(serviceIDAttribute, strUUID, CIMKeyBinding::STRING));
   CIMObjectPath reference1  = CIMObjectPath("localhost",mynamespace,
                                             CIMName(SlpTemplateClassName),
                                             keyBindings);

    //instance1.addProperty(CIMProperty("service_id", String(strUUID)));   // key
    //LONG_ATTRIBUTE_STRING.append("(service_id=").append(strUUID).append("),");

    populateTemplateField(instance1, serviceIDAttribute, strUUID);
    
    CDEBUG("populateData. getting objmgr and comm from " << _interopNamespace.getString());
    Array<CIMInstance> instances_ObjMgr = _ch.enumerateInstances(
                                             OperationContext(),
                                             _interopNamespace,
                                             CIMName(CIMObjectManagerClassName),
                                             false, false, false,false, CIMPropertyList());
    
    Array<CIMInstance> instances_ObjMgrComm = _ch.enumerateInstances(
                                             OperationContext(),
                                             _interopNamespace,
                                             CIMName(CIMObjectManagerCommMechName),
                                             false, false, false,false, CIMPropertyList());

    // Enumerating Instances for the CIM_ObjectManager class .......
    // Note that the CIM_OBjectManager is a singleton class.  It is an error if there is
    // more than one instance of this class.
    for (Uint32 i = 0; i < instances_ObjMgr.size(); i++)
    {
        Uint32 NumProperties;
        CIMInstance i1 = instances_ObjMgr[i];
        NumProperties = i1.getPropertyCount();
        for(Uint32 j=0;j<NumProperties;j++)
        {
            CIMProperty p1=i1.getProperty(j);
            CIMValue v1=p1.getValue();
            CIMName n1=p1.getName();
            
            // ATTN: Rewrite this to find the property.  More logical
            if (n1.equal("Name"))
            {
                //instance1.addProperty(CIMProperty("template_url_syntax",v1.toString()));
                //LONG_ATTRIBUTE_STRING.append("(template_url_syntax=").append(v1.toString()).append("),");
                populateTemplateField(instance1, serviceUrlSyntax,v1.toString());
            }
            else if (n1.equal("ElementName"))
            {
                //instance1.addProperty(CIMProperty("service_hi_name",v1.toString()));
                //LONG_ATTRIBUTE_STRING.append("(service_hi_name=").append(v1.toString()).append("),");
                populateTemplateField(instance1, serviceHiName,v1.toString());
            }
            else if (n1.equal("Description"))
            {
              //instance1.addProperty(CIMProperty("service_hi_description",v1.toString()));
              //LONG_ATTRIBUTE_STRING= LONG_ATTRIBUTE_STRING + "(service_hi_description=" + v1.toString() + "),";
              populateTemplateField(instance1, serviceHiDescription,v1.toString());
            }
        }
    }

     // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")
     
    String IPAddress = "192.168.1.120";
    /*
    FILE *vf;
     char str1[20];
     Uint32 len,pos=0;
     const char hosts_file[] = "/etc/hosts";
     char buffer[MAXPATHLEN];
     Uint32 tempvar;
     
     vf = fopen(hosts_file, "r");
     if (vf)
     {
        while (fgets(buffer, MAXPATHLEN, vf) != NULL)
          { }
     }
     fclose(vf); 
     for(tempvar =0;tempvar<strlen(buffer);tempvar++)
     {
	if(buffer[tempvar] != '\t')
	{
		str1[tempvar] = buffer[tempvar];
	}
        else
           break;
    }
    str1[tempvar] = '\0';
    strcat(str1,":5988");
    */
    IPAddress.append(":5988");
    
    //instance1.addProperty(CIMProperty("service_location_tcp",String(str1)));    
    //LONG_ATTRIBUTE_STRING.append("(service_location_tcp=").append(String(str1)).append("),");
    populateTemplateField(instance1, serviceLocationTCP,IPAddress);

    // Default properties for PG_SLPWBEMClass
    //instance1.addProperty(CIMProperty("template_type",String("wbem")));
    //LONG_ATTRIBUTE_STRING.append("(template_type=").append(String("wbem")).append("),");
    
    populateTemplateField(instance1, templateType, String("wbem"));
    
    //instance1.addProperty(CIMProperty("template_version",String("1.0")));
    //LONG_ATTRIBUTE_STRING.append("(template_version=").append(String("1.0")).append("),");
    populateTemplateField(instance1, templateVersionAttribute, String(templateVersion));
    
    //instance1.addProperty(CIMProperty("template_description",String("This template describes the attributes used for advertising CIM Servers.")));
    //LONG_ATTRIBUTE_STRING.append("(template_description=").append(String("This template describes the attributes used for advertising CIM Servers.")).append("),");
    populateTemplateField(instance1, templateDescriptionAttribute,String(templateDescription));
    
    // InterOp Schema
    //instance1.addProperty(CIMProperty("CIM_InteropSchemaNamespace", String("/root/PG_Interop")));
    //LONG_ATTRIBUTE_STRING.append("(CIM_InteropSchemaNamespace=").append(String("/root/PG_Interop")).append("),");
    populateTemplateField(instance1, CIM_InteropSchemaNamespaceAttribute, CIM_InteropSchemaNamespace);

    //Getting values from CIM_ObjectManagerCommunicationMechanism Class .......
    CDEBUG("Process object Manager Comm Instances. Found " << instances_ObjMgrComm.size() << " Instances."); 
    for (Uint32 i = 0; i < instances_ObjMgrComm.size(); i++)
    {
        CIMInstance i1 = instances_ObjMgrComm[i];
        // ATTN: KS: Temp because we are getting multiple objects back
        // Code is wrong anyway since need to build arrays, not new attributes.
        if (i > 0)
        {
            break;
        }

        // ATTN: KS Loop through all properties Note: This does not make it easy to
        // distinguish requied vs. optional
        Uint32 NumProperties;
        NumProperties = i1.getPropertyCount();
        for(Uint32 j=0;  j<NumProperties;j++)
        {
            CIMProperty p1=i1.getProperty(j);
            CIMName n1 = p1.getName();
            CIMValue v1= p1.getValue();
    
            if (n1.equal(otherCommunicationMechanismAttribute))
            {
        
                /* From the mof file, CIM_ObjectManagerCommunicationMechanism.CommunicationMechanism
                    ValueMap {"0", "1", "2"}, 
                    Values {"Unknown", "Other", "CIM-XML"}  */
                   //instance1.addProperty(CIMProperty("CommunicationMechanism",v1.toString()));
                   //LONG_ATTRIBUTE_STRING.append("(CommunicationMechanism=").append(v1.toString()).append("),");
                populateTemplateField(instance1, communicationMechanismAttribute,v1.toString());
                
                if (String::equalNoCase(v1.toString(),"1"))
                {
                     index = i1.findProperty(CIMName(otherCommunicationMechanismDescriptionAttribute));
                     CIMProperty temppr = i1.getProperty(index);
                     //instance1.addProperty(CIMProperty("OtherCommunicationMechanismDescription",(temppr.getValue()).toString()));
                     //LONG_ATTRIBUTE_STRING.append("(OtherCommunicationMechanismDescription=").append(v1.toString()).append("),");
                     populateTemplateField(instance1, otherCommunicationMechanismDescriptionAttribute,(temppr.getValue()).toString());
                }
            }
            else if (n1.equal("Version"))
            {  
              //instance1.addProperty(CIMProperty("ProtocolVersion",v1.toString()));
              //LONG_ATTRIBUTE_STRING.append("(ProtocolVersion=").append(v1.toString()).append("),");
              populateTemplateField(instance1, String("ProtocolVersion"),v1.toString());
            }
            else if (n1.equal("FunctionalProfileDescriptions"))
            {  
                Array<String> descriptions;

                v1.get(descriptions);
                String desList = arrayToString(descriptions);
                populateTemplateField(instance1,functionalProfilesSupportedAttribute, desList);
                if (String::equalNoCase(v1.toString(),"Other"))
                {
                  Uint32 temp = i1.findProperty(CIMName(otherProfileDescriptionAttribute));
                  CIMProperty temppr = i1.getProperty(temp);
                  //LONG_ATTRIBUTE_STRING.append("(OtherProfileDescription=").append(temppr.getValue().toString()).append("),");
                  populateTemplateField(instance1, otherProfileDescriptionAttribute, temppr.getValue().toString());
                }
            }


            else if (n1.equal(multipleOperationsSupportedAttribute))
            {  
            //instance1.addProperty(CIMProperty("MultipleOperationsSupported",v1.toString()));
            //LONG_ATTRIBUTE_STRING.append("(MultipleOperationsSupported=").append(v1.toString()).append("),");
            populateTemplateField(instance1, multipleOperationsSupportedAttribute,v1.toString());
            }
            
            else if (n1.equal(authenticationMechanismDescriptionsAttribute))
            {
            Array<String> authenticationDescriptions;
            v1.get(authenticationDescriptions);
            String authList = arrayToString(authenticationDescriptions);

            //instance1.addProperty(CIMProperty("AuthenticationMechanismsSupported",v1.toString())); 
            //LONG_ATTRIBUTE_STRING.append("(AuthenticationMechanismsSupported=").append(v1.toString()).append("),");
            populateTemplateField(instance1, authenticationMechanismsSupportedAttribute,authList);
            /***************
            Array<Uint16> arr;
            v1.get(arr);
            Uint32 sizeofarray = v1.getArraySize();
            for (Uint32 loopvar=0;loopvar<sizeofarray;loopvar++)
            {
                   if(arr[loopvar] == 1) //Means "Other"
               {
                     //Following block fails because the OtherAuthenticationDescription property is NOT currently registered inthe repository
                     Uint32 pos;
                     if ((pos = i1.findProperty(CIMName("OtherAuthenticationDescription"))) != PEG_NOT_FOUND)
                     {
                        CIMProperty p = i1.getProperty(temp);
                        String v = p.getValue();
                        cout << "OtherAuthenticationDescription = " << (temppr.getValue()).toString() << endl;
                        //instance1.addProperty(CIMProperty("OtherAuthenticationDescription",(temppr.getValue()).toString()));
                        //LONG_ATTRIBUTE_STRING.append("(OtherAuthenticationDescription=").append(temppr.getValue().toString()).append("),");
                        
                        populateTemplateField(instance1, otherAuthenticationDescriptionAttribute,v.toString());
            
                     }
               }			   
            }*******************/
        }
    
       }
    }


  // Getting property values from CIM_Namespace class
  /*
    for (Uint32 i = 0; i < instanceNames_Namespace.size(); i++)
          {
             CIMInstance i1 = _ch.getInstance(OperationContext(),mynamespace, instanceNames_Namespace[i],
				                          false,false,false,CIMPropertyList());
             Uint32 NumProperties;
             NumProperties = i1.getPropertyCount();
             for(Uint32 i=0;i<NumProperties;i++)
             {
                 CIMProperty p1=i1.getProperty(i);
                 CIMValue v1=p1.getValue();
                 CIMName n1=p1.getName();
                 if (n1.equal("Name"))
                 {
                   instance1.addProperty(CIMProperty("Namespace",v1.toString()));
                   LONG_ATTRIBUTE_STRING.append("(Namespace=").append(v1.toString()).append("),");
                 }
		        else if (n1.equal("ClassInfo"))
                 {
                   instance1.addProperty(CIMProperty("classinfo",v1.toString()));
                   LONG_ATTRIBUTE_STRING.append("(classinfo=").append(v1.toString()).append("),");
                 }
             }
        }
    */
    
    // fill in the classname information (namespace and classinfo).
    String classInfoList;
    String nameSpaceList;

    nameSpaceList =  getNameSpaceInfo( CIMNamespaceName(mynamespace), classInfoList);
    CDEBUG("Return from getNameSpaceInfo");
    //for(Uint32 i=0;i < nameSpaceList.size();i++)
    //{
    //instance1.addProperty(CIMProperty("Namespace", nameSpaceList[i].toString()));
    //LONG_ATTRIBUTE_STRING.append("(Namespace=").append(nameSpaceList[i].toString()).append("),");
    //instance1.addProperty(CIMProperty("classinfo",classInfoList[i]));
    //LONG_ATTRIBUTE_STRING.append("(classinfo=").append(classInfoList[i]).append("),");
    populateTemplateField(instance1, namespaceAttribute, nameSpaceList);
    populateTemplateField(instance1, classinfoAttribute, classInfoList);
    //}

    //Create the WBEMSLPTemplate instance from all the data gathered above

     _instances.append(instance1);
     _instanceNames.append(reference1);

   //Begin registering the service.
   CDEBUG("Template:\n" << slpTemplateInstance);
   String ServiceID = "service:serviceid:";

   instance1.addProperty(CIMProperty(CIMName("RegisteredTemplate"), slpTemplateInstance));
   ServiceID = ServiceID + strUUID;
   CString CServiceID = ServiceID.getCString();
   CString CstrUUID = slpTemplateInstance.getCString();
   

   slp_agent.test_registration((const char *)CServiceID , 
                        (const char *)CstrUUID,
                        serviceName,
                        "DEFAULT"); 
   // register only local cimom service and hence only one registeration

   slp_agent.srv_register((const char *)CServiceID ,
                        (const char *)CstrUUID,
                        serviceName,
                        "DEFAULT", 
                        0xffff);
   
   // start the background thread - nothing is actually advertised until this function returns. 
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
}

void SLPProvider::initialize(CIMOMHandle & handle)
{
   _ch = handle;
   initFlag = false;
   
}

SLPProvider::SLPProvider(void)
{
}

SLPProvider::~SLPProvider(void)
{
}

void SLPProvider::getInstance(
                const OperationContext & context,
                const CIMObjectPath & instanceReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{

   // convert a potential fully qualified reference into a local reference
    if(initFlag == false)
    {
    populateData();
    initFlag=true;
    }
    
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
                                                                                                                                                                                                                                                                               
}

void SLPProvider::enumerateInstances(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                const Boolean includeQualifiers,
                const Boolean includeClassOrigin,
                const CIMPropertyList & propertyList,
                InstanceResponseHandler & handler)
{

    if(initFlag == false)
    {
        populateData();
        initFlag=true;
    }
    
    // begin processing the request
    handler.processing();
    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
       // deliver instance
       handler.deliver(_instances[i]);
    }
    
    // complete processing the request
      handler.complete();
                                                                                 
}

void SLPProvider::enumerateInstanceNames(
                const OperationContext & context,
                const CIMObjectPath & classReference,
                ObjectPathResponseHandler & handler)
{

    // ATTN: KS 15 dec 03.
    // Note that this mechanism assumes that we will populate
    // the template once at CIMOM startup and never again.
    // This is not a good assumption.
    if(initFlag == false)
    {
       populateData();
       initFlag=true;
    }
    
    // begin processing the request
    handler.processing();
    
    for(Uint32 i = 0, n =_instances.size(); i < n; i++)
    {
    // deliver reference
    handler.deliver(_instanceNames[i]);
    }
    // complete processing the request
    handler.complete();
	                                                                                 	

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
