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
// Author: Karl Schopmeyer (k.schopmeyer@opengroup.org)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Karl Schopmeyer - Created Cim_Namespace capabilities.
//              Karl Schopmeyer - added objectmanager and communication classes
//              Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3194
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3659
//              Alex Dunfey, EMC (Dunfey_alexander@emc.com) for PEP 244
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
//      CIM_ProtocolAdapter  (Note: Removed because deprecated class in cim 2.9)
//      CIM_Namespace -- Only creates are allowed directly against this class.
//              This allows the publice class CIM_Namespace to be used to
//              create namespaces.  Modifies, deletes must use the returned
//              paths. Enumerates references, etc. all use hiearchy.
//              NOTE: Changes the class to PG_Namespace and returns that
//              objectpath
//
//      PG_Namespace - Pegasus particular subclass of CIM_Namespace that
//      add the parameters for shared namespaces
//
//      PG_CIMXMLCommunicationMechanism - Pegasus subclass of 
//      CIM_CIMXMLCommunicationMechanism that adds support for passing
//      additional communication parameters (ie. port, https vs. http, etc.)
//   
//      It also services the Interop associations tied to these classes
//      including:
//      CIM_NamespaceInManager
//      ...
//      Interop forces all creates to the PEGASUS_NAMESPACENAME_INTEROP 
//      namespace. There is a test on each operation that returns 
//      the Invalid Class CIMDError
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
#include <Pegasus/Common/StatisticalData.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/PegasusVersion.h>


#include <stdlib.h>

//The following include is needed for gethostbyname
#if defined(PEGASUS_OS_TYPE_WINDOWS)
#include <objbase.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

PEGASUS_USING_STD;

#define CDEBUG(X)
#define LDEBUG()
//#define CDEBUG(X) PEGASUS_STD(cout) << "InteropProvider (" << __LINE__ << ") " << X << PEGASUS_STD(endl)
//#define LDEBUG(X) Logger::put (Logger::DEBUG_LOG, "InteropProvider", Logger::TRACE, "$0", X)

//**************************************************************************
//
//    Constants representing the class names processed
//
//**************************************************************************

const char * thisProvider = "InteropProvider";

// This Mutex serializes access to the instance change CIM requests. Keeps from
// mixing instance creates, modifications, and deletes. This keeps the provider
// from simultaneously execute creates, modifications, and deletes of instances.
// While these operations are largely protected by the locking mechanisms of the
// repository this mutex guarantees that the provider will not simultaneously
// execute the instance change operations.
Mutex changeControlMutex;

// Values and ValueMap qualifier names
static const CIMName VALUES_QUALIFIERNAME("Values");
static const CIMName VALUEMAP_QUALIFIERNAME("ValueMap");

// Some Commonly shared property names
static const CIMName COMMON_PROPERTY_NAME("Name");
static const CIMName COMMON_PROPERTY_CREATIONCLASSNAME("CreationClassName");
static const CIMName COMMON_PROPERTY_SYSTEMNAME("SystemName");
static const CIMName COMMON_PROPERTY_SYSTEMCREATIONCLASSNAME(
    "SystemCreationClassName");
static const CIMName COMMON_PROPERTY_ELEMENTNAME("ElementName");
static const CIMName COMMON_PROPERTY_INSTANCEID("InstanceId");

static const String CIMXMLProtocolVersion = "1.0";

// Property names for CIM_Namespace Class
#define CIM_NAMESPACE_PROPERTY_NAME  COMMON_PROPERTY_NAME
#define CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME \
    COMMON_PROPERTY_CREATIONCLASSNAME
#define CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME \
    COMMON_PROPERTY_SYSTEMCREATIONCLASSNAME
#define CIM_NAMESPACE_PROPERTY_SYSTEMNAME COMMON_PROPERTY_SYSTEMNAME
static const CIMName CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME =
        CIMName ("ObjectManagerCreationClassName");
static const CIMName CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME =
        CIMName ("ObjectManagerName");
static const CIMName CIM_NAMESPACE_PROPERTY_CLASSINFO =
        CIMName ("ClassInfo");
static const CIMName CIM_NAMESPACE_PROPERTY_DESCRIPTIONOFCLASSINFO =
        CIMName ("DescriptionOfClassInfo");
static const CIMName CIM_NAMESPACE_PROPERTY_CLASSTYPE("ClassType");

// Additional Property names for PG_Namespace Class

static const CIMName PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED =
        CIMName ("SchemaUpdatesAllowed");
static const CIMName PG_NAMESPACE_PROPERTY_ISSHAREABLE =
        CIMName ("IsShareable");
static const CIMName PG_NAMESPACE_PROPERTY_PARENTNAMESPACE =
        CIMName ("ParentNamespace");
#define PG_NAMESPACE_PROPERTY_NAME COMMON_PROPERTY_NAME

// Property names for ObjectManager Class
#define OM_PROPERTY_NAME COMMON_PROPERTY_NAME
#define OM_PROPERTY_ELEMENTNAME COMMON_PROPERTY_ELEMENTNAME
#define OM_PROPERTY_CREATIONCLASSNAME COMMON_PROPERTY_CREATIONCLASSNAME
static const CIMName OM_PROPERTY_GATHERSTATISTICALDATA(
    "GatherStatisticalData");
static const CIMName OM_PROPERTY_DESCRIPTION("Description");
static const CIMName OM_PROPERTY_COMMUNICATIONMECHANISM(
    "CommunicationMechanism");
static const CIMName OM_PROPERTY_FUNCTIONALPROFILESSUPPORTED(
    "FunctionalProfilesSupported");
static const CIMName OM_PROPERTY_FUNCTIONALPROFILEDESCRIPTIONS(
    "FunctionalProfileDescriptions");
static const CIMName OM_PROPERTY_AUTHENTICATIONMECHANISMSSUPPORTED(
    "AuthenticationMechanismsSupported");
static const CIMName OM_PROPERTY_AUTHENTICATIONMECHANISMDESCRIPTIONS(
    "AuthenticationMechanismDescriptions");
static const CIMName OM_PROPERTY_MULTIPLEOPERATIONSSUPPORTED(
    "MultipleOperationsSupported");
static const CIMName OM_PROPERTY_VERSION("Version");
static const CIMName OM_PROPERTY_OPERATIONALSTATUS("OperationalStatus");
static const CIMName OM_PROPERTY_STARTED("Started");

// Property Names for CIMXML CommunicationMechanism
static const CIMName CIMXMLCOMMMECH_PROPERTY_CIMVALIDATED("CIMValidated");
static const CIMName CIMXMLCOMMMECH_PROPERTY_COMMUNICATIONMECHANISM(
        "CommunicationMechanism");
static const CIMName CIMXMLCOMMMECH_PROPERTY_FUNCTIONALPROFILESSUPPORTED(
        "FunctionalProfilesSupported");
static const CIMName CIMXMLCOMMMECH_PROPERTY_FUNCTIONALPROFILEDESCRIPTIONS(
        "FunctionalProfileDescriptions");
static const CIMName CIMXMLCOMMMECH_PROPERTY_AUTHENTICATIONMECHANISMSSUPPORTED(
        "AuthenticationMechanismsSupported");
static const
CIMName CIMXMLCOMMMECH_PROPERTY_AUTHENTICATIONMECHANISMDESCRIPTIONS(
        "AuthenticationMechanismDescriptions");
static const CIMName CIMXMLCOMMMECH_PROPERTY_MULTIPLEOPERATIONSSUPPORTED(
        "MultipleOperationsSupported");
static const CIMName CIMXMLCOMMMECH_PROPERTY_VERSION("Version");
static const CIMName CIMXMLCOMMMECH_PROPERTY_NAMESPACETYPE("namespaceType");
static const CIMName CIMXMLCOMMMECH_PROPERTY_NAMESPACEACCESSPROTOCOL(
    "namespaceAccessProtocol");
static const CIMName CIMXMLCOMMMECH_PROPERTY_IPADDRESS("IPAddress");
#define CIMXMLCOMMMECH_PROPERTY_ELEMENTNAME OM_PROPERTY_ELEMENTNAME
#define CIMXMLCOMMMECH_PROPERTY_OPERATIONALSTATUS OM_PROPERTY_OPERATIONALSTATUS
#define CIMXMLCOMMMECH_PROPERTY_NAME OM_PROPERTY_NAME
#define CIMXMLCOMMMECH_PROPERTY_CREATIONCLASSNAME OM_PROPERTY_CREATIONCLASSNAME

// Property names for RegisteredProfile
#define REGISTEREDPROFILE_PROPERTY_INSTANCEID COMMON_PROPERTY_INSTANCEID
static const CIMName REGISTEREDPROFILE_PROPERTY_ADVERTISETYPES(
    "AdvertiseTypes");
static const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDNAME(
    "RegisteredName");
static const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDVERSION(
    "RegisteredVersion");
static const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION(
    "RegisteredOrganization");
static const CIMName REGISTEREDPROFILE_PROPERTY_OTHERREGISTEREDORGANIZATION(
    "OtherRegisteredOrganization");

// Property names for ElementConformsToProfile
static const CIMName ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD(
    "ConformantStandard");
static const CIMName ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT(
    "ManagedElement");

// Property names for ProviderProfileCapabilities
static const CIMName PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE(
    "RegisteredProfile");
static const CIMName PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES(
    "RegisteredSubProfiles");
static const CIMName PROFILECAPABILITIES_PROPERTY_PROFILEVERSION(
    "ProfileVersion");
static const CIMName PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDPROFILE(
    "OtherRegisteredProfile");
static const CIMName PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDSUBPROFILES(
    "OtherRegisteredSubProfiles");
static const CIMName PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDORGANIZATION(
    "OtherRegisteredOrganization");
static const CIMName PROFILECAPABILITIES_PROPERTY_CONFORMINGELEMENTS(
    "ConformingElements");

// Property names for Provider Referenced Profiles
static const CIMName REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILES(
    "RegisteredProfiles");
static const CIMName REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILES(
    "DependentProfiles");
static const CIMName REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILEVERSIONS(
    "RegisteredProfileVersions");
static const CIMName REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILEVERSIONS(
    "DependentProfileVersions");
static const CIMName REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILES(
    "OtherRegisteredProfiles");
static const CIMName REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILES(
    "OtherDependentProfiles");
static const CIMName
REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILEORGANIZATIONS(
    "OtherRegisteredProfileOrganizations");
static const CIMName
REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILEORGANIZATIONS(
    "OtherDependentProfileOrganizations");

// Property names for ProviderModule class
#define PROVIDERMODULE_PROPERTY_NAME  COMMON_PROPERTY_NAME
static const CIMName PROVIDERMODULE_PROPERTY_VENDOR("Vendor");
static const CIMName PROVIDERMODULE_PROPERTY_VERSION("Version");
static const CIMName PROVIDERMODULE_PROPERTY_INTERFACETYPE("InterfaceType");
static const CIMName PROVIDERMODULE_PROPERTY_MAJORVERSION("MajorVersion");
static const CIMName PROVIDERMODULE_PROPERTY_MINORVERSION("MinorVersion");
static const CIMName PROVIDERMODULE_PROPERTY_REVISIONNUMBER("RevisionNumber");
static const CIMName PROVIDERMODULE_PROPERTY_BUILDNUMBER("BuildNumber");

// Property names for Provider class
#define PROVIDER_PROPERTY_NAME  COMMON_PROPERTY_NAME
static const CIMName PROVIDER_PROPERTY_PROVIDERMODULENAME(
    "ProviderModuleName");

// Property names for PG_Capabilities class
#define CAPABILITIES_PROPERTY_PROVIDERMODULENAME \
    PROVIDER_PROPERTY_PROVIDERMODULENAME
static const CIMName CAPABILITIES_PROPERTY_PROVIDERNAME("ProviderName");

// Property names for PG_ProviderCapabilities
#define PROVIDERCAPABILITIES_PROPERTY_PROVIDERMODULENAME \
    PROVIDER_PROPERTY_PROVIDERMODULENAME
#define PROVIDERCAPABILITIES_PROPERTY_PROVIDERNAME \
    CAPABILITIES_PROPERTY_PROVIDERNAME
static const CIMName PROVIDERCAPABILITIES_PROPERTY_CLASSNAME("ClassName");
static const CIMName PROVIDERCAPABILITIES_PROPERTY_NAMESPACES("Namespaces");

// Generic property names for Dependency and its subclasses
static const CIMName PROPERTY_ANTECEDENT("Antecedent");
static const CIMName PROPERTY_DEPENDENT("Dependent");

// Property names for SoftwareIdentity
#define SOFTWAREIDENTITY_PROPERTY_NAME  COMMON_PROPERTY_NAME
#define SOFTWAREIDENTITY_PROPERTY_MAJORVERSION \
    PROVIDERMODULE_PROPERTY_MAJORVERSION
#define SOFTWAREIDENTITY_PROPERTY_MINORVERSION \
    PROVIDERMODULE_PROPERTY_MINORVERSION
#define SOFTWAREIDENTITY_PROPERTY_REVISIONNUMBER \
    PROVIDERMODULE_PROPERTY_REVISIONNUMBER
#define SOFTWAREIDENTITY_PROPERTY_BUILDNUMBER \
    PROVIDERMODULE_PROPERTY_BUILDNUMBER
static const CIMName SOFTWAREIDENTITY_PROPERTY_INSTANCEID("InstanceID");
static const CIMName SOFTWAREIDENTITY_PROPERTY_VERSION("VersionString");
static const CIMName SOFTWAREIDENTITY_PROPERTY_MANUFACTURER("Manufacturer");
static const CIMName SOFTWAREIDENTITY_PROPERTY_CLASSIFICATIONS(
    "Classifications");

// Property names for InstalledSoftwareIdentity
static const CIMName INSTALLEDSOFTWAREIDENTITY_PROPERTY_SYSTEM("System");
static const CIMName INSTALLEDSOFTWAREIDENTITY_PROPERTY_INSTALLEDSOFTWARE(
    "InstalledSoftware");

// Implementation-specific constant strings
static const String SNIA_NAME("SNIA");
static const String SNIA_VER_110("1.1.0");
static const String INTEROP_PROVIDER_NAME("Interoperability Provider");
static const String PEGASUS_MODULE_NAME = String(PEGASUS_CIMOM_GENERIC_NAME) +
    String(" ") + String(PEGASUS_PRODUCT_NAME);
static const String PEGASUS_INTERNAL_PROVIDER_TYPE("Internal Control Provider");
static const String PEGASUS_DYNAMIC("__DYNAMIC_");
static const Uint32 PEGASUS_DYNAMIC_LEN(PEGASUS_DYNAMIC.size());

// Create a context container to prevent reentry into the 
// Defines to serve as the ENUM for class selection for instance
// operations.

enum TARGET_CLASS {
        PG_NAMESPACE,
        PG_OBJECTMANAGER,
        PG_CIMXMLCOMMUNICATIONMECHANISM,
        PG_NAMESPACEINMANAGER,
        PG_COMMMECHANISMFORMANAGER,
        PG_REGISTEREDPROFILE,
        PG_REGISTEREDSUBPROFILE,
        PG_REFERENCEDPROFILE,
        PG_ELEMENTCONFORMSTOPROFILE,
        PG_SUBPROFILEREQUIRESPROFILE,
        PG_SOFTWAREIDENTITY,
        PG_ELEMENTSOFTWAREIDENTITY,
        PG_INSTALLEDSOFTWAREIDENTITY,
        PG_COMPUTERSYSTEM,
        PG_HOSTEDOBJECTMANAGER,
        PG_HOSTEDACCESSPOINT
};

inline String concatPathArray(Array<CIMObjectPath>& p)
{
    String rtn;
    for (Uint32 i = 0 ; i < p.size() ; i++)
    {
        if (i > 0)
            rtn.append(" ");
        rtn.append(p[i].toString());
    }
    return(rtn);
}

inline CIMObjectPath buildInstancePath(const CIMClass & cimClass,
                                const String & hostName,
                                const CIMNamespaceName & nameSpace,
                                const CIMInstance & instance)
{
    CIMObjectPath objPath = instance.buildPath(cimClass);
    objPath.setHost(hostName);
    objPath.setNameSpace(nameSpace);
    return objPath;
}

template <class RetClass>
RetClass getRequiredValue(const CIMInstance & instance,
                          const CIMName & propName)
{
    RetClass retVal;
    Uint32 index = instance.findProperty(propName);
    if(index == PEG_NOT_FOUND)
    {
        throw CIMOperationFailedException("Instance " +
            instance.getPath().toString() +
            " missing expected property " + propName.getString());
    }
    const CIMValue & tmpVal = instance.getProperty(index).getValue();
    if(tmpVal.isNull())
    {
        throw CIMOperationFailedException("Instance " +
            instance.getPath().toString() +
            " has unexpected NULL value for property " + propName.getString());
    }

    tmpVal.get(retVal);

    return retVal;
}

