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

/* TODO: 12 Feb 2004
    Add the association functions
    UUID generation should become a system function since it will be used
    by many providers, etc. as part of id generation.
*/

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include "InteropProvider.h"
#include "Guid.h"
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
#include <Pegasus/Config/ConfigManager.h>

#include <sstream>
#include <string>

#include <stdlib.h>

//The following include is needed for gethostbyname
#if defined(PEGASUS_OS_TYPE_WINDOWS)
#include <objbase.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define CDEBUG(X)
//#define CDEBUG(X) PEGASUS_STD(cout) << "InteropProvider " << X << PEGASUS_STD(endl)
//#define CDEBUG(X) Logger::put (Logger::DEBUG_LOG, "Linux_ProcessorProvider", Logger::INFORMATION, "$0", X)
//#define CDEBUG(X) {std::stringstream ss; std::string r;ss << X;ss>>r; PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, r)}
 

//**************************************************************************
//
//    Constants representing the class names processed
//
//**************************************************************************

// The following should be moved to somewhere like constants.
static const String PegasusInstanceIDGlobalPrefix = "PEG";

/**
    The constants representing the class names we process
*/
static const CIMName __NAMESPACE_CLASSNAME  = CIMName ("__Namespace");
static const CIMName CIM_NAMESPACE_CLASSNAME  = CIMName ("CIM_Namespace");
static const CIMName CIM_OBJECTMANAGER_CLASSNAME  = CIMName ("CIM_ObjectManager");
static const CIMName CIM_OBJECTMANAGERCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("CIM_ObjectManagerCommunicationMechanism");
static const CIMName PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME  = 
        CIMName ("PG_CIMXMLCommunicationMechanism");
static const CIMName CIM_COMMMECHANISMFORMANAGER_CLASSNAME  = 
        CIMName ("CIM_CommMechanismForManager");
static const CIMName CIM_NAMESPACEINMANAGER_CLASSNAME  = 
        CIMName ("CIM_NamespaceInManager");

// Property Names for __Namespace Class
static const CIMName NAMESPACE_PROPERTYNAME  = CIMName ("Name");
static const CIMNamespaceName ROOTNS  = CIMNamespaceName ("root");


// Property names for CIM_ObjectManager Class
static const CIMName OM_GATHERSTATISTICALDATA  =
 CIMName ("GatherStatisticalData");
// Property for the slp template.
static const CIMName OM_DESCRIPTIONPROPERTY =
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
     PG_CIMXMLCOMMUNICATIONMECHANISM = 4
     };

 enum targetAssocClass{
     CIM_NAMESPACEINMANAGER =1,
     CIM_COMMMECHANISMFORMANAGER=2
 };

//***************************************************************
// Provider Utility Functions
//***************************************************************
/*
void _removeQualifiers(CIMProperty& p)
{

    Uint32 count;
    while((count = p.getQualifierCount()) > 0)
        p.removeQualifier(count - 1);
}
void _removeQualifiers(CIMInstance& cimInstance)
{
    // remove qualifiers of the class
    Uint32 count;
    while((count = cimInstance.getQualifierCount()) > 0)
        cimInstance.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimInstance.getPropertyCount(); i++)
    {
        _removeQualifiers(cimInstance.getProperty(i));
    }
}
*/

