//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
//              Karl Schopmeyer - Created Cim_Namespace capabilities.
//              Karl Schopmeyer - added objectmanager and communication classes
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
//      CIM_Namespace (Effective Pegasus 2.4 we use PG_Namespace which
//      is a subclass of CIM_Namespace with additional properties for
//      shared namespaces.
//      
//      It also services the Interop associations tied to these classes
//      including:
//      CIM_NamespaceInManager
//      ...
//      This is a control provider and as such uses the Tracer functions
//      for data and function traces.  Since we do not expect high volume
//      use we added a number of traces to help diagnostics.  
///////////////////////////////////////////////////////////////////////////////
/* TODO LIST:
    1. Finish the association functions
    
    2. UUID generation should become a system function since it will be used
       by many providers, etc. as part of id generation.
       
    3. Review the key parameters on create, etc. to be sort out which are
       required from user and which we can supply.  I think we are asking too
       much of the user right now.
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
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/HashTable.h>


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
#define LDEBUG()
//#define CDEBUG(X) PEGASUS_STD(cout) << "InteropProvider " << X << PEGASUS_STD(endl)
//#define LDEBUG(X) Logger::put (Logger::DEBUG_LOG, "InteropProvider", Logger::INFORMATION, "$0", X)
// The following is attempt to use the tracer for CDEBUG.  Not working. 1 sept 2004
//#include <cstring>
//#include <stdcxx/stream/strstream>
// This one sucks because not applicable to separate executables.
// looks like we use trace for the externals.
// requires using PEGASUS_USING_PEGASUS
//#define CDEBUG(X) {ostrstream os; os << X; char* tmp = os.str(); PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, String(tmp));delete tmp;} 
//#define CDEBUG(X) {stringstream os; os << X;string os_str = os.str(); const char* tmp = os_str.c_str(); PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, String(tmp); }

//**************************************************************************
//
//    Constants representing the class names processed
//
//**************************************************************************

const char * thisProvider = "InteropProvider";
// The following should be moved to somewhere like constants.
static const String PegasusInstanceIDGlobalPrefix = "PEG";

/**
    The constants representing the class names we process
*/
// ATTN DELETE: static const CIMName __NAMESPACE_CLASSNAME  = CIMName ("__Namespace");
static const CIMName CIM_NAMESPACE_CLASSNAME  = CIMName ("CIM_Namespace");
static const CIMName PG_NAMESPACE_CLASSNAME  = CIMName ("PG_Namespace");

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

// Additional Property names for PG_Namespace Class

static const CIMName PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED = 
        CIMName ("SchemaUpdatesAllowed");
static const CIMName PG_NAMESPACE_PROPERTY_ISSHAREABLE = 
        CIMName ("IsShareable");
static const CIMName PG_NAMESPACE_PROPERTY_PARENTNAMESPACE = 
        CIMName ("ParentNamespace");
static const CIMName PG_NAMESPACE_PROPERTY_NAME = 
        CIMName ("Name");

// Defines to serve as the ENUM for class selection for instance
// operations.

enum targetClass{
        //__NAMESPACE = 1,
        PG_NAMESPACE = 1,
        CIM_OBJECTMANAGER = 2,
        PG_CIMXMLCOMMUNICATIONMECHANISM = 3,
        CIM_NAMESPACEINMANAGERINST =4,
        CIM_COMMMECHANISMFORMANAGERINST=5
    };
    
 enum targetAssocClass{
     CIM_NAMESPACEINMANAGER =1,
     CIM_COMMMECHANISMFORMANAGER=2
 };

//***************************************************************
// Provider Utility Functions
//***************************************************************

/** get one string property from an instance. Note that these functions simply
    return the default value if the property cannot be found or is of the wrong
    type thus, in reality being a maintenance problem since there is no 
    error indication.
    @param instance CIMInstance from which we get property value
    @param propertyName String name of the property containing the value
    @param default String optional parameter that is substituted if the property does
    not exist, is Null, or is not a string type. The substitute is String::EMPTY
    @return String value found or defaultValue.
*/
String _getPropertyValue(const CIMInstance& instance, const CIMName& propertyName, const String& defaultValue)
{
    String output = defaultValue;
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_STRING)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(output);
        }
    }
    return(output);
}