//*************************************************************
//  Constructor
//**********************************************************
InteropProvider::InteropProvider(CIMRepository * rep) : repository(rep),
    hostName(System::getHostName()), namespacesInitialized(false)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,"InteropProvider::InteropProvider");

    // Initialize the object manager
    CIMInstance objectManager = getObjectManagerInstance();
    objectManager.getProperty(objectManager.findProperty(
        OM_PROPERTY_NAME)).getValue().get(objectManagerName);

    Uint32 gatherDataIndex = objectManager.findProperty(
        OM_PROPERTY_GATHERSTATISTICALDATA);
    if(gatherDataIndex != PEG_NOT_FOUND)
    {
        CIMConstProperty gatherDataProp =
            objectManager.getProperty(gatherDataIndex);
        if (gatherDataProp.getType() == CIMTYPE_BOOLEAN)
        {
            CIMValue gatherDataVal  = gatherDataProp.getValue();
            if (!gatherDataVal.isNull())
            {
                Boolean gatherData;
                gatherDataVal.get(gatherData);
                if (gatherData == true) 
                {
                    StatisticalData* sd = StatisticalData::current();
                    sd->setCopyGSD(true);
                }
            }
        }
    }

    profileCapabilitiesClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES, false, true, false);
    providerClassifications.append(Uint16(5)); // "Instrumentation"

    // Initialize the namespaces so that all namespaces with the
    // CIM_elementConformsToProfile class also have the
    // PG_elementConformsToProfile class
    if(!namespacesInitialized)
    {
        Array<CIMNamespaceName> namespaceNames = 
            repository->enumerateNameSpaces();
        CIMClass conformsClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE);
        CIMClass profileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE);
        for(Uint32 i = 0, n = namespaceNames.size(); i < n; ++i)
        {
            // Check if the PG_ElementConformsToProfile class is present
            CIMNamespaceName & currentNamespace = namespaceNames[i];

            CIMClass tmpCimClass;
            CIMClass tmpPgClass;
            CIMClass tmpPgProfileClass;
            try
            {
                // Look for these classes in the same try-block since the
                // second depends on the first
                tmpCimClass = repository->getClass(currentNamespace,
                    PEGASUS_CLASSNAME_CIM_ELEMENTCONFORMSTOPROFILE);
                tmpPgClass = repository->getClass(currentNamespace,
                    PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE);
            }
            catch(...)
            {
            }
            try
            {
                tmpPgProfileClass = repository->getClass(currentNamespace,
                    PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE);
            }
            catch(...)
            {
                // Note: if any of the above three classes aren't found,
                // an exception will be thrown, which we can ignore since it's
                // an expected case
                // TBD: Log trace message?
            }

            // If the CIM_ElementConformsToProfile class is present, but
            // the PG_ElementConformsToProfile or PG_RegisteredProfile
            // class is not, then add it to that namespace.
            //
            // Note that we don't have to check for the
            // CIM_RegisteredProfile class because if the
            // CIM_ElementConformsToProfile class is present, the
            // CIM_RegisteredProfile class must also be present.
            if(!tmpCimClass.isUninitialized())
            {
                if(tmpPgClass.isUninitialized())
                {
                    CIMObjectPath newPath = conformsClass.getPath();
                    newPath.setNameSpace(currentNamespace);
                    conformsClass.setPath(newPath);
                    repository->createClass(currentNamespace,
                        conformsClass);
                }
                if(tmpPgProfileClass.isUninitialized())
                {
                    CIMObjectPath newPath = conformsClass.getPath();
                    newPath.setNameSpace(currentNamespace);
                    conformsClass.setPath(newPath);
                    repository->createClass(currentNamespace,
                        profileClass);
                }
            }
        }

        namespacesInitialized = true;
    }

    // Now cache the Registration info used for ElementConformsToProfile assoc
    cacheProfileRegistrationInfo();
    PEG_METHOD_EXIT();
}

//***************************************************************
// Provider Utility Functions
//***************************************************************

inline String boolToString(Boolean x)
{
    return (x ? "true" : "false");
}

inline String propertyListToString(const CIMPropertyList& pl)
{
    if(pl.isNull())
        return "NULL";
    else if(pl.size() == 0)
        return "EMPTY";

    String tmp;
    for (Uint32 i = 0; i < pl.size() ; i++)
    {
        if (i > 0)
            tmp.append(", ");
        tmp.append(pl[i].getString());
    }
    return tmp;
}

/** Determines if the namespace is allowable for this operation.
    This provider is designed to accept either all namespaces or
    limit itself to just one for operations.  In all cases, it
    will provide the required answers and use the correct namespace
    for any persistent information.  However, it may be configured
    to either accept input operations from any namespace or simply
    from one (normally the interop namespace).
    @ objectReference for the operation.  This must include the
    namespace and class name for the operation.
    @return Returns normally if the namespace test is passed. Otherwise
    it generates a CIMException (CIM_ERR_NOT_SUPPORTED)
    @exception CIMException(CIM_ERR_NOT_SUPPORTED)
*/
bool namespaceSupported(const CIMObjectPath & path)
{
    // To allow use of all namespaces, uncomment the following line
    // return;
    if(path.getNameSpace().getString() == PEGASUS_NAMESPACENAME_INTEROP)
      return true;

    throw CIMNotSupportedException(path.getClassName().getString() +
      " in namespace " + path.getNameSpace().getString());
    
    return false;
}

/* complete the instance by setting the complete path into the instance
   and executing the instance filter to set the qualifiers, classorigin and
   propertylist in accordance with the input.  Note that this can only remove
   characteristics, except for the path completion so that it expects instances
   with qualifiers included, class origin included and a complete property
   list.
*/
void normalizeInstance(CIMInstance& instance, const CIMObjectPath& path,
                       Boolean includeQualifiers, Boolean includeClassOrigin,
                       const CIMPropertyList& propertyList)
{
    CIMObjectPath p = instance.getPath();
    p.setHost(path.getHost());
    p.setNameSpace(path.getNameSpace());

    instance.setPath(p);
    instance.filter(includeQualifiers,
                    includeClassOrigin,
                    propertyList );
}

/** get one string property from an instance. Note that these functions simply
    return the default value if the property cannot be found or is of the wrong
    type thus, in reality being a maintenance problem since there is no
    error indication.
    @param instance CIMInstance from which we get property value
    @param propertyName String name of the property containing the value
    @param default String optional parameter that is substituted if the property
    does not exist, is Null, or is not a string type. The substitute is
    String::EMPTY
    @return String value found or defaultValue.
*/
String getPropertyValue(const CIMInstance& instance,
    const CIMName& propertyName, const String& defaultValue)
{
    String output = defaultValue;
    Uint32 pos = instance.findProperty(propertyName);
    if(pos != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if(p1.getType() == CIMTYPE_STRING)
        {
            CIMValue v1  = p1.getValue();

            if(!v1.isNull())
                v1.get(output);
        }
        else
        {
            throw CIMInvalidParameterException(
                "Incorrect Property Type for Property " +
                propertyName.getString());
        }
    }

    return output;
}

// Overload of getPropertyValue for boolean type
Boolean getPropertyValue(const CIMInstance& instance,
    const CIMName& propertyName, const Boolean defaultValue)
{
    Boolean output = defaultValue;
    Uint32 pos = instance.findProperty(propertyName);
    if(pos != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_BOOLEAN)
        {
            CIMValue v1  = p1.getValue();

            if (!v1.isNull())
                v1.get(output);
        }
        else
        {
            throw CIMInvalidParameterException(
                "Incorrect Property Type for Property " +
                propertyName.getString());
        }
    }
    return(output);
}

/** get Host IP address from host name. If the
    host name is not provided, uses internal function.
    If everything fails, gets the definition normally
    used for localhost (127.0.0.1).

    @param hostName String with the name of the host. Allows
    String:EMPTY and in that case, gets it directly from system.
    @param namespaceType - Uint32 representing the
    access protocol for this request.  This is exactly
    the definition in the PG_CIMXMLCommunicationMechanism
    mof for the property namespaceAccessProtocol.
    @param port String defining the port to be used.  If 
    String::EMPTY, it is not valid and the defaultPortNumber
    is to be inserted.
    @param defaultPortNumber Uint32 defining a default port
    number to be used if port string is not provided.
    @return String with the IP address to be used. This must
    be the complete address sufficient to access the
    IP address. Therefore, it includes the port number.
*/
String getHostAddress(const String & hostName, Uint32 namespaceType,
    const String & port, Uint32 defaultPortNumber)
{
  String ipAddress;
  if(hostName == String::EMPTY)
      ipAddress = System::getHostIP(System::getHostName());
  else
      ipAddress = System::getHostIP(hostName);

  if(ipAddress == String::EMPTY)
  {
      // set default address if everything else failed
      ipAddress = String("127.0.0.1");
  }

  // Question: is there a case where we leave off the port number.
  // Code to get the property service_location_tcp ( which is equivalent to
  // "IP address:5988")

  // if port is valid port number, we use it.  Else use
  // the default portnumber provided.
  // One or the other MUST not be zero.
  ipAddress.append(":");
  if(port == String::EMPTY)
  {
      // convert portNumber to ascii
      char buffer[32];
      sprintf(buffer, "%u", defaultPortNumber);
      ipAddress.append(buffer);
  }
  else
  {
      ipAddress.append(port);
  }

  return ipAddress;
}
 /** Fills in the CIMOperation functional profiles and corresponding description
     array.  This function is closely linked to compile and configuration
     features in the CIM Server to determine if certain features are 
     enabled and/or compiled.  Definitions correspond to the DMTF SLP template
     version 1.0.
     @param Array<Uint16> profiles provides an array for the profiles
     @return Array<String> with the corresponding profile text descriptions
*/
 Array<String> getFunctionalProfiles(Array<Uint16> & profiles)
 {
     Array<String> profileDescriptions;
     // Note that zero and 1 are unknown and other. Not used by us
     // 2 - 5 are not optional in Pegasus
     profiles.append(2);
     profileDescriptions.append("Basic Read");

     profiles.append(3);
     profileDescriptions.append("Basic Write");

     profiles.append(4);
     profileDescriptions.append("Schema Manipulation");

     profiles.append(5);
     profileDescriptions.append("Instance Manipulation");

     ConfigManager* configManager = ConfigManager::getInstance();
     if (String::equal(configManager->getCurrentValue(
         "enableAssociationTraversal"), "true"))
     {
         profiles.append(6);
         profileDescriptions.append("Association Traversal");
     }
#ifndef PEGASUS_DISABLE_EXECQUERY
     profiles.append(7);
     profileDescriptions.append("Query Execution");
#endif
     profiles.append(8);
     profileDescriptions.append("Qualifier Declaration");

     if (String::equal(configManager->getCurrentValue(
         "enableIndicationService"), "true"))
     {
         profiles.append(9);
         profileDescriptions.append("Indications");
     }

     return(profileDescriptions);
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
Boolean validateRequiredProperty(const CIMInstance& instance,
    const CIMName& propertyName, const String& value)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateRequiredProperty()");
    Uint32 pos = instance.findProperty (propertyName);

    if(pos == PEG_NOT_FOUND)
    {
        PEG_METHOD_EXIT();
        return false;
    }
    //
    //  Get the property
    //
    CIMConstProperty theProperty = instance.getProperty(pos);
    const CIMValue theValue = theProperty.getValue();
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


Boolean validateRequiredProperty(const CIMInstance& instance,
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

Boolean validateRequiredProperty(const CIMObjectPath& objectPath,
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

/* Verify that this is one of the legal classnames for instance operations and
   return an indicator as to which one it is.
   @param - Classname
   @return - Uint32 indicating type
   @Exceptions - throws CIMNotSupportedException if invalid class.
*/
TARGET_CLASS translateClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::translateClassInput");
    if(className.equal(PEGASUS_CLASSNAME_PG_OBJECTMANAGER))
        return PG_OBJECTMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM))
        return PG_CIMXMLCOMMUNICATIONMECHANISM;

    else if(className.equal(PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER))
        return PG_NAMESPACEINMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER))
        return PG_COMMMECHANISMFORMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE))
        return PG_REGISTEREDPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE))
        return PG_REGISTEREDSUBPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE))
        return PG_REFERENCEDPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE))
        return PG_ELEMENTCONFORMSTOPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE))
        return PG_SUBPROFILEREQUIRESPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY))
        return PG_SOFTWAREIDENTITY;

    else if(className.equal(PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY))
        return PG_ELEMENTSOFTWAREIDENTITY;

    else if(className.equal(PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY))
        return PG_INSTALLEDSOFTWAREIDENTITY;

    else if(className.equal(PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM))
        return PG_COMPUTERSYSTEM;

    else if(className.equal(PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER))
        return PG_HOSTEDOBJECTMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT))
        return PG_HOSTEDACCESSPOINT;

    // Last entry, reverse test and return OK if PG_Namespace
    // Note: Changed to PG_Namespace for CIM 2.4
    else if(!className.equal(PEGASUS_CLASSNAME_PG_NAMESPACE))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    PEG_METHOD_EXIT();
    return PG_NAMESPACE;
}

TARGET_CLASS translateAssocClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::translateAssocClassInput");

    if(className.equal(PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER))
        return PG_NAMESPACEINMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER))
        return PG_COMMMECHANISMFORMANAGER;

    else if(className.equal(PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE))
        return PG_REFERENCEDPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE))
        return PG_ELEMENTCONFORMSTOPROFILE;

    else if(className.equal(PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY))
        return PG_ELEMENTSOFTWAREIDENTITY;

    // Last entry, reverse test and return OK if PG_SubProfileRequiresProfile
    else if(!className.equal(PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE))
    {
        throw CIMNotSupportedException(className.getString() +
          " not supported by association operations in the Interop Provider");
    }

    PEG_METHOD_EXIT();
    return PG_SUBPROFILEREQUIRESPROFILE;
}


/*
 * Retrieves the user ID from the operation context.
 */
String retrieveUserID(const OperationContext & context)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_validateUserID");
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

/** Set the value of a property defined by property name in the instance
    provided.
    Sets a String into the value field unless the property name cannot be found.
    If the property cannot be found, it simply returns.
    ATTN: This function does not pass an error back if property not found.
    @param instance CIMInstance in which to set property value
    @param propertyName CIMName of property in which value will be set.
    @param value String value to set into property

*/
void setPropertyValue(CIMInstance& instance, const CIMName& propertyName,
    const CIMValue & value)
{
    Uint32 pos = instance.findProperty(propertyName);
    if(pos != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(value);
}

/** add the correct values to the common keys defined for all of the classes.
    This is systemcreationclassname and systemname
    Note that if the properties do not exist, we simply ignore them.
*/
void fixInstanceCommonKeys(CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_fixInstanceCommonKeys()");

    setPropertyValue(instance, COMMON_PROPERTY_SYSTEMCREATIONCLASSNAME,
            PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM.getString());

    // Add property SystemName
    setPropertyValue(instance, COMMON_PROPERTY_SYSTEMNAME,
            System::getFullyQualifiedHostName());
    PEG_METHOD_EXIT();
}

/** builds one instance of the class named className. Gets Class defintion and
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
CIMInstance InteropProvider::buildInstanceSkeleton(
      const CIMNamespaceName & nameSpace,
      const CIMName& className,
      CIMClass& returnedClass)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceSkeleton()");
    // get class with lo = false, qualifier = true classorig = true
    returnedClass = repository->getClass(nameSpace,
        className, false, true, true);
    CIMInstance skeleton = returnedClass.buildInstance(true,true,
        CIMPropertyList());

    PEG_METHOD_EXIT();
    return skeleton;
}

/* build a single instance of the cimxmlcommunicationmechanism class
   using the parameter provided as the name property.
   Builds the complete instance and sets the path into it.
   @parm name String representing the name to be used for this object.
   @return CIMInstance of the class
*/
CIMInstance InteropProvider::buildCIMXMLCommunicationMechanismInstance(
            const String& namespaceType,
            const Uint16& accessProtocol,
            const String& IPAddress,
            const CIMClass & targetClass)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::buildCIMXMLCommunicationMechanismInstance()");
    CIMInstance instance = targetClass.buildInstance(false, false,
        CIMPropertyList());

    fixInstanceCommonKeys(instance);

    //CreationClassName
    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_CREATIONCLASSNAME,
            PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM.getString());

    //Name, this CommunicationMechanism.  We need to make it unique.  To do this
    // we simply append the commtype to the classname since we have max of two
    // right now.
    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_NAME,
            (String("PEGASUSCOMM") + namespaceType));

    // CommunicationMechanism Property - Force to 2.
    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_COMMUNICATIONMECHANISM,
        Uint16(2));

    //Functional Profiles Supported Property.
    Array<Uint16> profiles;
    Array<String> profileDescriptions = getFunctionalProfiles(profiles);

    // Set functional profiles in instance
    setPropertyValue(instance,
        CIMXMLCOMMMECH_PROPERTY_FUNCTIONALPROFILESSUPPORTED, profiles);

    setPropertyValue(instance,
        CIMXMLCOMMMECH_PROPERTY_FUNCTIONALPROFILEDESCRIPTIONS,
        profileDescriptions);

    // Multiple OperationsSupported Property
    setPropertyValue(instance,
        CIMXMLCOMMMECH_PROPERTY_MULTIPLEOPERATIONSSUPPORTED, false);

    // AuthenticationMechanismsSupported Property
    Array<Uint16> authentications;
    Array<String> authenticationDescriptions;

    //TODO - get from system.
    authentications.append(3);
    //authenticationDescriptions.append("Basic");

    setPropertyValue(instance,
        CIMXMLCOMMMECH_PROPERTY_AUTHENTICATIONMECHANISMSSUPPORTED,
        authentications);

    setPropertyValue(instance,
        CIMXMLCOMMMECH_PROPERTY_AUTHENTICATIONMECHANISMDESCRIPTIONS,
        authenticationDescriptions);

    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_VERSION,
        CIMXMLProtocolVersion);

    // Obsolete function
    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_NAMESPACETYPE,
        namespaceType);

    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_NAMESPACEACCESSPROTOCOL,
        accessProtocol);

    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_IPADDRESS,
        IPAddress);

    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_ELEMENTNAME,
        String("Pegasus CIMXML Communication Mechanism"));

    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_CIMVALIDATED,
        Boolean(false));

    Array<Uint16> opStatus;
    opStatus.append(2); // "OK"
    setPropertyValue(instance, CIMXMLCOMMMECH_PROPERTY_OPERATIONALSTATUS,
        opStatus);

    // build the instance path and set into instance
    CIMObjectPath objPath = instance.buildPath(targetClass);
    objPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    objPath.setHost(hostName);
    instance.setPath(objPath);

    PEG_METHOD_EXIT();
    return instance;
}

