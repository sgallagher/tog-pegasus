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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Karl Schopmeyer - Add Cim_Namespace capabilities.
//              Karl Schopmeyer - Temp added objectmanager and communication classes
//
//%////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//  Interop Provider - This provider services those classes from the
//  DMTF Interop schema association with the CIMOM itself
//
//  This provider services the following classes:
//      CIMObjectManager
//      CIM_ObjectManagerCommunicationMechanism
//      CIM_CIMXMLCommunicationMechanism
//      CIM_ProtocolAdapter
//      CIM_Namespace
//      
//      It also services the Interop associations tied to these classes
//      including:
//      CIM_NamespaceInManager
//      ...

//      CIM Version: Interop Provider was written for CIM 2.7 adn 2.8.
//       Note: all 2.8 functions are controlled by a flag and can be
//      disabled.
///////////////////////////////////////////////////////////////////////////////

/* STATUS: In process but running 12 August 2003 KS */

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include "InteropProvider.h"
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/XmlWriter.h>

#include <sstream>
#include <string>

#include <stdlib.h>

//The following include is needed for UUID Generator
#if defined(PEGASUS_OS_TYPE_WINDOWS)
#include <objbase.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//#define CDEBUG(X)
#define CDEBUG(X) PEGASUS_STD(cout) << "InteropProvider " << X << PEGASUS_STD(endl)
//#define CDEBUG(X) Logger::put (Logger::DEBUG_LOG, "Linux_ProcessorProvider", Logger::INFORMATION, "$0", X)
//#define CDEBUG(X) {std::stringstream ss; std::string r;ss << X;ss>>r; PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, r)}
 
static const String PegasusInstanceIDGlobalPrefix = "PEG";
//    Constants representing the class names processed

/**
    The constants representing the class names we process
*/
static const CIMName __NAMESPACE_CLASSNAME  = CIMName ("__Namespace");
static const CIMName CIM_NAMESPACE_CLASSNAME  = CIMName ("CIM_Namespace");
static const CIMName CIM_OBJECTMANAGER_CLASSNAME  = CIMName ("CIM_ObjectManager");
static const CIMName CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("CIM_ObjectManagerCommunicationMechanism");
static const CIMName CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("CIM_CIMXMLCommunicationMechanism");


// Property Names for __Namespace Class
static const CIMName NAMESPACE_PROPERTYNAME  = CIMName ("Name");
static const CIMNamespaceName ROOTNS  = CIMNamespaceName ("root");


// Property names for CIM_ObjectManager Class
static const CIMName OM_GATHERSTATISTICALDATA  =
 CIMName ("GatherStatisticalData");
// Property for the slp template.
static const CIMName OM_DESCRIPTION =
    CIMName("Description");


// Property Names for ObjectManagerCommunicationMechanism Class
static const CIMName OM_COMMUNICATIONMECHANISM  = 
        CIMName ("CommunicationMechanism");
static const CIMName OM_FUNCTIONALPROFILESSUPPORTED  =
 CIMName ("FunctionalProfilesSupported");
static const CIMName OM_FUNCTIONALPROFILEDESCRIPTIONS  =
 CIMName ("FunctionalProfileDescriptions");
static const CIMName OM_AUTHENTICATIONMECHANISMSSUPPORTED  =
 CIMName ("AuthenticationMechanismsSupported");
static const CIMName OM_AUTHENTICATIONMECHANISMDESCRIPTIONS  =
 CIMName ("AuthenticationMechanismDescriptions");
static const CIMName OM_MULTIPLEOPERATIONSSUPPORTED  =
 CIMName ("MultipleOperationsSupported");
static const CIMName OM_VERSION  =
 CIMName ("Version");

// Property Names for CIMXML CommunicationMechanism

static const CIMName CIMVALIDATED  =
 CIMName ("CIMValidated");

static const String CIMXMLProtocolVersion = "1.0";

// Property names for CIM_Namespace Class

static const CIMName CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME = 
        CIMName ("SystemCreationClassName");
static const CIMName CIM_NAMESPACE_PROPERTY_SYSTEMNAME = 
        CIMName ("SystemName");
static const CIMName CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME = 
        CIMName ("ObjectManagerCreationClassName");
static const CIMName CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME = 
        CIMName ("ObjectManagerName");
static const CIMName CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME = 
        CIMName ("CreationClassName");
static const CIMName CIM_NAMESPACE_PROPERTY_NAME  = CIMName ("Name");
static const CIMName CIM_NAMESPACE_PROPERTY_CLASSINFO = 
        CIMName ("ClassInfo");
static const CIMName CIM_NAMESPACE_PROPERTY_DESCRIPTIONOFCLASSINFO = 
        CIMName ("DescriptionOfClassInfo");



// Defines to serve as the ENUM for class selection for instance
// operations.

enum targetClass{
     __NAMESPACE = 1,
     CIM_NAMESPACE = 2,
     CIM_OBJECTMANAGER = 3,
     CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM = 4,
     CIM_CIMXMLCOMMUNICATIONMECHANISM = 5
     };

//***************************************************************
// Provider Utility Functions
//***************************************************************

/** Builds the UUID string for this CIMOM.
**/
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

  String strUUID;

// ------------------------------------------
// GUID string for Windows
// ------------------------------------------
#if defined(PEGASUS_OS_TYPE_WINDOWS)
  GUID guid;
  HRESULT hres = CoCreateGuid(&guid);
  if (hres == S_OK)
    {
      WCHAR guid_strW[38] = { L"" };
      char guid_str[100];
      ::memset(&guid_str, 0, sizeof(guid_str));
      if (StringFromGUID2(guid, guid_strW, sizeof(guid_strW)) > 0)
        {
          Uint32 numBytes =  sizeof(guid_strW)/sizeof(guid_strW[0]);
          WideCharToMultiByte(CP_ACP, 0, guid_strW, numBytes, guid_str, sizeof(guid_str), NULL, NULL);
          // exclude the first and last chars (i.e., { and })
          for (Uint32 i=1; i<sizeof(guid_str); i++)
            {
              if (guid_str[i] != '}')
                {
                  strUUID.append(Char16(guid_str[i]));
                }
              else
                break;
            }
        }
    }
  
// ------------------------------------------
// GUID string for Unix and other transients
// ------------------------------------------
#elif defined(PEGASUS_OS_TYPE_UNIX)
#elif defined(PEGASUS_OS_TYPE_NSK)
#else
# error "Unsupported platform"
#endif

    if (strUUID == String::EMPTY)
      {
        strUUID = "6B29FC40-CA47-1067-B31D-00DD010662DA";
      }
    //strUUID[36]='\0';
    return (String(strUUID));
}

