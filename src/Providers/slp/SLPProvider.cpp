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
#include <string.h>


//The following include is needed for UUID Generator
#include <uuid/uuid.h>


#include <set>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <time.h>
#include <utmp.h>
#include <regex.h>
#include <dirent.h>

PEGASUS_NAMESPACE_BEGIN
extern "C" PEGASUS_EXPORT CIMProvider * PegasusCreateProvider(const String & name)
{

 if(String::equalNoCase(name, "SLPProvider") ||
  String::equalNoCase(name, "SLPProvider(PROVIDER)"))
 {
  return(new SLPProvider());
 }

 return(0);
}



void SLPProvider::populateData(void)
{
   String LONG_ATTRIBUTE_STRING="";
   String mynamespace = "root/cimv2";
   Uint32 index=10;

   //UUID Generation code starts 
   //Uses "libuuid.a" in /usr/lib directory. Make sure it's there. Also Makefile has "-luuid" added to $EXTRA_LIBRARIES
   uuid_t outUUID;
   int j;
   char strUUID[37], tempstrUUID[3];
   
   uuid_generate_random(outUUID);
   strcpy(strUUID, "");
   
   for(j=0;j<16;j++)
   {
       sprintf(tempstrUUID, "%2.2x", outUUID[j]);
       tempstrUUID[2]='\0';
       strcat(strUUID,tempstrUUID);
       if(j==3 || j==7 || j==11)
           strcat(strUUID,"-");
   }
   
   strUUID[36]='\0';
   //UUID Generation code ends
   
   Array<CIMKeyBinding> keyBindings;
   keyBindings.append(CIMKeyBinding("service_id", strUUID, CIMKeyBinding::STRING));
   CIMObjectPath reference1  = CIMObjectPath("//localhost",mynamespace,CIMName("CIM_WBEMSLPTemplate"),
                                                                      keyBindings);

   CIMInstance instance1("CIM_WBEMSLPTemplate");
   instance1.addProperty(CIMProperty("service_id", String(strUUID)));   // key
   LONG_ATTRIBUTE_STRING.append("(service_id=").append(String(strUUID)).append("),");   
    
   Array<CIMObjectPath> instanceNames_ObjMgr = _ch.enumerateInstanceNames(
                                             OperationContext(),
                                             mynamespace,
                                             CIMName("CIM_ObjectManager"));

    Array<CIMObjectPath> instanceNames_ObjMgrComm = _ch.enumerateInstanceNames(
                                             OperationContext(),
                                             mynamespace,
                                             CIMName("CIM_ObjectManagerCommunicationMechanism"));

    //EnumerateInstanceNames on the class CIM_Namespace is not working because of some restrictions by HP people

    //Currently enumerateInst* methods on the CIM_Namespace class are not working
    //Uncomment block below after that is resolved
   /* Array<CIMObjectPath> instanceNames_Namespace = _ch.enumerateInstanceNames(
                                             OperationContext(),
                                             mynamespace,
                                             CIMName("CIM_Namespace"));
    */

    
    // Enumerating InstanceNames for the CIM_ObjectManager class .......

    for (Uint32 i = 0; i < instanceNames_ObjMgr.size(); i++)
      {
        CIMInstance i1 = _ch.getInstance(OperationContext(),mynamespace, instanceNames_ObjMgr[i],
			false,false,false,CIMPropertyList());
        Uint32 NumProperties;
        NumProperties = i1.getPropertyCount();
        for(Uint32 i=0;i<NumProperties;i++)
         {
	     CIMProperty p1=i1.getProperty(i);
             CIMValue v1=p1.getValue();
             CIMName n1=p1.getName();

	      if(String::equalNoCase(n1, "Name"))
	      {
	          instance1.addProperty(CIMProperty("template_url_syntax",v1.toString()));
		  LONG_ATTRIBUTE_STRING.append("(template_url_syntax=").append(v1.toString()).append("),");
	      }
	      else if(String::equalNoCase(n1, "ElementName"))
	      {
		  instance1.addProperty(CIMProperty("service_hi_name",v1.toString()));
		  LONG_ATTRIBUTE_STRING.append("(service_hi_name=").append(v1.toString()).append("),");
	      }
              else if(String::equalNoCase(n1, "Description"))
              {
                  instance1.addProperty(CIMProperty("service_hi_description",v1.toString()));
		  LONG_ATTRIBUTE_STRING= LONG_ATTRIBUTE_STRING + "(service_hi_description=" + v1.toString() + "),";
              }
	      
	 }
      }

     // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")
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
    
    instance1.addProperty(CIMProperty("service_location_tcp",String(str1)));    
    LONG_ATTRIBUTE_STRING.append("(service_location_tcp=").append(String(str1)).append("),");


   // Default properties for CIM_SLPWBEMClass
      instance1.addProperty(CIMProperty("template_type",String("wbem")));
      LONG_ATTRIBUTE_STRING.append("(template_type=").append(String("wbem")).append("),");

      instance1.addProperty(CIMProperty("template_version",String("1.0")));
      LONG_ATTRIBUTE_STRING.append("(template_version=").append(String("1.0")).append("),");

      instance1.addProperty(CIMProperty("template_description",String("This template describes the attributes used for advertising CIM Servers.")));
      LONG_ATTRIBUTE_STRING.append("(template_description=").append(String("This template describes the attributes used for advertising CIM Servers.")).append("),");

  // InterOp Schema
      instance1.addProperty(CIMProperty("CIM_InteropSchemaNamespace", String("/root/PG_Interop")));
      LONG_ATTRIBUTE_STRING.append("(CIM_InteropSchemaNamespace=").append(String("/root/PG_Interop")).append("),");

  //Getting values from CIM_ObjectManagerCommunicationMechanism Class .......
					

   
for (Uint32 i = 0; i < instanceNames_ObjMgrComm.size(); i++)
{
  CIMInstance i1 = _ch.getInstance(OperationContext(),mynamespace,
                                     instanceNames_ObjMgrComm[i],false,false,false,CIMPropertyList());
  Uint32 NumProperties;
  NumProperties = i1.getPropertyCount();
  for(Uint32 i=0;i<NumProperties;i++)
  {
      CIMProperty p1=i1.getProperty(i);
      CIMName n1 = p1.getName();
      CIMValue v1= p1.getValue();
      if (String::equalNoCase(n1,"CommunicationMechanism"))
      {
       
	/* From the mof file, CIM_ObjectManagerCommunicationMechanism.CommunicationMechanism
        ValueMap {"0", "1", "2"}, 
        Values {"Unknown", "Other", "CIM-XML"}  */
       instance1.addProperty(CIMProperty("CommunicationMechanism",v1.toString()));
       LONG_ATTRIBUTE_STRING.append("(CommunicationMechanism=").append(v1.toString()).append("),");

        if (String::equalNoCase(v1.toString(),"1"))
        {
                
	 index = i1.findProperty(CIMName("OtherCommunicationMechanismDescription"));
         CIMProperty temppr = i1.getProperty(index);
         instance1.addProperty(CIMProperty("OtherCommunicationMechanismDescription",(temppr.getValue()).toString()));
	 LONG_ATTRIBUTE_STRING.append("(OtherCommunicationMechanismDescription=").append(v1.toString()).append("),");
        }
      }
      else if (String::equalNoCase(n1,"Version"))
      {  
          instance1.addProperty(CIMProperty("ProtocolVersion",v1.toString()));
	  LONG_ATTRIBUTE_STRING.append("(ProtocolVersion=").append(v1.toString()).append("),");
      }
      else if (String::equalNoCase(n1,"ProfilesSupported"))
      {  
        if (String::equalNoCase(v1.toString(),"Other"))
        {
          Uint32 temp = i1.findProperty(CIMName("OtherProfileDescription"));
          CIMProperty temppr = i1.getProperty(temp);
	  LONG_ATTRIBUTE_STRING.append("(OtherProfileDescription=").append(temppr.getValue().toString()).append("),");
        }
      }
      else if (String::equalNoCase(n1,"MultipleOperationsSupported"))
      {  
          instance1.addProperty(CIMProperty("MultipleOperationsSupported",v1.toString()));
	  LONG_ATTRIBUTE_STRING.append("(MultipleOperationsSupported=").append(v1.toString()).append("),");
      }
     
      else if (String::equalNoCase(n1,"AuthenticationMechanismsSupported"))
      {  
        instance1.addProperty(CIMProperty("AuthenticationMechanismsSupported",v1.toString())); 
	LONG_ATTRIBUTE_STRING.append("(AuthenticationMechanismsSupported=").append(v1.toString()).append("),");
	Array<Uint16> arr;
	v1.get(arr);
	Uint32 sizeofarray = v1.getArraySize();
	for (Uint32 loopvar=0;loopvar<sizeofarray;loopvar++)
	{
           if(arr[loopvar] == 1) //Means "Other"
	   {
             //Following block fails because the OtherAuthenticationDescription property is NOT currently registered inthe repository
	     
             /*Uint32 temp = i1.findProperty(CIMName("OtherAuthenticationDescription"));
             CIMProperty temppr = i1.getProperty(temp);
             cout << "OtherAuthenticationDescription = " << (temppr.getValue()).toString() << endl;
	     instance1.addProperty(CIMProperty("OtherAuthenticationDescription",(temppr.getValue()).toString()));
	     LONG_ATTRIBUTE_STRING.append("(OtherAuthenticationDescription=").append(temppr.getValue().toString()).append("),");*/
		     
	   }			   
	}
	
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
                 if(String::equalNoCase(n1, "Name"))
                 {
                   instance1.addProperty(CIMProperty("Namespace",v1.toString()));
                   LONG_ATTRIBUTE_STRING.append("(Namespace=").append(v1.toString()).append("),");
                 }
		 else if(String::equalNoCase(n1, "ClassInfo"))
                 {
                   instance1.addProperty(CIMProperty("classinfo",v1.toString()));
                   LONG_ATTRIBUTE_STRING.append("(classinfo=").append(v1.toString()).append("),");
                 }
             }
        }
    */
    
    //Create the WBEMSLPTemplate instance from all the data gathered above

     _instances.append(instance1);
     _instanceNames.append(reference1);

   //Begin registering the service.

   String ServiceID = "service:serviceid:";
   ServiceID = ServiceID + String(strUUID);
   CString CServiceID = ServiceID.getCString();
   CString CstrUUID = (LONG_ATTRIBUTE_STRING).getCString();
   

   slp_agent.test_registration((const char *)CServiceID , 
                                (const char *)CstrUUID,
			       "service:wbem.pegasus",
			       "DEFAULT"); 
   
   // register only local cimom service and hence only one registeration

   slp_agent.srv_register((const char *)CServiceID ,
                           (const char *)CstrUUID,
			  "service:wbem.pegasus",
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
   // (class name and keys only).
     CIMObjectPath localReference = CIMObjectPath(
                                    String(),
                                    String(),
                                    instanceReference.getClassName(),
                                    instanceReference.getKeyBindings());
   // begin processing the request

   handler.processing();

   // instance index corresponds to reference index

   for(Uint32 i = 0, n = _instances.size(); i < n; i++)
   {
       CIMObjectPath localReference_frominstanceNames = CIMObjectPath(
                                    String(),
                                    String(),
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