Array<CIMInstance> InteropProvider::enumCIMXMLCommunicationMechanismInstances()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::enumCIMXMLCommunicationMechanismInstances");

    ConfigManager* configManager = ConfigManager::getInstance();
    Boolean enableHttpConnection = String::equal(
        configManager->getCurrentValue("enableHttpConnection"), "true");
    Boolean enableHttpsConnection = String::equal(
        configManager->getCurrentValue("enableHttpsConnection"), "true");

    Array<CIMInstance> instances;
    Uint32 namespaceAccessProtocol;
    String namespaceType;

    CIMClass commMechClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM, false, true,
            false);
    if (enableHttpConnection)
    {
        namespaceAccessProtocol = 2;
        namespaceType = "http";
        Uint32 portNumberHttp;
        String httpPort = configManager->getCurrentValue("httpPort");
        if (httpPort == String::EMPTY)
        {
            portNumberHttp = System::lookupPort(WBEM_HTTP_SERVICE_NAME,
                WBEM_DEFAULT_HTTP_PORT);
        }
        CIMInstance instance = 
            buildCIMXMLCommunicationMechanismInstance(
                namespaceType,
                namespaceAccessProtocol,
                getHostAddress(hostName, namespaceAccessProtocol, httpPort,
                    portNumberHttp), commMechClass);
        instances.append(instance);
    }

    if (enableHttpsConnection)
    {
        namespaceAccessProtocol = 3;
        namespaceType = "https";
        Uint32 portNumberHttps;
        String httpsPort = configManager->getCurrentValue("httpsPort");
        if (httpsPort == String::EMPTY)
        {
            portNumberHttps = System::lookupPort(WBEM_HTTPS_SERVICE_NAME,
                WBEM_DEFAULT_HTTPS_PORT);
        }
        CIMInstance instance = 
            buildCIMXMLCommunicationMechanismInstance(
                namespaceType,
                namespaceAccessProtocol,
                getHostAddress(hostName, namespaceAccessProtocol, httpsPort,
                    portNumberHttps), commMechClass);

        instances.append(instance);
    }


    PEG_METHOD_EXIT();
    return instances;
}

/** get an instance of the CIM_ObjectManager class filling out
    the required properties if one does not already exist in the
    repository. This function will either return an instance
    or throw an exception.
    @param includeQualifiers Boolean
    @param includeClassOrigin Boolean
    @param propertylist CIMPropertyList
    @return CIMInstance with a single built instance of the class
    @exception repository instances if exception to enumerateInstances
        for this class.
*/
CIMInstance InteropProvider::getObjectManagerInstance()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::getObjectManagerInstance");

    // Try to get the current object.  If true then it is already created.
    CIMInstance instance;
    bool found = false;
    // If a real object path is not specified (only the class and
    // and namespace are present), then we can enumerate and pick
    // the first instance (if any) and then set found to true
    Array<CIMInstance> tmpInstances = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_OBJECTMANAGER, true, false, false, false,
        CIMPropertyList());
    Uint32 numInstances = tmpInstances.size();
    if(numInstances == 1)
    {
        instance = tmpInstances[0];
    }
    PEGASUS_ASSERT(numInstances <= 1);


    if(instance.isUninitialized())
    {
        //
        // No instance in the repository. Build new instance and save it.
        //
        CIMClass omClass;
        instance = buildInstanceSkeleton(PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_OBJECTMANAGER, omClass);

        fixInstanceCommonKeys(instance);

        setPropertyValue(instance, OM_PROPERTY_CREATIONCLASSNAME,
            PEGASUS_CLASSNAME_PG_OBJECTMANAGER.getString());
        setPropertyValue(instance, OM_PROPERTY_NAME,
            String(PEGASUS_INSTANCEID_GLOBAL_PREFIX) + ":" + Guid::getGuid());
        setPropertyValue(instance, OM_PROPERTY_ELEMENTNAME, String("Pegasus"));
        Array<Uint16> operationalStatus;
        operationalStatus.append(2);
        setPropertyValue(instance, OM_PROPERTY_OPERATIONALSTATUS,
            operationalStatus);
        setPropertyValue(instance, OM_PROPERTY_STARTED,
            CIMValue(Boolean(true)));

        //
        //Description property this object manager instance
        // default is Pegasus CIM_Server Version. Get from
        // fields defined in PegasusVersion.
        // TODO. Add as an alternative the capability to get this
        // from config parameters.
        // If PEGASUS_CIMOM_DESCRIPTION is non-zero length, use it.
        // Otherwise build form the components below.
        String descriptionStatus;
        String pegasusProductStatus(PEGASUS_PRODUCT_STATUS);
        if(pegasusProductStatus.size() > 0)
            descriptionStatus = " " + pegasusProductStatus;

        String description = (String(PEGASUS_CIMOM_DESCRIPTION).size() != 0) ?
                String(PEGASUS_CIMOM_DESCRIPTION)
            :
                String(PEGASUS_CIMOM_GENERIC_NAME) + " " +
                String(PEGASUS_PRODUCT_NAME) + " Version " +
                String(PEGASUS_PRODUCT_VERSION) +
                descriptionStatus;

        setPropertyValue(instance, OM_PROPERTY_DESCRIPTION, description);

        //Property GatherStatisticalData. Initially this is set to false
        // and can then be modified by a modify instance on the instance.

        Boolean gatherStatDataFlag = false;

        setPropertyValue(instance, OM_PROPERTY_GATHERSTATISTICALDATA,
            Boolean(gatherStatDataFlag));

        // Set the statistics property into the Statisticaldata class so that
        // it can perform statistics gathering if necessary.
    #ifndef PEGASUS_DISABLE_PERFINST
        StatisticalData* sd = StatisticalData::current();
        sd->setCopyGSD(gatherStatDataFlag);
    #endif

        // write instance to the repository
        CIMObjectPath instancePath;

        instancePath = repository->createInstance(
            PEGASUS_NAMESPACENAME_INTEROP, instance);
        instance.setPath(instancePath);
    }

    CIMObjectPath currentPath = instance.getPath();
    currentPath.setHost(hostName);
    currentPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instance.setPath(currentPath);
    PEG_METHOD_EXIT();
    return instance;
}

/** get an instance of the PG_ComputerSystem class produced by the
    ComputerSystem provider in the root/cimv2 namespace.
    This function will either return an instance
    or throw an exception.
    @param includeQualifiers Boolean
    @param includeClassOrigin Boolean
    @param propertylist CIMPropertyList
    @return CIMInstance with a single built instance of the class
    @exception ObjectNotFound exception if a ComputerSystem instance cannot
        be retrieved.
*/
CIMInstance InteropProvider::getComputerSystemInstance()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::getComputerSystemInstance");

    // Try to get the current object.  If true then it is already created.
    CIMInstance instance;

    // If a real object path is not specified (only the class and
    // and namespace are present), then we can enumerate and pick
    // the first instance (if any) and then set found to true
    AutoMutex mut(interopMut);
    Array<CIMInstance> tmpInstances = cimomHandle.enumerateInstances(
        OperationContext(),
        PEGASUS_NAMESPACENAME_CIMV2,
        PEGASUS_CLASSNAME_PG_COMPUTERSYSTEM, true, false, false, false,
        CIMPropertyList());
    Uint32 numInstances = tmpInstances.size();
    PEGASUS_ASSERT(numInstances <= 1);
    if(numInstances > 0)
    {
        instance = tmpInstances[0];
        CIMObjectPath tmpPath = instance.getPath();
        tmpPath.setHost(hostName);
        tmpPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
        instance.setPath(tmpPath);
    }

    if(instance.isUninitialized())
    {
        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_FOUND,
            "Could not find ComputerSystem instance");
    }

    PEG_METHOD_EXIT();
    return instance;
}

/** get an instance of the PG_HostedObjectManager class produced by the
    ComputerSystem provider in the root/cimv2 namespace.
    This function will either return an instance
    or throw an exception.
    @param includeQualifiers Boolean
    @param includeClassOrigin Boolean
    @param propertylist CIMPropertyList
    @return CIMInstance with a single built instance of the class
    @exception ObjectNotFound exception if a ComputerSystem instance cannot
        be retrieved.
*/
CIMInstance InteropProvider::getHostedObjectManagerInstance()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::getHostedObjectManagerInstance");

    // Try to get the current object.  If true then it is already created.
    CIMInstance instance;
    bool found = false;

    CIMObjectPath csPath = getComputerSystemInstance().getPath();
    CIMObjectPath omPath = getObjectManagerInstance().getPath();
    String csPathString = csPath.toString();
    String omPathString = omPath.toString();

    CIMClass hostedOMClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_HOSTEDOBJECTMANAGER,
        false, true, false);

    instance = hostedOMClass.buildInstance(false, false, CIMPropertyList());

    setPropertyValue(instance, PROPERTY_ANTECEDENT,
        CIMValue(csPath));
    setPropertyValue(instance, PROPERTY_DEPENDENT,
        CIMValue(omPath));

    instance.setPath(instance.buildPath(hostedOMClass));

    PEG_METHOD_EXIT();
    return instance;
}

Array<CIMInstance> InteropProvider::enumHostedAccessPointInstances()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::enumHostedAccessPointInstance");
    Array<CIMInstance> instances;

    CIMObjectPath csPath = getComputerSystemInstance().getPath();
    Array<CIMInstance> commMechs = enumCIMXMLCommunicationMechanismInstances();
    CIMClass hapClass = repository->getClass(PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_HOSTEDACCESSPOINT, false, true, false);
    for(Uint32 i = 0, n = commMechs.size(); i < n; ++i)
    {
        CIMInstance & currentCommMech = commMechs[i];
        CIMInstance hapInstance = hapClass.buildInstance(false, false,
            CIMPropertyList());
        setPropertyValue(hapInstance, PROPERTY_ANTECEDENT, csPath);
        setPropertyValue(hapInstance, PROPERTY_DEPENDENT,
            currentCommMech.getPath());
        hapInstance.setPath(hapInstance.buildPath(hapClass));
        instances.append(hapInstance);
    }

    PEG_METHOD_EXIT();
    return instances;
}

/** Get the instances of CIM_Namespace. Gets all instances of the namespace from
    the repository namespace management functions. Builds instances that
    match all of the request attributes.
*/
Array<CIMInstance> InteropProvider::enumNamespaceInstances()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");

    Array<CIMNamespaceName> namespaceNames = repository->enumerateNameSpaces();
    Array<CIMInstance> instanceArray;

    // Build instances of PG namespace since that is the leaf class
    for (Uint32 i = 0, n = namespaceNames.size(); i < n; i++)
    {
       instanceArray.append(
           buildNamespaceInstance(namespaceNames[i].getString()));
    }

    PEG_METHOD_EXIT();
    return instanceArray;
}

/* build the full instances set of of the association class NamespacInManager.

    NOTE: THe input object path is not really use at this point.
*/
Array<CIMInstance> InteropProvider::enumNamespaceInManagerInstances()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::buildInstancesNamespaceInManager");

    Array<CIMInstance> namespaceInstances = enumNamespaceInstances();

    CIMObjectPath objectManagerPath = getObjectManagerInstance().getPath();

    Array<CIMInstance> assocInstances;
    CIMClass targetClass;

    CIMInstance instanceskel = buildInstanceSkeleton(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_NAMESPACEINMANAGER, targetClass);
    // Build and instance for each namespace instance.
    for (Uint32 i = 0 ; i < namespaceInstances.size() ; i++)
    {
        CIMInstance instance = instanceskel.clone();
        setPropertyValue(instance, PROPERTY_ANTECEDENT, objectManagerPath);
        setPropertyValue(instance, PROPERTY_DEPENDENT,
            namespaceInstances[i].getPath()); 

        CIMObjectPath objPath = instance.buildPath(targetClass);
        objPath.setHost(hostName);
        objPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
        instance.setPath(objPath);
        assocInstances.append(instance);
    }
    PEG_METHOD_EXIT();
    return assocInstances;
}
/* build the instances of the defined association.
*/
Array<CIMInstance> InteropProvider::enumCommMechanismForManagerInstances()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::enumCommMechanismForManagerInstances");

    Array<CIMInstance> commInstances =
        enumCIMXMLCommunicationMechanismInstances();

    CIMInstance instanceObjMgr = getObjectManagerInstance();

    CIMObjectPath refObjMgr = instanceObjMgr.getPath();

    Array<CIMInstance> assocInstances;
    CIMClass targetClass;
    CIMInstance instanceskel = buildInstanceSkeleton(
        PEGASUS_NAMESPACENAME_INTEROP, 
        PEGASUS_CLASSNAME_PG_COMMMECHANISMFORMANAGER, targetClass);
    for (Uint32 i = 0, n = commInstances.size(); i < n; ++i)
    {
        CIMInstance instance = instanceskel.clone();

        setPropertyValue(instance, PROPERTY_ANTECEDENT, refObjMgr);

        setPropertyValue(instance, PROPERTY_DEPENDENT,
          commInstances[i].getPath());

        instance.setPath(instance.buildPath(targetClass));
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
CIMInstance InteropProvider::buildNamespaceInstance(
    const String & nameSpace)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::buildInstancePGNamespace");

    CIMClass targetClass;
    CIMInstance instance = buildInstanceSkeleton(
        PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PG_NAMESPACE,
        targetClass);

    fixInstanceCommonKeys(instance);

    //ObjectManagerCreationClassName
    setPropertyValue(instance,
        CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
        PEGASUS_CLASSNAME_PG_OBJECTMANAGER.getString());

    //ObjectManagerName
    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
        objectManagerName);

    //CreationClassName
    // Class in which this was created,
    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
        PEGASUS_CLASSNAME_PG_NAMESPACE.getString());
    //Name
    // This is the namespace name itself
    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,
        nameSpace);

    //ClassInfo
    // Set the classinfo to unknown and the description to namespace.
    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CLASSINFO, Uint16(0));
    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_DESCRIPTIONOFCLASSINFO,
        String("namespace"));

    setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CLASSTYPE,
        Uint16(2));



    //
    //  Everything above was commmon to CIM Namespace.  The following is
    //  PG_Namespace Properties
    //
    // ATTN: KS Get the correct values for these entities from repository
    //       interface.

    CIMRepository::NameSpaceAttributes attributes;
    repository->getNameSpaceAttributes(nameSpace, attributes);
    String parent = String::EMPTY;
    String name = String::EMPTY;
    Boolean shareable = false;
    Boolean updatesAllowed = true;
    for (CIMRepository::NameSpaceAttributes::Iterator i = attributes.start();
        i; i++)
    {
        String key=i.key();
        String value = i.value();
        if(String::equalNoCase(key,"shareable"))
        {
            if (String::equalNoCase(value,"true"))
                shareable=true;
        }
        else if(String::equalNoCase(key,"updatesAllowed"))
        {
            if (String::equalNoCase(value,"false"))
                updatesAllowed=false;
        }
        // Test to be sure we are returning proper namespace name
        else if (String::equalNoCase(key,"name"))
        {
            if (!String::equalNoCase(value, nameSpace))
            {
                PEG_METHOD_EXIT();
                // This is poor exception since it reflects internal error. Do
                // error log
                throw CIMNotSupportedException(
                    "Namespace attribute rtnd error for key " + key +
                    "expected " + nameSpace + value + " in " +
                    String(thisProvider));
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
            // Poor error definition since it reflects internal error. do error
            // log
            throw PEGASUS_CIM_EXCEPTION (CIM_ERR_NOT_SUPPORTED, nameSpace +
                " namespace attribute " + key + " option not supported in" +
                String(thisProvider));
        }
    }
    setPropertyValue(instance, PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED,
        updatesAllowed);
    setPropertyValue(instance, PG_NAMESPACE_PROPERTY_ISSHAREABLE, shareable);
    setPropertyValue(instance, PG_NAMESPACE_PROPERTY_PARENTNAMESPACE, parent);
	  setPropertyValue(instance, PG_NAMESPACE_PROPERTY_NAME, name);

    CIMObjectPath objPath = instance.buildPath(targetClass);
    objPath.setHost(hostName);
    objPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instance.setPath(objPath);
    PEG_METHOD_EXIT();
    return instance;
}