// Overload of _getPropertyValue for boolean type
Boolean _getPropertyValue(const CIMInstance& instance, const CIMName& propertyName, const Boolean defaultValue)
{
    Boolean output = defaultValue;
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_BOOLEAN)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(output);
        }
    }
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
  String ipAddress;
  
  if (hostName == String::EMPTY)
    	hostName = System::getHostName();

  if ((ipAddress = System::getHostIP(hostName)) == String::EMPTY)
  {
      // set default address if everything else failed
      ipAddress = String("127.0.0.1");
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_verifyValidClassInput");
    if (className.equal(CIM_OBJECTMANAGER_CLASSNAME))
        return CIM_OBJECTMANAGER;

    if (className.equal(PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME))
        return PG_CIMXMLCOMMUNICATIONMECHANISM;

    // Last entry, reverse test and return OK if PG_Namespace
    // Note: Changed to PG_Namespace for CIM 2.4
    if (!className.equal(PG_NAMESPACE_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    PEG_METHOD_EXIT();
    return PG_NAMESPACE;
}

targetAssocClass _verifyValidAssocClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_verifyValidAssocClassInput");
    if (className.equal(CIM_NAMESPACEINMANAGER_CLASSNAME))
        return CIM_NAMESPACEINMANAGER;
    // Last entry, reverse test and return OK if CIM_CommMech....
    if (!className.equal(CIM_COMMMECHANISMFORMANAGER_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    PEG_METHOD_EXIT();
    return CIM_COMMMECHANISMFORMANAGER;
}

/* validate the authorization of the user name against the namespace.
*/
String _validateUserID(const OperationContext & context)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateUserID");
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

    PEG_METHOD_EXIT();
    return userName;
}

/** Set the value of a property defined by property name in the instance provided.
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceSkeleton()");
    CIMClass myClass;

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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesPGCIMXMLCommunicationMechanism");
    
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
    PEG_METHOD_EXIT();
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

/** get either the local or disk CIMObjectmanager instance.
    @return Boolean true if already exists. False if 
    not initialized. Also returns with the current version set
    into local parameter.
    ATTN: Probably should get rid of the local parameter since
    this is used so infrequently, waste of space.
*/
Boolean InteropProvider::_getInstanceCIMObjectManager(
                        const Boolean includeQualifiers,
                        const Boolean includeClassOrigin,
                        const CIMPropertyList& propertyList)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstanceCIMObjectManager");
    // If there is already an instance of this class local use it.
    if (!instanceOfCIMObjectManager.isUninitialized())
    {
        PEG_METHOD_EXIT();
        return(true);
    }
    // Try to get persistent instance from repository
    Array<CIMInstance> instances;
    try
    {
        instances = _repository->enumerateInstances(_operationNamespace,
                      CIM_OBJECTMANAGER_CLASSNAME, true, false, includeQualifiers,
                        includeClassOrigin, propertyList);
    
        CDEBUG("_buildInstanceCIMOBJMGR Found Instance in repository " << instances.size() );
    
        if (instances.size() >= 1)
        {
            // set this instance into global variable.
            instanceOfCIMObjectManager = instances[0];
    
            // log entry if there is more than one instance.
            // Some day we may support multiple entries to see other CIMOMs but
            // for now this should be illegal and represent an internal error.
            // but we will still continue to use the first entry.
            if (instances.size() > 1)
            {
                Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
                    "Error. Multiple definitons of : $0", CIM_OBJECTMANAGER_CLASSNAME.getString());
            }
            return(true);
        }
        else
            return(false);
    }
    catch(CIMException& e)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Error. Cannot access $0 in repository", CIM_OBJECTMANAGER_CLASSNAME.getString());
        PEG_METHOD_EXIT();
        throw e;
    }
    catch(Exception& e)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Error. Cannot access $0 in repository", CIM_OBJECTMANAGER_CLASSNAME.getString());
        PEG_METHOD_EXIT();
        throw e;
    }
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
            "InteropProvider::_buildInstanceCIMObjectManager");

    // Try to get the current object.  If true then it is already created.
    if (_getInstanceCIMObjectManager(includeQualifiers,includeClassOrigin,propertyList))
    {
        PEG_METHOD_EXIT();
        return(instanceOfCIMObjectManager);
    }
    //
    // No instance in the repository. Build new instance and save it.
    //
    CIMInstance instance = _buildInstanceSkeleton(CIM_OBJECTMANAGER_CLASSNAME);

    _fixInstanceCommonKeys(instance);

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,CIM_OBJECTMANAGER_CLASSNAME.getString());

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,buildObjectManagerName());

    _setPropertyValue(instance, CIMName("ElementName"), String("Pegasus"));

    //
    //Description property this object manager instance
    // default is Pegasus CIM_Server Version.
    // Provided undocumented option to get this from the environment.
    // This should probably be removed or documented.
    //
    String description;
    char * envDescription;
    envDescription = getenv("PEGASUS_CIMOM_DESCRIPTION");

    description = (envDescription) ?
        envDescription :
        "Pegasus " + String(PEGASUS_PRODUCT_NAME) + " Version " +
            String(PEGASUS_PRODUCT_VERSION);

    _setPropertyValue(instance, CIMName("Description"), description);

    //Property GatherStatisticalData. Initially this is set to false
    // and can then be modified by a modify instance on the instance.
    
    Boolean gatherStatDataFlag = false;

    _setPropertyValue(instance, OM_GATHERSTATISTICALDATA, Boolean(gatherStatDataFlag));

    // ATTN: This one is a problem KS rethink this.
    // the following is a temporary hack to set the value of the statistics
    // gathering function dynamically.  We simply get the correct value
    // and call the internal method to set it each time this object is
    // built.