/** get one string property from an instance.
    @param instance CIMInstance from which we get property value
    @param propertyName String name of the property containing the value
    @param default String optional parameter that is substituted if the property does
    not exist, is Null, or is not a string type. The substitute is String::EMPTY
    @return String value found or defaultValue.
*/
String _getPropertyValue(const CIMInstance& instance, const CIMName& propertyName, const String& defaultValue = String::EMPTY)
{
    String output;
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_STRING)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(output);
            else
                output = defaultValue;
        }
        else
            output = defaultValue;
    }
    else
        output = defaultValue;
    return(output);
}
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
String _getHostAddress(String hostName)
{
  // set default address
  String ipAddress("127.0.0.1");
  
  if (hostName == String::EMPTY)
    	hostName = System::getHostName();

  struct hostent * phostent;
  struct in_addr   inaddr;
  
  if ((phostent = ::gethostbyname((const char *)hostName.getCString())) != NULL)
    {
     ::memcpy( &inaddr, phostent->h_addr,4);
      ipAddress = ::inet_ntoa( inaddr );
    }
  return ipAddress;
}

 Array<String> _getFunctionalProfiles(Array<Uint16> profiles)
 {
     Array<String> profileDescriptions;
     profiles.append(2); profileDescriptions.append("Basic Read");
     profiles.append(3); profileDescriptions.append("Basic Write");
     profiles.append(4); profileDescriptions.append("Schema Manipulation");
     profiles.append(5); profileDescriptions.append("Instance Manipulation");
     profiles.append(6); profileDescriptions.append("Association Traversal");
     profiles.append(8); profileDescriptions.append("Qualifier Declaration");
     profiles.append(9); profileDescriptions.append("Indications");
     return(profileDescriptions);
 }

/*  get the prefix that will be part of the cimom identification
    This can be either the default PEG or if the environment
    variable PEGASUS_TRADEMARK_PREFIX is defined this is used.
    NOTE: getting from the environment variable is a hack until
    we can come up with a better source.
    @return String containing the unique name for the CIMOM ID
*/