void validatePGNamespaceKeys(const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::validatePGNamespaceKeys");

    Boolean valid = true;
    CIMName propertyName;
    if (!validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }
    if (!validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getFullyQualifiedHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }
    if (!validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                PEGASUS_CLASSNAME_PG_OBJECTMANAGER.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    // ATTN: This one still a problem.  We have to get the name first
    if (!validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!validateRequiredProperty(objectPath,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!validateRequiredProperty(objectPath,
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
Boolean completeProperty(CIMInstance& instance,
    const CIMName& propertyName,
    const String& value)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::completeProperty()");

    Uint32 pos;

    if (!validateRequiredProperty(instance,
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
            setPropertyValue(instance, propertyName, value);
        }
    }

    PEG_METHOD_EXIT();
    return(true);
}
Boolean completeCIMNamespaceKeys(CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_completeCIMNamespaceKeys");

    Boolean valid = true;
    CIMName propertyName;

    if (!completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName ()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }

    if (!completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getFullyQualifiedHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }

    if (!completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                PEGASUS_CLASSNAME_PG_OBJECTMANAGER.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    if (!completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!completeProperty(instance,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!completeProperty(instance,
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
    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "CIM_Namespace Keys Valid");
    PEG_METHOD_EXIT();
    return(valid);
}


void validateCIMNamespaceKeys(const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::validateCIMNamespaceKeys");
    Boolean valid = true;
    CIMName propertyName;
    if (!validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
                System::getSystemCreationClassName ()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME;
        valid = false;
    }

    if (!validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
                System::getFullyQualifiedHostName ()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_SYSTEMNAME;
        valid = false;
    }

    if (!validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME,
                PEGASUS_CLASSNAME_PG_OBJECTMANAGER.getString()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERCREATIONCLASSNAME;
        valid = false;
    }

    // ATTN: This one still a problem.  We have to get the name first
    if (!validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME,
                String::EMPTY))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_OBJECTMANAGERNAME;
        valid = false;
    }
    if (!validateRequiredProperty(instance,
                CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                System::getHostName()))
    {
        propertyName = CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME;
        valid = false;
    }

    if (!validateRequiredProperty(instance,
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
    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "CIM_Namespace Keys Valid");
    PEG_METHOD_EXIT();
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
String getKeyValue(const CIMObjectPath& instanceName, const CIMName& keyName)
{
    Array<CIMKeyBinding> kbArray = instanceName.getKeyBindings();

    // find the correct key binding
    for (Uint32 i = 0; i < kbArray.size(); i++)
    {
        if (kbArray[i].getName() == keyName)
            return (kbArray[i].getValue());
    }

    throw CIMInvalidParameterException("Invalid key property: " +
        keyName.getString());
}

String getKeyValue(const CIMInstance& instance, const CIMName& keyName)
{
    Uint32 pos;
    CIMValue propertyValue;

    pos = instance.findProperty(keyName);
    if (pos == PEG_NOT_FOUND)
       throw CIMPropertyNotFoundException
           (keyName.getString());

    propertyValue = instance.getProperty(pos).getValue();
    if (propertyValue.getType() != CIMTYPE_STRING)
       throw CIMInvalidParameterException("Invalid type for property: "
                             + keyName.getString());
    String name;
    propertyValue.get(name);
    return(name);
}

/** Test for valid CIMReferences from an association instance. If there is a
    role property, tests if there is a match for this role and the target
    object. Confirms that this role and this reference exist in the target
    instance.

    @param target - The target path for the association. Localization assumed.
    @param instance - The association class instance we are searching for
                      references
    @param role - The role we require. I there is no role, this is String::EMPTY
    @return - returns Boolean true if target is found in a reference that is
    the same role
 */
Boolean isInstanceValidReference(const CIMObjectPath& target,
                                  const CIMInstance& instance,
                                  const String& role)
{
    // Test if role parameter is valid property.
    Uint32 pos;
    if (role != String::EMPTY)
    {
        // Test if property with this role exists.
        if ((pos = instance.findProperty(role)) == PEG_NOT_FOUND)
            throw CIMException(CIM_ERR_INVALID_PARAMETER);

         // Check to be sure this is a reference property
         // This test may not be necessary. Combine it into the loop.
         if (instance.getProperty(pos).getType() != CIMTYPE_REFERENCE)
             throw CIMException(CIM_ERR_INVALID_PARAMETER);
    }
    //Search instance for all reference properties
    for (Uint32 j = 0; j < instance.getPropertyCount() ; j++)
    {
        const CIMConstProperty p = instance.getProperty(j);
        if (p.getType() == CIMTYPE_REFERENCE)
        {
            // If there is no role or the role is the same as this property name
            CIMValue v = p.getValue();
            CIMObjectPath path;
            v.get(path);

            // if no role or role ==this role and target = this path, rtn true.
            if ((role == String::EMPTY) || (CIMName(role) == p.getName()))
            {
                // and if target is identical to reference path
                if (target.identical(path))
                    return(true);
            }
        }
    }
    return( false );
}
/** Filters the input of instances (which contain path info)
    using the assocClassName, assocRole, ResultClassName and
    resultRole. Removes any instances from the list that
    do not match the filters.
    @instances Array<CIMInstance to filter.
    @
 * TODO - Shouldn't we remove rather than copy??? faster.
 * TODO - why extra check for no resultClass??
*/
Array<CIMObject> filterReferenceInstances(Array<CIMInstance>& instances,
                      const CIMObjectPath& targetobjectName,
                      const CIMName& resultClass,
                      const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "filterReferenceInstances()");

    CIMObjectPath targetReference = CIMObjectPath(
                            String(),
                            CIMNamespaceName(),
                            targetobjectName.getClassName(),
                            targetobjectName.getKeyBindings());
    Array<CIMObject> rtnObjects;
    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        if (resultClass.isNull() ||
            resultClass.equal(instances[i].getClassName()))
        {
            // if this association instance has this role in targetReference
            if (isInstanceValidReference(targetobjectName, instances[i],
                resultRole))
            {
                rtnObjects.append(instances[i]);
            }
        }
    }
    PEG_METHOD_EXIT();
    return( rtnObjects );
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

    AutoMutex autoMut(changeControlMutex);

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s createInstance. InstanceReference= %s",
        thisProvider,
        (const char *) instanceReference.toString().getCString());

    // test for legal namespace for this provider. Exception if not
    namespaceSupported(instanceReference);

    handler.processing();

    CIMInstance localInstance;
    const CIMName & instClassName = instanceReference.getClassName();
    TARGET_CLASS classEnum = PG_NAMESPACE;
    if(instClassName.equal(PEGASUS_CLASSNAME_CIM_NAMESPACE))
    {
        localInstance = CIMInstance(PEGASUS_CLASSNAME_PG_NAMESPACE);
        // Copy the properties of the source instance
        for (Uint32 i = 0 ; i < myInstance.getQualifierCount() ; i++)
        {
            localInstance.addQualifier(myInstance.getQualifier(i).clone());
        }

        for (Uint32 i = 0 ; i < myInstance.getPropertyCount() ; i++)
        {
            localInstance.addProperty(myInstance.getProperty(i).clone());
        }
    }
    else
    {
        classEnum = translateClassInput(instanceReference.getClassName());
    }

    
    CIMObjectPath newInstanceReference;
    CIMNamespaceName newNamespaceName;

    if (classEnum == PG_NAMESPACE)
    {
#ifdef PEGASUS_OS_OS400
        MessageLoaderParms mparms(
            "ControlProviders.InteropProvider.CREATE_INSTANCE_NOT_ALLOWED",
            "Create instance operation not allowed by Interop Provider for class $0.",
            PEGASUS_CLASSNAME_PG_NAMESPACE.getString());
        throw CIMNotSupportedException(mparms);
#else
        // Create local instance to complete any keys if not created above.
        if (localInstance.isUninitialized())
        {
            localInstance = myInstance.clone();
        }

        completeCIMNamespaceKeys(localInstance);
        // Validate that keys are as required. Does its own exception.
        newNamespaceName = getKeyValue(myInstance,
            CIM_NAMESPACE_PROPERTY_NAME);

        CIMNamespaceName opNameSpace = instanceReference.getNameSpace();
        newInstanceReference = buildInstancePath(
            repository->getClass(opNameSpace, PEGASUS_CLASSNAME_PG_NAMESPACE,
                false, true, false),
            instanceReference.getHost(), opNameSpace,
            localInstance);
#endif
    }
    else   // Invalid class for the create functions.
    {
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException(
          "InteropProvider::CreateInstance Not allowed for class " +
          instanceReference.getClassName().getString());
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
        if(localInstance.findProperty(
            PG_NAMESPACE_PROPERTY_ISSHAREABLE) != PEG_NOT_FOUND)
        {
            if(getPropertyValue(myInstance,
                PG_NAMESPACE_PROPERTY_ISSHAREABLE, false))
            {
                attributes.insert("shareable","true");
                shareable = true;
            }
            else
            {
                attributes.insert("shareable", "false");
            }
        }
        else
        {
            attributes.insert("shareable", "false");
        }

        // Optional property.  Set false if not found.
        if(localInstance.findProperty(
            PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED) != PEG_NOT_FOUND)
        {
            if (getPropertyValue(myInstance,
                PG_NAMESPACE_PROPERTY_SCHEMAUPDATESALLOWED, false))
            {
                attributes.insert("updatesAllowed","true");
                updatesAllowed = true;
            }
            else
            {
                attributes.insert("updatesAllowed", "false");
            }
        }
        else
        {
            attributes.insert("updatesAllowed", "false");
        }

        // ATTN: Need to reflect and dependencies between these properties.
        // Right now this lets anything happen.
        if (localInstance.findProperty(
            PG_NAMESPACE_PROPERTY_PARENTNAMESPACE) != PEG_NOT_FOUND)
        {
            String parent = getPropertyValue(myInstance,
                PG_NAMESPACE_PROPERTY_PARENTNAMESPACE, String::EMPTY);
            if (parent != String::EMPTY)
                attributes.insert("parent",parent);
        }
        repository->createNameSpace(newNamespaceName, attributes);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "Namespace = " + newNamespaceName.getString() +
                " successfully created.");
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "Create Namespace: Shareable = $0, Updates allowed: $1,  Parent: $2",
            newNamespaceName.getString(), shareable? 
                    "true" : "false", shareable? "true" : "false", parent );

    }
    catch(const CIMException&)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(const Exception&)
    {
        PEG_METHOD_EXIT();
        throw;
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
    const CIMName instClassName = instanceName.getClassName();
#ifndef PEGASUS_OS_OS400
    AutoMutex autoMut(changeControlMutex);

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s deleteInstance. instanceName= %s",
        thisProvider,
        (const char *) instanceName.toString().getCString());

    if(instClassName == PEGASUS_CLASSNAME_PG_NAMESPACE)
    {
        handler.processing();
        CIMNamespaceName deleteNamespaceName;
        // validate requred keys.  Exception out if not valid
        validatePGNamespaceKeys(instanceName);

        deleteNamespaceName = getKeyValue(instanceName,
            PG_NAMESPACE_PROPERTY_NAME);

        if (deleteNamespaceName.equal(PEGASUS_NAMESPACENAME_ROOT))
        {
            throw CIMNotSupportedException("root namespace cannot be deleted.");
        }

        repository->deleteNameSpace(deleteNamespaceName);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "Namespace = " + deleteNamespaceName.getString() +
                " successfully deleted.");

        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
            "Interop Provider Delete Namespace: $0",
            deleteNamespaceName.getString());
        handler.complete();

        PEG_METHOD_EXIT();
        return;
    }

#endif
    MessageLoaderParms mparms(
        "ControlProviders.InteropProvider.DELETE_INSTANCE_NOT_ALLOWED",
        "Delete instance operation not allowed by Interop Provider for class $0.",
        instClassName.getString());
    throw CIMNotSupportedException(mparms);
}

/** Local version of getInstance to be used by other functions in the
    the provider.  Returns a single instance.  Note that it always
    returns an instance.  If none was found, it is unitinialitized.
*/
CIMInstance InteropProvider::localGetInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceName,
    const CIMPropertyList & propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::localGetInstance");

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s getInstance. instanceName= %s , PropertyList= %s",
        thisProvider,
        (const char *)instanceName.toString().getCString(),
        (const char *)propertyListToString(propertyList).getCString());

    // Test if we're looking for something outside of our namespace. This will
    // happen during associators calls from PG_RegisteredProfile instances
    // through the PG_ElementConformsToProfile association
    CIMNamespaceName opNamespace = instanceName.getNameSpace();
    CIMName opClass = instanceName.getClassName();
    if(opNamespace != PEGASUS_NAMESPACENAME_INTEROP &&
        opClass != PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE)
    {
        AutoMutex mut(interopMut);
        return cimomHandle.getInstance(context, opNamespace,
            instanceName, false, false, false, propertyList);
    }

    // create reference from host, namespace, class components of
    // instance name

    CIMObjectPath ref;
    ref.setHost(instanceName.getHost());
    ref.setClassName(opClass);
    ref.setNameSpace(opNamespace);

    // Enumerate instances for this class. Returns all instances
    // Note that this returns paths setup and instances already
    // filtered per the input criteria.
    Array<CIMInstance> instances =  localEnumerateInstances(
            context,
            ref,
            propertyList);

    // deliver a single instance if found.
    CIMInstance rtnInstance;

    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
       if (instanceName == instances[i].getPath())
       {
           /* DEBUG SUPPORT
           Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "%s getInstance return instance number %u\npath: %s\n %s\n",
               thisProvider, i,
               (instances[i].getPath().toString().getCString()),
               ( ( CIMObject) instances[i]).toString().getCString());
           *****/
          rtnInstance = instances[i];
          break;
          // TODO Add test for duplicates somewhere.
       }
    }
    PEG_METHOD_EXIT();
    return rtnInstance;
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
    // test for legal namespace for this provider. Exception if not
    namespaceSupported(instanceName);

    handler.processing();

    CIMInstance myInstance = localGetInstance(
                    context,
                    instanceName,
                    propertyList);

    if (!myInstance.isUninitialized())
        handler.deliver(myInstance);
    else
        throw CIMObjectNotFoundException(instanceName.toString());
    
    handler.complete();
}

//******************************************************************************
//   localEnumerateInstances 
//   EnumerateInstances equivalent to external but returns instances
//   Used by other operations to build instances for processing
//   Note that this delivers instances as a group rather than incrementally.
//    This technique should only be used for small groups of instances.
//******************************************************************************

Array<CIMInstance> InteropProvider::localEnumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::localEnumerateInstances()");
    const CIMName & className = ref.getClassName();
    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s enumerateInstances. referenc= %s , PropertyList= %s",
        thisProvider,
        (const char *)className.getString().getCString(),
        (const char *)propertyListToString(propertyList).getCString());

    // Verify that ClassName is correct and get value
    TARGET_CLASS classEnum  = translateClassInput(className);

    Array<CIMInstance> instances;
    switch(classEnum)
    {
        case PG_OBJECTMANAGER:
        {
            instances.append(getObjectManagerInstance());
            break;
        }
        case PG_CIMXMLCOMMUNICATIONMECHANISM:
        {
            instances = enumCIMXMLCommunicationMechanismInstances();
            break;
        }
        case PG_NAMESPACEINMANAGER:
        {
            instances = enumNamespaceInManagerInstances();
            break;
        }
        case PG_COMMMECHANISMFORMANAGER:
        {
            instances = enumCommMechanismForManagerInstances();
            break;
        }
        case PG_NAMESPACE:
        {
            instances = enumNamespaceInstances();
            break;
        }
        case PG_REGISTEREDPROFILE:
        {
            instances = enumRegisteredProfileInstances();
            break;
        }
        case PG_REGISTEREDSUBPROFILE:
        {
            instances = enumRegisteredSubProfileInstances();
            break;
        }
        case PG_REFERENCEDPROFILE:
        {
            instances = enumReferencedProfileInstances();
            break;
        }
        case PG_ELEMENTCONFORMSTOPROFILE:
        {
            instances = enumElementConformsToProfileInstances(context,
                ref.getNameSpace());
            break;
        }
        case PG_SUBPROFILEREQUIRESPROFILE:
        {
            instances = enumSubProfileRequiresProfileInstances();
            break;
        }
        case PG_SOFTWAREIDENTITY:
        {
            instances = enumSoftwareIdentityInstances();
            break;
        }
        case PG_ELEMENTSOFTWAREIDENTITY:
        {
            instances = enumElementSoftwareIdentityInstances();
            break;
        }
        case PG_INSTALLEDSOFTWAREIDENTITY:
        {
            instances = enumInstalledSoftwareIdentityInstances();
            break;
        }
        case PG_COMPUTERSYSTEM:
        {
            instances.append(getComputerSystemInstance());
            break;
        }
        case PG_HOSTEDOBJECTMANAGER:
        {
            instances.append(getHostedObjectManagerInstance());
            break;
        }
        case PG_HOSTEDACCESSPOINT:
        {
            instances = enumHostedAccessPointInstances();
            break;
        }
        default:
            PEG_METHOD_EXIT();
            throw CIMNotSupportedException(className.getString() +
              " not supported by Interop Provider enumerate");
    }

    // Filter and deliver the resulting instances
    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        normalizeInstance(instances[i], ref, false,
            false, propertyList);
    }

    PEG_METHOD_EXIT();
    return instances;
}