/* Test the keys in the CIM_Namespace for valid values
   This includes all of the keys above the name key.
   THis is a dummy for now.
   ATTN: KS Extend and finish this function.
*/
Boolean _validateProperties(const CIMObjectPath& path)
{
    return true;
}
Boolean _validateProperties(const CIMInstance& instance)
{
    return true;
}

/* validate that the property exists, is string type and
   optionally the value itself. Note processes only String
   properties
   @param - Instance to search for property.
   @param - Property Name
   @value - String value. If not String::EMPTY, compare to 
   value in the property
   @return - ture if passes all tests
*/
Boolean _validateRequiredProperty(const CIMInstance& instance, 
                          const CIMName& propertyName,
                          const String& value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateRequiredProperty()");
    Uint32 pos;
    if ((pos = instance.findProperty (propertyName)) == PEG_NOT_FOUND)
        return(false);
    //
    //  Get the property
    //
    CIMConstProperty theProperty = instance.getProperty(pos);
    const CIMValue theValue = theProperty.getValue ();
    String valueField;
    theValue.get(valueField);
    //
    //  Required property must have a non-null value
    //
    if ((theValue.getType() != CIMTYPE_STRING) || (theValue.isNull()))
    {
        PEG_METHOD_EXIT();
        return(false);
    }
    if ((value == String::EMPTY) || (valueField == value))
    {
        PEG_METHOD_EXIT();
        return(true);
    }
    PEG_METHOD_EXIT();
    return(false);
}

Boolean _validateRequiredProperty(const CIMInstance& instance,
                          const CIMName& propertyName,
                          const Uint16& value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::_validateRequiredProperty()");
    
    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Validate "
        + propertyName.getString());
    Uint32 pos;
    if ((pos = instance.findProperty (propertyName)) == PEG_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
        return(false);
    }
    //
    //  Get the property
    //
    CIMConstProperty theProperty = instance.getProperty(pos);
    CIMValue theValue = theProperty.getValue ();
    //
    //  Required property must have a non-null value
    //
    if ((theValue.getType() != CIMTYPE_UINT16) 
        || (theValue.isNull())  ) 
//        || (theValue.getValue != value()) )
    {
        PEG_METHOD_EXIT();
        return(false);
    }
    PEG_METHOD_EXIT();
    return(true);
}
Boolean _validateRequiredProperty(const CIMObjectPath& objectPath,
                          const CIMName& propertyName,
                          const String value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateRequiedProperty()");
    Array<CIMKeyBinding> kbArray = objectPath.getKeyBindings();

    // find the correct key binding
    for (Uint32 i = 0; i < kbArray.size(); i++)
    {
        if (kbArray[i].getName() == propertyName)
        {
            if (value != String::EMPTY)
            {
                if (value !=kbArray[i].getValue())
                {
                    PEG_METHOD_EXIT();
                    return(true);
                }
            }
        }
    }
    PEG_METHOD_EXIT();
    return(true);
}

/* Query the repository for array of all namespacenames
*/        
Array<CIMNamespaceName> InteropProvider::_enumerateNameSpaces()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_enumerateNameSpaces()");
    Array<CIMNamespaceName> namespaceNames;
    
    try
    {
        namespaceNames = _repository->enumerateNameSpaces();
    }
    catch(CIMException& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }
    catch(Exception& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }

    PEG_METHOD_EXIT();
    return(namespaceNames);
}

/* get a class defintion. Gets the class defined by
    the parameters. Generates exception of class not defined.
   @param namespace in which to look for the class.
   @param name of class to get.
   @return the CIMClass object
   @Exceptions any repository exceptions if class not found.
*/ 
CIMClass InteropProvider::_getClass(const CIMNamespaceName& nameSpace,
                                    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getClass");
    CIMClass myClass;
    CDEBUG("_getClass: Get Class from repository. Namespace= " << nameSpace << " Class= " <<  className.getString());
    // ATTN: The inUnitialized function is NOT necessary.
    if (myClass.isUninitialized())
    {
        try
        {
            myClass = _repository->getClass(nameSpace, className );
        }
        catch(CIMException& e)
        {
            CDEBUG("_getClass CIMException: " << e.getMessage());
            PEG_METHOD_EXIT();
            throw e;
        }
        catch(Exception& e)
        {
            CDEBUG("_getClass CIMException: " << e.getMessage());
            PEG_METHOD_EXIT();
            throw e;
        }
    }
    CDEBUG("_getClass: Class Acquired");
    PEG_METHOD_EXIT();
    return myClass;
}


/* Verify that this is one of the legal classnames and
   return indicator which.
   @param - Classname
   @return - Uint32 indicating type
   @Exceptions - throws CIMNotSupportedException if invalid class.
*/
targetClass _verifyValidClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_verifyValidClassInput()");
    CDEBUG("Class Name Input = " << className.getString());
    
    if (className.equal(CIM_NAMESPACE_CLASSNAME))
    {
        PEG_METHOD_EXIT();
        return CIM_NAMESPACE;
    }

    if (className.equal(CIM_OBJECTMANAGER_CLASSNAME))
    {
        PEG_METHOD_EXIT();
        return CIM_OBJECTMANAGER;
    }

    if (className.equal(CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME))
    {
        PEG_METHOD_EXIT();
        return CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM;
    }

    if (className.equal(CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME))
    {
        PEG_METHOD_EXIT();
        return CIM_CIMXMLCOMMUNICATIONMECHANISM;
    }

    // Last entry, reverse test and returnOK if CIM_Namespace
    if (!className.equal(CIM_NAMESPACE_CLASSNAME))
    {
        CDEBUG("Invalid Class received " << className.getString());
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");
    }

    PEG_METHOD_EXIT();
    return CIM_NAMESPACE;
}

/* validate the authorization of the user name against the namespace.
*/
String _validateUserID(const OperationContext & context)
{
    //ATTN-DME-P3-20020522: ADD AUTHORIZATION CHECK TO __NAMESPACE PROVIDER
    String userName;
    try
    {
        IdentityContainer container = context.get(IdentityContainer::NAME);
        userName = container.getUserName();
    }
    catch (...)
    {
       userName = String::EMPTY;
    }
    return userName;
}