#ifndef PEGASUS_DISABLE_PERFINST
    StatisticalData* sd = StatisticalData::current();
    sd->setCopyGSD(gatherStatDataFlag);
#endif

    // write the instance to the repository
    CIMObjectPath instancePath;
    // Add the instance path to this if necessary ATTN ATTN:
    try
    {
        instancePath = _repository->createInstance(_operationNamespace,
                       instance );
    }
    catch(CIMException& e)
    {
        // ATTN: KS generate log error if this not possible
        PEG_METHOD_EXIT();
        throw e;
    }
    catch(Exception& e)
    {
        // ATTN: Generate log error.
        PEG_METHOD_EXIT();
        throw e;
    }
    instance.setPath(instancePath);
    CDEBUG("CIMObjectMgr path = " << instancePath.toString());
    // Save this instance for other requests
    instanceOfCIMObjectManager = instance;
    PEG_METHOD_EXIT();
    return(instanceOfCIMObjectManager);
}

/** Get the instances of CIM_Namespace. Gets all instances of the namespace from
    the repository namespace management functions
    Pegasus 2.4 - This now gets CIM_Namespace and its subclass PG_Namespace
*/
Array<CIMInstance> InteropProvider::_getInstancesCIMNamespace(const Boolean& includeQualifiers,
                            const Boolean& includeClassOrigin,
                            const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");

    CDEBUG("_getinstancesPGNamespace");
    Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();
    CDEBUG("_getInstancesCIMNamespace. count = " << namespaceNames.size());
    Array<CIMInstance> instanceArray;

    // We build instances of PG namespace since that is the leaf class
    for (Uint32 i = 0; i < namespaceNames.size(); i++)
    {
       instanceArray.append( _buildInstancePGNamespace(namespaceNames[i]));
    }
    CDEBUG("Build this many PG_Namespace Instances. count= " << instanceArray.size());
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

    // search the instances for one with the name property value = input parameter.
    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        if (_getPropertyValue(instances[i], CIMName("name"), String::EMPTY) == nameSpace.getString())
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesNamespaceInManager");

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

    PEG_METHOD_EXIT();
    return(assocInstances);
}

Array<CIMInstance> InteropProvider::_buildInstancesCommMechanismForManager()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesCommMechanismForManager");

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

    PEG_METHOD_EXIT();
    return(assocInstances);
}

