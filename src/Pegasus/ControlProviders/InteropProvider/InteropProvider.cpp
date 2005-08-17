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
//#define CDEBUG(X) PEGASUS_STD(cout) << "InteropProvider (" << __LINE__ << ") " << X << PEGASUS_STD(endl)
//#define LDEBUG(X) Logger::put (Logger::DEBUG_LOG, "InteropProvider", Logger::TRACE, "$0", X)

//**************************************************************************
//
//    Constants representing the class names processed
//
//**************************************************************************

const char * thisProvider = "InteropProvider";

// This Mutex serializes access to the instance change CIM requests. Keeps from mixing
// instance creates, modifications, and deletes. This keeps the provider from
// simultaneously execute creates, modifications, and deletes of instances. While
// these operations are largely protected by the locking mechanisms of the 
// repository this mutex guarantees that the provider will not simultaneously
// execute the instance change operations.
Mutex changeControlMutex;
/**
    The constants representing the class names we process
*/
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

// Property Names for CIM_Namespace Class
static const CIMName NAMESPACE_PROPERTYNAME  = CIMName ("Name");

// Root Namespace Name for test.
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
        CIM_NAMESPACE = 1,
        PG_NAMESPACE = 2,
        CIM_OBJECTMANAGER = 3,
        PG_CIMXMLCOMMUNICATIONMECHANISM = 4,
        CIM_NAMESPACEINMANAGERINST =5,
        CIM_COMMMECHANISMFORMANAGERINST=6,
        CIM_NAMESPACEINMANAGER=7
    };

 enum targetAssocClass{
     CIM_NAMESPACEINMANAGERASSOC = 1,
     CIM_COMMMECHANISMFORMANAGERASSOC=2
 };


String _showPathArray(Array<CIMObjectPath>& p)
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

//*************************************************************
//  Constructor
//**********************************************************
InteropProvider::InteropProvider(CIMRepository* repository)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,"InteropProvider::InteropProvider");
     _repository = repository;

    //***********************************************
    // This is a tempory fix untill there is a method created for the InteropProvider to 
    // do it's inalization work. This fix sets StatisticalData::CopyGSD, enabling the 
    //statistical gathering function.

     Boolean InstancesExists = true;
     Array<CIMInstance> instance;

     try
     { 
         instance = repository->enumerateInstances(PEGASUS_NAMESPACENAME_INTEROP, 
                                                   CIM_OBJECTMANAGER_CLASSNAME);
     }
     catch(Exception e)
     {
         InstancesExists = false;
     }

     if(instance.size() > 0 && InstancesExists)
     {                               
        Boolean output = false;
        Uint32 pos;
        if ((pos = instance[0].findProperty(CIMName("GatherStatisticalData"))) != PEG_NOT_FOUND)
        {
            CIMConstProperty p1 = instance[0].getProperty(pos);
            if (p1.getType() == CIMTYPE_BOOLEAN)
            {
                CIMValue v1  = p1.getValue();
                if (!v1.isNull())
                {
                    v1.get(output);
                    if (v1 == true) 
                    {
                        StatisticalData* sd = StatisticalData::current();
                        sd->setCopyGSD(true);
                    }
                }
            }                 
        }  
    }
    PEG_METHOD_EXIT();
 }

//***************************************************************
// Provider Utility Functions
//***************************************************************

String _showBool(Boolean x)
{
    return(x? "true" : "false");
}