/** Add the keys common to a number of the Interop instances to the provided
    instance. This includes SystemCreationClassName and System Name
    @param instance of CIMInstance to which these are to be added.
*/
void _buildInstanceCommonKeys(CIMInstance& instance)
{
    
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCommonKeys()");
    
    String SystemCreationClassName = System::getSystemCreationClassName ();
    if (SystemCreationClassName == String::EMPTY)
    {
        // This in place because global is often Empty
        SystemCreationClassName = "CIM_ComputerSystem";
    }

    // Add property SystemCreationClassName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                     SystemCreationClassName)));

    // Add property SystemName
    String SystemName = System::getHostName();
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                     SystemName)));
    
    PEG_METHOD_EXIT();
}


/* build instance of the CIMObjectManagerCommunicationClass
*/
CIMInstance _buildInstancCIMObjectManagerCommunicationMechanism()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMObjectCommunicationMechanism()");
    
    CIMInstance instance(CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME);

    _buildInstanceCommonKeys(instance);


    //CreationClassName - Class this instance created from.
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
            CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME.getString() )));
    
    //String name = "PegasusCIMXMLCommunicationMechanism";
    //Name, this CommunicationMechanism. The use of this name is not
    // clear right now
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_NAME,
                     CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME.getString() )));

    // CommunicationMechanism Property. Set to 2, CIMXML for now.
    instance.addProperty(
        (CIMProperty(OM_COMMUNICATIONMECHANISM,
                     Uint16(2) )));

    //Functional Profiles Supported Property.
    Array<Uint16> profiles;
    Array<String> profileDescriptions;

    profiles.append(2); profileDescriptions.append("Basic Read");
    profiles.append(3); profileDescriptions.append("Basic Write");
    profiles.append(4); profileDescriptions.append("Schema Manipulation");
    profiles.append(5); profileDescriptions.append("Instance Manipulation");
    profiles.append(6); profileDescriptions.append("Association Traversal");
    profiles.append(8); profileDescriptions.append("Qualifier Declaration");
    profiles.append(9); profileDescriptions.append("Indications");

    CIMValue profileValue(profiles);
    CIMValue profileDescriptionsValue(profileDescriptions);

    instance.addProperty(
        (CIMProperty(OM_FUNCTIONALPROFILESSUPPORTED,
                     profileValue )));
    instance.addProperty(
        (CIMProperty(OM_FUNCTIONALPROFILEDESCRIPTIONS,
                     profileDescriptionsValue )));

    // Multiple OperationsSupported Property
    instance.addProperty(
        (CIMProperty(OM_MULTIPLEOPERATIONSSUPPORTED,
                     Boolean(false) )));

    // AuthenticationMechanismsSupported Property

    Array<Uint16> authentications;
    Array<String> authenticationDescriptions;

    // The following is a hack, supplying values from fixed info.
    authentications.append(3); authenticationDescriptions.append("basic");

    CIMValue authenticationValue(authentications);
    CIMValue authenticationDescriptionsValue(authenticationDescriptions);
    instance.addProperty(
        (CIMProperty(OM_AUTHENTICATIONMECHANISMSSUPPORTED,
                     authenticationValue )));
    instance.addProperty(
        (CIMProperty(OM_AUTHENTICATIONMECHANISMDESCRIPTIONS,
                     authenticationDescriptionsValue )));
    
    //Version property
    instance.addProperty(
        (CIMProperty(OM_VERSION,
                     CIMXMLProtocolVersion )));

    PEG_METHOD_EXIT();
    return(instance);
}


CIMInstance _buildInstancCIMXMLCommunicationMechanism()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMXMLCommunicationMechanism()");
    CDEBUG("_BuildInstanceCIMXMLCommunicationMechanism");
    CIMInstance instance(CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME);

    _buildInstanceCommonKeys(instance);

    //CreationClassName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
            CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME.getString() )));
    
    //Name, this CommunicationMechanism.
    String name = "PegasusCommunicationMechanism";
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_NAME,
                     name )));

    // CommunicationMechanism Property
    instance.addProperty(
        (CIMProperty(OM_COMMUNICATIONMECHANISM,
                     Uint16(2) )));

    // CommunicationMechanism Property
    // KS why duplicate property add.  Should not work.
    //instance.addProperty(
    //    (CIMProperty(OM_COMMUNICATIONMECHANISM,
    //                 Uint16(2) )));

    //Functional Profiles Supported Property.
    Array<Uint16> profiles;
    Array<String> profileDescriptions;

    profiles.append(2); profileDescriptions.append("Basic Read");
    profiles.append(3); profileDescriptions.append("Basic Write");
    profiles.append(4); profileDescriptions.append("Schema Manipulation");
    profiles.append(5); profileDescriptions.append("Instance Manipulation");
    profiles.append(6); profileDescriptions.append("Association Traversal");
    profiles.append(8); profileDescriptions.append("Qualifier Declaration");
    profiles.append(9); profileDescriptions.append("Indications");

    CIMValue profileValue(profiles);
    CIMValue profileDescriptionsValue(profileDescriptions);
    
    instance.addProperty(
        (CIMProperty(OM_FUNCTIONALPROFILESSUPPORTED,
                     profileValue )));
    instance.addProperty(
        (CIMProperty(OM_FUNCTIONALPROFILEDESCRIPTIONS,
                     profileDescriptionsValue )));

    // Multiple OperationsSupported Property
    instance.addProperty(
        (CIMProperty(OM_MULTIPLEOPERATIONSSUPPORTED,
                     Boolean(false) )));

    // AuthenticationMechanismsSupported Property

    Array<Uint16> authentications;
    Array<String> authenticationDescriptions;

    authentications.append(3); authenticationDescriptions.append("basic");

    CIMValue authenticationValue(authentications);
    CIMValue authenticationDescriptionsValue(authenticationDescriptions);
    
    instance.addProperty(
        (CIMProperty(OM_AUTHENTICATIONMECHANISMSSUPPORTED,
                     authenticationValue )));
    instance.addProperty(
        (CIMProperty(OM_AUTHENTICATIONMECHANISMDESCRIPTIONS,
                     authenticationDescriptionsValue )));
    
    //Version property
    instance.addProperty(
        (CIMProperty(OM_VERSION,
                     CIMXMLProtocolVersion )));
    PEG_METHOD_EXIT();
    return(instance);
}
/* Gets the value for the CIMObjectManager name.  This is a key
   property with the following characteristics.
   1. It is persistent. This must be persistent through CIMOM
   restarts.  We will save it in the instance database to achieve this. 
   2. It must be unique. We cannot create duplicate CIMOM names
   3. It is based on the DMTF description of how unique InstanceIds
   are defined (Trademark/etc followed by unique identification.

*/
String buildObjectManagerName()
{
    return(getUUIDString());
}