//***************************************************************************
//                EnumerateInstances - External Operation call
//    Delivers instances back through response handler.
//***************************************************************************
void InteropProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList,
    InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::enumerateInstances()");

    // test for legal namespace for this provider. Exception if not
    namespaceSupported(ref);
    handler.processing();
    // Call the internal enumerateInstances to generate instances of defined
    // class.  This expects the instances to be returned complete including
    // complete path.
    handler.deliver(localEnumerateInstances(context, ref, propertyList));
    handler.complete();
    PEG_METHOD_EXIT();
}

/** Modify the existing object Manager Object.  Only a single property
    modification is allowed, the statistical data setting.  Any other change is
    rejected with an exception
    @param instanceReference - Reference for the instance to be modified.
    @param modifiedIns CIMInstance defining the change. If this includes more
    than a single property, the propertyList must specify modification only of
    the statisticaldata property.
    @includeQualifiers Boolean which must be false unless there are no
    qualifiers in the modifiedIns.
    @propertyList CIMPropertyList defining the property to be modified if there
    is more than one property in the modifiedIns.
    @Exceptions CIMInvalidParameterException if the parameters are not valid for
    the modification.
 */
void InteropProvider::modifyObjectManagerInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance& modifiedIns,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::modifyObjectManagerInstance");

    // Modification only allowed when Performance staticistics are active
#ifndef PEGASUS_DISABLE_PERFINST
    // the only allowed modification is one property, statistical data
    Uint32 propListSize = propertyList.size();
    for(Uint32 i = 0, n = propertyList.size(); i < n; ++i)
    {
        if(propertyList[0] != OM_PROPERTY_GATHERSTATISTICALDATA)
        {
            throw CIMNotSupportedException(String("Only modification of ") +
                OM_PROPERTY_GATHERSTATISTICALDATA.getString() + " allowed");
        }
    }

    /*
    Array<CIMName> plA;
    plA.append(CIMName(OM_PROPERTY_GATHERSTATISTICALDATA));
    CIMPropertyList allowedModifyPropertyList(plA);

    // returns only if no exception and there is property to modify.
    String errorMessage;
    if(!isModifyAllowed(context, instanceReference, modifiedIns,
        includeQualifiers, propertyList, allowedModifyPropertyList,
        errorMessage))
    {
        if(errorMessage.size() > 0)
        {
            throw CIMInvalidParameterException(errorMessage);
        }
        return;
    }*/

    Boolean statisticsFlag;
    CIMInstance myInstance;

    // We modify only if this property exists.
    // could either use the property from modifiedIns or simply replace
    // value in property from object manager.
    if (modifiedIns.findProperty(OM_PROPERTY_GATHERSTATISTICALDATA) !=
        PEG_NOT_FOUND)
    {
        myInstance = getObjectManagerInstance();
        if(myInstance.isUninitialized())
        {
            throw CIMObjectNotFoundException(instanceReference.toString());
        }
        statisticsFlag = getPropertyValue(modifiedIns,
            OM_PROPERTY_GATHERSTATISTICALDATA, false);
        // set the changed property into the instance
        setPropertyValue(myInstance, OM_PROPERTY_GATHERSTATISTICALDATA,
            statisticsFlag);
    }
    else
    {
        // if statistics property not in place, simply exit. Nothing to do
        // not considered an error
        PEG_METHOD_EXIT();
        return;
    }
    // Modify the instance on disk
    try
    {
        repository->modifyInstance(instanceReference.getNameSpace(),
            myInstance, false,  propertyList);
    }
    catch(const CIMException&)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    catch(const Exception&)
    {
        PEG_METHOD_EXIT();
        throw;
    }
    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "Interop Provider Set Statistics gathering in CIM_ObjectManager: $0",
        (statisticsFlag? "true" : "false"));
    StatisticalData* sd = StatisticalData::current();
    sd->setCopyGSD(statisticsFlag);
    PEG_METHOD_EXIT();
    return;

#else
    PEG_METHOD_EXIT();
    throw CIMNotSupportedException
        (OM_PROPERTY_GATHERSTATISTICALDATA.getString() + 
                " modify operation not supported by Interop Provider");
#endif
}
//***************************************************************************
//***************************************************************************
//                modifyInstance
//***************************************************************************
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

    AutoMutex autoMut(changeControlMutex);

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s modifyInstance. instanceReference= %s, includeQualifiers= %s, PropertyList= %s",
        thisProvider,
        (const char *) instanceReference.toString().getCString(),
        (const char *) boolToString(includeQualifiers).getCString(),
        (const char *) propertyListToString(propertyList).getCString());

    // test for legal namespace for this provider. Exception if not
    namespaceSupported(instanceReference);

    CIMName className =  instanceReference.getClassName();
    
    // begin processing the request
    handler.processing();

    if (className.equal(PEGASUS_CLASSNAME_PG_OBJECTMANAGER))
    {
        modifyObjectManagerInstance(context, instanceReference,modifiedIns,
            includeQualifiers, propertyList);
    }
    else
    {
        throw CIMNotSupportedException("Delete instance of class " +
          className.getString());
    }

    handler.complete();
    PEG_METHOD_EXIT();
    return;
}

//***************************************************************************
//***************************************************************************
//                enumerateInstanceNames
//***************************************************************************
//***************************************************************************

void InteropProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & classReference,
    ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::enumerateInstanceNames()");

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s enumerateInstanceNames. classReference= %s",
        thisProvider,
        (const char *) classReference.toString().getCString());

    // test for legal namespace for this provider. Exception if not
    // namespaceSupported(classReference);
    // NOTE: Above is commented out because the routing tables will always
    // do the right thing and that's the only way requests get here.

    // begin processing the request
    handler.processing();

    Array<CIMInstance> instances = localEnumerateInstances(
        context,
        classReference,
        CIMPropertyList());

    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        handler.deliver(instances[i].getPath());
    }

    handler.complete();
    PEG_METHOD_EXIT();
}

//**************************************************************
//**************************************************************
// Associators Operation Call
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
            "InteropProvider::associators()");
    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s associators. objectName= %s , assocClass= %s resultClass= %s role= %s resultRole %includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)associationClass.getString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)resultRole.getCString(),
        (const char *)boolToString(includeQualifiers).getCString(),
        (const char *) boolToString(includeClassOrigin).getCString(),
        (const char *)propertyListToString(propertyList).getCString());

    handler.processing();
    String originRole = role;
    String targetRole = resultRole;
    Array<CIMInstance> refs = localReferences(context, objectName,
        associationClass, originRole, targetRole, CIMPropertyList(),
        resultClass);
    for(Uint32 i = 0, n = refs.size(); i < n; ++i)
    {
        CIMInstance & currentRef = refs[i];
        CIMObjectPath currentTarget = getRequiredValue<CIMObjectPath>(
            currentRef, targetRole);
        CIMInstance tmpInstance = localGetInstance(context, currentTarget,
            propertyList);
        tmpInstance.setPath(currentTarget);
        handler.deliver(tmpInstance);
    }
    handler.complete();

    PEG_METHOD_EXIT();
}

//**************************************************************
//**************************************************************
// AssociatorNames Operation Function
//**************************************************************
//**************************************************************
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

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s associatorNames. objectName= %s , assocClass= %s resultClass= %s role= %s resultRole",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)associationClass.getString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)resultRole.getCString());

    handler.processing();
        String originRole = role;
    String targetRole = resultRole;
    Array<CIMInstance> refs = localReferences(context, objectName,
        associationClass, originRole, targetRole, CIMPropertyList(),
        resultClass);
    for(Uint32 i = 0, n = refs.size(); i < n; ++i)
    {
        CIMInstance & currentRef = refs[i];
        CIMObjectPath currentTarget = getRequiredValue<CIMObjectPath>(
            currentRef, targetRole);
        handler.deliver(currentTarget);
    }
    handler.complete();
    PEG_METHOD_EXIT();
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

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s references. objectName= %s , resultClass= %s role= %s includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)boolToString(includeQualifiers).getCString(),
        (const char*) boolToString(includeClassOrigin).getCString(),
        (const char *)propertyListToString(propertyList).getCString());

    handler.processing();
    String tmpRole = role;
    String tmpTarget;
    Array<CIMInstance> refs =
        localReferences(context, objectName, resultClass, tmpRole, tmpTarget);
    for(Uint32 i = 0, n = refs.size(); i < n; ++i)
      handler.deliver((CIMObject)refs[i]);
    handler.complete();
    PEG_METHOD_EXIT();
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

    handler.processing();

    String tmpRole = role;
    String tmpTarget;
    Array<CIMInstance> refs =
        localReferences(context, objectName, resultClass, tmpRole, tmpTarget);
    for(Uint32 i = 0, n = refs.size(); i < n; ++i)
    {
        handler.deliver(refs[i].getPath());
    }

    handler.complete();

    PEG_METHOD_EXIT();
}

bool InteropProvider::validAssocClassForObject(
    const CIMName & assocClass, const CIMName & originClass,
    const CIMNamespaceName & opNamespace,
    String & originProperty, String & targetProperty)
{
    TARGET_CLASS assocClassEnum = translateClassInput(assocClass);
    TARGET_CLASS originClassEnum;
    if(assocClassEnum != PG_ELEMENTCONFORMSTOPROFILE)
    {
        originClassEnum = translateClassInput(originClass);
    }
    else
    {
        // First check the classes served by the Interop provider...
        if(opNamespace != PEGASUS_NAMESPACENAME_INTEROP ||
            (originClass != PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE &&
            originClass != PEGASUS_CLASSNAME_PG_OBJECTMANAGER))
        {
            // Search the cached conformingElements list for the originClass,
            // returning false if it is not found
            bool found = false;

            PEGASUS_ASSERT(conformingElements.size() ==
                elementNamespaces.size());
            for(Uint32 i = 0, n = conformingElements.size(); i < n; ++i)
            {
                CIMNameArray & elementList = conformingElements[i];
                CIMNamespaceArray & namespaceList = elementNamespaces[i];
                PEGASUS_ASSERT(elementList.size() == namespaceList.size());
                for(Uint32 j = 0, m = elementList.size(); j < m; ++j)
                {
                    CIMName & curElement = elementList[j];
                    if((curElement == originClass ||
                      curElement.getString().find(PEGASUS_DYNAMIC) == 0) &&
                      opNamespace == namespaceList[j])
                    {
                        found = true;
                        break;
                    }
                }
                if(found)
                    break;
            }

            if(!found)
                return false;
        }
    }

    CIMName expectedTargetRole;
    CIMName expectedOriginRole;

    switch(assocClassEnum)
    {
      case PG_NAMESPACEINMANAGER:
          if(originClassEnum == PG_OBJECTMANAGER)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_NAMESPACE)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
          break;
      case PG_COMMMECHANISMFORMANAGER:
          if(originClassEnum == PG_OBJECTMANAGER)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
          break;
      case PG_ELEMENTCONFORMSTOPROFILE:
          if(originClass.equal(PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE))
          {
              expectedTargetRole =
                  ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT;
              expectedOriginRole =
                  ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD;
          }
          else
          {
              expectedTargetRole =
                  ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD;
              expectedOriginRole =
                  ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT;
          }
          break;
      case PG_SUBPROFILEREQUIRESPROFILE:
          if(originClassEnum == PG_REGISTEREDPROFILE)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_REGISTEREDSUBPROFILE)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
          break;
      case PG_ELEMENTSOFTWAREIDENTITY:
          if(originClassEnum == PG_SOFTWAREIDENTITY)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_REGISTEREDPROFILE ||
              originClassEnum == PG_REGISTEREDSUBPROFILE)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
          break;
      case PG_INSTALLEDSOFTWAREIDENTITY:
          if(originClassEnum == PG_SOFTWAREIDENTITY)
          {
              expectedTargetRole = INSTALLEDSOFTWAREIDENTITY_PROPERTY_SYSTEM;
              expectedOriginRole =
                  INSTALLEDSOFTWAREIDENTITY_PROPERTY_INSTALLEDSOFTWARE;
          }
          else if(originClassEnum == PG_COMPUTERSYSTEM)
          {
              expectedTargetRole =
                  INSTALLEDSOFTWAREIDENTITY_PROPERTY_INSTALLEDSOFTWARE;
              expectedOriginRole = INSTALLEDSOFTWAREIDENTITY_PROPERTY_SYSTEM;
          }
          break;
      case PG_HOSTEDACCESSPOINT:
          if(originClassEnum == PG_COMPUTERSYSTEM)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
      case PG_HOSTEDOBJECTMANAGER:
          if(originClassEnum == PG_COMPUTERSYSTEM)
          {
              expectedTargetRole = PROPERTY_DEPENDENT;
              expectedOriginRole = PROPERTY_ANTECEDENT;
          }
          else if(originClassEnum == PG_OBJECTMANAGER)
          {
              expectedTargetRole = PROPERTY_ANTECEDENT;
              expectedOriginRole = PROPERTY_DEPENDENT;
          }
          break;
      default:
          break;
    }

    if(expectedTargetRole.isNull() ||
        expectedOriginRole.isNull())
    {
        return false;
    }

    if(targetProperty.size() == 0)
    {
        targetProperty = expectedTargetRole.getString();
    }
    else if(!expectedTargetRole.equal(targetProperty))
    {
       return false;
    }

    if(originProperty.size() == 0)
    {
        originProperty = expectedOriginRole.getString();
    }
    else if(!expectedOriginRole.equal(originProperty))
    {
       return false;
    }
    return true;
}

Array<CIMInstance> InteropProvider::localReferences(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & assocClass,
    String & originProperty,
    String & targetProperty,
    const CIMPropertyList & propertyList,
    const CIMName & targetClass)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "InteropProvider::localReferences()");

    Array<CIMInstance> instances;
    CIMName originClass = objectName.getClassName();

    Array<CIMName> targetSubclasses;
    CIMNamespaceName lastTargetNamespace;
    CIMNamespaceName originNamespace(objectName.getNameSpace());

    if(validAssocClassForObject(assocClass, objectName.getClassName(),
        originNamespace, originProperty, targetProperty))
    {
        Array<CIMInstance> localInstances = localEnumerateInstances(context,
            CIMObjectPath(hostName, originNamespace,
                assocClass));
        for(Uint32 i = 0, n = localInstances.size(); i < n; ++i)
        {
            CIMInstance & currentInstance = localInstances[i];
            CIMObjectPath originPath = getRequiredValue<CIMObjectPath>(
                currentInstance, originProperty);
            originPath.setNameSpace(objectName.getNameSpace());
            originPath.setHost(objectName.getHost());
            if(originPath.identical(objectName))
            {
                if(!targetClass.isNull())
                {
                    // Have to check if the target reference is of the
                    // targetClass type. We first must determine all the
                    // possible subclasses of the targetClass in the target
                    // namespace.
                    CIMObjectPath targetPath = getRequiredValue<CIMObjectPath>(
                        currentInstance, targetProperty);

                    CIMNamespaceName targetNamespace(
                        targetPath.getNameSpace());
                    if(targetNamespace.isNull())
                    {
                        targetNamespace = originNamespace;
                        targetPath.setNameSpace(targetNamespace);
                    }
                    if(targetNamespace != lastTargetNamespace)
                    {
                        try
                        {
                            targetSubclasses = repository->enumerateClassNames(
                                targetNamespace, targetClass, true);
                        }
                        catch(...)
                        {
                            // If an exception was thrown during enumeration,
                            // then the base class didn't exist in the
                            // namespace, so the target instance retrieved
                            // must not match the targetClass parameter.
                            continue;
                        }
                        targetSubclasses.append(targetClass);
                        lastTargetNamespace = targetNamespace;
                    }

                    // Try to find the targetPath's class in the search space
                    CIMName targetPathClass = targetPath.getClassName();
                    for(Uint32 j = 0, m = targetSubclasses.size(); j < m; ++j)
                    {
                        if(targetPathClass == targetSubclasses[j])
                        {
                            instances.append(currentInstance);
                            break;
                        }
                    }
                }
                else
                {
                    instances.append(currentInstance);
                }
            }
        }
    }

    PEG_METHOD_EXIT();
    return instances;
}