static String _toStringPropertyList(const CIMPropertyList& pl)
{
    String tmp;
    for (Uint32 i = 0; i < pl.size() ; i++)
    {
        if (i > 0)
            tmp.append(", ");
        tmp.append(pl[i].getString());
    }
    return(tmp);
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
void _isNamespaceAllowed(const CIMObjectPath & path)
{
    // To allow use of all namespaces, uncomment the following line
    // return;
    if (path.getNameSpace().getString() != PEGASUS_NAMESPACENAME_INTEROP)
    {
            throw CIMNotSupportedException(
                          path.getClassName().getString());
    }
    else
        return;
}
/* set the hostname and namespace fields into the cimobjectpath
   of the defined instance
*/

void _setCompleteInstancePath(CIMInstance& instance,
                           const CIMObjectPath& inputPath)
{
    CIMObjectPath p = instance.getPath();
    p.setHost(inputPath.getHost());
    p.setNameSpace(inputPath.getNameSpace());

    instance.setPath(p);
}

/* complete the instance by setting the complete path into the instance
   and executing the instance filter to set the qualifiers, classorigin and
   propertylist in accordance with the input.  Note that this can only remove
   characteristics, except for the path completion so that it expects instances
   with qualifiers included, class origin included and a complete property
   list.
*/


 void _finishInstance(CIMInstance& instance, const CIMObjectPath& path, Boolean includeQualifiers,
     Boolean includeClassOrigin, const CIMPropertyList& propertyList)
 {
     _setCompleteInstancePath( instance, path);
     instance.filter(includeQualifiers,
                     includeClassOrigin,
                     propertyList );
 }
static String _showPropertyList(const CIMPropertyList& pl)
{
    if (pl.isNull())
        return("NULL");

    String tmp;

    tmp.append((pl.size() == 0) ? "Empty" : _toStringPropertyList(pl));
    return(tmp);
}

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
        else
            throw CIMInvalidParameterException(
                "Incorrect Property Type for Property " + propertyName.getString());
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
String _getHostAddress(String & hostName, Uint32  namespaceType,
    const String & port, const Uint32 defaultPortNumber)
{
  String ipAddress;

  if (hostName == String::EMPTY)
    	hostName = System::getHostName();

  if ((ipAddress = System::getHostIP(hostName)) == String::EMPTY)
  {
      // set default address if everything else failed
      ipAddress = String("127.0.0.1");
  }
  // Question: is there a case where we leave off the port number.
  // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")

  // if port is valid port number, we use it.  Else use
  // the default portnumber provided.
  // One or the other MUST not be zero.
  ipAddress.append(":");
  if (port == String::EMPTY)
  {
      // convert portNumber to ascii
      char buffer[32];
      sprintf(buffer, "%u", defaultPortNumber);
      ipAddress.append(buffer);
  }
  else
      ipAddress.append(port);

  // now fillout the serviceIDAttribute from the object manager instance name property.
  // This is a key field so must have a value.
  //String strUUID = _getPropertyValue( instance_ObjMgr, namePropertyName, "DefaultEmptyUUID");

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
 Array<String> _getFunctionalProfiles(Array<Uint16> & profiles)
 {
     Array<String> profileDescriptions;
     // Note that zero and 1 are unknown and other. Not used by us
     // 2 - 5 are not optional in Pegasus
     profiles.append(2); profileDescriptions.append("Basic Read");
     profiles.append(3); profileDescriptions.append("Basic Write");
     profiles.append(4); profileDescriptions.append("Schema Manipulation");
     profiles.append(5); profileDescriptions.append("Instance Manipulation");

     ConfigManager* configManager = ConfigManager::getInstance();
     if (String::equal(configManager->getCurrentValue("enableAssociationTraversal"), "true"))
     {
         profiles.append(6); profileDescriptions.append("Association Traversal");
     }
#ifndef PEGASUS_DISABLE_EXECQUERY
     profiles.append(7); profileDescriptions.append("Query Execution");
#endif
     profiles.append(8); profileDescriptions.append("Qualifier Declaration");
     if (String::equal(configManager->getCurrentValue("enableIndicationService"), "true"))
     {
         profiles.append(9); profileDescriptions.append("Indications");
     }
     return(profileDescriptions);
 }

/*  get the prefix that will be part of the cimom identification.
    This prefix is obtained from the Pegasus Constants.h
    @return String containing the unique name for the CIMOM ID
*/

String getTrademarkCIMOMIDPrefix()
{

    return(PEGASUS_INSTANCEID_GLOBAL_PREFIX);
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

/*  _CheckRequiredProperty
    Note validate does about the same thing.
*/
Boolean _checkRequiredProperty(CIMInstance& instance,
    const CIMName& propertyName,
    const CIMType expectedType,
    const String & message)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "_checkRequiredProperty");

    Boolean propertyError = false;

    //
    //  Required property must exist in instance
    //
    if (instance.findProperty (propertyName) == PEG_NOT_FOUND)
        propertyError = true;
    else
    {
        //
        //  Get the property
        //
        CIMProperty theProperty = instance.getProperty
            (instance.findProperty (propertyName));
        CIMValue theValue = theProperty.getValue ();

        //
        //  Required property must have a non-null value
        //
        if (theValue.isNull ())
            propertyError = true;
        else
        {
            //
            // Must have type defined 
            //

            if ((theValue.getType ()) != expectedType)
                propertyError = true;
        }
    }
    PEG_METHOD_EXIT ();
    return(propertyError);
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
CIMClass InteropProvider::_getClass(const CIMObjectPath& objectPath,
                                    const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getClass");

    CIMClass myClass = _repository->getClass(objectPath.getNameSpace(), className,
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
static targetClass _verifyValidClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_verifyValidClassInput");
    if (className.equal(CIM_OBJECTMANAGER_CLASSNAME))
        return CIM_OBJECTMANAGER;

    if (className.equal(PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME))
        return PG_CIMXMLCOMMUNICATIONMECHANISM;

    if (className.equal(CIM_NAMESPACEINMANAGER_CLASSNAME))
        return CIM_NAMESPACEINMANAGER;

    if (className.equal(CIM_NAMESPACE_CLASSNAME))
        return CIM_NAMESPACE;

    // Last entry, reverse test and return OK if PG_Namespace
    // Note: Changed to PG_Namespace for CIM 2.4
    if (!className.equal(PG_NAMESPACE_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    PEG_METHOD_EXIT();
    return PG_NAMESPACE;
}

static targetAssocClass _verifyValidAssocClassInput(const CIMName& className)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_verifyValidAssocClassInput");
    if (className.equal(CIM_NAMESPACEINMANAGER_CLASSNAME))
        return CIM_NAMESPACEINMANAGERASSOC;

    // Last entry, reverse test and return OK if CIM_CommMech....
    if (!className.equal(CIM_COMMMECHANISMFORMANAGER_CLASSNAME))
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");

    PEG_METHOD_EXIT();
    return CIM_COMMMECHANISMFORMANAGERASSOC;
}

/* validate the authorization of the user name against the namespace.
*/
String _validateUserID(const OperationContext & context)
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

/** Set the value of a property defined by property name in the instance provided.
    Sets a String into the value field unless the property name cannot be found.
    If the property cannot be found, it simply returns.
    ATTN: This function does not pass an error back if property not found.
    @param instance CIMInstance in which to set property value
    @param propertyName CIMName of property in which value will be set.
    @param value String value to set into property

*/
void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const String& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const Boolean& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const Uint16& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const Array<String>& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const Array<Uint16>& value)
{
    Uint32 pos;
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
        instance.getProperty(pos).setValue(CIMValue(value));
}

void _setPropertyValue(CIMInstance& instance, const CIMName& propertyName, const CIMObjectPath& value)
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

    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_SYSTEMCREATIONCLASSNAME,
            SystemCreationClassName);

    // Add property SystemName
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_SYSTEMNAME,
            System::getFullyQualifiedHostName());
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
CIMInstance InteropProvider::_buildInstanceSkeleton(const CIMObjectPath & objectPath,
                                                    const CIMName& className,
                                                    CIMClass& returnedClass)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceSkeleton()");
    // get class with lo = false, qualifier = true classorig = true
    CIMClass myClass = _repository->getClass(objectPath.getNameSpace(),
                                        className, false, true, true);
    returnedClass = myClass;
    CIMInstance skeleton = myClass.buildInstance(true,true,CIMPropertyList());

    PEG_METHOD_EXIT();
    return(skeleton);
}

/* build a single instance of the cimxmlcommunicationmechanism class
   using the parameter provided as the name property.
   Builds the complete instance and sets the path into it.
   @parm name String representing the name to be used for this object.
   @return CIMInstance of the class
*/
CIMInstance InteropProvider::_buildInstancePGCIMXMLCommunicationMechanism(
            const CIMObjectPath& objectPath,
            const String& namespaceType,
            const Uint16& accessProtocol,
            const String& IPAddress)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePGCIMXMLCommunicationMechanism()");
    CIMClass targetClass;
    CIMInstance instance = _buildInstanceSkeleton(objectPath,
                            PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME,
                            targetClass);

    _fixInstanceCommonKeys(instance);

    //CreationClassName
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
            PG_CIMXMLCOMMUNICATIONMECHANISM_CLASSNAME.getString());

    //Name, this CommunicationMechanism.  We need to make it unique.  To do this
    // we simply append the commtype to the classname since we have max of two right
    // now.
    _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,
            (String("PEGASUSCOMM") + namespaceType));

    // CommunicationMechanism Property - Force to 2.
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

    //TODO - get from system.
    authentications.append(3);
    //authenticationDescriptions.append("Basic");

    _setPropertyValue(instance, OM_AUTHENTICATIONMECHANISMSSUPPORTED, authentications);

    _setPropertyValue(instance, OM_AUTHENTICATIONMECHANISMDESCRIPTIONS, authenticationDescriptions);

    _setPropertyValue(instance, OM_VERSION, CIMXMLProtocolVersion);

    // Obsolete function 
    _setPropertyValue(instance, "namespaceType", namespaceType);

    _setPropertyValue(instance, "namespaceAccessProtocol", accessProtocol);

    _setPropertyValue(instance, "IPAddress", IPAddress);

    // build the instance path and set into instance
    instance.setPath(instance.buildPath(targetClass));

    PEG_METHOD_EXIT();
    return(instance);
}