CIMInstance InteropProvider::_buildInstanceCIMObjectManager()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMObjectManager()");


    if (!instanceOfCIMObjectManager.isUninitialized())
    {
        PEG_METHOD_EXIT();
        return(instanceOfCIMObjectManager);
    }

    // Try to get persistent instance from repository
    Array<CIMInstance> instances;
    try
    {
        instances = _repository->enumerateInstances(_operationNamespace,
                      CIM_OBJECTMANAGER_CLASSNAME );
    }
    catch(CIMException& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }
    catch(Exception& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }
    
    if (instances.size() >= 1)
    {
        instanceOfCIMObjectManager = instances[0];
        PEG_METHOD_EXIT();
        return(instanceOfCIMObjectManager);
    }

    //
    // Must build new instance and save it.
    //
    CIMInstance instance(CIM_OBJECTMANAGER_CLASSNAME);
    instanceOfCIMObjectManager = instance;

    _buildInstanceCommonKeys(instanceOfCIMObjectManager);

    //CreationClassName -- This class.
    instanceOfCIMObjectManager.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                     CIM_OBJECTMANAGER_CLASSNAME.getString() )));
    //Name, this CIMObject Manager.
    instanceOfCIMObjectManager.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_NAME,
                     buildObjectManagerName() )));

    //ElementName this object manager
    instanceOfCIMObjectManager.addProperty(
        (CIMProperty(CIMName("ElementName"),
                     String("Pegasus") )));

    // Gets the object manager class for some info
    CIMClass objMgrClass = _getClass(_operationNamespace, "CIM_ObjectManager");
    Uint32 pos;

    String description = "Pegasus CIMOM Version 2.3.2";
    char * envDescription;
    envDescription = getenv("PEGASUS_CIMOM_DESCRIPTION");
    if (envDescription)
    {
        description = envDescription;
    }
    instanceOfCIMObjectManager.addQualifier(
        (CIMQualifier("Description", description)));


    //Property GatherStatisticalData. Note that today we do not
    // have a dynamic activation for this value.
    

#ifdef PEGASUS_HAS_PERFINST
    Boolean gatherStatData = true;
#else
    Boolean gatherStatData = false;
#endif
    instanceOfCIMObjectManager.addProperty(
        (CIMProperty(OM_GATHERSTATISTICALDATA,
                     Boolean(gatherStatData) )));
    
    // write the instance to the repository
    CIMObjectPath instancepath;
    try
    {
        instancepath = _repository->createInstance(_operationNamespace,
                       instanceOfCIMObjectManager );
    }
    catch(CIMException& e)
    {
        PEG_METHOD_EXIT();
        throw e;
    }
    catch(Exception& e)
    {
    PEG_METHOD_EXIT();
        throw e;
    }
    
    PEG_METHOD_EXIT();
    return(instanceOfCIMObjectManager);
}

/* generate one instance of the CIM_Namespace class with the
   properties
   @param namespace name to put into the class
   @exceptions - exceptions carried forward from create instance
   and addProperty.
*/


/* Create an instance of the CIM_Namespace class which is based
   on the following CIM MOF Specification
[Version ("2.6.0"), Description (
    "Namespace provides a domain (in other words, a container), "
    "in which the instances [of a class] are guaranteed to be "
    "unique per the KEY qualifier definitions.  It is named "
    "relative to the CIM_ObjectManager implementation that "
    "provides such a domain.") ]
class CIM_Namespace : CIM_ManagedElement {
        
    [Propagated("CIM_ObjectManager.SystemCreationClassName"), Key, 
        MaxLen (256), Description (
           "The scoping System's CreationClassName.") ]
    string SystemCreationClassName;

    [Propagated("CIM_ObjectManager.SystemName"), Key, MaxLen (256),
        Description ("The scoping System's Name.") ]
    string SystemName;

    [Propagated ("CIM_ObjectManager.CreationClassName"), Key,
        MaxLen (256), Description (
           "The scoping ObjectManager's CreationClassName.") ]
    string ObjectManagerCreationClassName;

    [Propagated ("CIM_ObjectManager.Name"), Key, MaxLen (256), 
        Description ("The scoping ObjectManager's Name.") ]
    string ObjectManagerName;

    [Key, MaxLen (256), Description (
        "CreationClassName indicates the name of the class or the "
        "subclass used in the creation of an instance. When used "
        "with the other key properties of this class, this property "
        "allows all instances of this class and its subclasses to "
        "be uniquely identified.") ]
    string CreationClassName;
        
    [Key, MaxLen (256), Description (
        "A string to uniquely identify the Namespace within "
        "the ObjectManager.") ]
    string Name;

    [Required, Write, Description (
        "Enumeration indicating the organization/schema of the "
        "Namespace's objects. For example, they may be instances "
        "of classes of a specific CIM version."),
        ValueMap {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                  "10", "200", "201", "202"},
        Values {"Unknown", "Other", "CIM 1.0", "CIM 2.0",       
              "CIM 2.1", "CIM 2.2", "CIM 2.3", "CIM 2.4", "CIM 2.5",
              "CIM 2.6", "CIM 2.7", "DMI Recast", "SNMP Recast", 
                  "CMIP Recast"},
        ModelCorrespondence {"CIM_Namespace.DescriptionOfClassInfo"} ]
    uint16 ClassInfo;

    [Write, Description (
        "A string providing more detail (beyond the general "
        "classification in ClassInfo) for the object hierarchy of "
        "the Namespace."),
        ModelCorrespondence {"CIM_Namespace.ClassInfo"} ]
    string DescriptionOfClassInfo;
};

*/
CIMInstance _buildInstanceCIMNamespace(const CIMNamespaceName & nameSpace)
{
    CDEBUG("_buildInstnaceCIMNamespace enter");
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMNamespace()");
    
    String ObjectManagerName = "ObjectManagerNameValue";
    String ClassInfo = "ClassInfo";
    String DescriptionOfClassInfo = "DescriptionOfClassInfo";
    
    CDEBUG("_buildInstnaceCIMNamespace create instance");
    CIMInstance instance(CIM_NAMESPACE_CLASSNAME);

    /*  The following moved to common create
    // Add the properties
    // SystemCreationClassName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                     SystemCreationClassName)));
    // SystemName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                     SystemName)));
    */

    CDEBUG("_buildInstnaceCIMNamespace build common keys");
    _buildInstanceCommonKeys(instance);

    CDEBUG("_buildInstnaceCIMNamespace add properties");
    //ObjectManagerCreationClassName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                     CIM_OBJECTMANAGER_CLASSNAME.getString())));
    //ObjectManagerName
    // This is the one we have to sort out ATTN: TBD KS P0
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                     ObjectManagerName)));
    //CreationClassName
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                     CIM_NAMESPACE_CLASSNAME.getString() )));
    //Name
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_NAME,
                     nameSpace.getString() )));

    // ATTN: KS 16 Jan 2004 We are not supplying this property right now because we do
    // not know what to put into it.
    //ClassInfo
    /*
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_CLASSINFO,
                     ClassInfo)));

    //DescriptionofClassInfo
    instance.addProperty(
        (CIMProperty(CIM_NAMESPACE_PROPERTY_DESCRIPTIONOFCLASSINFO,
                     DescriptionOfClassInfo)));
    */
    CDEBUG("_buildInstnaceCIMNamespace properties built. returning instance");
    PEG_METHOD_EXIT();
    return(instance);
}
void _validateCIMNamespaceKeys(const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateCIMNamespaceKeys");

    Boolean valid = true;
    CIMName propertyName;
    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName ()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }
    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }
    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                CIM_OBJECTMANAGER_CLASSNAME.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    // ATTN: This one still a problem.  We have to get the name first
    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!_validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_NAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_NAME;
        valid = false;
    }

    if (false)
    {
        PEG_METHOD_EXIT();
        throw CIMInvalidParameterException(
            "Invalid key property: " + propertyName.getString());
    }
    PEG_METHOD_EXIT();
}

