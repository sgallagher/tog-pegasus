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
    3. Consider seriously the concept of instance creation where the instance
    is provided and that instance is registered as a profile.  Not sure today what
    the level of validation, etc. would be required.
    4. We would like separate this function completely from the CIMOM so that the registration
    it could operate as first a separate process and second as a separte executable.
    5. Look at getting certain parameters as the default from the class.  This way we could
    use the class definition for defaults.
*/

#include "SLPProvider.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <Pegasus/Common/Constants.h>

#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Logger.h> // for Logger

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

const char * SlpProvider = "SlpProvider";
const char * SlpTemplateClassName = "PG_WBEMSLPTemplate";
const char * CIMObjectManagerClassName = "CIM_ObjectManager";
const char * CIMObjectManagerCommMechName = "CIM_ObjectManagerCommunicationMechanism";
const char * CIMNamespaceClassName = "CIM_Namespace";
const char * CIMCommMechanismForObjectManagerAdapterName =
              "CIM_CommMechanismForObjectManagerAdapterName";

const char * CIM_NamespaceInManager = "CIM_NamespaceInManager";

// This SLP service is named wbem.
// Internally the serviceName is wbem (in the template) and the serviceID is
// service:wbem for registration.
const char *serviceIDPrefix = "service";
const char *serviceName = "wbem";

const char *elementNamePropertyName = "ElementName";
const char *descriptionPropertyName = "Description";
const char *instanceIDPropertyName = "InstanceID";

//////////////////////////////////////////////////////////////////////////////////////// 
// Predefined Values that are part of the SLP template. These values are defined as part
// of the DMTF SLP template.
////////////////////////////////////////////////////////////////////////////////////////

// This is the current template version.  Should change only if there is a new template
// specification released.
const char * templateVersion = "1.0";

// Fixed test to be inserted in the templateDescription attribute.
const char * templateDescription = 
    "This template describes the attributes used for advertising Pegasus CIM Servers.";

// This name defines the Interop namespace.  It should be in a much more public place
// Than the middle of this provider.
String InteropSchemaNamespaceName = "root/PG_Interop";

// Default list of registered Profiles until we move to 2.8 and get list from
// the proper classes.
const char * registeredProfilesList = 
"SNIA:Array,SNIA:Array:Cluster,SNIA:Array:Access Points,SNIA:Array:Disk \
Drive,SNIA:Array:Location,SNIA:Array:LUN Mapping and Masking,SNIA:Array:Pool \
Manipulation Capabilities and Settings,SNIA:Array:Extent Mapping,SNIA:Array:LUN \
Creation,SNIA:Array:Software,SNIA:Server";

////////////////////////////////////////////////////////////////////////////////////
//  Names for attributes of SLP template and corresponding PG_WbemSLPtemplate class.
////////////////////////////////////////////////////////////////////////////////////
//
// Template attribute name constants This set of constants comes from the
// DMTF wbem template definition. The ...attribute variables are the character constants that
// define the template attributes.  Note that we build an instance of the wbemslpTemplate
// class for each created template and that class contains names similar to the attribute
// names.  The attributename is used for that also unless the property name in the class is
// different.  At this point, there are no differences except that all of the attribute
// names with dash in the name need a separate property name because dash is not a 
// legal CIMName.  We used "_" instead of "-" for the property names.

const char * templateTypeProperty = "template_type";
const char * templateTypeAttribute = "template-type";

const char * templateVersionProperty = "template_version";
const char * templateVersionAttribute = "template-version";

const char * templateDescriptionProperty = "template_description";
const char * templateDescriptionAttribute = "template-description";

const char * serviceUrlSyntaxProperty = "template_url_syntax";
const char * serviceUrlSyntaxAttribute = "template-url-syntax";

const char * serviceHiDescriptionProperty = "service_hi_description";
const char * serviceHiDescriptionAttribute = "service-hi-description";

const char * serviceHiNameProperty = "service_hi_name";
const char * serviceHiNameAttribute = "service-hi-name";

const char * serviceIDProperty = "service_id";
const char * serviceIDAttribute = "service-id";

const char * communicationMechanismAttribute = "CommunicationMechanism";