Array<CIMInstance> InteropProvider::_buildInstancesPGCIMXMLCommunicationMechanism(
                                            const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesPGCIMXMLCommunicationMechanism");

    ConfigManager* configManager = ConfigManager::getInstance();
    Boolean enableHttpConnection = String::equal(
        configManager->getCurrentValue("enableHttpConnection"), "true");
    Boolean enableHttpsConnection = String::equal(
        configManager->getCurrentValue("enableHttpsConnection"), "true");

    Array<CIMInstance> instances;
    Uint32 namespaceAccessProtocol;
    String namespaceType;

    // for each type, create the instance if that type is defined.
    String hostName = System::getHostName();
    if (enableHttpConnection)
    {
        namespaceAccessProtocol = 2;
        namespaceType = "http";
        Uint32 portNumberHttp;
        String httpPort = configManager->getCurrentValue("httpPort");
        if (httpPort == String::EMPTY)
        {
            portNumberHttp = System::lookupPort(WBEM_HTTP_SERVICE_NAME, WBEM_DEFAULT_HTTP_PORT);
        }
        CIMInstance instance = 
            _buildInstancePGCIMXMLCommunicationMechanism(
                objectPath,
                namespaceType,
                namespaceAccessProtocol,
                _getHostAddress(hostName, namespaceAccessProtocol, httpPort, portNumberHttp));
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
            portNumberHttps = System::lookupPort(WBEM_HTTPS_SERVICE_NAME, WBEM_DEFAULT_HTTPS_PORT);
        }
        CIMInstance instance = 
            _buildInstancePGCIMXMLCommunicationMechanism(
                objectPath,
                namespaceType,
                namespaceAccessProtocol,
                _getHostAddress(hostName, namespaceAccessProtocol, httpsPort, portNumberHttps));

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
*/
Boolean InteropProvider::_getInstanceFromRepositoryCIMObjectManager(
                        const CIMObjectPath& objectPath,
                        CIMInstance& rtnInstance,
                        const Boolean includeQualifiers,
                        const Boolean includeClassOrigin,
                        const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstanceFromRepositoryCIMObjectManager");

    // Try to get persistent instance from repository
    Array<CIMInstance> instances;
    try
    {
        instances = _repository->enumerateInstances(
                objectPath.getNameSpace(),
                CIM_OBJECTMANAGER_CLASSNAME, true, false, includeQualifiers,
                includeClassOrigin, propertyList);

        if (instances.size() >= 1)
        {
            // set this instance into global variable.
            rtnInstance = instances[0];

            // log entry if there is more than one instance.
            // Some day we may support multiple entries to see other CIMOMs but
            // for now this should be illegal and represent an internal error.
            // but we will still continue to use the first entry.
            if (instances.size() > 1)
            {
                Logger::put(Logger::ERROR_LOG,
                    System::CIMSERVER, Logger::INFORMATION,
                    "Error. Multiple definitons of : $0",
                    CIM_OBJECTMANAGER_CLASSNAME.getString());
            }
            return(true);
        }
        else
        {
            return(false);
        }
    }
    catch(const CIMException&)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Error. Cannot access $0 in repository", 
            CIM_OBJECTMANAGER_CLASSNAME.getString());
        PEG_METHOD_EXIT();
        throw;
    }
    catch(const Exception&)
    {
        Logger::put(Logger::ERROR_LOG, System::CIMSERVER, Logger::INFORMATION,
            "Error. Cannot access $0 in repository", 
            CIM_OBJECTMANAGER_CLASSNAME.getString());
        PEG_METHOD_EXIT();
        throw;
    }
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
CIMInstance InteropProvider::_getInstanceCIMObjectManager(
                        const CIMObjectPath& objectPath,
                        const Boolean includeQualifiers,
                        const Boolean includeClassOrigin,
                        const CIMPropertyList& propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceCIMObjectManager");

    // Try to get the current object.  If true then it is already created.
    CIMInstance instance;
    if (!_getInstanceFromRepositoryCIMObjectManager(objectPath,
                instance, includeQualifiers,includeClassOrigin,propertyList))
    {
        //
        // No instance in the repository. Build new instance and save it.
        //
        CIMClass targetClass;
        instance = _buildInstanceSkeleton(objectPath, CIM_OBJECTMANAGER_CLASSNAME,
                    targetClass);

        _fixInstanceCommonKeys(instance);

        _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_CREATIONCLASSNAME,
                CIM_OBJECTMANAGER_CLASSNAME.getString());

        _objectManagerName = buildObjectManagerName();
        _setPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,_objectManagerName);
        _setPropertyValue(instance, CIMName("ElementName"), String("Pegasus"));

        //
        //Description property this object manager instance
        // default is Pegasus CIM_Server Version. Get from
        // fields defined in PegasusVersion.
        // TODO. Add as an alternative the capability to get this
        // from config parameters.
        // If PEGASUS_CIMOM_DESCRIPTION is non-zero length, use it.
        // Otherwise build form the components below.

        String description = (String(PEGASUS_CIMOM_DESCRIPTION).size() != 0) ?
                String(PEGASUS_CIMOM_DESCRIPTION)
            :
                String(PEGASUS_CIMOM_GENERIC_NAME) + " " +
                String(PEGASUS_PRODUCT_NAME) + " Version " +
                String(PEGASUS_PRODUCT_VERSION) + " " +
                String(PEGASUS_PRODUCT_STATUS);

        _setPropertyValue(instance, CIMName("Description"), description);

        //Property GatherStatisticalData. Initially this is set to false
        // and can then be modified by a modify instance on the instance.

        Boolean gatherStatDataFlag = false;

        _setPropertyValue(instance, OM_GATHERSTATISTICALDATA, Boolean(gatherStatDataFlag));

        // Set the statistics property into the Statisticaldata class so that
        // it can perform statistics gathering if necessary.
    #ifndef PEGASUS_DISABLE_PERFINST
        StatisticalData* sd = StatisticalData::current();
        sd->setCopyGSD(gatherStatDataFlag);
    #endif

        // write instance to the repository
        CIMObjectPath instancePath;

        try
        {
            instancePath = _repository->createInstance(objectPath.getNameSpace(),
                           instance );
        }
        catch(const CIMException&)
        {
            // TODO ATTN: KS generate log error if this not possible
            PEG_METHOD_EXIT();
            throw;
        }
        catch(const Exception&)
        {
            // ATTN: Generate log error.
            PEG_METHOD_EXIT();
            throw;
        }
        instance.setPath(instancePath);
    }

    PEG_METHOD_EXIT();
    return(instance);
}