/* generate one instance of the CIM_Namespace class with the
   properties
   NOTE: CIM 2.4 - Changed to build PG namespace
   @param namespace name to put into the class
   @exceptions - exceptions carried forward from create instance
   and addProperty.
*/
CIMInstance InteropProvider::_buildInstancePGNamespace(const CIMNamespaceName & nameSpace)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePGNamespace");
    
    // ATTN: KS need to get the real objectManager name from elsewhere.  the only place
    // this exists is through the objectmanager object.  
    // ATTN: Should we be saving the objectmanager name somewhere internally either in
    // interop or more generally somewhere within the system for common access.
    String ObjectManagerName = "ObjectManagerNameValue";
    
    CIMInstance instance = _buildInstanceSkeleton(PG_NAMESPACE_CLASSNAME);

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

    //
    //  Everything above was commmon to CIM Namespace.  The following is PG_Namespace Properties
    //
    // ATTN: KS Get the correct values for these entities from repository interface.
    CDEBUG("_buildPGNS Instance get namespace attributes for namespace= " << nameSpace.getString()); 
	CIMRepository::NameSpaceAttributes attributes;
    _repository->getNameSpaceAttributes(nameSpace.getString(), attributes);
    String parent="";
	String name = "";
    Boolean shareable=false;
    Boolean updatesAllowed=true;
    for (CIMRepository::NameSpaceAttributes::Iterator i = attributes.start(); i; i++) 
    {
       String key=i.key();
       String value = i.value();
       CDEBUG("Show Attributes. key= " << key << " value= " << value);
       if (String::equalNoCase(key,"shareable"))
	   {
          if (String::equalNoCase(value,"true"))
              shareable=true;
	   }
       else if (String::equalNoCase(key,"updatesAllowed")) 
	   {
          if (String::equalNoCase(value,"false"))
              updatesAllowed=false;
	   }
       // Test to be sure we are returning proper namespace name
       else if (String::equalNoCase(key,"name"))
       {
          if (!String::equalNoCase(value, nameSpace.getString()))
          {
              PEG_METHOD_EXIT();
              // This is poor exception since it reflects internal error. Do error log
              throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, 
                  "Namespace attribute rtnd error for key " + key + "expected " +
                   nameSpace.getString()+ value + " in " + String(thisProvider));
          }

		  name = value;
       }
       else if (String::equalNoCase(key,"parent"))
	   {
          parent=value;
	   }
       else 
       {
          PEG_METHOD_EXIT();
          // Poor error definition since it reflects internal error. do error log
          throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, nameSpace.getString()+
              " namespace attribute " + key + " option not supported in"+ String(thisProvider));
       }
    }
    _setPropertyValue(instance, PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED, updatesAllowed);
    _setPropertyValue(instance, PG_NAMESPACE_PROPERTY_ISSHAREABLE, shareable);
    _setPropertyValue(instance, PG_NAMESPACE_PROPERTY_PARENTNAMESPACE, parent);
	_setPropertyValue(instance, PG_NAMESPACE_PROPERTY_NAME, name);

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

/** completes a property in the defined instance either
    by adding the complete property if it does not exist
    or by adding the value if the property does exist.
    Used to make sure that key properties for things
    like systemcreationclass are included in the 
    creation of new instances and that the
    values are logical for the CIMOM.
*/
Boolean _completeProperty(CIMInstance& instance,
    const CIMName& propertyName,
    const String& value)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_completeProperty()");

    Uint32 pos;

    if (!_validateRequiredProperty(instance,
                propertyName,
                value))
    {
        if ((pos = instance.findProperty (propertyName)) == PEG_NOT_FOUND)
        {
            // Add the property.  Should be from the class.
            PEG_METHOD_EXIT();
            return(false);
        }
        else
        {
            //
            _setPropertyValue(instance, propertyName, value);
        }
    }

    PEG_METHOD_EXIT();
    return(true);
}
Boolean _completeCIMNamespaceKeys(CIMInstance& instance)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_completeCIMNamespaceKeys");

    Boolean valid = true;
    CIMName propertyName;

    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName ()))
    {

        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }

    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }

    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                CIM_OBJECTMANAGER_CLASSNAME.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!_completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_NAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_NAME;
        valid = false;
    }

    if (!valid)
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "Invalid CIM_Namespace Key Property " +  propertyName.getString());
        PEG_METHOD_EXIT();
        throw CIMInvalidParameterException(
            "Invalid CIM_Namespace key property: " + propertyName.getString());
    }
    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "CIM_Namespace Keys Valid");
    PEG_METHOD_EXIT();
    return(valid);
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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildObjectPath");

    CIMObjectPath objectPath;
    objectPath = _buildInstancePath(name,className,instance);

    objectPath.setHost(System::getHostName());

    objectPath.setNameSpace(name);

    PEG_METHOD_EXIT();
    return(objectPath);
}

/* Given a class and instance build the instance path for a
   the object.  This builds all components of the path
   @param namespace name to build
   @return CIMObjectPath containing namespace, class and keybinding 
   components of path
   @exceptions - Passes repository exceptions.
*/