String getTrademarkCIMOMIDPrefix()
{

    char * trademark;
    trademark = getenv("PEGASUS_TRADEMARK_PREFIX");
    return((trademark)? trademark : PegasusInstanceIDGlobalPrefix);
}
/** Builds the UUID string for this CIMOM.
**/
String getUUIDString()
{
    return(Guid::getGuid());
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

/* Validate that the property exists, is string type and
   optionally the value itself. Note processes only String
   properties.
   @param Instance to search for property.
   @param CIMName containing property Name
   @value String containing value. If not String::EMPTY, compare to 
   value in the property
   @return True if passes all tests
*/
Boolean _validateRequiredProperty(const CIMInstance& instance, 
                          const CIMName& propertyName,
                          const String& value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateRequiredProperty()");
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
    const CIMValue theValue = theProperty.getValue ();
    //
    //  ATTN: Required property must have a non-null value
    //
    if ((theValue.getType() != CIMTYPE_STRING) || (theValue.isNull()))
    {
        PEG_METHOD_EXIT();
        return(false);
    }

    String valueField;
    theValue.get(valueField);
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
    //  ATTN:Required property must have a non-null value
    //
    if ((theValue.getType() != CIMTYPE_UINT16) 
        || (theValue.isNull()) ) 
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
   @return Array<CIMNamespaceName> with all namespaces
   @exception Passes all exception that repository may generate.
*/        
Array<CIMNamespaceName> InteropProvider::_enumerateNameSpaces()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_enumerateNameSpaces()");

    Array<CIMNamespaceName> namespaceNames;
    
    namespaceNames = _repository->enumerateNameSpaces();

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

    CIMClass myClass = _repository->getClass(nameSpace, className,
                            false,true,true);
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
    if (className.equal(CIM_OBJECTMANAGER_CLASSNAME))
        return CIM_OBJECTMANAGER;

    if (className.equal(PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME))
        return PG_CIMXMLCOMMUNICATIONMECHANISM;

    // Last entry, reverse test and return OK if CIM_Namespace
    if (!className.equal(CIM_NAMESPACE_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    return CIM_NAMESPACE;
}

targetAssocClass _verifyValidAssocClassInput(const CIMName& className)
{
    if (className.equal(CIM_NAMESPACEINMANAGER_CLASSNAME))
        return CIM_NAMESPACEINMANAGER;
    // Last entry, reverse test and return OK if CIM_Namespace
    if (!className.equal(CIM_COMMMECHANISMFORMANAGER_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    return CIM_COMMMECHANISMFORMANAGER;
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

/** set the value of a property defined by property name in the instance provided.
    Sets a String into the value field unless the property name cannot be found.
    If the property cannot be found, it simply returns.
    ATTN: This function does not pass an error back if property not found.
    @param instance CIMInstance in which to set property value
    @param propertyName CIMName of property in which value will be set.
    @param value String value to set into property
    
*/
void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const String& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const Boolean& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const Uint16& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const Array<String>& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const Array<Uint16>& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName propertyName, const CIMObjectPath& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

/** add the correct values to the common keys defined for all of the classes. This is
    systemcreationclassname and systemname
    Note that if the properties do not exist, we simply ignore them.
*/
void _fixInstanceCommonKeys(CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_fixInstanceCommonKeys()");
    String SystemCreationClassName = System::getSystemCreationClassName ();
    if (SystemCreationClassName == String::EMPTY)
    {
        //Attn: Get this globally. For now This in place because global is often Empty
        SystemCreationClassName = "CIM_ComputerSystem";
    }

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,SystemCreationClassName);

    // Add property SystemName

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_SYSTEMNAME,System::getHostName());
    PEG_METHOD_EXIT();
}


/** builds one instance of the class named className. Gets Class defintion and f
    fills in the correct properties from the class.  This requires a repository
    getclass request for each instance built. The skeleton is built by
    creating the instance and copying qualifiers and properties from
    the class. Finally the instance is cloned to separate it from the
    original objects.
    NOTE: This is very inefficient for anything larger than a few instances.
    We should separate the get from the createSkeleton.
    @param className CIMName of the class for which the instance is to be built
    @return CIMInstance of this class with properties complete.
    @exception passes on any exceptions received from the repository request.
*/
CIMInstance InteropProvider::_buildInstanceSkeleton(const CIMName& className)
{
    CIMClass myClass;
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceSkeleton()");

    CIMInstance skeleton(className);
        myClass = _repository->getClass(_operationNamespace, className, false, true, true);
    
    // copy the qualifiers
    for (Uint32 i = 0 ; i < myClass.getQualifierCount() ; i++)
        skeleton.addQualifier(myClass.getQualifier(i));

    // copy the properties
    for (Uint32 i = 0 ; i < myClass.getPropertyCount() ; i++)
        skeleton.addProperty(myClass.getProperty(i));

    PEG_METHOD_EXIT();
    return(skeleton.clone());
}

/* build a single instance of the cimxmlcommunicationmechanism class
   using the parameter provided as the name property
   @parm name String representing the name to be used for this object.
   @return CIMInstance of the class
*/
CIMInstance InteropProvider::_buildInstancePGCIMXMLCommunicationMechanism(
                                            const String& namespaceType,
                                            const String& IPAddress,
                                            const Boolean& includeQualifiers,
                                            const Boolean& includeClassOrigin,
                                            const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePGCIMXMLCommunicationMechanism()");
    
    CIMInstance instance = _buildInstanceSkeleton(PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME);

    _fixInstanceCommonKeys(instance);

    //CreationClassName
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME.getString());

    //Name, this CommunicationMechanism.
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME, PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME.getString());

    // CommunicationMechanism Property
    _setPropertyValue(instance, OM_COMMUNICATIONMECHANISM, Uint16(2));

    //Functional Profiles Supported Property.
    Array<Uint16> profiles;
    Array<String> profileDescriptions = _getFunctionalProfiles(profiles);

    // Set functional profiles in instance
    _setPropertyValue(instance, OM_FUNCTIONALPROFILESSUPPORTED, profiles);

    _setPropertyValue(instance, OM_FUNCTIONALPROFILEDESCRIPTIONS, profileDescriptions);

    // Multiple OperationsSupported Property
    _setPropertyValue(instance, OM_MULTIPLEOPERATIONSSUPPORTED, false);

    // AuthenticationMechanismsSupported Property
    Array<Uint16> authentications;
    Array<String> authenticationDescriptions;

    authentications.append(3); authenticationDescriptions.append("Basic");

    _setPropertyValue(instance, OM_AUTHENTICATIONMECHANISMSSUPPORTED, authentications);

    _setPropertyValue(instance, OM_AUTHENTICATIONMECHANISMDESCRIPTIONS, authenticationDescriptions);
    
    _setPropertyValue(instance, OM_VERSION, CIMXMLProtocolVersion);

    _setPropertyValue(instance, "namespaceType", namespaceType);

    _setPropertyValue(instance, "IPAddress", IPAddress);

    PEG_METHOD_EXIT();
    return(instance);
}

Array<CIMInstance> InteropProvider::_buildInstancesPGCIMXMLCommunicationMechanism(
                                            const Boolean includeQualifiers,
                                            const Boolean includeClassOrigin,
                                            const CIMPropertyList& propertyList)
{
    // This is a temporary hack to get the multiple connections.
    ConfigManager* configManager = ConfigManager::getInstance();
    Boolean enableHttpConnection = String::equal(
        configManager->getCurrentValue("enableHttpConnection"), "true");
    Boolean enableHttpsConnection = String::equal(
        configManager->getCurrentValue("enableHttpsConnection"), "true");

    String IPAddress = _getHostAddress(System::getHostName());
    Array<CIMInstance> instances;

    if (enableHttpConnection)
    {
        CDEBUG("building pgcimxmlinstances 1");
        CIMInstance instance = _buildInstancePGCIMXMLCommunicationMechanism(
                            "http", IPAddress,
                            includeQualifiers,
                            includeClassOrigin,
                            propertyList);
        instances.append(instance);
    }

    if (enableHttpsConnection)
    {
        CDEBUG("building pgcimxmlinstances 2");
        CIMInstance instance = _buildInstancePGCIMXMLCommunicationMechanism(
                                                "https", IPAddress,
                                                includeQualifiers,
                                                includeClassOrigin,
                                                propertyList);
        instances.append(instance);
    }
    return(instances);
}
/*  Gets the value for the CIMObjectManager name.  This is a key
    property with the following characteristics.
    1. It is persistent. This must be persistent through CIMOM
    restarts.  We will save it in the instance database to achieve this. 
    2. It must be unique. We cannot create duplicate CIMOM names
    3. It is based on the DMTF description of how unique InstanceIds
    are defined (Trademark/etc followed by unique identification.
    Use the Global constant PegasusInstanceIDGlobalPrefix as the
    prefix allowing this to be changed.
*/
String buildObjectManagerName()
{
    return(getTrademarkCIMOMIDPrefix() + ":" + getUUIDString());
}

/** build an instance of the CIM_ObjectManager class filling out
    the required properties
    @param includeQualifiers Boolean
    @param includeClassOrigin Boolean
    @param propertylist CIMPropertyList 
    @return CIMInstance with a single built instance of the class
    @exception repository instances if exception to enumerateInstances
        for this class.
*/

CIMInstance InteropProvider::_buildInstanceCIMObjectManager(
                        const Boolean includeQualifiers,
                        const Boolean includeClassOrigin,
                        const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMObjectManager()");

    // If there is already an instance of this class, use it locally use it.
    if (!instanceOfCIMObjectManager.isUninitialized())
    {
        PEG_METHOD_EXIT();
        return(instanceOfCIMObjectManager);
    }

    // Try to get persistent instance from repository
    Array<CIMInstance> instances;
    instances = _repository->enumerateInstances(_operationNamespace,
                  CIM_OBJECTMANAGER_CLASSNAME, true, false, includeQualifiers,
                    includeClassOrigin, propertyList);
    
    // ATTN: KS How do we really account for multiple instances of 
    // this class and determine which one is really us.
    if (instances.size() >= 1)
    {
        instanceOfCIMObjectManager = instances[0];
        PEG_METHOD_EXIT();
        return(instanceOfCIMObjectManager);
    }

    //
    // Repository empty. Must build new instance and save it.
    //
    CIMInstance instance = _buildInstanceSkeleton(CIM_OBJECTMANAGER_CLASSNAME);

    _fixInstanceCommonKeys(instance);
    instanceOfCIMObjectManager = instance;

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,CIM_OBJECTMANAGER_CLASSNAME.getString());

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,buildObjectManagerName());

    _setPropertyValue(instance, CIMName("ElementName"), String("Pegasus"));

    //Description property this object manager instance
    // default is Pegasus CIM_Server Version
    String description;
    char * envDescription;
    envDescription = getenv("PEGASUS_CIMOM_DESCRIPTION");

    if (envDescription)
        description = envDescription;
    else
        description = "Pegasus " + String(PEGASUS_NAME) + "Version " + String(PEGASUS_VERSION);

    _setPropertyValue(instance, CIMName("Description"), description);

    //Property GatherStatisticalData. Note that today we do not
    // have a dynamic activation for this value.

#ifdef PEGASUS_HAS_PERFINST
    Boolean gatherStatData = true;
#else
    Boolean gatherStatData = false;
#endif

    _setPropertyValue(instance, OM_GATHERSTATISTICALDATA, Boolean(gatherStatData));
    
    // write the instance to the repository
    CIMObjectPath instancepath;
    try
    {
        instancepath = _repository->createInstance(_operationNamespace,
                       instanceOfCIMObjectManager );
    }
    catch(CIMException& e)
    {
        // ATTN: KS generate log error if this not possible
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

/** Get the instances of CIM_Namespace
    ATTN: KS For the moment we build them.  We really want to get them from the repository and 
    compare with the existing namespaces.
*/
Array<CIMInstance> InteropProvider::_getInstancesCIMNamespace(const Boolean& includeQualifiers,
                            const Boolean& includeClassOrigin,
                            const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");
    CDEBUG("_getinstancesCIMNamespace");
    Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();

    Array<CIMInstance> instanceArray;

    for (Uint32 i = 0; i < namespaceNames.size(); i++)
    {
       instanceArray.append( _buildInstanceCIMNamespace(namespaceNames[i]));
    }
    PEG_METHOD_EXIT();
    return(instanceArray);
}

/** get the instance of namespace defined by the input parameter which is the object path
    for the instance required.
    ATTN: Note that this is incorrect. We are supplying the namespace name and need to supply
    the objectpath
    @param TBD
    @return CIMInstance with the found instance or CIMInstance() if nothing found.
*/
CIMInstance InteropProvider::_getInstanceCIMNamespace(const CIMNamespaceName & nameSpace)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");

    CDEBUG("_getinstanceCIMNamespace Gets ONE only from Namespace=" << nameSpace.getString());
    Array<CIMInstance> instances = _getInstancesCIMNamespace(true, true, CIMPropertyList());

    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        if (_getPropertyValue(instances[i], CIMName("name")) == nameSpace.getString())
        {
            PEG_METHOD_EXIT();
            return(instances[i]);
        }
    }
    PEG_METHOD_EXIT();
    CIMInstance nullInstance;
    return(nullInstance);
}