/** getObjectNamagerName returns the name property for this object manager
    return String name of this object manager.  This is a convience function
    to deliver only the name field from the CIM_ObjectManager object.
    If the object Manager has been created and the name saved in this 
    provider, that name is returned.  Else it calls the function to
    get the instance of the Object Manager.
    @return String containing the persistent name property for this
    object manager
    
*/

String InteropProvider::_getObjectManagerName()
{
    if (_objectManagerName != String::EMPTY)
    {
        return _objectManagerName;
    }
    else
    {
        CIMObjectPath path;
        path.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
        CIMInstance instance = _getInstanceCIMObjectManager(path, true, true, CIMPropertyList());
        // get the property name.
        String name = _getPropertyValue(instance, CIM_NAMESPACE_PROPERTY_NAME,
                                                String::EMPTY);
        return name;
    }

}

/** Get the instances of CIM_Namespace. Gets all instances of the namespace from
    the repository namespace management functions. Builds instances that
    match all of the request attributes.
*/
Array<CIMInstance> InteropProvider::_getInstancesCIMNamespace(
                            const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");

    Array<CIMNamespaceName> namespaceNames = _enumerateNameSpaces();
    Array<CIMInstance> instanceArray;

    // Build instances of PG namespace since that is the leaf class
    for (Uint32 i = 0; i < namespaceNames.size(); i++)
    {
       instanceArray.append( _buildInstancePGNamespace(objectPath, namespaceNames[i]));
    }
    PEG_METHOD_EXIT();
    return(instanceArray);
}

/** get the instance of namespace defined by the input parameter which is the object path
    for the instance required.
    ATTN: Note that this is incorrect. We are supplying the namespace name and need to supply
    the objectpath
    @param objectPath CIMObjectPath from request
    @param nameSpace CIMNamespaceName for instance to get
    @return CIMInstance with the found instance or CIMInstance() if nothing found.
*/
CIMInstance InteropProvider::_getInstanceCIMNamespace(const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_getInstancesCIMNamespace()");

    Array<CIMInstance> instances = _getInstancesCIMNamespace(objectPath);

    CIMNamespaceName nameSpace = objectPath.getNameSpace();
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
/* build the full instances set of of the association class NamespacInManager.

    NOTE: THe input object path is not really use at this point.
*/
Array<CIMInstance> InteropProvider::_buildInstancesNamespaceInManager(
                            const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesNamespaceInManager");

    Array<CIMInstance> namespaceInstances = _getInstancesCIMNamespace(objectPath);

    CIMInstance instanceObjMgr = _getInstanceCIMObjectManager(objectPath,
        true, true, CIMPropertyList());

    CIMObjectPath refObjMgr = instanceObjMgr.getPath();

    refObjMgr.setHost(objectPath.getHost());
    refObjMgr.setNameSpace(objectPath.getNameSpace());

    Array<CIMInstance> assocInstances;
    CIMClass targetClass;

    CIMInstance instanceskel = _buildInstanceSkeleton(objectPath, 
        CIM_NAMESPACEINMANAGER_CLASSNAME,
                            targetClass);
    // Build and instance for each namespace instance.
    for (Uint32 i = 0 ; i < namespaceInstances.size() ; i++)
    {
        CIMInstance instance = instanceskel.clone();
        _setPropertyValue(instance, CIMName("Antecedent"), refObjMgr);

        CIMObjectPath temp = namespaceInstances[i].getPath(); 
        temp.setHost(objectPath.getHost());
        temp.setNameSpace(objectPath.getNameSpace());
        _setPropertyValue(instance, CIMName("Dependent"), temp); 

        instance.setPath(instance.buildPath(targetClass));
        assocInstances.append(instance);
    }
    PEG_METHOD_EXIT();
    return(assocInstances);
}
/* build the instances of the defined association.
*/
Array<CIMInstance> InteropProvider::_buildInstancesCommMechanismForManager(
    const CIMObjectPath& objectPath)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancesCommMechanismForManager");

    Array<CIMInstance> commInstances = _buildInstancesPGCIMXMLCommunicationMechanism(
         objectPath);

    CIMInstance instanceObjMgr = _getInstanceCIMObjectManager(objectPath,
        true, true, CIMPropertyList());

    CIMObjectPath refObjMgr = instanceObjMgr.getPath();

    Array<CIMInstance> assocInstances;
    CIMClass targetClass;
    CIMInstance instanceskel = _buildInstanceSkeleton(objectPath, 
        CIM_NAMESPACEINMANAGER_CLASSNAME, targetClass);
    for (Uint32 i = 0 ; i < commInstances.size() ; i++)
    {
        CIMInstance instance = instanceskel.clone();

        _setPropertyValue(instance, CIMName("Antecedent"), refObjMgr);

        _setPropertyValue(instance, CIMName("Dependent"), commInstances[i].getPath());

        instance.setPath(instance.buildPath(targetClass));
        assocInstances.append(instance);
    }

    PEG_METHOD_EXIT();
    return(assocInstances);
}

/* generate one instance of the CIM_Namespace class with the
   properties
   NOTE: CIM 2.4 - Changed to build PG namespace
   @param objectPath
   @param namespace name to put into the class
   @exceptions - exceptions carried forward from create instance
   and addProperty.
*/
CIMInstance InteropProvider::_buildInstancePGNamespace(const CIMObjectPath& objectPath,
        CIMNamespaceName& nameSpace)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePGNamespace");

    // ATTN: KS need to get the real objectManager name from elsewhere.  the only place
    // this exists is through the objectmanager object.
    String ObjectManagerName = _getObjectManagerName();

    CIMClass targetClass;
    CIMInstance instance = _buildInstanceSkeleton(objectPath, PG_NAMESPACE_CLASSNAME,
                                                  targetClass);

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
              throw CIMNotSupportedException(
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

    instance.setPath(instance.buildPath(targetClass));
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
                System::getFullyQualifiedHostName()))
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
                System::getFullyQualifiedHostName()))
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
                System::getFullyQualifiedHostName ()))
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
CIMObjectPath InteropProvider::_buildObjectPath(const CIMObjectPath& objectPath,
                                                const CIMName& className,
                                                const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildObjectPath");

    CIMObjectPath rtnObjectPath;
    rtnObjectPath = _buildInstancePath(objectPath,className,instance);

    rtnObjectPath.setHost(objectPath.getHost());

    rtnObjectPath.setNameSpace(objectPath.getNameSpace());

    PEG_METHOD_EXIT();
    return(rtnObjectPath);
}