CIMObjectPath InteropProvider::_buildInstancePath(const CIMNamespaceName& name,
                                           const CIMName& className, 
                                           const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePath");

    // get the class CIM_Namespace class to use in building path
    // Exception out if Class does not exist in this namespace
    CIMClass thisClass = _getClass(name, className);

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
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_isNamespace");

     Boolean found = false;
     for(Uint32 i = 0; i < namespaceNames.size(); i++)
     {
        if(namespaceNames[i].equal ( namespaceName ))
            return true;
     }

     PEG_METHOD_EXIT();
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
        
        handler.processing();
        CIMNamespaceName newNamespaceName;
        CDEBUG("CreateInstance " << instanceReference.toString());
        // operation namespace needed internally to get class.
        _operationNamespace = instanceReference.getNameSpace();
        
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceReference.getClassName());

        String userName = _validateUserID(context);
        CIMObjectPath newInstanceReference;

        if ((classEnum == CIM_OBJECTMANAGER) || 
            (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM))
            throw CIMNotSupportedException("InteropProvider, Create Not allowed");

        if (classEnum == PG_NAMESPACE)
        {
            // Create local instance to complete any keys.
            CIMInstance localInstance = myInstance.clone();

            _completeCIMNamespaceKeys(localInstance);
            // Validate that keys are as required. Does its own exception.
            newNamespaceName = _getKeyValue(myInstance, CIM_NAMESPACE_PROPERTY_NAME);

            newInstanceReference = _buildInstancePath(_operationNamespace,
                                        PG_NAMESPACE_CLASSNAME, localInstance);
        }

        else   // Invalid class for the create functions.
        {
            PEGASUS_ASSERT(false);
        }

        // Create the new namespace
        try
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "Namespace = " + newNamespaceName.getString() + 
                    " to be created.");
            Boolean updatesAllowed = false;
            Boolean shareable = false;
            String parent = String::EMPTY;

            CIMRepository::NameSpaceAttributes attributes;
            // optional property.  Set false if not found.
            // ATTN: Should set to class default.
            if (myInstance.findProperty(PG_NAMESPACE_PROPERTY_ISSHAREABLE) != PEG_NOT_FOUND)
            {
                if (_getPropertyValue(myInstance, PG_NAMESPACE_PROPERTY_ISSHAREABLE, false))
                {
                    attributes.insert("shareable","true");
                    shareable = true;
                }
                else
                    attributes.insert("shareable", "false");
            }
            else
                attributes.insert("shareable", "false");

            // Optional property.  Set false if not found.
            if (myInstance.findProperty(PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED) != PEG_NOT_FOUND)
            {
                if (_getPropertyValue(myInstance, PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED, false))
                {
                    attributes.insert("updatesAllowed","true");
                    updatesAllowed = true;
                }
                else
                    attributes.insert("updatesAllowed", "false");
            }
            else
                attributes.insert("updatesAllowed", "false");

            // ATTN: Need to reflect and dependencies between these properties. Right now
            // this lets anything happen.
            if (myInstance.findProperty(PG_NAMESPACE_PROPERTY_PARENTNAMESPACE) != PEG_NOT_FOUND)
            {
                String parent = _getPropertyValue(myInstance, PG_NAMESPACE_PROPERTY_PARENTNAMESPACE, String::EMPTY);
                if (parent != String::EMPTY)
                    attributes.insert("parent",parent);
            }
            _repository->createNameSpace(newNamespaceName, attributes);
        
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "Namespace = " + newNamespaceName.getString() + 
                    " successfully created.");
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                "Create Namespace: Shareable = $0, Updates allows: $1,  Parent: $2",
                newNamespaceName.getString(), shareable? "true" : "false", shareable? "true" : "false", parent );

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
        if (classEnum == PG_NAMESPACE)
        {
            // validate requred keys.  Exception out if not valid
            _validateCIMNamespaceKeys(instanceName);
            
            deleteNamespaceName = _getKeyValue(instanceName, CIM_NAMESPACE_PROPERTY_NAME);
            CDEBUG("Delete namespace = " << deleteNamespaceName );
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
        
        if (classEnum == PG_NAMESPACE)
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
            PEGASUS_ASSERT(false);
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

            //Array<Sint8> tmp;
            ///XmlWriter::appendInstanceElement(tmp, instance);
            //tmp.append('\0');
            //Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            //        "Instance - XML content: $0", tmp.getData());
            ///XmlWriter::printInstanceElement(instance);
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


        if (classEnum == PG_NAMESPACE)
        {
            Array<CIMInstance> instances = _getInstancesCIMNamespace(includeQualifiers,
                                    includeClassOrigin, propertyList);

            handler.deliver(instances);
            handler.complete();
            PEG_METHOD_EXIT();
            return;
        }

        handler.complete();
        
        PEG_METHOD_EXIT();
    }