CIMObjectPath InteropProvider::_buildReference(const CIMInstance& instance, const CIMName& className)

{
    return(_buildObjectPath(_operationNamespace,className, instance));
}

Array<CIMInstance> InteropProvider::_buildInstancesNamespaceInManager()
{

    Array<CIMInstance> namespaceInstances = _getInstancesCIMNamespace(false,
                            false, CIMPropertyList());

    CIMInstance instanceObjMgr = _buildInstanceCIMObjectManager( true, true, CIMPropertyList());

    CIMObjectPath refObjMgr = _buildReference(instanceObjMgr, CIM_OBJECTMANAGER_CLASSNAME);

    Array<CIMInstance> assocInstances;

    for (Uint32 i = 0 ; i < namespaceInstances.size() ; i++)
    {
        CIMInstance instance = _buildInstanceSkeleton(CIM_NAMESPACEINMANAGER_CLASSNAME);

        _setPropertyValue(instance, CIMName("Antecdent"), refObjMgr);
        //ATTNATTN: this is weak qualifier.
        _setPropertyValue(instance, CIMName("Dependent"), _buildReference(namespaceInstances[i],
                                                            CIM_NAMESPACEINMANAGER_CLASSNAME));
        assocInstances.append(instance);
    }
    return(assocInstances);
}