/* Given a class and instance build the instance path for a
   the object.  This builds all components of the path
   @param namespace name to build
   @return CIMObjectPath containing namespace, class and keybinding
   components of path
   @exceptions - Passes repository exceptions.
*/

CIMObjectPath InteropProvider::_buildInstancePath(const CIMObjectPath& objectPath,
                                           const CIMName& className,
                                           const CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstancePath");

    // get the class  to use in building path
    // Exception if Class does not exist in this namespace
    CIMClass thisClass = _getClass(objectPath, className);

    CIMObjectPath ref = instance.buildPath(thisClass);

    PEG_METHOD_EXIT();
    return(ref);
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
}

/** Test for valid CIMReferences from an association instance. If there is a role 
    property, tests if there is a match for this role and the target object.
    Confirms that this role and this reference exist in the target instance.
 
    @param target - The target path for the association. Localization assumed.
    @param instance - The association class instance we are searching for references
    @param role - The role we require. I there is no role, this is String::EMPTY
    @return - returns Boolean true if target is found in a reference that is
    the same role
 */
Boolean _isInstanceValidReference(const CIMObjectPath& target,
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
Array<CIMObject> _filterReferenceInstances(Array<CIMInstance>& instances,
                      const CIMObjectPath& targetobjectName,
                      const CIMName& resultClass,
                      const String& resultRole)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "_filterReferenceInstances()");

    CIMObjectPath targetReference = CIMObjectPath(
                            String(),
                            CIMNamespaceName(),
                            targetobjectName.getClassName(),
                            targetobjectName.getKeyBindings());
    Array<CIMObject> rtnObjects;
    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
        if (resultClass.isNull() || resultClass.equal(instances[i].getClassName()))
        {
            // if this association instance has this role in targetReference, true
            if (_isInstanceValidReference(targetobjectName, instances[i], resultRole))
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
        _isNamespaceAllowed(instanceReference);

        handler.processing();

        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceReference.getClassName());

        String userName = _validateUserID(context);
        CIMObjectPath newInstanceReference;

        CIMNamespaceName newNamespaceName;
        CIMInstance localInstance;
        CIMObjectPath localInstanceReference = instanceReference;

        if (classEnum == CIM_NAMESPACE)
        {
            // create instance of PG_Namespace and continue.
            CIMInstance localPGInstance(PG_NAMESPACE_CLASSNAME);
            for (Uint32 i = 0 ; i < myInstance.getQualifierCount() ; i++)
            {
                localInstance.addQualifier(myInstance.getQualifier(i).clone());
            }

            for (Uint32 i = 0 ; i < myInstance.getPropertyCount() ; i++)
            {
                localInstance.addProperty(myInstance.getProperty(i).clone());
            }
            //TODO set path in instance???
            localInstanceReference.setNameSpace(PEGASUS_NAMESPACENAME_INTEROP);
            // set classEnum so next if interprets this as PG_Namespace
            classEnum = PG_NAMESPACE;
        }
        if (classEnum == PG_NAMESPACE)
        {
#ifdef PEGASUS_OS_OS400
            MessageLoaderParms mparms(
                "ControlProviders.InteropProvider.CREATE_INSTANCE_NOT_ALLOWED",
                "Create instance operation not allowed by Interop Provider for class $0.",
                PG_NAMESPACE_CLASSNAME.getString());
            throw CIMNotSupportedException(mparms);
#else
            // Create local instance to complete any keys if not created above.
            if (localInstance.isUninitialized())
            {
                localInstance = myInstance.clone();
            }

            _completeCIMNamespaceKeys(localInstance);
            // Validate that keys are as required. Does its own exception.
            newNamespaceName = _getKeyValue(myInstance, CIM_NAMESPACE_PROPERTY_NAME);

            newInstanceReference = _buildInstancePath(localInstanceReference,
                                        PG_NAMESPACE_CLASSNAME, localInstance);
#endif
        }

        else if ((classEnum == CIM_OBJECTMANAGER) ||
            (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM))
        {
            PEG_METHOD_EXIT();
            throw CIMNotSupportedException("InteropProvider, Create Not allowed");
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

        AutoMutex autoMut(changeControlMutex);

        Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "%s deleteInstance. instanceName= %s",
            thisProvider,
            (const char *) instanceName.toString().getCString());

        // test for legal namespace for this provider. Exception if not
        _isNamespaceAllowed(instanceName);

        handler.processing();
        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(instanceName.getClassName());

        String userName = _validateUserID(context);

        // Delete the instance since it may be in persistent storage
        if ((classEnum == CIM_OBJECTMANAGER))
        {
            try
            {
                _repository->deleteInstance(instanceName.getNameSpace(),instanceName);
            }
            catch(const CIMException&)
            {
                PEG_METHOD_EXIT();
                throw;
            }
        }
        else if (classEnum == PG_NAMESPACE)
        {
            CIMNamespaceName deleteNamespaceName;
#ifdef PEGASUS_OS_OS400
            MessageLoaderParms mparms(
                "ControlProviders.InteropProvider.DELETE_INSTANCE_NOT_ALLOWED",
                "Delete instance operation not allowed by Interop Provider for class $0.",
                PG_NAMESPACE_CLASSNAME.getString());
            throw CIMNotSupportedException(mparms);
#else
            // validate requred keys.  Exception out if not valid
            _validateCIMNamespaceKeys(instanceName);

            deleteNamespaceName = _getKeyValue(instanceName, CIM_NAMESPACE_PROPERTY_NAME);
#endif

            Array<CIMNamespaceName> namespaceNames;
            namespaceNames = _enumerateNameSpaces();

            if (deleteNamespaceName.equal (ROOTNS))
            {
               throw CIMNotSupportedException("root namespace cannot be deleted.");
            }

            _repository->deleteNameSpace(deleteNamespaceName);

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "Namespace = " + deleteNamespaceName.getString() +
                   " successfully deleted.");

            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
               "Interop Provider Delete Namespace: $0",
               deleteNamespaceName.getString());
        }
        else
        {
            throw CIMNotSupportedException("Delete Not allowed for " + instanceName.getClassName().getString());
        }

        handler.complete();

        PEG_METHOD_EXIT();
        return ;
    }