void _validateCIMNamespaceKeys(const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateCIMNamespaceKeys");
    Boolean valid = true;
    CIMName propertyName;
    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName ()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }

    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }

    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                CIM_OBJECTMANAGER_CLASSNAME.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    // ATTN: This one still a problem.  We have to get the name first
    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!_validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_NAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_NAME;
        valid = false;
    }

    if (false)
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "Invalid CIM_Namespace Key Property " +  propertyName.getString());
        PEG_METHOD_EXIT();
        throw CIMInvalidParameterException(
            "Invalid CIM_Namespace key property: " + propertyName.getString());
    }
    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "CIM_Namespace Keys Valid");
    PEG_METHOD_EXIT();
}

/* Given a namespace name, class and instance build the instance path for a
   the object.  This builds all components of the path
   @param namespace name to build
   @return CIMObjectPath containing namespace, class and keybinding 
   components of path
   @exceptions - TBD
*/
// ATTN: KS Build path from instance and instance from class.  Not sure
//   we want to always do this.  Consider change to build keys directly

CIMObjectPath InteropProvider::_buildInstancePath(const CIMNamespaceName& name,
                                           const CIMName className, 
                                           const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePath");

    // get the class CIM_Namespace class to use in building path

    // Exception out if Class does not exist in this namespace
    CIMClass thisClass = _getClass(_operationNamespace, className);

    // XmlWriter::printInstanceElement(instance);
    
    CIMObjectPath ref = instance.buildPath(thisClass);
    
    CDEBUG("_buildInstancePath. Built path. path = " << ref.toString() );
    PEG_METHOD_EXIT();
    return(ref);
}


/* _isNamespace determines if the namespace in the second
   parameter is in the array in the first parameter.
    @param array of possible namespaces
    @param canidate namespace
    @return - true if found
*/
Boolean _isNamespace(
        Array<CIMNamespaceName>& namespaceNames,
	CIMNamespaceName& namespaceName)

{
     Boolean found = false;
     for(Uint32 i = 0; i < namespaceNames.size(); i++)
     {
        if(namespaceNames[i].equal ( namespaceName))
        {
            return true;
        }
     }
     return false;
}

Boolean _isChild(
        CIMNamespaceName& parentNamespaceName,
	CIMNamespaceName& namespaceName)

{
    String parent = parentNamespaceName.getString();
    String child = namespaceName.getString();
   //
   //  If length of namespace name is shorter than or equal to the
   //  length of parent namespace name, cannot be a child
   //
   if (child.size () <= parent.size ())
   {
      return false;
   }

   //
   //  Compare prefix substring of namespace name with parent namespace name
   //
   else if (String::equalNoCase (child.subString (0, parent.size ()), parent))
   {
      return true;
   }
   return false;
}
//**************************************************************
// Overloaded functions to get key value with different params
//**************************************************************

/* find the name key in the keybindings and return the value.
    Executes exception if the key not found
    @param object path we will search
    @param keyName - Name of the key to find.
    @return value of name property
    @exceptions CIMInvalidParameterException
*/
String _getKeyValue(const CIMObjectPath& instanceName, const CIMName& keyName)
{
    Array<CIMKeyBinding> kbArray = instanceName.getKeyBindings();

    // find the correct key binding
    for (Uint32 i = 0; i < kbArray.size(); i++)
    {
        if (kbArray[i].getName() == keyName)
        {
            return (kbArray[i].getValue());
        }
    }
    throw CIMInvalidParameterException("Invalid key property: " + keyName.getString());
}

String _getKeyValue(const CIMInstance& instance, const CIMName& keyName)
{
    Uint32 pos;
    CIMValue propertyValue;

    pos = instance.findProperty(keyName);
    if (pos == PEG_NOT_FOUND)
    {
       throw CIMPropertyNotFoundException
           (NAMESPACE_PROPERTYNAME.getString());
    }
    
    propertyValue = instance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
    {
       throw CIMInvalidParameterException("Invalid type for property: "
                             + NAMESPACE_PROPERTYNAME.getString());
    }
    String name;
    propertyValue.get(name);
    return(name);
    //ATTN: KS Returns String whereas below returns CIMNamespaceName.
}

/* gets the key value for the __Namespace property "name"
   from the instance provided. Sets childNamespaceName and
   isRelativeName fields
   This overload called if instance provided.
*/
void _getKeyValue (
    const CIMInstance& namespaceInstance,
	CIMNamespaceName& childNamespaceName,
	Boolean& isRelativeName)