Array<CIMInstance> InteropProvider::_buildInstancesCommMechanismForManager()
{

    Array<CIMInstance> commInstances = _buildInstancesPGCIMXMLCommunicationMechanism(true,
         true, CIMPropertyList());

    CIMInstance instanceObjMgr = _buildInstanceCIMObjectManager( true, true, CIMPropertyList());

    CIMObjectPath refObjMgr = _buildReference(instanceObjMgr, CIM_OBJECTMANAGER_CLASSNAME);
    Array<CIMInstance> assocInstances;

    for (Uint32 i = 0 ; i < commInstances.size() ; i++)
    {
        
        CIMInstance instance = _buildInstanceSkeleton(CIM_COMMMECHANISMFORMANAGER_CLASSNAME);

        _setPropertyValue(instance,CIMName("Antecdent"), refObjMgr);
        //ATTNATTN: this is weak qualifier.
        _setPropertyValue(instance,CIMName("Dependent"), _buildReference(commInstances[i],CIM_COMMMECHANISMFORMANAGER_CLASSNAME));
        assocInstances.append(instance);
    }
    return(assocInstances);
}

/* generate one instance of the CIM_Namespace class with the
   properties
   @param namespace name to put into the class
   @exceptions - exceptions carried forward from create instance
   and addProperty.
*/
CIMInstance InteropProvider::_buildInstanceCIMNamespace(const CIMNamespaceName & nameSpace)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMNamespace()");
    
    // ATTN: KS need to get the real objectManager name from elsewhere.  the only place
    // this exists is through the objectmanager object.  
    // ATTN: Should we be saving the objectmanager name somewhere internally either in
    // interop or more generally somewhere within the system for common access.
    String ObjectManagerName = "ObjectManagerNameValue";
    
    CIMInstance instance = _buildInstanceSkeleton(CIM_NAMESPACE_CLASSNAME);

    _fixInstanceCommonKeys(instance);

    //ObjectManagerCreationClassName
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME, CIM_OBJECTMANAGER_CLASSNAME.getString());

    //ObjectManagerName
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME, ObjectManagerName);

    //CreationClassName
    // Class in which this was created, 
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME, CIM_NAMESPACE_CLASSNAME.getString());
    //Name
    // This is the namespace name itself
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME, nameSpace.getString());

    //ClassInfo
    // Set the classinfo to unknown and the description to namespace.
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CLASSINFO, Uint16(0));
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_DESCRIPTIONOFCLASSINFO, String("namespace"));

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