/** Local version of getInstance to be used by other functions in the
    the provider.  Returns a single instance.  Note that it always
    returns an instance.  If none was found, it is unitinialitized.
*/
CIMInstance InteropProvider::localGetInstance(
    const OperationContext & context,
    const CIMObjectPath & instanceName,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::localGetInstance");
    
    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s getInstance. instanceName= %s , includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
        thisProvider,
        (const char *)instanceName.toString().getCString(),
        (const char *)_showBool(includeQualifiers).getCString(),
        (const char*) _showBool(includeClassOrigin).getCString(),
        (const char *)_showPropertyList(propertyList).getCString());
    
    // Verify that ClassName is correct and get value
    targetClass classEnum  = _verifyValidClassInput(instanceName.getClassName());
    
    String userName = _validateUserID(context);
    
    // create reference from host, namespace, class components of
    // instance name

    CIMObjectPath ref;
    ref.setHost(instanceName.getHost());
    ref.setClassName(instanceName.getClassName());
    ref.setNameSpace(instanceName.getNameSpace());
    
    // Enumerate instances for this class. Returns all instances
    // Note that this returns paths setup and instances already
    // filtered per the input criteria.
    
    Array<CIMInstance> instances =  localEnumerateInstances(
            context,
            ref,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
    
    // deliver a single instance if found.
    CIMInstance rtnInstance;

    for (Uint32 i = 0 ; i < instances.size() ; i++)
    {
       if (instanceName == instances[i].getPath())
       {
           /* DEBUG SUPPORT
           Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
               "%s getInstance return instance number %u\npath: %s\n %s\n",thisProvider, i,
               (instances[i].getPath().toString().getCString()),
               ( ( CIMObject) instances[i]).toString().getCString());
           *****/
          rtnInstance = instances[i];
          break;
          // TODO Add test for duplicates somewhere.
       }
    }
    PEG_METHOD_EXIT();
    return(rtnInstance);
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
    _isNamespaceAllowed(instanceName);

    handler.processing();

    CIMInstance myInstance = localGetInstance(
                    context,
                    instanceName,
                    includeQualifiers,
                    includeClassOrigin,
                    propertyList);

    if (!myInstance.isUninitialized())
        handler.deliver(myInstance);
    else
        throw CIMObjectNotFoundException(instanceName.toString());
    
    handler.complete();
}

//*****************************************************************************************
//   localEnumerateInstances 
//   EnumerateInstances equivalent to external but returns instances
//   Used by other operations to build instances for processing
//   Note that this delivers instances as a group rather than incrementally.
//    This technique should only be used for small groups of instances.
//****************************************************************************************

Array<CIMInstance> InteropProvider::localEnumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
    {
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::localEnumerateInstances()");

        Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
            "%s enumerateInstances. referenc= %s , includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
            thisProvider,
            (const char *)ref.toString().getCString(),
            (const char *)_showBool(includeQualifiers).getCString(),
            (const char*) _showBool(includeClassOrigin).getCString(),
            (const char *)_showPropertyList(propertyList).getCString());

        // Verify that ClassName is correct and get value
        targetClass classEnum  = _verifyValidClassInput(ref.getClassName());

        String userName = _validateUserID(context);

        Array<CIMInstance> instances;
        if (classEnum == CIM_OBJECTMANAGER)
        {
            CIMInstance instance = _getInstanceCIMObjectManager(
                                    ref,
                                    includeQualifiers,
                                    includeClassOrigin,
                                    propertyList);
            instances.append(instance);
        }

        else if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
        {
            instances = _buildInstancesPGCIMXMLCommunicationMechanism(ref);
        }

        else if (classEnum == CIM_NAMESPACEINMANAGER)
        {
            instances = _buildInstancesNamespaceInManager(ref);
        }

        else if (classEnum == CIM_COMMMECHANISMFORMANAGERINST)
        {
            instances = _buildInstancesCommMechanismForManager(ref);
        }

        else if (classEnum == PG_NAMESPACE)
        {
            instances = _getInstancesCIMNamespace(ref);
        }
        else if (classEnum == CIM_NAMESPACE)
        {
            // returns nothing if CIM_Namespace
            // This class used only to create. Enumerates
            // automatically get the hiearchy.
        }
        else
        {
            PEG_METHOD_EXIT();
            throw CIMNotSupportedException
                (ref.getClassName().getString() + " not supported by Interop Provider enumerate");
        }

        // Filter and deliver the resulting instances
        for (Uint32 i = 0 ; i < instances.size() ; i++)
        {
            _finishInstance(instances[i],
                            ref,
                            includeQualifiers,
                            includeClassOrigin,
                            propertyList );

            /* Debug Trace of the Instances generated
            Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
                "%s enumerateInstances return instance number %u\npath: %s\n %s\n",thisProvider, i,
                (instances[i].getPath().toString().getCString()),
                ( ( CIMObject) instances[i]).toString().getCString());
            ****/

        }
        return(instances);
        PEG_METHOD_EXIT();
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
        PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "InteropProvider::enumerateInstances()");

        // test for legal namespace for this provider. Exception if not
        _isNamespaceAllowed(ref);

        handler.processing();

        // Call the internal enumerateInstances to generate instances of defined
        // class.  This expects the instances to be returned complete including
        // complete path.
        Array<CIMInstance> instances =  localEnumerateInstances(
                context,
                ref,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        handler.deliver(instances);
        handler.complete();
        
        PEG_METHOD_EXIT();
    }   

/** Determies if an instance modification is to be allowed. This includes
    tests to determine the validity of the modified instance, the property
    list provided, if qualifier modification was requested.
    @param context
    @param instanceReference CIMObjectPath defining the path of the instance
    to be modified.
    @param modifiedIns CIMInstance containing the modifications
    @param includeQualifiers Boolean defining if qualifiers are to be modified.
    @param propertyList CIMPropertyList provided with the request
    @param allowedModifyProperties CIMPropertyList defining properties that
    are allowed to be modified by the provider
    @return true if there are modifications that can be made.  Note that there
    is an additional decision to be made by the user whether the modifiedIns
    includes all of the properties or they are to be set to default.
    @exception CIM_ERR_NOT_SUPPORTED if the specified modification is not supported
    @exception CIM_ERR_INVALID_PARAMETER  if the modifiedInstance is invalid
 */