CIMInstance InteropProvider::buildRegisteredProfile(
    const String & instanceId,
    const String & profileName,
    const String & profileVersion,
    Uint16         profileOrganization,
    const String & otherProfileOrganization,
    const CIMClass & profileClass)
{
    CIMInstance instance = profileClass.buildInstance(
        false, false, CIMPropertyList());
    setPropertyValue(instance, REGISTEREDPROFILE_PROPERTY_INSTANCEID,
        instanceId);
    setPropertyValue(instance, REGISTEREDPROFILE_PROPERTY_REGISTEREDNAME,
        profileName);
    setPropertyValue(instance, REGISTEREDPROFILE_PROPERTY_REGISTEREDVERSION,
        profileVersion);
    setPropertyValue(instance,
        REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION,
        profileOrganization);
    if(profileOrganization == 1) // Other
    {
        setPropertyValue(instance,
            REGISTEREDPROFILE_PROPERTY_OTHERREGISTEREDORGANIZATION,
            otherProfileOrganization);
    }

    // Determine if this instance is a PG_RegisteredProfile and if SLP is
    // enabled in the configuration. If so, specify SLP as the advertise type.
    Array<Uint16> advertiseTypes;
    if(profileClass.getClassName() == PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE)
    {
        ConfigManager* configManager = ConfigManager::getInstance();
        if (String::equal(configManager->getCurrentValue("slp"), "true"))
        {
            advertiseTypes.append(3); // Advertised via SLP
        }
        else
        {
            advertiseTypes.append(2); // Not advertised
        }
    }
    else
    {
        advertiseTypes.append(2); // Not advertised
    }
    setPropertyValue(instance,
        REGISTEREDPROFILE_PROPERTY_ADVERTISETYPES,
        advertiseTypes);

    CIMObjectPath objPath = instance.buildPath(profileClass);
    objPath.setHost(hostName);
    objPath.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
    instance.setPath(objPath);

    return instance;
}

String translateValue(
    const String & value,
    const CIMName & propName,
    const CIMName & sourceQualifier,
    const CIMName & targetQualifier,
    const CIMClass & classDef)
{
    String mappedValue(String::EMPTY);
    Uint32 index = classDef.findProperty(propName);
    if(index != PEG_NOT_FOUND)
    {
        CIMConstProperty prop = classDef.getProperty(index);
        index = prop.findQualifier(sourceQualifier);
        if(index != PEG_NOT_FOUND)
        {
            Array<String> valueMapQual;
            prop.getQualifier(index).getValue().get(valueMapQual);
            for(Uint32 i = 0, n = valueMapQual.size(); i < n; ++i)
            {
                // If we have a match in the ValueMap qualifier, then get the
                // related string from the Values qualifier
                if(valueMapQual[i] == value)
                {
                    index = prop.findQualifier(targetQualifier);
                    if(index != PEG_NOT_FOUND)
                    {
                        Array<String> valuesQual;
                        prop.getQualifier(index).getValue().get(
                            valuesQual);
                        mappedValue = valuesQual[i];
                    }
                    break;
                }
            }
        }
    }

    return mappedValue;
}

inline String translateValue(Uint16 value, const CIMName & propName,
    const CIMName & sourceQualifier, const CIMName & targetQualifier,
    const CIMClass & classDef)
{
    return translateValue(CIMValue(value).toString(), propName,
      sourceQualifier, targetQualifier, classDef);
}

inline String buildProfileInstanceId(const String & organization,
                                     const String & name,
                                     const String & version)
{
    return organization + "+" + name + "+" + version;
}

String extractProfileInfo(const CIMInstance & profileCapabilities,
                          const CIMClass & capabilitiesClass,
                          const CIMClass & profileClass,
                          String & name,
                          String & version,
                          Uint16 & organization,
                          String & otherOrganization,
                          String & organizationName,
                          Array<String> & subprofileNames,
                          bool noSubProfileInfo = true)
{
    Uint16 registeredProfile = getRequiredValue<Uint16>(profileCapabilities,
        PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE);

    if(registeredProfile == 0) // Other
    {
        name = getRequiredValue<String>(profileCapabilities,
            PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDPROFILE);
        organizationName = getRequiredValue<String>(profileCapabilities,
            PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDORGANIZATION);
    }
    else
    {
        String mappedProfileName = translateValue(registeredProfile,
            PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE,
            VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME, capabilitiesClass);
        if(mappedProfileName.size() == 0)
        {
            throw CIMOperationFailedException(
                profileCapabilities.getPath().toString() +
                " has invalid property " +
                PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE.getString());
        }

        Uint32 index = mappedProfileName.find(Char16(':'));
        PEGASUS_ASSERT(index != PEG_NOT_FOUND);
        organizationName = mappedProfileName.subString(0, index);
        name = mappedProfileName.subString(index+1);
    }

    version = getRequiredValue<String>(profileCapabilities,
        PROFILECAPABILITIES_PROPERTY_PROFILEVERSION);

    if(!noSubProfileInfo)
    {
        Array<Uint16> registeredSubprofiles =
            getRequiredValue<Array<Uint16> >(profileCapabilities,
                PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES);
        Array<String> otherRegisteredSubprofiles;
        Uint32 otherSubprofileIndex = profileCapabilities.findProperty(
            PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDSUBPROFILES);
        Uint32 numOtherSubprofiles = 0;
        if(otherSubprofileIndex != PEG_NOT_FOUND)
        {
            profileCapabilities.getProperty(otherSubprofileIndex).getValue().
                get(otherRegisteredSubprofiles);
            numOtherSubprofiles = otherRegisteredSubprofiles.size();
        }
        otherSubprofileIndex = 0;

        for(Uint32 k = 0, x = registeredSubprofiles.size(); k < x; ++k)
        {
            Uint16 subprofileMapping = registeredSubprofiles[k];
            String subprofileName;
            if(subprofileMapping == 0) // "Other"
            {
                if(otherSubprofileIndex == numOtherSubprofiles)
                {
                    throw CIMOperationFailedException(
                        profileCapabilities.getPath().toString() +
                        " does not contain enough entries in property " +
                        PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDSUBPROFILES
                            .getString());
                }
                subprofileName =
                    otherRegisteredSubprofiles[otherSubprofileIndex++];
            }
            else
            {
                subprofileName = translateValue(
                    subprofileMapping,
                    PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES,
                    VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME,
                    capabilitiesClass);
                if(subprofileName.size() == 0)
                {
                    throw CIMOperationFailedException(
                        profileCapabilities.getPath().toString() +
                        " has invalid property " +
                        PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES.
                            getString());
                }

                Uint32 orgIndex = subprofileName.find(Char16(':'));
                if(orgIndex != PEG_NOT_FOUND)
                    subprofileName = subprofileName.subString(orgIndex+1);
            }

            subprofileNames.append(subprofileName);
        }
    }

    String organizationMapping = translateValue(organizationName,
        REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION,
        VALUES_QUALIFIERNAME, VALUEMAP_QUALIFIERNAME, profileClass);
    if(organizationMapping.size() == 0)
    {
        organization = 1;
        otherOrganization = organizationName;
    }
    else
    {
        organization = atoi((const char *)organizationMapping.getCString());
        if(organization == 1)
          otherOrganization = organizationName;
    }

    return buildProfileInstanceId(organizationName, name, version);
}

inline CIMObjectPath buildAntecedentPath(
    const String & hostName,
    const String & antecedentId,
    const CIMName & antecedentClass)
{
    Array<CIMKeyBinding> antecedentKeys;
    antecedentKeys.append(CIMKeyBinding(
        COMMON_PROPERTY_INSTANCEID,
        antecedentId,CIMKeyBinding::STRING));

    return CIMObjectPath(hostName,
        PEGASUS_NAMESPACENAME_INTEROP,
        antecedentClass,
        antecedentKeys);
}

inline CIMInstance buildDependencyInstanceFromPaths(
    const CIMObjectPath & antecedent,
    const CIMObjectPath & dependent,
    const CIMClass & dependencyClass)
{
    CIMInstance dependencyInst = dependencyClass.buildInstance(false, false,
            CIMPropertyList());
    setPropertyValue(dependencyInst, PROPERTY_ANTECEDENT,
        CIMValue(antecedent));
    setPropertyValue(dependencyInst, PROPERTY_DEPENDENT,
        CIMValue(dependent));
    dependencyInst.setPath(dependencyInst.buildPath(dependencyClass));
    return dependencyInst;
}

CIMInstance InteropProvider::buildDependencyInstance(
    const String & antecedentId,
    const CIMName & antecedentClass,
    const String & dependentId,
    const CIMName & dependentClass,
    const CIMClass & depClass)
{
    Array<CIMKeyBinding> dependentKeys;

    dependentKeys.append(CIMKeyBinding(
        COMMON_PROPERTY_INSTANCEID,
        dependentId,CIMKeyBinding::STRING));

    return buildDependencyInstanceFromPaths(
        buildAntecedentPath(hostName, antecedentId, antecedentClass),
        CIMObjectPath(hostName,
            PEGASUS_NAMESPACENAME_INTEROP,
            dependentClass,
            dependentKeys),
        depClass);
}

Array<CIMInstance> InteropProvider::getProfileInstances(
    const CIMName & profileType, const Array<String> & defaultSniaProfiles)
{
    Array<CIMInstance> instances;
    bool isRequiresProfileOperation = profileType.equal(
        PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE);
    Array<CIMInstance> profileCapabilities = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES);
    Array<String> instanceIDs;

    CIMClass registeredProfileClass;
    CIMClass subprofileReqProfileClass;
    if(isRequiresProfileOperation)
    {
        registeredProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE, false, true, false);
        subprofileReqProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE, false, true,
            false);
    }
    else
    {
        registeredProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP, profileType, false, true, false);
    }

    Uint32 i = 0;
    Uint32 n = profileCapabilities.size();
    for(; i < n; ++i)
    {
        // Extract the useful properties
        String profileName;
        Uint16 profileOrganization = 0;
        String otherOrganization;
        String profileVersion;
        String organizationName;
        bool justRegisteredProfileInfo = profileType.equal(
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE);
        CIMInstance & currentCapabilities = profileCapabilities[i];
        Array<String> profileNames;
        String profileId = extractProfileInfo(currentCapabilities,
            profileCapabilitiesClass, registeredProfileClass, profileName,
            profileVersion, profileOrganization, otherOrganization,
            organizationName, profileNames, justRegisteredProfileInfo);
        Array<String> tmpInstanceIds;

        if(justRegisteredProfileInfo)
        {
            tmpInstanceIds.append(profileId);
            profileNames.append(profileName);
        }
        else
        {
            for(Uint32 j = 0, m = profileNames.size(); j < m; ++j)
            {
                tmpInstanceIds.append(buildProfileInstanceId(organizationName,
                    profileNames[j], profileVersion));
            }
        }

        for(Uint32 j = 0, m = tmpInstanceIds.size(); j < m; ++j)
        {
            // See if we've already retrieved an equivalent RegisteredSubProfile
            bool unique = true;
            String tmpId;
            if(isRequiresProfileOperation)
              tmpId = profileId + ":" + tmpInstanceIds[j];
            else
              tmpId = tmpInstanceIds[j];
            for(Uint32 k = 0, x = instanceIDs.size(); k < x; ++k)
            {
                if(instanceIDs[k] == tmpId)
                {
                    unique = false;
                    break;
                }
            }

            if(unique)
            {
                if(isRequiresProfileOperation)
                {
                    instances.append(buildDependencyInstance(
                        profileId,
                        PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                        tmpInstanceIds[j],
                        PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                        subprofileReqProfileClass));
                }
                else
                {
                    instances.append(buildRegisteredProfile(tmpId,
                        profileNames[j], profileVersion, profileOrganization,
                        otherOrganization, registeredProfileClass));
                }
                instanceIDs.append(tmpId);
            }
        }
    }

    for(i = 0, n = defaultSniaProfiles.size(); i < n; ++i)
    {
        if(isRequiresProfileOperation)
        {
            static String serverProfileId(buildProfileInstanceId(
                SNIA_NAME, "Server", SNIA_VER_110));
            String subprofileId = buildProfileInstanceId(
                SNIA_NAME, defaultSniaProfiles[i], SNIA_VER_110);
            String compoundId = serverProfileId + ":" + subprofileId;
            bool unique = true;
            for(Uint32 k = 0, x = instanceIDs.size(); k < x; ++k)
            {
                if(instanceIDs[k] == compoundId)
                {
                    unique = false;
                    break;
                }
            }

            if(unique)
            {
                instances.append(buildDependencyInstance(
                    serverProfileId, PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                    subprofileId,
                    PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                    subprofileReqProfileClass));
            }
        }
        else
        {
                // We always have the Indication Subprofile
                const String & currentProfile = defaultSniaProfiles[i];
                String instanceId = buildProfileInstanceId(SNIA_NAME,
                    defaultSniaProfiles[i], SNIA_VER_110);
                bool defaultProfileUnique = true;
                for(Uint32 j = 0, m = instanceIDs.size(); j < m; ++j)
                {
                    if(instanceIDs[j] == instanceId)
                    {
                        defaultProfileUnique = false;
                        break;
                    }
                }

                if(defaultProfileUnique)
                {
                    instances.append(buildRegisteredProfile(instanceId,
                        currentProfile, SNIA_VER_110, 11 /*"SNIA"*/, String::EMPTY,
                        registeredProfileClass));
                    instanceIDs.append(instanceId);
                }
        }
    }

    return instances;
}

Array<CIMInstance> InteropProvider::enumRegisteredProfileInstances()
{
    static String serverProfileName("Server");
    Array<String> defaultSubprofiles;
    defaultSubprofiles.append(serverProfileName);

    return getProfileInstances(PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
        defaultSubprofiles);
}

String indicationProfileName("Indication");
String softwareProfileName("Software");

Array<CIMInstance> InteropProvider::enumRegisteredSubProfileInstances()
{
    Array<String> defaultSubprofiles;
    defaultSubprofiles.append(indicationProfileName);
    defaultSubprofiles.append(softwareProfileName);
    return getProfileInstances(PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
        defaultSubprofiles);
}

Array<CIMInstance> InteropProvider::enumSubProfileRequiresProfileInstances()
{
    Array<String> defaultSubprofiles;
    defaultSubprofiles.append(indicationProfileName);
    defaultSubprofiles.append(softwareProfileName);
    return getProfileInstances(PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE,
        defaultSubprofiles);
}