/** builds complete object path from instance and classinfo by building the full path
    with host and namespace names included.
*/

CIMObjectPath InteropProvider::_buildObjectPath(const CIMNamespaceName& name,
                                                const CIMName& className,
                                                const CIMInstance& instance)
{
    CIMObjectPath objectPath;
    objectPath = _buildInstancePath(name,className,instance);

    objectPath.setHost(System::getHostName());

    objectPath.setNameSpace(name);
    return(objectPath);
}

/* Given a class and instance build the instance path for a
   the object.  This builds all components of the path
   @param namespace name to build
   @return CIMObjectPath containing namespace, class and keybinding 
   components of path
   @exceptions - Passes repository exceptions.
*/
// ATTN: KS Build path from instance and instance from class.  Not sure
//   we want to always do this.  Consider change to build keys directly

CIMObjectPath InteropProvider::_buildInstancePath(const CIMNamespaceName& name,
                                           const CIMName& className, 
                                           const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePath");

    // get the class CIM_Namespace class to use in building path

    // Exception out if Class does not exist in this namespace
    CIMClass thisClass = _getClass(name, className);

    // XmlWriter::printInstanceElement(instance);
    CIMObjectPath ref = instance.buildPath(thisClass);
    
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
        if(namespaceNames[i].equal ( namespaceName ))
            return true;
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
      return false;
    
    //
    //  Compare prefix substring of namespace name with parent namespace name
    //
    else if (String::equalNoCase (child.subString (0, parent.size ()), parent))
      return true;
    
    return false;
}
//**************************************************************
// Overloaded functions to get key value with different params
//**************************************************************