Boolean InteropProvider::isModifyAllowed(const OperationContext & context,
    const CIMObjectPath & instanceReference,
    const CIMInstance& modifiedIns,
    const Boolean includeQualifiers,
    const CIMPropertyList& propertyList,
    const CIMPropertyList& allowedModifyProperties)
{
    // if property list size does not match allowed list.
    if ((propertyList.size() > allowedModifyProperties.size())
           || (propertyList.isNull())
       )
    {
        throw CIMNotSupportedException(
            "Only Modification of " + _toStringPropertyList(allowedModifyProperties)
             + " allowed");
    }
    // property list indicates nothing to modify return false
    if (propertyList.size() == 0)
    {
        return(false);
    }

    // DO NOT allow any qualifier modifications.
    if (includeQualifiers)
    {
        throw CIMNotSupportedException(
            "Qualifier Modification not allowed");
    }

    // Assure that nothing is in the propertylist and NOT in
    // the allowed list.
    Array<CIMName> allowedArray = allowedModifyProperties.getPropertyNameArray();
    for (Uint32 i = 0 ; i < propertyList.size() ; i++)
    {
        if (!Contains(allowedArray, propertyList[i]))
        {
            throw CIMNotSupportedException(
                "Only Modification of " + _toStringPropertyList(allowedModifyProperties)
                 + " allowed");
        }
    }
    // We have something to modify. Property List is valid
    return(true);
}
/** Modify the existing object Manager Object.  Only a single property modification
    is allowed, the statistical data setting.  Any other change is rejected
    with an exception
    @param instanceReference - Reference for the instance to be modified.
    @param modifiedIns CIMInstance defining the change. If this includes more than
    a single property, the propertyList must specify modification only of the
    statisticaldata property.
    @includeQualifiers Boolean which must be false unless there are no qualifiers
    in the modifiedIns.
    @propertyList CIMPropertyList defining the property to be modified if there
    is more than one property in the modifiedIns.
    @Exceptions CIMInvalidParameterException if the parameters are not valid for
    the modification.
 */