Array<CIMInstance> InteropProvider::enumReferencedProfileInstances()
{
    Array<CIMInstance> instances;

    // We have to collect all the Registered Profiles and Subprofiles in
    // separate lists since the ProviderReferencedProfile instance does not
    // have information telling us whether its entries are subprofiles or
    // profiles.
    Array<String> allRegisteredProfileIds;
    Array<String> allSubprofileIds;
    Array<CIMInstance> allRegisteredProfiles = enumRegisteredProfileInstances();
    Array<CIMInstance> allSubprofiles = enumRegisteredSubProfileInstances();
    Uint32 i = 0;
    Uint32 n = allRegisteredProfiles.size();
    for(; i < n; ++i)
    {
        allRegisteredProfileIds.append(getRequiredValue<String>(
            allRegisteredProfiles[i], REGISTEREDPROFILE_PROPERTY_INSTANCEID));
    }

    for(i = 0, n = allSubprofiles.size(); i < n; ++i)
    {
        allSubprofileIds.append(getRequiredValue<String>(
            allSubprofiles[i], REGISTEREDPROFILE_PROPERTY_INSTANCEID));
    }


    Array<CIMInstance> referencedProfiles = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES);

    CIMClass providerRefProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES,
            false, true, false);
    CIMClass referencedProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE,
            false, true, false);

    Array<String> instanceIds;
    for(i = 0, n = referencedProfiles.size(); i < n; ++i)
    {
        CIMInstance & currentReferencedProfile = referencedProfiles[i];
        Array<Uint16> registeredProfiles = getRequiredValue<Array<Uint16> >(
            currentReferencedProfile,
            REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILES);
        Array<Uint16> dependentProfiles = getRequiredValue<Array<Uint16> >(
            currentReferencedProfile,
            REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILES);
        Array<String> profileVersions = getRequiredValue<Array<String> >(
            currentReferencedProfile,
            REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILEVERSIONS);
        Array<String> dependentVersions = getRequiredValue<Array<String> >(
            currentReferencedProfile,
            REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILEVERSIONS);

        Uint32 m = registeredProfiles.size();
        if(m != dependentProfiles.size() || m != profileVersions.size() ||
            m != dependentVersions.size())
        {
            throw CIMOperationFailedException(
                currentReferencedProfile.getPath().toString() +
                " mismatch in num values between corresponding properties");
        }

        Uint32 otherProfilesIndex = 0;
        Uint32 otherDependentsIndex = 0;
        Uint32 numOtherProfiles = 0;
        Uint32 numOtherDependents = 0;
        Array<String> otherProfiles;
        Array<String> otherDependentProfiles;
        Array<String> otherProfileOrganizations;
        Array<String> otherDependentOrganizations;
        Uint32 index = currentReferencedProfile.findProperty(
            REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILES);
        if(index != PEG_NOT_FOUND)
        {
            currentReferencedProfile.getProperty(index).getValue().get(
                otherProfiles);
            numOtherProfiles = otherProfiles.size();
        }

        index = currentReferencedProfile.findProperty(
            REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILES);
        if(index != PEG_NOT_FOUND)
        {
            currentReferencedProfile.getProperty(index).getValue().get(
                otherDependentProfiles);
            numOtherDependents = otherDependentProfiles.size();
        }

        index = currentReferencedProfile.findProperty(
            REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILEORGANIZATIONS);
        if(index != PEG_NOT_FOUND)
        {
            currentReferencedProfile.getProperty(index).getValue().get(
                otherProfileOrganizations);
        }

        index = currentReferencedProfile.findProperty(
            REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILEORGANIZATIONS);
        if(index != PEG_NOT_FOUND)
        {
            currentReferencedProfile.getProperty(index).getValue().get(
                otherDependentOrganizations);
        }

        if(otherDependentOrganizations.size() != numOtherDependents ||
            otherProfileOrganizations.size() != numOtherProfiles)
        {
            throw CIMOperationFailedException(
                currentReferencedProfile.getPath().toString() +
                " mismatch in num values between corresponding properties");
        }

        for(Uint32 j = 0; j < m; ++j)
        {
            Uint16 currentProfile = registeredProfiles[j];
            Uint16 currentDependent = dependentProfiles[j];
            String profileName;
            String dependentName;
            String profileOrgName;
            String dependentOrgName;
            if(currentProfile == 0) // Other
            {
                if(otherProfilesIndex == numOtherProfiles)
                {
                    throw CIMOperationFailedException(
                        currentReferencedProfile.getPath().toString() +
                        " not enough entries in property " +
                        REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILES.
                            getString());
                }

                profileName = otherProfiles[otherProfilesIndex];
                profileOrgName =
                    otherProfileOrganizations[otherProfilesIndex++];
            }
            else
            {
              /*
              translateValue(const String & value, const CIMName & propName,
                      const CIMName & sourceQualifier,
                      const CIMName & targetQualifier,
                      const CIMClass & classDef)
              */
                profileName = translateValue(currentProfile,
                    REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILES,
                    VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME,
                    providerRefProfileClass);
                Uint32 index = profileName.find(Char16(':'));
                PEGASUS_ASSERT(index != PEG_NOT_FOUND);
                profileOrgName = profileName.subString(0, index);
                profileName = profileName.subString(index+1);
            }

            if(currentDependent == 0) // Other
            {
                if(otherDependentsIndex == numOtherDependents)
                {
                    throw CIMOperationFailedException(
                        currentReferencedProfile.getPath().toString() +
                        " not enough entries in property " +
                        REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILES.
                            getString());
                }

                dependentName = otherDependentProfiles[otherDependentsIndex];
                dependentOrgName =
                    otherDependentOrganizations[otherDependentsIndex++];
            }
            else
            {
                dependentName = translateValue(currentProfile,
                    REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILES,
                    VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME,
                    providerRefProfileClass);
                Uint32 index = dependentName.find(Char16(':'));
                PEGASUS_ASSERT(index != PEG_NOT_FOUND);
                dependentOrgName = dependentName.subString(0, index);
                dependentName = dependentName.subString(index+1);
            }

            String profileId = buildProfileInstanceId(profileOrgName,
                profileName, profileVersions[j]);
            String dependentId = buildProfileInstanceId(dependentOrgName,
                dependentName, dependentVersions[j]);
            String instanceId = profileId + ":" + dependentId;
            bool unique = true;
            for(Uint32 k = 0, x = instanceIds.size(); k < x; ++k)
            {
                if(instanceIds[k] == instanceId)
                {
                    unique = false;
                    break;
                }
            }

            if(unique)
            {
                // Now find out whether it is a Profile or Subprofile
                const CIMName * profileType = 0;
                const CIMName * dependentType = 0;
                for(Uint32 k = 0, x = allRegisteredProfileIds.size();
                    k < x; ++k)
                {
                    if(allRegisteredProfileIds[k] == profileId)
                    {
                        profileType = 
                            &PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE;
                        if(dependentType != 0)
                            break;
                    }
                    else if(allRegisteredProfileIds[k] == dependentId)
                    {
                        dependentType =
                            &PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE;
                        if(profileType != 0)
                            break;
                    }
                }

                instanceIds.append(instanceId);
                if(profileType == 0)
                    profileType = &PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE;
                if(dependentType == 0)
                    dependentType = &PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE;
                instances.append(buildDependencyInstance(profileId,
                    *profileType, dependentId, *dependentType,
                    referencedProfileClass));
            }
        }
    }
    return instances;
}

Array<String> findProviderNamespacesForElement(
    const String & moduleName, const String & providerName,
    const CIMName & elementClass, CIMRepository * repository,
    Array<CIMInstance> & providerCapabilitiesInstances)
{
    Array<CIMInstance> capabilities;
    if(providerCapabilitiesInstances.size() == 0)
    {
        Array<CIMName> propList;
        propList.append(PROVIDERCAPABILITIES_PROPERTY_PROVIDERMODULENAME);
        propList.append(PROVIDERCAPABILITIES_PROPERTY_PROVIDERNAME);
        propList.append(PROVIDERCAPABILITIES_PROPERTY_NAMESPACES);
        propList.append(PROVIDERCAPABILITIES_PROPERTY_CLASSNAME);
        capabilities = repository->enumerateInstances(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PROVIDERCAPABILITIES, true, true, false, false);
    }
    else
    {
        capabilities = providerCapabilitiesInstances;
    }

    for(Uint32 i = 0, n = capabilities.size(); i < n; ++i)
    {
        CIMInstance & currentCapabilities = capabilities[i];
        Uint32 propIndex = currentCapabilities.findProperty(
            PROVIDERCAPABILITIES_PROPERTY_PROVIDERMODULENAME);
        PEGASUS_ASSERT(propIndex != PEG_NOT_FOUND);
        String currentName;
        currentCapabilities.getProperty(propIndex).getValue().get(
            currentName);
        if(currentName == moduleName)
        {
            propIndex = currentCapabilities.findProperty(
                PROVIDERCAPABILITIES_PROPERTY_PROVIDERNAME);
            PEGASUS_ASSERT(propIndex != PEG_NOT_FOUND);
            currentCapabilities.getProperty(propIndex).getValue().get(
                currentName);
            if(currentName == providerName)
            {
                propIndex = currentCapabilities.findProperty(
                    PROVIDERCAPABILITIES_PROPERTY_CLASSNAME);
                PEGASUS_ASSERT(propIndex != PEG_NOT_FOUND);
                currentCapabilities.getProperty(propIndex).getValue().get(
                    currentName);
                if(elementClass.equal(CIMName(currentName)))
                {
                    propIndex = currentCapabilities.findProperty(
                      PROVIDERCAPABILITIES_PROPERTY_NAMESPACES);
                    PEGASUS_ASSERT(propIndex != PEG_NOT_FOUND);
                    Array<String> namespaces;
                    currentCapabilities.getProperty(propIndex).getValue().get(
                        namespaces);
                    return namespaces;
                }
            }
        }
    }

    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_FOUND,
        "Could not find provider capabilities registered for module " +
        moduleName + ", provider " + providerName + ", and class " +
        elementClass.getString());
}

CIMInstance buildElementConformsToProfile(
    const CIMObjectPath & currentProfile,
    const CIMObjectPath & currentElement,
    const CIMClass & elementConformsClass)
{
    Array<CIMName> elementPropArray;
    elementPropArray.append(
        ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD);
    elementPropArray.append(
        ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT);
    CIMPropertyList elementPropList(elementPropArray);

    CIMInstance tmpInstance =
        elementConformsClass.buildInstance(false, false,
            elementPropList);
    setPropertyValue(tmpInstance,
        ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD,
        currentProfile);
    setPropertyValue(tmpInstance,
        ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT,
        currentElement);
    tmpInstance.setPath(tmpInstance.buildPath(
        elementConformsClass));
    return tmpInstance;
}

Array<CIMInstance> InteropProvider::enumElementConformsToProfileInstances(
    const OperationContext & opContext, const CIMNamespaceName & opNamespace)
{
    CIMClass elementConformsClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE,
        false, true, false);

    AutoMutex holder(interopMut);
    Array<CIMInstance> instances;
    verifyCachedInfo();
    for(Uint32 i = 0, n = profileIds.size(); i < n; ++i)
    {
        String & profileId = profileIds[i];
        Array<CIMName> & elementList = conformingElements[i];
        Array<CIMNamespaceName> & namespaceList = elementNamespaces[i];
        Array<CIMObjectPath> conformingElementPaths;
        for(Uint32 j = 0, m = elementList.size(); j < m; ++j)
        {
            CIMName & currentElement = elementList[j];
            CIMNamespaceName & currentNamespace = namespaceList[j];

            if(opNamespace == PEGASUS_NAMESPACENAME_INTEROP ||
                opNamespace == currentNamespace)
            {
                String currentElementStr(currentElement.getString());
                if(currentElementStr.find(PEGASUS_DYNAMIC) == 0)
                {
                    // If the provider profile registration did not provide a
                    // list of conforming elements (presumably because there is
                    // no such definite list), then the provider is required
                    // to provide instances of ElementConformsToProfile in the
                    // vendor namespace, so we do not generate instances.
                    if(opNamespace != PEGASUS_NAMESPACENAME_INTEROP)
                    {
                        continue;
                    }
                    CIMName subclassName(
                        currentElementStr.subString(PEGASUS_DYNAMIC_LEN));
                    Array<CIMInstance> elementConformsInstances =
                        cimomHandle.enumerateInstances(opContext,
                        currentNamespace, subclassName, true, false, false,
                        true, CIMPropertyList());

                    // Retrieve the Conforming Element
                    for(Uint32 k = 0, x = elementConformsInstances.size();
                        k < x; ++k)
                    {
                        CIMInstance & currentInstance =
                            elementConformsInstances[k];

                        // Make sure that the current instance points to the
                        // current profile ID.
                        CIMObjectPath profilePath =
                            getRequiredValue<CIMObjectPath>(
                                elementConformsInstances[k],
                                ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD);
                        const Array<CIMKeyBinding> & keys =
                            profilePath.getKeyBindings();
                        if(keys.size() != 1)
                            continue;
                        if(keys.size() == 1 && keys[0].getValue() == profileId)
                        {
                            conformingElementPaths.append(
                                getRequiredValue<CIMObjectPath>(
                                currentInstance,
                                ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT));
                        }
                    }
                }
                else
                {
                    Array<CIMObjectPath> paths =
                        cimomHandle.enumerateInstanceNames(opContext,
                            currentNamespace, currentElement);
                    // Set the namespace in the paths just in case
                    for(Uint32 k = 0, x = paths.size();
                        k < x; ++k)
                    {
                        CIMObjectPath & curPath = paths[k];
                        curPath.setNameSpace(currentNamespace);
                        curPath.setHost(hostName);
                    }
                    conformingElementPaths.appendArray(paths);
                }
            }
        }

        Array<CIMKeyBinding> profileKeys;      
        profileKeys.append(CIMKeyBinding(
            REGISTEREDPROFILE_PROPERTY_INSTANCEID,
            profileIds[i],
            CIMKeyBinding::STRING));
        CIMObjectPath profilePath(hostName, PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
            profileKeys);

        for(Uint32 k = 0, x = conformingElementPaths.size(); k < x; ++k)
        {
            instances.append(buildElementConformsToProfile(profilePath,
                conformingElementPaths[k], elementConformsClass));
        }
    }



    // Now add the default instance: the association between the Server Profile
    // and the ObjectManager (if we're in the Interop namespace)
    if(opNamespace == PEGASUS_NAMESPACENAME_INTEROP)
    {
        // Build up the Object Path for the server profile
        Array<CIMKeyBinding> profileKeys;      
        profileKeys.append(CIMKeyBinding(
            REGISTEREDPROFILE_PROPERTY_INSTANCEID,
            buildProfileInstanceId(SNIA_NAME, "Server", SNIA_VER_110),
            CIMKeyBinding::STRING));
        CIMObjectPath serverProfile(hostName,
              PEGASUS_NAMESPACENAME_INTEROP,
              PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
              profileKeys);
        // Retrieve the Object Manager instance
        CIMInstance objManager = getObjectManagerInstance();

        instances.append(buildElementConformsToProfile(serverProfile,
            objManager.getPath(), elementConformsClass));
    }

    return instances;
}

CIMInstance InteropProvider::buildSoftwareIdentity(
        const String & module,
        const String & provider,
        const String & vendor,
        const String & version,
        Uint16 majorVersion,
        Uint16 minorVersion,
        Uint16 revisionNumber,
        Uint16 buildNumber,
        bool extendedVersionSupplied,
        const String & interfaceType)
{
    String instanceId(module + "+" + provider);
    String name(provider + " (" + interfaceType + ")");

    // Use double-ifs to prevent locking for every request
    if(softwareIdentityClass.isUninitialized())
    {
        AutoMutex autoMut(interopMut);
        if(softwareIdentityClass.isUninitialized())
        {
            softwareIdentityClass = repository->getClass(
                PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY, false, true, false);
        }
    }

    CIMInstance softwareIdentity = softwareIdentityClass.buildInstance(false,
        false, CIMPropertyList());

    setPropertyValue(softwareIdentity, SOFTWAREIDENTITY_PROPERTY_INSTANCEID,
        instanceId);
    setPropertyValue(softwareIdentity, SOFTWAREIDENTITY_PROPERTY_NAME, name);
    setPropertyValue(softwareIdentity, SOFTWAREIDENTITY_PROPERTY_VERSION,
        version);
    setPropertyValue(softwareIdentity, SOFTWAREIDENTITY_PROPERTY_MANUFACTURER,
        vendor);
    setPropertyValue(softwareIdentity,
        SOFTWAREIDENTITY_PROPERTY_CLASSIFICATIONS, providerClassifications);
    if(extendedVersionSupplied)
    {
        setPropertyValue(softwareIdentity,
            SOFTWAREIDENTITY_PROPERTY_MAJORVERSION, majorVersion);
        setPropertyValue(softwareIdentity,
            SOFTWAREIDENTITY_PROPERTY_MINORVERSION, minorVersion);
        setPropertyValue(softwareIdentity,
            SOFTWAREIDENTITY_PROPERTY_REVISIONNUMBER, revisionNumber);
        setPropertyValue(softwareIdentity,
            SOFTWAREIDENTITY_PROPERTY_BUILDNUMBER, buildNumber);
    }

    softwareIdentity.setPath(softwareIdentity.buildPath(softwareIdentityClass));

    return softwareIdentity;
}

void InteropProvider::extractSoftwareIdentityInfo(
    const CIMInstance & providerInstance,
    String & moduleName,
    String & providerName,
    String & vendor,
    String & version,
    Uint16 & majorVersion,
    Uint16 & minorVersion,
    Uint16 & revisionNumber,
    Uint16 & buildNumber,
    bool & extendedVersionSupplied,
    String & interfaceType)
{
    // Get the module and provider name from the PG_ProviderInstance supplied
    moduleName = getRequiredValue<String>(providerInstance,
        PROVIDER_PROPERTY_PROVIDERMODULENAME);
    providerName = getRequiredValue<String>(providerInstance,
        PROVIDER_PROPERTY_NAME);

    // Now retrieve the software info from the desired PG_ProviderModule
    Array<CIMName> propertyList;
    propertyList.append(PROVIDERMODULE_PROPERTY_NAME);
    propertyList.append(PROVIDERMODULE_PROPERTY_VENDOR);
    propertyList.append(PROVIDERMODULE_PROPERTY_VERSION);
    propertyList.append(PROVIDERMODULE_PROPERTY_INTERFACETYPE);
    Array<CIMInstance> providerModules = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDERMODULE,
        true, true, false, false, CIMPropertyList(propertyList));

    Uint32 moduleIndex = PEG_NOT_FOUND;
    for(Uint32 i = 0, n = providerModules.size(); i < n; ++i)
    {
        CIMInstance & currentModule = providerModules[i];
        Uint32 index = currentModule.findProperty(
            PROVIDERMODULE_PROPERTY_NAME);
        if(index != PEG_NOT_FOUND)
        {
            String currentModuleName;
            currentModule.getProperty(index).getValue().get(currentModuleName);
            if(moduleName == currentModuleName)
            {
                moduleIndex = i;
                break;
            }
        }
    }
    if(moduleIndex == PEG_NOT_FOUND)
    {
        throw CIMOperationFailedException(
            "Could not retrieve provider module for provider " +
            providerInstance.getPath().toString());
    }

    CIMInstance & providerModule = (CIMInstance &)providerModules[moduleIndex];

    version = getRequiredValue<String>(providerModule,
        PROVIDERMODULE_PROPERTY_VERSION);
    vendor = getRequiredValue<String>(providerModule,
        PROVIDERMODULE_PROPERTY_VENDOR);
    interfaceType = getRequiredValue<String>(providerModule,
        PROVIDERMODULE_PROPERTY_INTERFACETYPE);

    // Now see if optional extended version information is available
    extendedVersionSupplied = false;
    Uint32 majorIndex = providerInstance.findProperty(
        PROVIDERMODULE_PROPERTY_MAJORVERSION);
    if(majorIndex != PEG_NOT_FOUND)
    {
        CIMValue majorValue = providerInstance.getProperty(majorIndex).getValue();
        if(!majorValue.isNull())
        {
            extendedVersionSupplied = true;
            majorValue.get(majorVersion);
            minorVersion = 0;
            revisionNumber = 0;
            buildNumber = 0;

            // Get the Version if present
            Uint32 index = providerInstance.findProperty(
                PROVIDERMODULE_PROPERTY_VERSION);
            if(index != PEG_NOT_FOUND)
            {
                CIMValue propValue =
                  providerInstance.getProperty(index).getValue();
                if(!propValue.isNull())
                {
                    propValue.get(version);
                }
            }

            // Get the Minor version if present
            index = providerInstance.findProperty(
                PROVIDERMODULE_PROPERTY_MINORVERSION);
            if(index != PEG_NOT_FOUND)
            {
                CIMValue propValue =
                  providerInstance.getProperty(index).getValue();
                if(!propValue.isNull())
                {
                    propValue.get(minorVersion);
                }
            }

            // Get the revision number if present
            index = providerInstance.findProperty(
                PROVIDERMODULE_PROPERTY_REVISIONNUMBER);
            if(index != PEG_NOT_FOUND)
            {
                CIMValue propValue =
                  providerInstance.getProperty(index).getValue();
                if(!propValue.isNull())
                {
                    propValue.get(revisionNumber);
                }
            }

            // Get the build number if present
            index = providerInstance.findProperty(
                    PROVIDERMODULE_PROPERTY_BUILDNUMBER);
            if(index != PEG_NOT_FOUND)
            {
                CIMValue propValue =
                  providerInstance.getProperty(index).getValue();
                if(!propValue.isNull())
                {
                    propValue.get(buildNumber);
                }
            }
        }
    }
}