{
    //Validate key property
    
    Uint32 pos;
    CIMValue propertyValue;
    
    // [Key, MaxLen (256), Description (
    //       "A string that uniquely identifies the Namespace "
    //       "within the ObjectManager.") ]
    // string Name;
    
    pos = namespaceInstance.findProperty(NAMESPACE_PROPERTYNAME);
    if (pos == PEG_NOT_FOUND)
    {
       throw CIMPropertyNotFoundException
           (NAMESPACE_PROPERTYNAME.getString());
    }
    
    propertyValue = namespaceInstance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
    {
       throw CIMInvalidParameterException("Invalid type for property: "
                             + NAMESPACE_PROPERTYNAME.getString());
    }
    
    String cnsName;
    propertyValue.get(cnsName);
    childNamespaceName = CIMNamespaceName (cnsName);
    
    isRelativeName = !(childNamespaceName.isNull());

}
/* gets the key value for the __Namespace property "name"
   from the instance provided. Sets childNamespaceName and
   isRelativeName fields
   This overload called if object path provided.
*/

void _getKeyValue (
	const CIMObjectPath&  instanceName,
	CIMNamespaceName& childNamespaceName,
	Boolean& isRelativeName)
{

    Array<CIMKeyBinding> kbArray = instanceName.getKeyBindings();
    if ((kbArray.size() == 1) &&
            (kbArray[0].getName() == NAMESPACE_PROPERTYNAME))
    {
       childNamespaceName = CIMNamespaceName (kbArray[0].getValue());
       isRelativeName = !(childNamespaceName.isNull());
    }
    else
    {
       throw CIMInvalidParameterException("Invalid key property:  ");
    }
}

/* generate the full namespace name from the parent and child
   components
   @param namespaceNames - List of all namespaces
   @param parentNamespaceName
   @param childNamespaceName
   @param Boolean isrelative
   @return full namespacename created from parent + child
   Note that if isrelative is true, parent is tested for validty
*/
CIMNamespaceName _generateFullNamespaceName(
        Array<CIMNamespaceName>& namespaceNames,
	CIMNamespaceName& parentNamespaceName,
	CIMNamespaceName& childNamespaceName,
	Boolean isRelativeName)

{
    // If isRelativeName is true, then the parentNamespace
    // MUST exist
    //
    CIMNamespaceName fullNamespaceName;

    if (isRelativeName)
    {
      if (!_isNamespace(namespaceNames, parentNamespaceName))
      {
         throw CIMObjectNotFoundException("Parent namespace does not exist: "
                                  + parentNamespaceName.getString());
      }
      // Create full namespace name by prepending parentNamespaceName
      fullNamespaceName = CIMNamespaceName (parentNamespaceName.getString() 
          + "/" + childNamespaceName.getString());
    }
    else
    {
      fullNamespaceName = parentNamespaceName;
    }
    return(fullNamespaceName);

}

//***************************************************************************
//  The following section is the Instance Operation processors
//***************************************************************************
//                createInstance
//***************************************************************************
void InteropProvider::createInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceReference,
    const CIMInstance& myInstance,
	ObjectPathResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::createInstance()");
        
        CIMNamespaceName childNamespaceName;
        CIMNamespaceName newNamespaceName;
        Boolean isRelativeName;
        CDEBUG("CreateInstance " << instanceReference.toString());
        // operation namespace needed internally to get class.
        _operationNamespace = instanceReference.getNameSpace();
        
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceReference.getClassName());    
        CDEBUG("Class Validated");
        String userName = _validateUserID(context);
        CIMObjectPath newInstanceReference;
        CDEBUG("UserIDValidated");
        if ((classEnum == CIM_OBJECTMANAGER) || 
            (classEnum == CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM)|| 
            (classEnum == CIM_CIMXMLCOMMUNICATIONMECHANISM))
            throw CIMNotSupportedException("InteropProvider, Create Not allowed");

        CDEBUG("CreateInstance: No test for CIM_NamepsaceClass");
        if (classEnum == CIM_NAMESPACE)
        {
            CDEBUG("Create Class from CIM_Namespace");

            // Validate that keys are as required. Does its own exception.
            _validateCIMNamespaceKeys(myInstance);
            CDEBUG("_validateCIMNamespaceKeys passed");
            String namespaceName;
            newNamespaceName = _getKeyValue(myInstance, CIM_NAMESPACE_PROPERTY_NAME);
            CDEBUG("CreateInstance: rtn form _getKeyValue ");

            CIMInstance instance = _buildInstanceCIMNamespace(namespaceName);
            CDEBUG("CreateInstance:Rtn from _BuildInstanceCIMNamespace for namespace= " << namespaceName);
            newInstanceReference = _buildInstancePath(CIMNamespaceName(namespaceName),
                                        CIM_NAMESPACE_CLASSNAME, instance);
            CDEBUG("CreateInstance:Go to common create functions");
        }

        else   // Process the __Namespace request to get namespace name value
        {
            CDEBUG("CreateInstance: Fell through to __Namespace processing");
            _getKeyValue(myInstance, childNamespaceName, isRelativeName);
            CIMNamespaceName parentNamespaceName = instanceReference.getNameSpace();
            
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "childNamespaceName = " + childNamespaceName.getString() +
               ", isRelativeName = " +
               (isRelativeName?String("true"):String("false")) +
               ", parentNamespaceName = " + parentNamespaceName.getString());
            CDEBUG("__Namespace Process. parent= " << parentNamespaceName.getString() );
            Array<CIMNamespaceName> namespaceNames;
            namespaceNames = _enumerateNameSpaces();
            
            newNamespaceName = _generateFullNamespaceName(
                namespaceNames, parentNamespaceName,
                         childNamespaceName, isRelativeName);
            
            // return key (i.e., CIMObjectPath) for newly created namespace
            
            Array<CIMKeyBinding> keyBindings;
            keyBindings.append(CIMKeyBinding(NAMESPACE_PROPERTYNAME,
                 isRelativeName?childNamespaceName.getString():
                                    parentNamespaceName.getString(),
                                         CIMKeyBinding::STRING));
            //Add namespace class and keybindings
            newInstanceReference.set(String::EMPTY, parentNamespaceName,
                                         __NAMESPACE_CLASSNAME, keyBindings);
        }
        // Create the new namespace
        try
        {
            CDEBUG("Create namespace " << newNamespaceName.getString());
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "Namespace = " + newNamespaceName.getString() + 
                    " to be created.");
            
            _repository->createNameSpace(newNamespaceName);
        
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "Namespace = " + newNamespaceName.getString() + 
                    " successfully created.");
            // ATTN: Add standardlog entry here.
        }
        catch(CIMException& e)
        {
           PEG_METHOD_EXIT();
           throw e;
        }
        catch(Exception& e)
        {
           PEG_METHOD_EXIT();
           throw e;
        }
        
        // begin processing the request
        handler.processing();


       handler.deliver(newInstanceReference);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return;
   }