void InteropProvider::modifyObjectManagerInstance(const OperationContext & context,
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
    Array<CIMName> plA;
    plA.append(CIMName(OM_GATHERSTATISTICALDATA));
    CIMPropertyList allowedModifyPropertyList(plA);

    // returns only if no exception and there is property to modify.
    if (isModifyAllowed(context, instanceReference, modifiedIns,
        includeQualifiers, propertyList, allowedModifyPropertyList))
    {
        CIMInstance instance;
        instance = _getInstanceCIMObjectManager(instanceReference,
                true, true, CIMPropertyList());

        CIMObjectPath tmpPath;
        tmpPath.setClassName(instanceReference.getClassName());
        tmpPath.setKeyBindings(instanceReference.getKeyBindings());
        if (!(tmpPath == instance.getPath()) )
            throw CIMObjectNotFoundException(instanceReference.toString());
    }
    else    // nothing to modify. return
    {
        return;
    }
    Boolean statisticsFlag;
    CIMInstance myInstance;

    // We modify only if this property exists.
    // could either use the property from modifiedIns or simply replace
    // value in property from object manager.
    if (modifiedIns.findProperty(OM_GATHERSTATISTICALDATA) != PEG_NOT_FOUND)
    {
        myInstance = _getInstanceCIMObjectManager(instanceReference,
                                    false, false, propertyList);
        statisticsFlag = _getPropertyValue(modifiedIns, OM_GATHERSTATISTICALDATA, false);
        // set the changed property into the instance
        _setPropertyValue(myInstance, OM_GATHERSTATISTICALDATA, statisticsFlag);
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
        _repository->modifyInstance(instanceReference.getNameSpace(),
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
        (OM_GATHERSTATISTICALDATA.getString() + 
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
        (const char *) _showBool(includeQualifiers).getCString(),
        (const char *) _showPropertyList(propertyList).getCString());

    // test for legal namespace for this provider. Exception if not
    _isNamespaceAllowed(instanceReference);

    // ATTN: KS 31 August 2004. This must test for privileged user.

    CIMName className =  instanceReference.getClassName();
    targetClass classEnum  = _verifyValidClassInput(className);

    String userName = _validateUserID(context);
    // begin processing the request
    handler.processing();

    if (classEnum == CIM_OBJECTMANAGER)
    {
        modifyObjectManagerInstance(context, instanceReference,modifiedIns,
            includeQualifiers, propertyList);
        // for the moment allow modification of the statistics property only
    }

    else if (classEnum == PG_CIMXMLCOMMUNICATIONMECHANISM)
    {
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException
            (" Modification of " + className.getString() + " not supported by Interop Provider");
    }
    else if (classEnum == PG_NAMESPACE)
    {
#ifdef PEGASUS_OS_OS400
            MessageLoaderParms mparms(
                "ControlProviders.InteropProvider.MODIFY_INSTANCE_NOT_ALLOWED",
                "Modify instance operation not allowed by Interop Provider for class $0.",
                PG_NAMESPACE_CLASSNAME.getString());
            throw CIMNotSupportedException(mparms);
#else
        // for the moment allow modification of the statistics property only
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException
            (className.getString() + " not supported by Interop Provider");
#endif
    }
    else
    {
        PEG_METHOD_EXIT();
        throw CIMNotSupportedException(
            className.getString() + " not supported by Interop Provider");
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
        _isNamespaceAllowed(classReference);

        targetClass classEnum  = 
                _verifyValidClassInput(classReference.getClassName());

        String userName = _validateUserID(context);

        // begin processing the request
        handler.processing();

        Boolean includeQualifiers = true;
        Boolean includeClassOrigin = true;
        
        Array<CIMInstance> instances =  localEnumerateInstances(
                context,
                classReference,
                includeQualifiers,
                includeClassOrigin,
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
// Association Functions
//**************************************************************
//**************************************************************

/** _filterInstancesToTargetPaths - Filters one associaton and returns references that represent the result of
    filtering on resultclass and role.  Any reference that matches the resultclass and role
    and not the target is returned
    @param assocInstance - The association instance being processed.
    @param targetObjectPath - The original target. This is required since this is the one reference
    we don't want.
    @resultClass - The resultClass we want to filter on
    @resultRole  - The result role we want to filter on
    @return - returns the CIMObjectPaths that represent the other side of the association that pass 
    the resultClass and resultRole filters.
 */

// TODO convert the CIMOBjectPath back to const.
Array<CIMObjectPath> _filterAssocInstanceToTargetPaths(const CIMObject& assocInstance,
                    const CIMObjectPath& targetObjectPath,
                    const CIMName resultClass,
                    const String resultRole)
{
    Array<CIMObjectPath> returnPaths;
    // get all reference properties except for target.
    for (Uint32 i = 0 ; i < assocInstance.getPropertyCount() ; i++)
    {
        CIMConstProperty p = assocInstance.getProperty(i);

        if (p.getType() == CIMTYPE_REFERENCE)
        {
            CIMValue v = p.getValue();
            CIMObjectPath path;
            v.get(path);
            
            if (!targetObjectPath.identical(path))
            {
                if (resultClass.isNull() || resultClass == path.getClassName())
                {
                    if ((resultRole == String::EMPTY) || (p.getName() == CIMName(resultRole)))
                        returnPaths.append(path);
                }
            }
        }
    }
    CDEBUG("_filterAssoc PathsReturned. Count = " << returnPaths.size() 
	   << "\n" << _showPathArray(returnPaths));
    return( returnPaths );
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
        (const char *)_showBool(includeQualifiers).getCString(),
        (const char*) _showBool(includeClassOrigin).getCString(),
        (const char *)_showPropertyList(propertyList).getCString());

    // test for legal namespace for this provider. Exception if not
    _isNamespaceAllowed(objectName);

    handler.processing();
    // Get references for this object. Note that this
    // uses the associationClass as resultClass.
    Array<CIMObject> referenceObjects = localReferences(
            context,
            objectName,
            associationClass,
            role,
            includeQualifiers,
            includeClassOrigin,
            propertyList);
 
    // Note that Result Class is not always there.  We need to account for
    // both case where it exists and where it does not exist.
    // enumerate all reference instances of the resultClass (i.e. the association)
    // create path for resultClass

    // for each reference object, get all non-target references
    Array<CIMObjectPath> resultPaths;
    for (Uint32 i = 0 ; i < referenceObjects.size() ; i++)
    {
        // get Other references from each reference object.
        Array<CIMObjectPath> tempPath =
            _filterAssocInstanceToTargetPaths(
                                referenceObjects[i],
                                objectName,
                                resultClass,
                                resultRole);
        for (Uint32 i = 0 ; i < tempPath.size() ; i++)
        {
            resultPaths.append(tempPath[i]);
        }
    }

    // get the instance using getInstance.
    for (Uint32 i = 0 ; i < resultPaths.size() ; i++)
    {
        CIMInstance thisInstance = localGetInstance(context, resultPaths[i],
                                        includeQualifiers,
                                        includeClassOrigin, propertyList);
        if (!thisInstance.isUninitialized())
        {
            handler.deliver(thisInstance);
        }
    }
    handler.processing();
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
	//throw CIMNotSupportedException("AssociationProvider::associatorNames");

    // test for legal namespace for this provider. Exception if not
    _isNamespaceAllowed(objectName);

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s associatorNames. objectName= %s , assocClass= %s resultClass= %s role= %s resultRole",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)associationClass.getString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)resultRole.getCString());

    handler.processing();
    // Get references for this object. Note that this
    // uses the associationClass as resultClass.
    Array<CIMObject> referenceObjects = localReferences(
            context,
            objectName,
            associationClass,
            role,
            false,
            false,
            CIMPropertyList());
 
    // Note that Result Class is not always there.  We need to account for
    // both case where it exists and where it does not exist.
    // enumerate all reference instances of the resultClass (i.e. the association)
    // create path for resultClass

    // for each reference object, get all non-target references
    Array<CIMObjectPath> resultPaths;
    for (Uint32 i = 0 ; i < referenceObjects.size() ; i++)
    {
        // get Other references from each association object.
        Array<CIMObjectPath> tempPath = 
            _filterAssocInstanceToTargetPaths(referenceObjects[i],
            objectName, resultClass, resultRole);

        for (Uint32 i = 0 ; i < tempPath.size() ; i++)
        {
            resultPaths.append(tempPath[i]);
        }
    }
    handler.deliver(resultPaths);

    handler.processing();
    handler.complete();
    PEG_METHOD_EXIT();
}


Array<CIMObject> InteropProvider::localReferences(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const CIMName & resultClass,
	const String & role,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList)
{

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::localReferences()");
    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s references. objectName= %s , resultClass= %s role= %s includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)_showBool(includeQualifiers).getCString(),
        (const char*) _showBool(includeClassOrigin).getCString(),
        (const char *)_showPropertyList(propertyList).getCString());

    Array<CIMInstance> rtnInstances;

    // operation namespace needed internally to get class.
    String userName = _validateUserID(context);

    // determine if valid class result class
    CIMName targetAssocClassName = resultClass;

    targetAssocClass classEnum  = _verifyValidAssocClassInput(targetAssocClassName);

    // resultClass should NEVER be NULL in Pegasus.
    assert(!resultClass.isNull());

    // enumerate all reference instances of the resultClass (i.e. the association)
    CIMObjectPath classReference(objectName.getHost(), objectName.getNameSpace(),
                                 resultClass);

    Array<CIMInstance> assocInstances = 
        localEnumerateInstances(context,
                                classReference,
                                true, true, CIMPropertyList());

    Array<CIMObject> rtnObjects = _filterReferenceInstances(
                    assocInstances, objectName, resultClass, role);

    PEG_METHOD_EXIT();
    return(rtnObjects);
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

    // test for legal namespace for this provider. Exception if not
    _isNamespaceAllowed(objectName);

    Tracer::trace(TRC_CONTROLPROVIDER, Tracer::LEVEL4,
        "%s references. objectName= %s , resultClass= %s role= %s includeQualifiers= %s, includeClassOrigin= %s, PropertyList= %s",
        thisProvider,
        (const char *)objectName.toString().getCString(),
        (const char *)resultClass.getString().getCString(),
        (const char *)role.getCString(),
        (const char *)_showBool(includeQualifiers).getCString(),
        (const char*) _showBool(includeClassOrigin).getCString(),
        (const char *)_showPropertyList(propertyList).getCString());

    handler.processing();
    Array<CIMObject> rtnObjects =
                    localReferences(context,
                                    objectName,
                                    resultClass,
                                    role,
                                    includeQualifiers,
                                    includeClassOrigin,
                                    propertyList);

    handler.deliver(rtnObjects);
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

    // test for legal namespace for this provider. Exception if not
    _isNamespaceAllowed(objectName);

    CDEBUG("::referenceNames(): object= " << objectName.toString() << " result Class= " << resultClass.getString());

    // operation namespace needed internally to get class.
    String userName = _validateUserID(context);

    // begin processing the request
    handler.processing();

    Array<CIMObject> rtnObjects =
        localReferences(context,
                        objectName,
                        resultClass,
                        role,
                        true,
                        true,
                        CIMPropertyList());

    for (Uint32 i = 0 ; i < rtnObjects.size() ; i++ )
    {
        CIMObjectPath ref = rtnObjects[i].getPath();
        CDEBUG("referenceNames returns: " << ref.toString());
        handler.deliver(ref);
    }

    handler.complete();

    PEG_METHOD_EXIT();
}

PEGASUS_NAMESPACE_END
// END_OF_FILE