const char * otherCommunicationMechanismAttribute = "otherCommunicationMechanism";

const char * otherCommunicationMechanismDescriptionAttribute = "OtherCommunicationMechanismDescription";

const char * InteropSchemaNamespaceAttribute = "InteropSchemaNamespace";

const char * protocolVersionAttribute = "ProtocolVersion";

const char * functionalProfilesSupportedAttribute = "FunctionalProfilesSupported";

const char * functionalProfileDescriptionsAttribute = "FunctionalProfileDescriptions";

const char * otherProfileDescriptionAttribute = "OtherProfileDescription";

const char * multipleOperationsSupportedAttribute = "MultipleOperationsSupported";

const char * authenticationMechanismsSupportedAttribute =  "AuthenticationMechanismsSupported";

const char * otherAuthenticationDescriptionsAttribute = "OtherAuthenticationDescription";

const char * authenticationMechanismDescriptionsAttribute = "AuthenticationMechanismDescriptions";

const char * namespaceAttribute = "Namespace";

const char * classinfoAttribute =  "Classinfo";

const char * registeredProfilesSupportedAttribute = "RegisteredProfilesSupported";


CIMNamespaceName _interopNamespace = PEGASUS_NAMESPACENAME_INTEROP;

CIMName namePropertyName = "name";

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

/** convert an array of strings to a CSV string.
    @param s Array<String> with input strings
    @return String with CSV String from array
*/
String _arrayToString(const Array<String>& s)
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

/** append the input string and if output not empty a comma separator
    param s String to which we will append s1.
    param s1 String to be appended
*/
void _appendCSV(String& s, const String& s1)
{
    if (s.size() != 0)
        s.append(",");
    s.append(s1);
}

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
    CDEBUG("_getPropertyValue for name= " << propertyName.getString() << " default= " << defaultValue);
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

//*******************************************************************
//
// SLPProvider Protected and private methods
//
//*******************************************************************