void InteropProvider::modifyObjectManagerInstance(const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance& modifiedIns,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::modifyInstanceManagerInstance");
    // the only allowed modification is this one property, statistical data
    if (modifiedIns.findProperty(OM_GATHERSTATISTICALDATA) != PEG_NOT_FOUND)
    {
        // ATTN: This function is a design problem.
        // ATTN: Should the ifdef include everything????
        // the following is a temporary hack to set the value of the statistics
        // gathering function dynamically.  We simply get the  value from input
        // and call the internal method to set it each time this object is
        // built.
#ifndef PEGASUS_DISABLE_PERFINST
        StatisticalData* sd = StatisticalData::current();
        Boolean statisticsFlag = _getPropertyValue(modifiedIns, OM_GATHERSTATISTICALDATA, false);  
        sd->setCopyGSD(statisticsFlag);
        if ( ! _getInstanceCIMObjectManager(false, false, CIMPropertyList()))
        {
            _buildInstanceCIMObjectManager(true, true, CIMPropertyList());
        }

        // ATTN: Think we need to clone here to avoid any issues of overwriting.
        _setPropertyValue(instanceOfCIMObjectManager, OM_GATHERSTATISTICALDATA, statisticsFlag);  
        CIMObjectPath instancePath;

        // Modify the object on disk
        try
        {
            _repository->modifyInstance(_operationNamespace,
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
            // ATTN: Generate log error.
            PEG_METHOD_EXIT();
            throw e;
        }
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Interop Provider Set Statistics gathering in CIM_ObjectManager: $0",
            (statisticsFlag? "true" : "false"));
        // modify returns nothing normally.
        return;
#endif

    }
    PEG_METHOD_EXIT();
    // ATTN Expand this defintion to be more precise since it allows only mod of
    // one property and that property MUST be in the instance to be modifiable.
    throw CIMNotSupportedException
        (OM_GATHERSTATISTICALDATA.getString() + " modify operation not supported by Interop Provider");
}
//***************************************************************************
//                modifyInstance
//***************************************************************************
void InteropProvider::modifyInstance(const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance& modifiedIns,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    ResponseHandler & handler)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::modifyInstance");

    // operation namespace needed internally to get class.

    // ATTN: KS 31 August 2004. This must test for privileged user.
    _operationNamespace = instanceReference.getNameSpace();
    CIMName className =  instanceReference.getClassName();
    targetClass classEnum  = _verifyValidClassInput(instanceReference.getClassName());    

    String userName = _validateUserID(context);
    // begin processing the request
    handler.processing();

    if (classEnum == CIM_OBJECTMANAGER)
    {
        modifyObjectManagerInstance(context, instanceReference,modifiedIns,
            includeQualifiers, propertyList, handler);
        // for the moment allow modification of the statistics property only
    }

    else if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
    {

        PEG_METHOD_EXIT();
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");
    }
    else if (classEnum == PG_NAMESPACE)
    {
        // for the moment allow modification of the statistics property only
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");
    }
    else
    {
        PEGASUS_ASSERT(false);  // should never get here.
    }

    handler.complete();
    PEG_METHOD_EXIT();
    return;
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
        
        // Deliver a single instance because there should only be one instance.
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

        // Deliver all possible instances of this class
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

        if (classEnum == PG_NAMESPACE)
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

        if (classEnum == CIM_COMMMECHANISMFORMANAGERINST)
        {
            Array<CIMInstance> instances = _buildInstancesCommMechanismForManager();
            for (Uint32 i = 0 ; i < instances.size() ; i++ )
            {
                CIMObjectPath ref = _buildObjectPath(_operationNamespace,
                        CIM_COMMMECHANISMFORMANAGER_CLASSNAME, instances[i]);
                handler.deliver(ref);
            }
        }

        if (classEnum == CIM_NAMESPACEINMANAGERINST)
        {
            Array<CIMInstance> instances = _buildInstancesNamespaceInManager();
            for (Uint32 i = 0 ; i < instances.size() ; i++ )
            {
                CIMObjectPath ref = _buildObjectPath(_operationNamespace,
                        CIM_NAMESPACEINMANAGER_CLASSNAME, instances[i]);
                handler.deliver(ref);
            }
        }


        // ATTN: Exception response because of error
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
	//throw CIMNotSupportedException("AssociationProvider::associators");
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
	//throw CIMNotSupportedException("AssociationProvider::associatorNames");
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
	//throw CIMNotSupportedException("AssociationProvider::references");
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