Array<CIMInstance> InteropProvider::enumSoftwareIdentityInstances()
{
    Array<CIMInstance> instances;

    Array<CIMInstance> registeredProviders = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_PROVIDER);
    for(Uint32 i = 0, n = registeredProviders.size(); i < n; ++i)
    {
        String moduleName;
        String providerName;
        String version;
        String vendor;
        String interfaceType;
        Uint16 majorVersion;
        Uint16 minorVersion;
        Uint16 revisionNumber;
        Uint16 buildNumber;
        bool extendedVersionInfo;
        extractSoftwareIdentityInfo(registeredProviders[i], moduleName,
            providerName, vendor, version, majorVersion, minorVersion,
            revisionNumber, buildNumber, extendedVersionInfo, interfaceType);

        instances.append(buildSoftwareIdentity(moduleName, providerName,
            vendor, version, majorVersion, minorVersion, revisionNumber,
            buildNumber, extendedVersionInfo, interfaceType));
    }

    // Always have the Interop provider
    instances.append(buildSoftwareIdentity(PEGASUS_MODULE_NAME,
        INTEROP_PROVIDER_NAME, PEGASUS_CIMOM_GENERIC_NAME,
        PEGASUS_PRODUCT_VERSION,
        0, 0, 0, 0, false, // no extended revision info
        PEGASUS_INTERNAL_PROVIDER_TYPE));
    return instances;
}

Array<CIMInstance> InteropProvider::enumElementSoftwareIdentityInstances()
{
    Array<CIMInstance> instances;

    Array<CIMInstance> profileCapabilities = repository->enumerateInstances(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES);

    CIMClass elementSoftwareIdentityClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_ELEMENTSOFTWAREIDENTITY, false, true, false);

    CIMObjectPath csPath = getComputerSystemInstance().getPath();

    for(Uint32 i = 0, n = profileCapabilities.size(); i < n; ++i)
    {
        CIMInstance & currentCapabilities = profileCapabilities[i];
        String version;
        String organizationName;
        Array<String> subprofiles;
        String dummyStr;
        Uint16 dummyInt = 0;
        String profileId = extractProfileInfo(currentCapabilities,
            profileCapabilitiesClass,
            repository->getClass(PEGASUS_NAMESPACENAME_INTEROP,
                PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE, false, true, false),
            dummyStr, // Throw away profile name
            version,
            dummyInt, // Throw away organization enum
            dummyStr, // Throw away otherOrganization, we get it below
            organizationName,
            subprofiles,
            false); // Get subprofile information
        String moduleName = getRequiredValue<String>(currentCapabilities,
            CAPABILITIES_PROPERTY_PROVIDERMODULENAME);
        String providerName = getRequiredValue<String>(currentCapabilities,
            CAPABILITIES_PROPERTY_PROVIDERNAME);

        String softwareInstanceId = moduleName + "+" + providerName;
        instances.append(buildDependencyInstance(
            softwareInstanceId,
            PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY,
            profileId,
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
            elementSoftwareIdentityClass));

        for(Uint32 j = 0, m = subprofiles.size(); j < m; ++j)
        {
            instances.append(buildDependencyInstance(
                softwareInstanceId,
                PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY,
                buildProfileInstanceId(organizationName, subprofiles[j],
                    version),
                PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                elementSoftwareIdentityClass));
        }
    }

    // Create default association between Server profile, Indications
    // subprofile, and the Pegasus Interoperability provider software identity
    String interopSoftwareIdentity = PEGASUS_MODULE_NAME + "+" +
        INTEROP_PROVIDER_NAME;
    String serverProfileId = buildProfileInstanceId(SNIA_NAME, "Server",
        SNIA_VER_110);
    String indicationProfileId = buildProfileInstanceId(SNIA_NAME,
        "Indication", SNIA_VER_110);
    String softwareProfileId = buildProfileInstanceId(SNIA_NAME,
        "Software", SNIA_VER_110);

    instances.append(buildDependencyInstance(interopSoftwareIdentity,
        PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY, serverProfileId,
        PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
        elementSoftwareIdentityClass));

    instances.append(buildDependencyInstance(interopSoftwareIdentity,
        PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY, indicationProfileId,
        PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
        elementSoftwareIdentityClass));

    instances.append(buildDependencyInstance(interopSoftwareIdentity,
        PEGASUS_CLASSNAME_PG_SOFTWAREIDENTITY, softwareProfileId,
        PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
        elementSoftwareIdentityClass));

    return instances;
}

Array<CIMInstance> InteropProvider::enumInstalledSoftwareIdentityInstances()
{
    // All of the software identity instances are associated to the
    // ComputerSystem on which the object manager resides. Simply loop through
    // all the instances and build the association.
    Array<CIMInstance> instances;
    CIMObjectPath csPath = getComputerSystemInstance().getPath();
    Array<CIMInstance> softwareInstances = enumSoftwareIdentityInstances();
    CIMClass installedSoftwareClass;
    CIMInstance skeletonInst =  buildInstanceSkeleton(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_INSTALLEDSOFTWAREIDENTITY, installedSoftwareClass);
    for(Uint32 i = 0, n = softwareInstances.size(); i < n; ++i)
    {
        CIMInstance installedSoftwareInstance = skeletonInst.clone();
        setPropertyValue(installedSoftwareInstance,
            INSTALLEDSOFTWAREIDENTITY_PROPERTY_INSTALLEDSOFTWARE,
            CIMValue(softwareInstances[i].getPath()));
        setPropertyValue(installedSoftwareInstance,
            INSTALLEDSOFTWAREIDENTITY_PROPERTY_SYSTEM,
            CIMValue(csPath));
        installedSoftwareInstance.setPath(installedSoftwareInstance.buildPath(
            installedSoftwareClass));
        instances.append(installedSoftwareInstance);
    }

    return instances;
}

typedef Array<String> StringArray;

void InteropProvider::verifyCachedInfo()
{
    // TBD: May need an algorithm to determine whether or not the information
    // cached by the Interop Provider is out of date in some way
}

void InteropProvider::cacheProfileRegistrationInfo()
{
    Array<CIMInstance> instances;
    Array<CIMInstance> providerCapabilitiesInstances;
    // Retrieve all of the provider profile registration info
    Array<CIMName> propList;
    propList.append(CAPABILITIES_PROPERTY_PROVIDERMODULENAME);
    propList.append(CAPABILITIES_PROPERTY_PROVIDERNAME);
    propList.append(PROFILECAPABILITIES_PROPERTY_PROFILEVERSION);
    propList.append(PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE);
    propList.append(PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDPROFILE);
    propList.append(PROFILECAPABILITIES_PROPERTY_CONFORMINGELEMENTS);
    Array<CIMInstance> providerProfileInstances =
        repository->enumerateInstances(PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES, true, true,
            false, false, CIMPropertyList(propList));
    CIMClass elementConformsClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_ELEMENTCONFORMSTOPROFILE,
        false, true, false);
    CIMClass registeredProfileClass = repository->getClass(
        PEGASUS_NAMESPACENAME_INTEROP,
        PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
        false, true, false);
    Array<CIMInstance> capabilities;

    // Loop through the provider profile info to determine what profiles are
    // supported by what providers, and to build the ElementConformsToProfile
    // associations.
    for(Uint32 i = 0, n = providerProfileInstances.size(); i < n; ++i)
    {
        CIMInstance & currentProfileInstance = providerProfileInstances[i];
        String moduleName = getRequiredValue<String>(currentProfileInstance,
            CAPABILITIES_PROPERTY_PROVIDERMODULENAME);
        String providerName = getRequiredValue<String>(currentProfileInstance,
            CAPABILITIES_PROPERTY_PROVIDERNAME);
        String profileName;
        Uint16 profileOrganization = 0;
        String otherOrganization;
        String profileVersion;
        String organizationName;
        Array<String> profileNames;
        String profileId = extractProfileInfo(currentProfileInstance,
            profileCapabilitiesClass, registeredProfileClass, profileName,
            profileVersion, profileOrganization, otherOrganization,
            organizationName, profileNames, true);
        Uint32 propIndex = currentProfileInstance.findProperty(
              PROFILECAPABILITIES_PROPERTY_CONFORMINGELEMENTS);

        Array<CIMName> elementPropArray;
        elementPropArray.append(
            ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD);
        elementPropArray.append(
            ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT);
        CIMPropertyList elementPropList(elementPropArray);

        Array<CIMName> conformingElementsForProfile;
        Array<CIMNamespaceName> elementNamespacesForProfile;

        Array<String> elementClasses;
        currentProfileInstance.getProperty(propIndex).getValue().get(
            elementClasses);
        //if(propIndex == PEG_NOT_FOUND)
        if(elementClasses.size() == 0)
        {
            // Get the namespaces in which this provider operates and trim down
            // the list of capabilities instaces to just those that are related
            // to this one.
            String moduleName = getRequiredValue<String>(
                currentProfileInstance,
                CAPABILITIES_PROPERTY_PROVIDERMODULENAME);
            String providerName = getRequiredValue<String>(
                currentProfileInstance,
                CAPABILITIES_PROPERTY_PROVIDERNAME);
            if(capabilities.size() == 0)
            {
                Array<CIMName> propList;
                propList.append(
                    PROVIDERCAPABILITIES_PROPERTY_PROVIDERMODULENAME);
                propList.append(PROVIDERCAPABILITIES_PROPERTY_PROVIDERNAME);
                propList.append(PROVIDERCAPABILITIES_PROPERTY_NAMESPACES);
                propList.append(PROVIDERCAPABILITIES_PROPERTY_CLASSNAME);
                capabilities = repository->enumerateInstances(
                    PEGASUS_NAMESPACENAME_INTEROP,
                    PEGASUS_CLASSNAME_PROVIDERCAPABILITIES, true, true, false,
                    false);
            }
            Array<CIMInstance> capabilitiesForProvider;
            Array<CIMNamespaceName> namespacesForProvider;
            Array<CIMNameArray> subclassesForNamespace;
            for(Uint32 j = 0, m = capabilities.size(); j < m; ++j)
            {
                CIMInstance & currentInstance = capabilities[j];
                String curModuleName = getRequiredValue<String>(
                    currentInstance, CAPABILITIES_PROPERTY_PROVIDERMODULENAME);
                String curProviderName = getRequiredValue<String>(
                    currentInstance, CAPABILITIES_PROPERTY_PROVIDERNAME);
                if(curModuleName == moduleName &&
                    curProviderName == providerName)
                {
                    CIMName currentClass(getRequiredValue<String>(
                        currentInstance,
                        PROVIDERCAPABILITIES_PROPERTY_CLASSNAME));
                    capabilitiesForProvider.append(currentInstance);
                    StringArray curNamespaces =
                        getRequiredValue<StringArray>(currentInstance,
                            PROVIDERCAPABILITIES_PROPERTY_NAMESPACES);
                    Sint32 z = 0;
                    Sint32 y = curNamespaces.size();

                    // If one of the namespaces is Interop, then continue
                    bool interopNamespaceFound = false;
                    for(; z < y; ++z)
                    {
                        if(CIMNamespaceName(curNamespaces[z]) ==
                            PEGASUS_NAMESPACENAME_INTEROP)
                        {
                            interopNamespaceFound = true;
                            break;
                        }
                    }
                    if(interopNamespaceFound)
                        continue;

                    // See if the current namespaces are already listed
                    for(Sint32 z = 0, y = curNamespaces.size(); z < y; ++z)
                    {
                        Sint32 foundIndex = -1;
                        CIMNamespaceName curNamespace = curNamespaces[z];
                        Uint32 k = 0;
                        Uint32 x = namespacesForProvider.size();
                        for(; k < x; ++k)
                        {
                            if(curNamespace == namespacesForProvider[k])
                            {
                                foundIndex = (Sint32)k;
                                break;
                            }
                        }
                        if(foundIndex == -1)
                        {
                            // Get all the subclasses of
                            // ElementConformsToProfile in the namespace and
                            // cache them.
                            foundIndex = namespacesForProvider.size();
                            Array<CIMName> subClasses = 
                                repository->enumerateClassNames(curNamespace,
                                PEGASUS_CLASSNAME_CIM_ELEMENTCONFORMSTOPROFILE,
                                true);
                            subClasses.append(
                                PEGASUS_CLASSNAME_CIM_ELEMENTCONFORMSTOPROFILE
                                );
                            namespacesForProvider.append(curNamespace);
                            subclassesForNamespace.append(subClasses);
                        }

                        // Now search to see if the current class is one of the
                        // subclasses in this namespace, and finally, if it is
                        // add it to the list
                        Array<CIMName> & subClasses =
                            subclassesForNamespace[foundIndex];
                        for(k = 0, x = subClasses.size(); k < x; ++k)
                        {
                            if(subClasses[k] == currentClass)
                            {
                                String dynamicElement = PEGASUS_DYNAMIC +
                                    currentClass.getString();
                                conformingElementsForProfile.append(
                                    dynamicElement);
                                elementNamespacesForProfile.append(
                                    curNamespace);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            //Array<String> elementClasses;
            //currentProfileInstance.getProperty(propIndex).getValue().get(
            //    elementClasses);
            for(Uint32 j = 0, m = elementClasses.size(); j < m; ++j)
            {
                CIMName elementClass(elementClasses[j]);
                Array<String> searchNamespaces =
                    findProviderNamespacesForElement(
                        moduleName, providerName,
                        elementClass,
                        repository,
                        providerCapabilitiesInstances);
                Uint32 k = 0;
                Uint32 x = searchNamespaces.size();
                for(; k < x; ++k)
                {
                    conformingElementsForProfile.append(elementClass);
                    elementNamespacesForProfile.append(searchNamespaces[k]);
                }
            }
        }

        Sint32 foundIndex = -1;
        for(Sint32 j = 0, m = profileIds.size(); j < m; ++j)
        {
            if(profileIds[j] == profileId)
            {
                foundIndex = j;
                break;
            }
        }

        if(foundIndex >= 0)
        {
            // Append the results to already existing entries
            conformingElements[foundIndex].appendArray(
                conformingElementsForProfile);
            elementNamespaces[foundIndex].appendArray(
                elementNamespacesForProfile);
        }
        else
        {
            profileIds.append(profileId);
            conformingElements.append(conformingElementsForProfile);
            elementNamespaces.append(elementNamespacesForProfile);
        }
    }

    // Now cache the defaults: PG_RegisteredProfile and PG_ObjectManager
    /*
    String serverProfileId(buildProfileInstanceId(
      SNIA_NAME, "Server", SNIA_VER_110));
    profileIds.append(serverProfileId);
    Array<CIMName> tmpNames;
    Array<CIMNamespaceName> tmpNamespaces;
    tmpNames.append(PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE);
    tmpNames.append(PEGASUS_CLASSNAME_PG_OBJECTMANAGER);
    tmpNamespaces.append(PEGASUS_NAMESPACENAME_INTEROP);
    tmpNamespaces.append(PEGASUS_NAMESPACENAME_INTEROP);

    conformingElements.append(tmpNames);
    elementNamespaces.append(tmpNamespaces);*/
}

// END_OF_FILE