/** builds one instance of the class named className. Gets Class definition and
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
CIMInstance SLPProvider::_buildInstanceSkeleton(const CIMName& className)
{
    CIMClass myClass;
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
            "InteropProvider::_buildInstanceSkeleton()");

    CIMInstance skeleton(className);
        myClass = _cimomHandle.getClass(OperationContext(), _nameSpace,
                                className, false, true, true,CIMPropertyList());
    
    // copy the qualifiers
    for (Uint32 i = 0 ; i < myClass.getQualifierCount() ; i++)
        skeleton.addQualifier(myClass.getQualifier(i));

    // copy the properties
    for (Uint32 i = 0 ; i < myClass.getPropertyCount() ; i++)
        skeleton.addProperty(myClass.getProperty(i));

    PEG_METHOD_EXIT();
    return(skeleton.clone());
}
/* remove the instances created as part of the registration and unregister.
  This is completely unregisters this this provider and disconnects it
  from the slp agent.
  ATTN: We could also change status so it could be unloaded.
*/
void SLPProvider::deregisterSLP()
{
    // clear existing instances
    _instanceNames.clear();
    _instances.clear();
    
    // unregister from the agent
    slp_agent.unregister();
    initFlag = false;
    
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

    char * regList = getenv("PEGASUS_REGISTERED_PROFILES");

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
        CIMNamespaceInstances = _cimomHandle.enumerateInstances(
                                         OperationContext(),
                                         PEGASUS_NAMESPACENAME_INTEROP,
                                         CIMName(CIMNamespaceClassName),
                                         true, true, true, true,
                                         CIMPropertyList());
    }
    catch (const exception&)
    {
        //ATTN: KS... catch if we get error here. In particular unsupported class
        CDEBUG("Error on Namespaces acquisition");
        return(names);
    }
    CDEBUG("Namespaces found. Count= " << CIMNamespaceInstances.size());

    // Extract the namespace names and class info from the objects.
    for (Uint32 i = 0 ; i < CIMNamespaceInstances.size() ; i++)
    {
        String temp = _getPropertyValue(CIMNamespaceInstances[i], CIMName(namePropertyName));
        if (temp != String::EMPTY)
        {
            _appendCSV(names, temp );
            _appendCSV(classInfo, _getPropertyValue(CIMNamespaceInstances[i], CIMName(classinfoAttribute), ""));
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
    This uses the SLPProvider objects for the reference to the current instance and the
    SLP template string being built.
    @param slpTemplateInstance string for registration information. Each call adds the defined
    attribute to this string.
    @param instance CIMInstance being created. This is an instance of WBEMSlpTemplate. Each call
    adds the defined property value to the instance in the property defined by instanceFieldName
    @param regfieldName String defining the name of the field to populate in the registration
    @param value String defining the value with which to populate the field
    @param insstanceFieldName (Optional) String defining the name of the field to populate in the instance.
*/
void _addSeparator(String& s)
{
    // if not first entry, set newline
    if (s != String::EMPTY)
    // bug 1737  EOL confuses some UA 
        // s(",\n");
        s.append(",");
}

void SLPProvider::populateTemplateField(CIMInstance& instance, 
                                        const String& attributeFieldName,
                                        const String& value,
                                        const String& instancePropertyName)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::populateTemplateField()");

    String localInstancePropertyName = ((instancePropertyName == String::EMPTY)?
                        attributeFieldName : instancePropertyName);
    CDEBUG("input Property name= " << instancePropertyName);
    CDEBUG("Populate TemplateField name= " << localInstancePropertyName << ", " 
        << attributeFieldName << ". Value= " << value);

    // Add the property to the instance.
    instance.addProperty(CIMProperty(CIMName(localInstancePropertyName), value));

    // if not first entry, set newline

    _addSeparator(_currentSLPTemplateString);
    //if (_currentSLPTemplateString != String::EMPTY)
    //// bug 1737  EOL confuses some UA _currentSLPTemplateStringappend(",\n");
    //    _currentSLPTemplateString.append(",\n");

    // Add entry as (name=value)
    _currentSLPTemplateString.append("(" + attributeFieldName + "=" + value + ")");
 }

/** populate a template filed using a string array as input rather than
    a single String.  The parameters are the same as the other overload
    except that this defines an array of strings rather than a single string.
*/
void SLPProvider::populateTemplateField(CIMInstance& instance, 
                                        const String& attributeFieldName,
                                        const Array<String>& value,
                                        const String& instancePropertyName)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::populateTemplateField()");

    String localInstancePropertyName = ((instancePropertyName == String::EMPTY)?
                        attributeFieldName : instancePropertyName);

    String accumulatedValue = _arrayToString(value);

    CDEBUG("Populate TemplateField name= " << localInstancePropertyName << ", " 
        << attributeFieldName << ". Value= " << accumulatedValue);

    // Add the property to the instance.
    instance.addProperty(CIMProperty(CIMName(instancePropertyName), accumulatedValue));

    // if not first entry, set newline
    _addSeparator(_currentSLPTemplateString);
    //if (_currentSLPTemplateString != String::EMPTY)
    //// bug 1737  EOL confuses some UA _currentSLPTemplateStringappend(",\n");
    //    _currentSLPTemplateString.append(",\n");

    // Add entry as (name=value)
    _currentSLPTemplateString.append("(" + attributeFieldName + "=" + accumulatedValue + ")");
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
Boolean SLPProvider::populateRegistrationData(const String &protocol,
                        const String& IPAddress,
                        const CIMInstance& instance_ObjMgr,
                        const CIMInstance& instance_ObjMgrComm)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::populateRegistrationData()");
    CDEBUG("PopulageRegistrationdata for " << protocol << " address " << IPAddress);
    // Clear the template instance
    _currentSLPTemplateString.clear();

    Uint32 index=10;
    //_currentSLPTemplateCIMInstance
    CIMInstance instance1(SlpTemplateClassName);

    // Code to get the property service_location_tcp ( which is equivalent to "IP address:5988")
    // Need to tie these two together.
    Uint32 portNumber;

    // Match the protocol and port number from internal information.
    if (protocol.find("https") != PEG_NOT_FOUND)
         portNumber = System::lookupPort(WBEM_HTTPS_SERVICE_NAME,
            WBEM_DEFAULT_HTTPS_PORT); 
    else
        portNumber = System::lookupPort(WBEM_HTTP_SERVICE_NAME,
            WBEM_DEFAULT_HTTP_PORT);
    
    // convert portNumber to ascii
    char buffer[32];
    
    // now fillout the serviceIDAttribute from the object manager instance name property.
    // This is a key field so must have a value.
    String strUUID = _getPropertyValue( instance_ObjMgr, namePropertyName, "DefaultEmptyUUID");

    // template-url-syntax=string
    // #The template-url-syntax MUST be the WBEM URI Mapping of
    // #the location of one service access point offered by the WBEM Server 
    // #over TCP transport. This attribute must provide sufficient addressing 
    // #information so that the WBEM Server can be addressed directly using 
    // #the URL.  The WBEM URI Mapping is defined in the WBEM URI Mapping 
    // #Specification 1.0.0 (DSP0207).
    // # Example: (template-url-syntax=https://localhost:5989)

    sprintf(buffer, "%u", portNumber);
    String serviceUrlSyntaxValue = protocol + "://" + IPAddress + ":" + buffer;
    populateTemplateField(instance1, serviceUrlSyntaxAttribute, serviceUrlSyntaxValue,
        serviceUrlSyntaxProperty);

    //populateTemplateField(instance1, serviceLocationTCP,IPAddress);

    //service-id=string L
    //# The ID of this WBEM Server. The value MUST be the 
    //# CIM_ObjectManager.Name property value.
    populateTemplateField(instance1, serviceIDAttribute, strUUID,
        serviceIDProperty);

    // get the properties from the cimobject class.
    for(Uint32 j=0; j < instance_ObjMgr.getPropertyCount(); j++)
    {
        CIMConstProperty p1=instance_ObjMgr.getProperty(j);
        CIMValue v1=p1.getValue();
        CIMName n1=p1.getName();

        // service-hi-name=string O
        // # This string is used as a name of the CIM service for human
        // # interfaces. This attribute MUST be the
        // # CIM_ObjectManager.ElementName property value.
        if (n1.equal(elementNamePropertyName))
            populateTemplateField(instance1, serviceHiNameAttribute, v1.toString(),
                serviceHiNameProperty);

        // service-hi-description=string O
        // # This string is used as a description of the CIM service for
        // # human interfaces.This attribute MUST be the 
        // # CIM_ObjectManager.Description property value.
        else if (n1.equal(descriptionPropertyName))
          populateTemplateField(instance1, serviceHiDescriptionAttribute, v1.toString(),
              serviceHiDescriptionProperty);
    }

    // template type property.
    CDEBUG("test attr= " << templateTypeAttribute  << " Property name= " << templateTypeProperty);

    //populateTemplateField(instance1, templateTypeAttribute, String("wbem"),
    //    templateTypeProperty);
    populateTemplateField(instance1, templateTypeAttribute, String(serviceName),
        templateTypeProperty);
    
    populateTemplateField(instance1,templateVersionAttribute, String(templateVersion),
        templateVersionProperty);
    
    populateTemplateField(instance1, templateDescriptionAttribute,String(templateDescription),
                            templateDescriptionProperty);
    
    // InterOp Schema
    populateTemplateField(instance1, InteropSchemaNamespaceAttribute, InteropSchemaNamespaceName);
    CDEBUG("Before instance_objMgrComm. instance count = " << instance_ObjMgrComm.getPropertyCount());
    // ATTN: KS Loop through all properties Note: This does not make it easy to
    // distinguish required vs. optional but works for now.
    for(Uint32 j=0;  j < instance_ObjMgrComm.getPropertyCount(); j++)
    {
        CDEBUG("MgrCommLoop. count " << j);
        CIMConstProperty p1=instance_ObjMgrComm.getProperty(j);
        CIMName n1 = p1.getName();
        CIMValue v1= p1.getValue();

        CDEBUG("MgrCommLoop. count " << j << " Name= " << n1.getString());


        if (n1.equal(communicationMechanismAttribute))
            populateTemplateField(instance1, communicationMechanismAttribute,v1.toString());

        else if (n1.equal(otherCommunicationMechanismAttribute))
        {
            
            if (String::equalNoCase(v1.toString(),"1"))
            {
                 //index = instance_ObjMgrComm.findProperty(CIMName(otherCommunicationMechanismDescriptionAttribute));
                 //CIMConstProperty temppr = instance_ObjMgrComm.getProperty(index);
                 String tmp = _getPropertyValue(instance_ObjMgrComm, CIMName(otherCommunicationMechanismDescriptionAttribute));
                 populateTemplateField(instance1, otherCommunicationMechanismDescriptionAttribute,tmp);
            }
        }
        else if (n1.equal("Version"))
          populateTemplateField(instance1, protocolVersionAttribute,v1.toString());

        else if (n1.equal("FunctionalProfileDescriptions"))
        {  
            Array<String> descriptions;
            v1.get(descriptions);

            String desList = _arrayToString(descriptions);
            populateTemplateField(instance1, functionalProfilesSupportedAttribute, desList);

            if (String::equalNoCase(v1.toString(),"Other"))
            {
              Uint32 pos = instance_ObjMgrComm.findProperty(CIMName(otherProfileDescriptionAttribute));
              CIMConstProperty temppr = instance_ObjMgrComm.getProperty(pos);
              String tmp = _getPropertyValue(instance_ObjMgrComm, CIMName(otherProfileDescriptionAttribute));
              populateTemplateField(instance1, otherProfileDescriptionAttribute, tmp);
            }
        }

        else if (n1.equal(multipleOperationsSupportedAttribute))
            populateTemplateField(instance1, multipleOperationsSupportedAttribute,v1.toString());
        
        else if (n1.equal(authenticationMechanismDescriptionsAttribute))
        {
            Array<String> authenticationDescriptions;
            v1.get(authenticationDescriptions);
            String authList = _arrayToString(authenticationDescriptions);

            populateTemplateField(instance1,
                                    authenticationMechanismsSupportedAttribute,
                                    authList);
        }
    }
        
    // fill in the classname information (namespace and classinfo).
    String classInfoList;
    String nameSpaceList =  getNameSpaceInfo( PEGASUS_NAMESPACENAME_INTEROP, classInfoList);

    populateTemplateField(instance1, namespaceAttribute, nameSpaceList);

    populateTemplateField(instance1, classinfoAttribute, classInfoList);

    // set the current time into the instance
    instance1.addProperty(CIMProperty(CIMName("registeredTime"), CIMDateTime::getCurrentDateTime()));

    // populate the RegisteredProfiles Supported attribute.

    populateTemplateField(instance1, 
        registeredProfilesSupportedAttribute, getRegisteredProfileList());

    //Begin registering the service. Keep this debug.
    CDEBUG("Template:\n" << _currentSLPTemplateString);
    
    // Add the template to the instance as a diagnostic for the moment.
    instance1.addProperty(CIMProperty(CIMName("RegisteredTemplate"), _currentSLPTemplateString));


    String fullServiceName = serviceIDPrefix + String(":") + serviceName;
    // generate the serviceID which is prefix:serviceName:serviceUrl 
    String ServiceID = fullServiceName + String(":") + serviceUrlSyntaxValue;

    //String ServiceID = serviceName + String(":") + serviceUrlSyntaxValue;

    CDEBUG("Service URL: " << ServiceID);

    // Fill out the CIMObjectpath for the slp instance. The key for this class is the instanceID.
    // Simply use the count of instances as the ID for the moment.

    Array<CIMKeyBinding> keyBindings;

    keyBindings.append(CIMKeyBinding(instanceIDPropertyName, ServiceID, CIMKeyBinding::STRING));
    CIMObjectPath reference1  = CIMObjectPath("localhost",PEGASUS_NAMESPACENAME_INTEROP,
                                              CIMName(SlpTemplateClassName),
                                              keyBindings);
    // set the key property into the instance.
    instance1.addProperty(CIMProperty(CIMName(instanceIDPropertyName), ServiceID));
    
    // Make a Cstring from the registration information, etc for api
    CString CfullServiceName = fullServiceName.getCString();
    CString CServiceID = ServiceID.getCString();
    CString CstrRegistration = _currentSLPTemplateString.getCString();
    
    // Test the registration

    CDEBUG("TEST_REG: " << (const char *)CServiceID << " serviceName: " << serviceName);

    Uint32 errorCode = slp_agent.test_registration((const char *)CServiceID , 
                        (const char *)CstrRegistration,
                        (const char*)CfullServiceName,
                        "DEFAULT");

    if (errorCode != 0)
    {
        CDEBUG("Test Instance Error. Code=" << errorCode);
        Logger::put(Logger::ERROR_LOG, SlpProvider, Logger::SEVERE,
            "SLP Registration Failed: test_registration. Code $0", errorCode);
        return(false);
    }

    CDEBUG("Tested Registration of instancd Good");
    // register this information.
    Boolean goodRtn = slp_agent.srv_register((const char *)CServiceID ,
                        (const char *)CstrRegistration,
                        (const char *)CfullServiceName,
                        "DEFAULT", 
                        0xffff);

    if (!goodRtn)
    {
        CDEBUG("Register Instance Error. Code=" << errorCode);
        Logger::put(Logger::ERROR_LOG, SlpProvider, Logger::SEVERE,
            "SLP Registration Failed: srv_registration.");
        return(false);
    }

    // Add the registered instance to the current active list.

    CDEBUG("Registered Instance internally Good");
    _instances.append(instance1);
    _instanceNames.append(reference1);
    PEG_METHOD_EXIT();
    return(true);
}