/*  find the name key in the keybindings and return the value.
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
            return (kbArray[i].getValue());
    }

    throw CIMInvalidParameterException("Invalid key property: " + keyName.getString());
}

String _getKeyValue(const CIMInstance& instance, const CIMName& keyName)
{
    Uint32 pos;
    CIMValue propertyValue;

    pos = instance.findProperty(keyName);
    if (pos == PEG_NOT_FOUND)
       throw CIMPropertyNotFoundException
           (NAMESPACE_PROPERTYNAME.getString());
    
    propertyValue = instance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
       throw CIMInvalidParameterException("Invalid type for property: "
                             + NAMESPACE_PROPERTYNAME.getString());
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
    
    pos = namespaceInstance.findProperty(NAMESPACE_PROPERTYNAME);
    if (pos == PEG_NOT_FOUND)
    {
       throw CIMPropertyNotFoundException
           (NAMESPACE_PROPERTYNAME.getString());
    }
    
    propertyValue = namespaceInstance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
       throw CIMInvalidParameterException("Invalid type for property: "
                             + NAMESPACE_PROPERTYNAME.getString());
    
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
       throw CIMInvalidParameterException("Invalid key property:  ");
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
            (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM))
            throw CIMNotSupportedException("InteropProvider, Create Not allowed");

        CDEBUG("CreateInstance: No test for CIM_NamepsaceClass");
        if (classEnum == CIM_NAMESPACE)
        {
            // Validate that keys are as required. Does its own exception.
            _validateCIMNamespaceKeys(myInstance);
            String namespaceName;
            newNamespaceName = _getKeyValue(myInstance, CIM_NAMESPACE_PROPERTY_NAME);

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

        CIMNamespaceName childNamespaceName;
        CIMNamespaceName deleteNamespaceName;
        Boolean isRelativeName;
        
        _operationNamespace = instanceName.getNameSpace();
        handler.processing();
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceName.getClassName());    
        
        String userName = _validateUserID(context);
        
        
        if ((classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM))
            throw CIMNotSupportedException("Delete Not allowed");
        
        // Delete the instance since it may be in persistent storage
        if ((classEnum == CIM_OBJECTMANAGER))
        {
            CIMInstance instance;
            try
            {
            instance = _repository->getInstance(_operationNamespace, instanceName);

            // If instance found, then delete it.
            _repository->deleteInstance(_operationNamespace,instanceName);

            PEG_METHOD_EXIT();
            handler.complete();
            return;
            }
            catch(CIMException& e)
            {
                PEG_METHOD_EXIT();
                throw e;
            }
        }

        Array<CIMNamespaceName> namespaceNames;
        namespaceNames = _enumerateNameSpaces();
        if (classEnum == CIM_NAMESPACE)
        {
            // validate requred keys.  Exception out if not valid
            _validateCIMNamespaceKeys(instanceName);
            
            deleteNamespaceName = _getKeyValue(instanceName, CIM_NAMESPACE_PROPERTY_NAME);
            CDEBUG("Delete namespace = " << deleteNamespaceName );
        }
        else  // Procesing for __namespace
        {
    
            CIMNamespaceName childNamespaceName;
            CIMNamespaceName deleteNamespaceName;
            Boolean isRelativeName;
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
    const CIMPropertyList & propertyList,
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
            CIMInstance instance = _buildInstanceCIMObjectManager(includeQualifiers,
                                        includeClassOrigin, propertyList);
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        

        if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
        {
            // ATTN: test for correct instance KS: Priority 1    
            Array <CIMInstance> instances = _buildInstancesPGCIMXMLCommunicationMechanism(
                                    includeQualifiers,
                                    includeClassOrigin, propertyList);
            handler.deliver(instances[0]);
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

            instance = _getInstanceCIMNamespace(namespaceName);
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
            CIMInstance instance = _buildInstanceCIMObjectManager(includeQualifiers,
                                    includeClassOrigin,
                                    propertyList);
            handler.deliver(instance);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        
        if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
        {
            Array<CIMInstance> instances = _buildInstancesPGCIMXMLCommunicationMechanism(includeQualifiers,
                                    includeClassOrigin, propertyList);
            CDEBUG("Build instances of PGCIMXML. count= " << instances.size());
            handler.deliver(instances);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }


        if (classEnum == CIM_NAMESPACE)
        {
            Array<CIMInstance> instances = _getInstancesCIMNamespace(includeQualifiers,
                                    includeClassOrigin, propertyList);

            handler.deliver(instances);
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
            CIMInstance instance = _buildInstanceCIMObjectManager( true, true, CIMPropertyList());
            CIMObjectPath ref = _buildInstancePath(_operationNamespace,
                CIM_OBJECTMANAGER_CLASSNAME, instance);
            handler.deliver(ref);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }
        
        if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
        {
            Array<CIMInstance> instances = _buildInstancesPGCIMXMLCommunicationMechanism(true,
                 true, CIMPropertyList());

            for (Uint32 i = 0 ; i < instances.size() ; i++)
            {
                CIMObjectPath ref = _buildInstancePath(_operationNamespace,
                    PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME, instances[i]);
                handler.deliver(ref);
            }
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }

        if (classEnum == CIM_NAMESPACE)
        {
            Array<CIMInstance> instances = _getInstancesCIMNamespace(false,
                                    false, CIMPropertyList());
            CDEBUG("EvalNames. Found instances. Count= " << instances.size());
            for (Uint32 i = 0 ; i < instances.size() ; i++)
            {
                CIMObjectPath ref = _buildInstancePath(_operationNamespace,
                                            CIM_NAMESPACE_CLASSNAME, instances[i]);
                handler.deliver(ref);
            }

            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }

        // __Namespace processing.
        CIMNamespaceName parentNamespaceName = classReference.getNameSpace();
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
           "parentNamespaceName = " + parentNamespaceName.getString());
        CDEBUG("Enumerate Instance Names. ns = " << parentNamespaceName.getString());
        
        // Get list of all namespaces
        Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();

        // Build the cimObjectPath for each namespace found
        for (Uint32 i = 0; i < namespaceNames.size(); i++)
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::associatorNames()");
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::associatorNames()");
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::references()");
	throw CIMNotSupportedException("AssociationProvider::references");
}

void _filterAssocInstances(Array<CIMInstance>& instances,
                      CIMName assocClassName,
                      String assocRole,
                      CIMName resultClassName = CIMName(),
                      String resultRole = String::EMPTY)
{
    return;
}
void InteropProvider::referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::referenceNames()");
    CDEBUG("::referenceNames(): object= " << objectName.toString() << " result Class= " << resultClass.getString());
                    
    // operation namespace needed internally to get class.
    _operationNamespace = objectName.getNameSpace();
    String userName = _validateUserID(context);
    // begin processing the request
    handler.processing();

    // determine if valid class result class

    CIMName targetAssocClassName = resultClass;
    
    CIMName targetClassName = objectName.getClassName();

    targetAssocClass classEnum  = _verifyValidAssocClassInput(targetAssocClassName);


    Array<CIMInstance> assocInstances;

    if (classEnum == CIM_COMMMECHANISMFORMANAGER)
        assocInstances = _buildInstancesCommMechanismForManager();

    if (classEnum == CIM_NAMESPACEINMANAGER)
        assocInstances = _buildInstancesNamespaceInManager();

    _filterAssocInstances(assocInstances, resultClass, role);

    for (Uint32 i = 0 ; i < assocInstances.size() ; i++ )
    {
        CIMObjectPath ref = _buildObjectPath(_operationNamespace,
                targetAssocClassName, assocInstances[i]);
        CDEBUG("referenceNames returns: " << ref.toString());
        handler.deliver(ref);

    }
    handler.complete();

    PEG_METHOD_EXIT();
    // Convert the instances to referenceNames response

}

PEGASUS_NAMESPACE_END
// END_OF_FILE