//***************************************************************************
//                deleteInstance
//***************************************************************************
void InteropProvider::deleteInstance(
	const OperationContext & context,
	const CIMObjectPath & instanceName,
	ResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::deleteInstance");
        CDEBUG("deleteInstance" << instanceName.toString());
        CIMNamespaceName childNamespaceName;
        CIMNamespaceName deleteNamespaceName;
        Boolean isRelativeName;
        
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceName.getClassName());    
        
        String userName = _validateUserID(context);

        Array<CIMNamespaceName> namespaceNames;
        namespaceNames = _enumerateNameSpaces();

        if ((classEnum == CIM_OBJECTMANAGER) || 
            (classEnum == CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM)|| 
            (classEnum == CIM_CIMXMLCOMMUNICATIONMECHANISM))
            throw CIMNotSupportedException("Delete Not allowed");

        if (classEnum == CIM_NAMESPACE)
        {
            // validate requred keys.  Exception out if not valid
            _validateCIMNamespaceKeys(instanceName);
            
            deleteNamespaceName = _getKeyValue(instanceName, CIM_NAMESPACE_PROPERTY_NAME);
            CDEBUG("Delete namespace = " << deleteNamespaceName );
        }
        else  // Procesing for __namespace
        {
    
           _getKeyValue(instanceName, childNamespaceName, isRelativeName);
           CIMNamespaceName parentNamespaceName = instanceName.getNameSpace();
    
           PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
    	       "childNamespaceName = " + childNamespaceName.getString() +
    	       (isRelativeName?String("true"):String("false")) +
    	       ", parentNamespaceName = " + parentNamespaceName.getString());
    
           // begin processing the request
    
           deleteNamespaceName = _generateFullNamespaceName(
               namespaceNames, parentNamespaceName,
                         childNamespaceName, isRelativeName);
        }

	    // ATTN: KS Why THis??? 
        if (deleteNamespaceName.equal (ROOTNS))
       {
           throw CIMNotSupportedException("root namespace cannot be deleted.");
       }

	   _repository->deleteNameSpace(deleteNamespaceName);

	   PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
	       "Namespace = " + deleteNamespaceName.getString() + 
               " successfully deleted.");

       Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
           "Interop Provider Delete Namespace: $0",
           deleteNamespaceName.getString());

       handler.processing();

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return ;
    }

//***************************************************************************
//                getInstance
//***************************************************************************
void InteropProvider::getInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceName,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & properatyList,
    InstanceResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::getInstance");
        
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceName.getClassName());    
        
        String userName = _validateUserID(context);
        
        // begin processing the request
        handler.processing();
        if (classEnum == CIM_OBJECTMANAGER)
        {
            CIMInstance instance = _buildInstanceCIMObjectManager();
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        
        if (classEnum == CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM)
        {
            // At this point there are no instances of this class.
            // It is specialized to CIMXMLCommunicationMechanism
            //CIMInstance instance = _buildInstancCIMObjectCommunicationMechanism();
            //handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }


        if (classEnum == CIM_CIMXMLCOMMUNICATIONMECHANISM)
        {
            CIMInstance instance = _buildInstancCIMXMLCommunicationMechanism();
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;

        }
        
        // Get List of namespaces
        Array<CIMNamespaceName> namespaceNames;
        namespaceNames = _enumerateNameSpaces();
        CIMInstance instance;

        
        if (classEnum == CIM_NAMESPACE)
        {
            // Not clear what we have to take into account here.
            // get the namespace from the name value.
            // should check the other keys to see if valid.
            CIMNamespaceName namespaceName;
            namespaceName = _getKeyValue(instanceName, CIM_NAMESPACE_PROPERTY_NAME);
            // ATTN: Why this CIMNamespaceName parentNamespaceName = instanceName.getNameSpace();
            
            if (!_isNamespace(namespaceNames, namespaceName))
            {
                throw CIMObjectNotFoundException("Namespace does not exist: "
                                     + namespaceName.getString());
            }
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "Namespace = " + namespaceName.getString() + " successfully found.");
            instance = _buildInstanceCIMNamespace(namespaceName);
        }
        else  // processing for __Namespace
        {
            CIMNamespaceName childNamespaceName;
            CIMNamespaceName getNamespaceName;
            Boolean isRelativeName;
            
            _getKeyValue(instanceName, childNamespaceName, isRelativeName);
            CIMNamespaceName parentNamespaceName = instanceName.getNameSpace();
            
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "childNamespaceName = " + childNamespaceName.getString() +
               (isRelativeName?String("true"):String("false")) +
               ", parentNamespaceName = " + parentNamespaceName.getString());
            
            
            getNamespaceName = _generateFullNamespaceName(
                namespaceNames, parentNamespaceName,
                         childNamespaceName, isRelativeName);
            
            // exception if not valid namespace
            if (!_isNamespace(namespaceNames, getNamespaceName))
            {
              throw CIMObjectNotFoundException("Namespace deos not exist: "
                                     + getNamespaceName.getString());
            }
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "Namespace = " + getNamespaceName.getString() + 
                   " successfully found.");
            
            //Set name of class
            CIMInstance instance(__NAMESPACE_CLASSNAME);
            
            //
            // construct the instance
            //
            instance.addProperty(CIMProperty(NAMESPACE_PROPERTYNAME,
            isRelativeName?childNamespaceName.getString():
                              parentNamespaceName.getString()));
            //instance.setPath(instanceName);
       }

       handler.deliver(instance);

       // complete processing the request
       handler.complete();

       PEG_METHOD_EXIT();
       return ;
    }