/** issue all necessary SLP registrations. Gets the objects that are required to
    to provide information for registration and calls populate function to create
    a registration for each communication adapter represented by a communication
    object.
    return: Boolean.  Indicates if there was an error in the registration.
*/
Boolean SLPProvider::issueSLPRegistrations()
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::issueSLPREgistrations()");
    CDEBUG("issueSLPReg");
    // This should be the interop namespace.
    // Get the CIM_ObjectManager instance
    Boolean getByAssociator = false;
    CDEBUG("issueSLPRegistrations. Get object manager from namespace= " 
                << PEGASUS_NAMESPACENAME_INTEROP.getString());
    Array<CIMInstance> instancesObjMgr = _cimomHandle.enumerateInstances(
                                             OperationContext(),
                                             PEGASUS_NAMESPACENAME_INTEROP,
                                             CIMName(CIMObjectManagerClassName),
                                             false, false, false,false, CIMPropertyList());
    // Try to get the objmgrcommmech via the association first
    CDEBUG("Registration found Obj Mgr. No Instance = " << instancesObjMgr.size());
    /*
    Array<CIMObjectPath> pathsObjMgr = _cimHandle.enumerateInstanceNames(
                                        OperationContext(),
                                        PEGASUS_NAMESPACENAME_INTEROP,
                                        CIMName(CIMObjectManagerClassName));
    CIMObjectPath objectManagerPath =  pathsObjMgr[0];
    try
    {
    Array<CIMObject> objects = _cimomHandle.associators(
                            OperationContext(),                      // context
                            PEGASUS_NAMESPACE_INTEROP,               // namespace
                            objectManagerPath,                        // Target Object
                            CIM_CommMechanismForObjectManagerAdapter, // assocClass
                            CIMClass(),                             // resultClass
                            String::EMPTY,                          // role
                            String::EMPTY,                          // resultRole
                            true,                                   // includeQualifiers
                            true,                                   // includeClassOrigin
                            CIMPropertyList();                      // PropertyList
                            );
                                            

                                            
    }
        catch(CIMException& e)
    {
        if (e.getCode()) ==  CIM_NOT_SUPPORTED)
            getByAssociator = false;
    }
    */
        // get instances of CIM_ObjectManagerCommMechanism and subclasses directly
    Array<CIMInstance> instancesObjMgrComm = _cimomHandle.enumerateInstances(
                                             OperationContext(),
                                             PEGASUS_NAMESPACENAME_INTEROP,
                                             CIMName(CIMObjectManagerCommMechName),
                                             false, false, false,false, CIMPropertyList());
    

    CDEBUG("Registration found Obj Mgr Comm. No Instance = " << instancesObjMgrComm.size());
    //Loop to create an SLP registration for each communication mechanism
    // Note that this depends on getting from the PG_Class for communication.
    Uint32 itemsRegistered = 0;

    for (Uint32 i = 0; i < instancesObjMgrComm.size(); i++)
    {
        // get protocol property
        String protocol  = _getPropertyValue(instancesObjMgrComm[i], CIMName("namespaceType"), "http");
        
        // get ipaddress property
        String IPAddress = _getPropertyValue(instancesObjMgrComm[i], CIMName("IPAddress"), "127.0.0.1");

        // create a registration instance, test and register it.
        if (populateRegistrationData(protocol, IPAddress, instancesObjMgr[0], instancesObjMgrComm[i]))
            itemsRegistered++;
    }

    // Start the Service Agent.  Note that the actual registrations are part of the populatetemplate
    // function so that the various templates are already created.
    if (itemsRegistered != 0)
    {
        CDEBUG("SLP Registration. Items to register = " << itemsRegistered);
        try
        {
            slp_agent.start_listener();
        }
        catch(...)
        {
            throw CIMOperationFailedException("Start SLP Listener Failed");
        }
    
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
        // Log slp agent started.
        PEG_METHOD_EXIT();
        return(true);
    }

    // ATTN: Log failure to register because no communication mechanisms found.
    // This error reflects NO communications mechanism objects found.  We assume that we MUST always
    // have at least one communication mechanism object for a registration and for a CIMOM.  Anything
    // else should be considered an error.  Reflect this in the log.
    PEG_METHOD_EXIT();
    return(false);
}

void SLPProvider::initialize(CIMOMHandle & handle)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::initialize()");

   _cimomHandle = handle;
   initFlag = false;

   // Do not allow unload.
   _cimomHandle.disallowProviderUnload();
   
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
    //if(initFlag == false)
    //    issueSLPRegistrations();
    
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
    
    // begin processing the request
    handler.processing();

    CDEBUG("enumerateInstances. count instances=" << _instances.size());
    for(Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
       // deliver instance
       handler.deliver(_instances[i]);
       //XmlWriter::printInstanceElement( _instances[i]);
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
    //if(initFlag == false)
    //    issueSLPRegistrations();
    
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

/* process the invokemethods defined for this class. The methods include
   register - forces registration of the CIMOM characteristics as a template.
   This function can also be done by issuing enumerateinstance calls but the
   register is more precise.
   unregister - Forces any registrations to be deregistered and the slp-agen
   to be closed.
   update - causes the existing registration to be updated with new information
*/
void SLPProvider::invokeMethod(
	const OperationContext & context,
	const CIMObjectPath & objectReference,
	const CIMName & methodName,
	const Array<CIMParamValue> & inParameters,
	MethodResultResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
      "SLPProvider::invokeMethod()");
    CDEBUG("invokeMethod. method= " << methodName.getString());

    _nameSpace = objectReference.getNameSpace();
    // convert a fully qualified reference into a local reference
    // (class name and keys only).
    CIMObjectPath localReference = CIMObjectPath(
        String(),
        CIMNamespaceName(),
        objectReference.getClassName(),
        objectReference.getKeyBindings());
    
    handler.processing();
    
    Uint32 response = 0;
    if (objectReference.getClassName().equal (SlpTemplateClassName))
    {
        if (methodName.equal ("register"))
        {
            if(initFlag == false)
                if (issueSLPRegistrations())
                    response = 0;
                else
                    response = 2;
            else
                response = 1;
        }
        else if (methodName.equal ("unregister"))
        {
            if(initFlag == true)
            {
                deregisterSLP();
            }
        }
        else if (methodName.equal ("update"))
        {
            // ATTN: delete current instances
            issueSLPRegistrations();
        }

        else
        {
            // ATTN: Not sure that this is correct exception
            // for illegal method name.
            String e =  "SLPServiceProvider does not support" +
                methodName.getString() + " method.";
            throw CIMNotSupportedException( e);
        }
    }
    handler.deliver(CIMValue(response));
    handler.complete();
    PEG_METHOD_EXIT();
}

// Do not allow termination of this function.
// ATTN: Note that we should allow termination if unregistered.
/*   Remove this function completely as it is no longer supported.
Boolean SLPProvider::tryterminate(void)
{
   return false;
}
*/ 
void SLPProvider::terminate(void)
{
    deregisterSLP();
    delete this;
}

PEGASUS_NAMESPACE_END