//***************************************************************************
//                enumerateInstances
//***************************************************************************
void InteropProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler & handler)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::enumerateInstances()");
        // Verify that ClassName is correct and get value

        targetClass classEnum  = _verifyValidClassInput(ref.getClassName());    
        
        // operation namespace needed internally to get class.
        _operationNamespace = ref.getNameSpace();
        //String userName = _validateUserID(context);
        
        // The following 3 classes deliver a single instance because
        // that is all there is today.
        if (classEnum == CIM_OBJECTMANAGER)
        {
            CIMInstance instance = _buildInstanceCIMObjectManager();
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        
        if (classEnum == CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM)
        {
            // At this point there are instances of this class.  It is specialized
            // to the CIMXMLCommunicationMechanism class.
            //CIMInstance instance = _buildInstancCIMObjectCommunicationMechanism();
            //handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }

        if (classEnum == CIM_CIMXMLCOMMUNICATIONMECHANISM)
        {
            CIMInstance instance = _buildInstancCIMXMLCommunicationMechanism();
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;

        }
        
        // ATTN: Fix this up.  should not be here.
        CIMNamespaceName parentNamespaceName = ref.getNameSpace();

        // ATTN KS Fix this so references both types of namespace
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
           "parentNamespaceName = " + parentNamespaceName.getString());
        
        // begin processing the request
        handler.processing();
        
        Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();
        
        Array<CIMInstance> instanceArray;
        CDEBUG("Found " << namespaceNames.size() << " namespaces.");
        // Build response objects based on class requested
        for (Uint32 i = 0; i < namespaceNames.size(); i++)
        {
            CDEBUG("For namespace' " << namespaceNames[i].getString());
            CDEBUG("Evaluate ClassEnum" << classEnum);
            if (classEnum == CIM_NAMESPACE)
            {
                CDEBUG("Evaluate CIM_Namespace" << classEnum);
                // Create a valid CIM_Namespace Instance
                CIMInstance instance = _buildInstanceCIMNamespace(namespaceNames[i]);
                instanceArray.append(instance);
                
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "Namespace = " + namespaceNames[i].getString());
            
            }
            else // the else covers __NAMESPACE
            {
                // Build the instances. For now simply build the __Namespace instances
                // the only property is name.
                if (_isChild(parentNamespaceName, namespaceNames[i]))
                {
                    CIMInstance instance(__NAMESPACE_CLASSNAME);
                    instance.addProperty(
                        (CIMProperty(NAMESPACE_PROPERTYNAME,
                        namespaceNames[i].getString().subString
                            (parentNamespaceName.getString().size()+1,
                            namespaceNames[i].getString().size()-
                        parentNamespaceName.getString().size()-1))));

                    instanceArray.append(instance);

                    //instance.setPath(instanceName);
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                    "childNamespace = " + namespaceNames[i].getString());
                }
            }
        }
        handler.deliver(instanceArray);
        
        // complete processing the request
        handler.complete();
        
        PEG_METHOD_EXIT();
    }

//***************************************************************************
//                enumerateInstanceNames
//***************************************************************************

void InteropProvider::enumerateInstanceNames(
	const OperationContext & context,
	const CIMObjectPath & classReference,
        ObjectPathResponseHandler & handler)
    {
    	PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
                "InteropProvider::enumerateInstanceNames()");

        // operation namespace needed internally to get class.
        _operationNamespace = classReference.getNameSpace();

        targetClass classEnum  = _verifyValidClassInput(classReference.getClassName());    
    
        String userName = _validateUserID(context);
        // begin processing the request
        handler.processing();
        
        // The following 3 classes deliver a single instance because
        // that is all there is today.
        if (classEnum == CIM_OBJECTMANAGER)
        {
            CIMInstance instance = _buildInstanceCIMObjectManager();
            CIMObjectPath ref = _buildInstancePath(CIMNamespaceName(),
                CIM_OBJECTMANAGER_CLASSNAME, instance);
            handler.deliver(ref);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        
        if (classEnum == CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM)
        {
            // At this point there are no instances of this class. It is specialized
            // to CIMXMLCommunicationMechanism.
            //CIMInstance instance = _buildInstancCIMObjectCommunicationMechanism();
            //CIMObjectPath ref = _buildInstancePath(CIMNamespaceName(),
            //    CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME, instance);
            //handler.deliver(ref);
            //handler.complete();
            PEG_METHOD_EXIT();
            return;
        }


        if (classEnum == CIM_CIMXMLCOMMUNICATIONMECHANISM)
        {
            CIMInstance instance = _buildInstancCIMXMLCommunicationMechanism();
            CIMObjectPath ref = _buildInstancePath(CIMNamespaceName(),
                CIM_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME, instance);
            handler.deliver(ref);
            handler.complete();
            PEG_METHOD_EXIT();
            return;

        }
    	

        // ATTN: Move this trace
        CIMNamespaceName parentNamespaceName = classReference.getNameSpace();
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
           "parentNamespaceName = " + parentNamespaceName.getString());
        CDEBUG("Enumerate Instance Names. ns = " << parentNamespaceName.getString());
        
        // Get list of all namespaces
        Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();

        // Build the cimObjectPath for each namespace found
        for (Uint32 i = 0; i < namespaceNames.size(); i++)
        {
            if (classEnum == CIM_NAMESPACE)
            {
                
                CDEBUG("Calling BuildInstancePath for "<< namespaceNames[i].getString() );
                CIMInstance instance = _buildInstanceCIMNamespace(namespaceNames[i]);
                CIMObjectPath ref = _buildInstancePath(CIMNamespaceName(namespaceNames[i]),
                                            CIM_NAMESPACE_CLASSNAME, instance);
    
                handler.deliver(ref);
    
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                    "namespace = " + namespaceNames[i].getString());
            }
            else
            {
                Array<CIMKeyBinding> keyBindings;
                // Build the __Namespace objectpath
                // Note that for the moment, the only property is name.
                if (_isChild(parentNamespaceName, namespaceNames[i]))
                {
                    keyBindings.clear();
                      keyBindings.append(CIMKeyBinding(NAMESPACE_PROPERTYNAME,
                          namespaceNames[i].getString().subString
                          (parentNamespaceName.getString().size()+1,
                          namespaceNames[i].getString().size()-
                          parentNamespaceName.getString().size()-1),
                          CIMKeyBinding::STRING));
        
                      CIMObjectPath ref(String::EMPTY, parentNamespaceName,
                      __NAMESPACE_CLASSNAME, keyBindings);
        
                      handler.deliver(ref);
                      PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                          "childNamespace = " + namespaceNames[i].getString());
                }
            }
        }
    
    	handler.complete();
    
        PEG_METHOD_EXIT();
    }


//**************************************************************
//**************************************************************
// Association Functions
//**************************************************************
//**************************************************************

void InteropProvider::associators(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	throw CIMNotSupportedException("AssociationProvider::associators");
}

void InteropProvider::associatorNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & associationClass,
	const CIMName & resultClass,
	const String & role,
	const String & resultRole,
	ObjectPathResponseHandler & handler)
{
	throw CIMNotSupportedException("AssociationProvider::associatorNames");
}

void InteropProvider::references(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	ObjectResponseHandler & handler)
{
	throw CIMNotSupportedException("AssociationProvider::references");
}

void InteropProvider::referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	ObjectPathResponseHandler & handler)
{
	throw CIMNotSupportedException("AssociationProvider::referenceNames");
}

PEGASUS_NAMESPACE_END
// END_OF_FILE
