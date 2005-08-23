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
// Authors: Alagaraja Ramasubramanian, IBM Corporation
//          Seema Gupta, IBM Corporation
//          Subodh Soni, IBM Corporation
//
// Modified By: Karl Schopmeyer, (k.schopmeyer@opengroup.org)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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

   To initiate the SA within Pegasus, the system must make a first PEG_WBEMSLPTemplate
   enumerateInstances call to this provider or execute the register method.
   That initiates the build of the registration information and the startup of the SA.

   Note that this providers sets itself so that it cannot be removed once started.
*/

/* TODO
    1.We can make this more flexible by allowing the following:
        a. method to force reregistration.  Today, once you register it is finished.
        b. createInstance, deleteInstance to allow adding and deleting other
        registrations.
    2. Today this module assumes that we will populate the registration once at
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
    6. Determine if we really want to get enumerated  attribute strings from the class or from
    a string tied to the template.  If we get them from the class we are committed to
    whatever is in the class.  However, in reality, the class and the template could
    be different in some cases.  For the moment we are getting them from the class
    and the template and class enumerations do match.
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

#define CDEBUG(X)
//#define CDEBUG(X) PEGASUS_STD(cout) << "SLPProvider " << X << PEGASUS_STD(endl)

PEGASUS_NAMESPACE_BEGIN

    PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

//******************************************************************
//
// Constants
//
//*****************************************************************

const char SlpProvider[] = "SlpProvider";
const char SlpTemplateClassName[] = "PG_WBEMSLPTemplate";
const char CIMObjectManagerClassName[] = "CIM_ObjectManager";
const char CIMObjectManagerCommMechName[] = "CIM_ObjectManagerCommunicationMechanism";
const char PGObjectManagerCommMechName[] = "PG_CIMXMLCommunicationMechanism";
const char CIMNamespaceClassName[] = "CIM_Namespace";
const char CIMCommMechanismForObjectManagerAdapterName[] =
    "CIM_CommMechanismForObjectManagerAdapterName";

const char CIM_NamespaceInManager[] = "CIM_NamespaceInManager";

// This SLP service is named wbem.
// Internally the serviceName is wbem (in the template) and the serviceID is
// service:wbem for registration.
const char serviceIDPrefix[] = "service";
const char serviceName[] = "wbem";

const char elementNamePropertyName[] = "ElementName";
const char descriptionPropertyName[] = "Description";
const char instanceIDPropertyName[] = "InstanceID";

////////////////////////////////////////////////////////////////////////////////////////
// Predefined Values that are part of the SLP template. These values are defined as part
// of the DMTF SLP template.
////////////////////////////////////////////////////////////////////////////////////////

// This is the current template version.  Should change only if there is a new template
// specification released.
const char templateVersion[] = "1.0";

// Fixed test to be inserted in the templateDescription attribute.
const char templateDescription[] =
    "This template describes the attributes used for advertising Pegasus CIM Servers.";

// This name defines the Interop namespace.  It should be in a much more public place
// Than the middle of this provider.
String InteropSchemaNamespaceName = "root/PG_Interop";

// Default list of Registered Profiles is empty
const char defaultRegisteredProfilesList[] = "";

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

const char templateTypeProperty[] = "template_type";
const char templateTypeAttribute[] = "template-type";

const char templateVersionProperty[] = "template_version";
const char templateVersionAttribute[] = "template-version";

const char templateDescriptionProperty[] = "template_description";
const char templateDescriptionAttribute[] = "template-description";

const char serviceUrlSyntaxProperty[] = "template_url_syntax";
const char serviceUrlSyntaxAttribute[] = "template-url-syntax";

const char serviceHiDescriptionProperty[] = "service_hi_description";
const char serviceHiDescriptionAttribute[] = "service-hi-description";

const char serviceHiNameProperty[] = "service_hi_name";
const char serviceHiNameAttribute[] = "service-hi-name";

const char serviceIDProperty[] = "service_id";
const char serviceIDAttribute[] = "service-id";

const char communicationMechanismAttribute[] = "CommunicationMechanism";

const char otherCommunicationMechanismAttribute[] = "otherCommunicationMechanism";

const char otherCommunicationMechanismDescriptionAttribute[] = "OtherCommunicationMechanismDescription";

const char InteropSchemaNamespaceAttribute[] = "InteropSchemaNamespace";

const char protocolVersionAttribute[] = "ProtocolVersion";

const char functionalProfilesSupportedAttribute[] = "FunctionalProfilesSupported";

const char functionalProfileDescriptionsAttribute[] = "FunctionalProfileDescriptions";

const char otherProfileDescriptionAttribute[] = "OtherProfileDescription";

const char multipleOperationsSupportedAttribute[] = "MultipleOperationsSupported";

const char authenticationMechanismsSupportedAttribute[] =  "AuthenticationMechanismsSupported";

const char otherAuthenticationDescriptionsAttribute[] = "OtherAuthenticationDescription";

const char authenticationMechanismDescriptionsAttribute[] = "AuthenticationMechanismDescriptions";

const char namespaceAttribute[] = "Namespace";

const char classinfoAttribute[] =  "Classinfo";

const char registeredProfilesSupportedAttribute[] = "RegisteredProfilesSupported";

CIMNamespaceName _interopNamespace = PEGASUS_NAMESPACENAME_INTEROP;

CIMName namePropertyName = "name";

// This is the dynamic entry point into this dynamic module. The name of
// this provider is "FamilyProvider" which is appened to
// "PegasusCreateProvider_" to form a symbol name. This function is called
// by the ProviderModule to load this provider.
//
// NOTE: The name of the provider must be correct to be loadable.

#ifdef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
const int MAXPATHLEN=2000;
#endif

extern "C" PEGASUS_EXPORT CIMProvider * PegasusCreateProvider(const String & name)
{

    if (String::equalNoCase(name, "SLPProvider") ||
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

String _showStringArray(const Array<String>& s)
{
    String o;
    for (Uint32 i = 0 ; i < s.size() ; i++)
    {
        if (i > 0)
        {
            o.append(" ");
        }
        o.append(s[i]);
    }
    return o;
}
/** Returns value string from the value qualifier for
    the current value of an enumerated property. This function assumes
    that the qualifiers are in the property in the instance itself. The
    function determines if the qualifiers value and valuemap exist and
    are valid and then uses the property value to find the value
    and get the corresponding valueMap string.
    NOTE: Today this function assumes that the qualifiers are in the
    instance.  In the long term this is incorrect.  We need to get the
    class
    @param myInstance CIMInstance containing the property and with
    the qualifiers in the instance.
    @return String The value from the qualifier.
    @exception throws CIM_ERR_FAILED error if the value and
    value map information is not correct.
*/
String _getValueQualifier(
    const CIMConstProperty& instanceProperty, 
    const CIMClass& thisClass)
{
    CIMName propertyName = instanceProperty.getName();
    CIMValue propertyValue = instanceProperty.getValue();

    Uint32 pos;
    CIMConstProperty classProperty;

    if ((pos = thisClass.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        classProperty = thisClass.getProperty(pos);
    }
    else
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, 
            "SLPProvider Property find error. Class " +
            thisClass.getClassName().getString() + " Property "
            + propertyName.getString());

    // validate the valueMap qualifiers in the Class Property

    Uint32 posValueMap;
    String error = String::EMPTY;
    Array<String> va1;
    Array<String> va2;

    if ((posValueMap = classProperty.findQualifier("valueMap")) != PEG_NOT_FOUND)
    {
        CIMConstQualifier qValueMap = classProperty.getQualifier(posValueMap);

        if (!qValueMap.isArray() || (qValueMap.getType() != CIMTYPE_STRING))
        {
            error = "Error in valueMap Qualifier";
        }
        else
        {
            CIMValue q1 = qValueMap.getValue();
            q1.get(va1);

            // validate the value qualifier
            Uint32 posValue;
            if ((posValue = classProperty.findQualifier("values")) == PEG_NOT_FOUND)
                error = "No value Qualifier";
            else
            {
                CIMConstQualifier qValue = classProperty.getQualifier(posValue);
                if (!qValue.isArray() || (qValue.getType() != CIMTYPE_STRING))
                {
                    error = "Invalid value Qualifier";
                }
                else
                {
                    CIMValue q2 = qValue.getValue();
                    q2.get(va2);

                    // Test if the array size for the two values is the same.
                    if (va2.size() != va1.size())
                        error = "Size error on value Qualifier";
                }
            }
        }
    }
    else
    {
        error = "No valueMap Qualifier";
    }

    if (error != String::EMPTY)
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, " Qualifier Value mapping error. "
            + error + " " 
            + propertyName.getString());
    }
    // Need to find the values of the property in the ValueMap. If the
    // property is an array, then all of the elements need to be converted
    // and a concatenated string should be returned. If it is not an array
    // then just the one value needs to be found in the ValueMap qualifier
    // and returned.
    if (propertyValue.isArray())
    {
        // TBD: Need to handle generic property values
        if (propertyValue.getType() == CIMTYPE_UINT16)
        {
            Array<Uint16> values;
            propertyValue.get(values);
            String convertedValues;
            bool found = true;
            for (Uint32 i = 0, size = values.size(); i < size && found; i++)
            {
                if (i != 0)
                    convertedValues.append(Char16(','));

                String currentValueString = CIMValue(values[i]).toString();
                found = false;
                for (Uint32 j = 0, qSize = va1.size(); j < qSize && !found; j++)
                {
                    if (currentValueString == va1[j])
                    {
                        convertedValues.append(va2[j]);
                        found = true;
                    }
                }
            }

            if (found)
            {
                return convertedValues;
            }
        }
    }
    else
    {
        String targetValueString = propertyValue.toString();

        // find this valueMap and get corresponding value
        for (Uint32 i = 0 ; i < va1.size() ; i++)
        {
            if (targetValueString == va1[i])
            {
                // Return corresponding value in value qualifier
                return(va2[i]);
            }
        }
    }

    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
        " Qualifier Value mapping error. Invalid property value " 
        + propertyName.getString());
}

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

String _arrayToString(const Array<Uint16>& s)
{
    String output;
    for (Uint32 i = 0 ; i < s.size() ; i++)
    {
        if (i > 0)
            output.append(",");
        char buffer[128];
        sprintf(buffer,"%i", s[i]);
        output.append(String(buffer));
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
String _getPropertyValueString(
    const CIMInstance& instance, 
    const CIMName& propertyName,
    const String& defaultValue = String::EMPTY)
{
    String output;
    Uint32 pos;
    CDEBUG("_getPropertyValue String for name= " << propertyName.getString()
        << " default= " << defaultValue);
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


Uint16 _getPropertyValueUint16(
    const CIMInstance& instance, 
    const CIMName& propertyName,
    const Uint16& defaultValue = 0)
{
    Uint16 output;
    Uint32 pos;
    CDEBUG("_getPropertyValue Uint16 for name= " << propertyName.getString()
        << " default= " << defaultValue);
    if ((pos = instance.findProperty(propertyName)) != PEG_NOT_FOUND)
    {
        CIMConstProperty p1 = instance.getProperty(pos);
        if (p1.getType() == CIMTYPE_UINT16)
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
void _setPropertyValue(
    CIMInstance& instance, 
    const CIMName propertyName, 
    const String& value)
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
        "SLPProvider::_buildInstanceSkeleton()");

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
/* Remove the instances created as part of the registration and unregister.
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

/** get the list of registered profiles for the SLP template.
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

    const CIMNamespaceName NAMESPACE = CIMNamespaceName("root/PG_InterOp");
    const CIMName          CLASSNAME = CIMName("CIM_RegisteredProfile");
    String                  regitem = String::EMPTY;
    String                  reglist = String::EMPTY;

    Array<CIMInstance>  cimInstances;
    CIMClass RO_Class;

    Array<String> regarray;

    Boolean         deepInheritance = true;
    Boolean         localOnly = true;
    Boolean         includeQualifiers = false;
    Boolean         includeClassOrigin = false;

    try
    {
        RO_Class = _cimomHandle.getClass(
            OperationContext(),
            NAMESPACE,  
            CLASSNAME, 
            localOnly,  
            includeQualifiers,
            includeClassOrigin,
            CIMPropertyList());
    }

    catch (Exception &)
    {
        CDEBUG("SLPProvider::getRegisteredProfiles: get class error");

        Logger::put(Logger::ERROR_LOG, SlpProvider, Logger::SEVERE,
            "getRegisteredProfiles: get class error");

        return(defaultRegisteredProfilesList); 
    }


    try
    {
        //
        // Enumerate Instances.
        //
        cimInstances = _cimomHandle.enumerateInstances(
            OperationContext(),
            NAMESPACE,  
            CLASSNAME, 
            deepInheritance,
            localOnly,  
            includeQualifiers,
            includeClassOrigin,
            CIMPropertyList());


    }
    catch (Exception &)
    {
        CDEBUG("SLPProvider::getRegisteredProfiles: enum instances error");
        Logger::put(Logger::ERROR_LOG, SlpProvider, Logger::SEVERE,
            "getRegisteredProfiles: get class error");

        return(defaultRegisteredProfilesList); 
    }

    CDEBUG ("SLPProvider::getRegisteredProfiles: Total Number of Instances: " << cimInstances.size());

    Uint32 j = 0;
    for (Uint32 i = 0, n = cimInstances.size(); i < n; i++)
    {
        if (cimInstances[i].getClassName() != "CIM_RegisteredProfile")
            continue;

        Uint32 index_RO = cimInstances[i].findProperty("RegisteredOrganization");
        Uint32 index_RN = cimInstances[i].findProperty("RegisteredName");    

        CIMConstProperty RO_Property = cimInstances[i].getProperty(index_RO);
        CIMName RO_PropertyName = RO_Property.getName();
        CIMValue RO_v1= RO_Property.getValue();


        String RegOrg = _getValueQualifier(RO_Property, RO_Class);
        if (RegOrg == String::EMPTY)
        {
            RegOrg = "Unknown";
        }


        CIMConstProperty RN_Property = cimInstances[i].getProperty(index_RN);
        CIMName RN_PropertyName = RN_Property.getName();

        regitem.append(RegOrg);
        regitem.append(":");
        regitem.append(RN_PropertyName.getString());
        regarray[j] = regitem;
        j++;
    }
    if (j > 0)
    {
        reglist = _arrayToString(regarray);
    }

    PEG_METHOD_EXIT();
    return(reglist);
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
String SLPProvider::getNameSpaceInfo(const CIMNamespaceName& nameSpace, String& classInfo)
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

    // Determine if there are any classInfo attributes available.
    Boolean classInfoFound = false;
    for (Uint32 i = 0 ; i < CIMNamespaceInstances.size() ; i++)
    {
        if (_getPropertyValueUint16(CIMNamespaceInstances[i], CIMName(classinfoAttribute)) != 0)
        {
            classInfoFound = true;
            break;
        }
    }
    // Extract the namespace names and class info from the objects.
    for (Uint32 i = 0 ; i < CIMNamespaceInstances.size() ; i++)
    {
        String temp = _getPropertyValueString(CIMNamespaceInstances[i],
                                              CIMName(namePropertyName), "");
        if (temp != String::EMPTY)
        {
            _appendCSV(names, temp);
            // Append to ClassInfo only if at least one valid classinfo property provided
            if (classInfoFound)
            {
                // Append the Classinfo property value into the string for the attribute.
                char buffer[32];
                sprintf(buffer, "%u",_getPropertyValueUint16(CIMNamespaceInstances[i],
                                                             CIMName(classinfoAttribute))); 
                _appendCSV(classInfo, buffer);
            }
        }
        else
            Logger::put(Logger::ERROR_LOG, SlpProvider, Logger::WARNING,
                "SLP Registration bypassed Namespace attribute: $0 property error in CIM_Namespace class.");

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
    @param insstanceFieldName (Optional) String defining the name of the field to populate in
    the instance.
*/
void _addSeparator(String& s)
{
    // if not first entry, set newline
    if (s != String::EMPTY)
        // bug 1737  EOL between attributes confuses some UA
        // s(",\n");
        s.append(",");
}

void SLPProvider::populateTemplateField(
    CIMInstance& instance,
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
void SLPProvider::populateTemplateField(
    CIMInstance& instance,
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

    // Add entry as (name=value)
    _currentSLPTemplateString.append("(" + attributeFieldName + "=" + accumulatedValue + ")");
}

/** populates the SLP template and its corresponding instance. Creates one
    instance of slp registration data, tests the registration and registers
    the information.  The corresponding instance of PG_WBEMSLPTemplate represents
    the data in the registration. It uses largely information from the
    CIM_ObjectManager and its corresponding communication classes plus
    information for namespaces from CIM_Namespace.
    @param protocol String defining http or https. This will become the
    namespaceComponent part of the address in the template.
    @IPAddress - IP address we are to register. This should be a complete
    address minus the namespaceType component
    @param instance_ObgMgr CIMInstance of CIM_ObjectManager
    @param instance-ObjMgrComm CIMInstance of Interop communication class.
    @return - Boolean - True if successful
*/
Boolean SLPProvider::populateRegistrationData(
    const String &protocol,
    const String& IPAddress,
    const CIMInstance& instance_ObjMgr,
    const CIMInstance& instance_ObjMgrComm,
    const CIMClass& commMechClass)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,
        "SLPProvider::populateRegistrationData()");

    // Clear the template instance
    _currentSLPTemplateString.clear();

    CIMInstance templateInstance(SlpTemplateClassName);

    // template-url-syntax=string
    // #The template-url-syntax MUST be the WBEM URI Mapping of
    // #the location of one service access point offered by the WBEM Server
    // #over TCP transport. This attribute must provide sufficient addressing
    // #information so that the WBEM Server can be addressed directly using
    // #the URL.  The WBEM URI Mapping is defined in the WBEM URI Mapping
    // #Specification 1.0.0 (DSP0207).
    // # Example: (template-url-syntax=https://localhost:5989)

    String serviceUrlSyntaxValue = protocol + "://" + IPAddress;

    // Fillout the serviceIDAttribute from the object manager instance name property.
    // This is a key field so must have a value.

    String strUUID = _getPropertyValueString( instance_ObjMgr, namePropertyName, "DefaultEmptyUUID");

    populateTemplateField(templateInstance, serviceUrlSyntaxAttribute, serviceUrlSyntaxValue,
        serviceUrlSyntaxProperty);

    //service-id=string L
    //# The ID of this WBEM Server. The value MUST be the
    //# CIM_ObjectManager.Name property value.
    populateTemplateField(templateInstance, serviceIDAttribute, strUUID,
        serviceIDProperty);

    // get service-hi-name and description from the instance of the
    // objectmanager class
    for (Uint32 j = 0 ; j < instance_ObjMgr.getPropertyCount() ; j++)
    {
        CIMConstProperty p1=instance_ObjMgr.getProperty(j);
        CIMValue v1=p1.getValue();
        CIMName propertyName = p1.getName();

        // service-hi-name=string O
        // # This string is used as a name of the CIM service for human
        // # interfaces. This attribute MUST be the
        // # CIM_ObjectManager.ElementName property value.
        if (propertyName.equal(elementNamePropertyName))
            populateTemplateField(templateInstance, serviceHiNameAttribute, v1.toString(),
                serviceHiNameProperty);

        // service-hi-description=string O
        // # This string is used as a description of the CIM service for
        // # human interfaces.This attribute MUST be the
        // # CIM_ObjectManager.Description property value.
        else if (propertyName.equal(descriptionPropertyName))
            populateTemplateField(templateInstance, serviceHiDescriptionAttribute, v1.toString(),
                serviceHiDescriptionProperty);
    }

    //    templateTypeProperty);
    populateTemplateField(templateInstance, templateTypeAttribute, String(serviceName),
        templateTypeProperty);

    populateTemplateField(templateInstance,templateVersionAttribute, String(templateVersion),
        templateVersionProperty);

    populateTemplateField(templateInstance, templateDescriptionAttribute,String(templateDescription),
        templateDescriptionProperty);

    // InterOp Schema
    populateTemplateField(templateInstance, InteropSchemaNamespaceAttribute, InteropSchemaNamespaceName, String::EMPTY);

    // Loop through all properties and process each.
    // Note: This does not make it easy to distinguish required vs. optional but works.
    for (Uint32 j = 0;  j < instance_ObjMgrComm.getPropertyCount(); j++)
    {
        // get the property value
        CIMConstProperty thisProperty = instance_ObjMgrComm.getProperty(j);
        CIMName propertyName = thisProperty.getName();
        CIMValue v1= thisProperty.getValue();

        if (propertyName.equal(communicationMechanismAttribute))
        {

            String thisValue = _getValueQualifier(thisProperty, commMechClass);
            if (thisValue == String::EMPTY)
            {
                thisValue = "Unknown";
            }
            populateTemplateField(templateInstance, communicationMechanismAttribute, thisValue);
        }

        else if (propertyName.equal(otherCommunicationMechanismAttribute))
        {
            if (String::equalNoCase(v1.toString(),"1"))
            {
                //index = instance_ObjMgrComm.findProperty(CIMName(otherCommunicationMechanismDescriptionAttribute));
                //CIMConstProperty temppr = instance_ObjMgrComm.getProperty(index);
                String tmp = _getPropertyValueString(
                    instance_ObjMgrComm,
                    CIMName(otherCommunicationMechanismDescriptionAttribute),
                    String::EMPTY);

                populateTemplateField(templateInstance, otherCommunicationMechanismDescriptionAttribute,tmp);
            }
        }
        else if (propertyName.equal("Version"))
            populateTemplateField(templateInstance, protocolVersionAttribute,v1.toString());

        else if (propertyName.equal("FunctionalProfileDescriptions"))
        {
            Array<String> descriptions;
            v1.get(descriptions);

            String desList = _arrayToString(descriptions);
            populateTemplateField(templateInstance, functionalProfilesSupportedAttribute, desList);

            if (String::equalNoCase(v1.toString(),"Other"))
            {
                Uint32 pos = instance_ObjMgrComm.findProperty(CIMName(otherProfileDescriptionAttribute));
                CIMConstProperty temppr = instance_ObjMgrComm.getProperty(pos);
                String tmp = _getPropertyValueString(instance_ObjMgrComm, CIMName(otherProfileDescriptionAttribute), String::EMPTY);
                populateTemplateField(templateInstance, otherProfileDescriptionAttribute, tmp);
            }
        }

        else if (propertyName.equal(multipleOperationsSupportedAttribute))
            populateTemplateField(templateInstance, multipleOperationsSupportedAttribute,v1.toString());

        else if (propertyName.equal(authenticationMechanismsSupportedAttribute))
        {
            String thisValue = _getValueQualifier(thisProperty, commMechClass);
            populateTemplateField(templateInstance,
                authenticationMechanismsSupportedAttribute,
                thisValue);
        }
        else if (propertyName.equal(authenticationMechanismDescriptionsAttribute))
        {
            Array<String> authenticationDescriptions;
            v1.get(authenticationDescriptions);
            if (authenticationDescriptions.size() > 0)
            {
                String authDesList = _arrayToString(authenticationDescriptions);

                populateTemplateField(templateInstance,
                    authenticationMechanismDescriptionsAttribute,
                    authDesList);
            }
        }
    }

    // fill in the classname information (namespace and classinfo).
    String classInfoList;
    String nameSpaceList =  getNameSpaceInfo( PEGASUS_NAMESPACENAME_INTEROP, classInfoList);

    populateTemplateField(templateInstance, namespaceAttribute, nameSpaceList);

    if (classInfoList.size() != 0)
    {
        populateTemplateField(templateInstance, classinfoAttribute, classInfoList);
    }

    // set the current time into the instance
    templateInstance.addProperty(CIMProperty(CIMName("registeredTime"), CIMDateTime::getCurrentDateTime()));

    // populate the RegisteredProfiles Supported attribute.

    populateTemplateField(templateInstance,
        registeredProfilesSupportedAttribute, getRegisteredProfileList());

    //Begin registering the service. Keep this debug.
    CDEBUG("Template:\n" << _currentSLPTemplateString);

    // Add the template to the instance as a diagnostic for the moment.
    templateInstance.addProperty(CIMProperty(CIMName("RegisteredTemplate"), _currentSLPTemplateString));


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
    templateInstance.addProperty(CIMProperty(CIMName(instanceIDPropertyName), ServiceID));

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
    _instances.append(templateInstance);
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
    Boolean getByAssociator = false;

    // get the PG communication mechanism class.  Used as part of the populate
    // An exception here is caught in the facade.
    CIMClass pg_CIMXMLClass = _cimomHandle.getClass(
        OperationContext(),
        PEGASUS_NAMESPACENAME_INTEROP,
        CIMName(PGObjectManagerCommMechName),
        false, true, true, CIMPropertyList());

    // Get the CIM_ObjectManager instance
    Array<CIMInstance> instancesObjMgr;
    try
    {
        instancesObjMgr = _cimomHandle.enumerateInstances(
            OperationContext(),
            PEGASUS_NAMESPACENAME_INTEROP,
            CIMName(CIMObjectManagerClassName),
            false, false, false,false, CIMPropertyList());
    }
    catch (const Exception &)
    {
        CDEBUG("Exception caught on enumerateInstances(CIM_ObjectManager):=" << e.getMessage());
    }

    // Try to get the objmgrcommmech via the association first
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
        true, false, true,true, CIMPropertyList());

    //Loop to create an SLP registration for each communication mechanism
    // Note that this depends on getting from the PG_Class for communication.
    Uint32 itemsRegistered = 0;

    for (Uint32 i = 0; i < instancesObjMgrComm.size(); i++)
    {
        // get protocol property
        String protocol  = _getPropertyValueString(instancesObjMgrComm[i],
            CIMName("namespaceType"), "http");

        Uint16 accessProtocol = _getPropertyValueUint16(instancesObjMgrComm[i],
            CIMName("namespaceAccessProtocol"));
        // get ipaddress property
        String IPAddress = _getPropertyValueString(instancesObjMgrComm[i],
            CIMName("IPAddress"), "127.0.0.1");
        // create a registration instance, test and register it.
        if (populateRegistrationData(protocol,
            IPAddress,
            instancesObjMgr[0],
            instancesObjMgrComm[i],
            pg_CIMXMLClass))
        {
            itemsRegistered++;
        }
    }

    // Start the Service Agent.  Note that the actual registrations are part of the populatetemplate
    // function so that the various templates are already created.
    if (itemsRegistered != 0)
    {
        try
        {
            slp_agent.start_listener();
        }
        catch (...)
        {
            throw CIMOperationFailedException("Start SLP Listener Failed");
        }

        Uint32 finish, now, msec;
        System::getCurrentTime(now, msec);
        finish = now + 10;

        // wait for 10 seconds. Earlier wait for 30 secs caused the client to timeout !
        while (finish > now)
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

SLPProvider::SLPProvider()    
{
}

SLPProvider::~SLPProvider()    
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
    for (Uint32 i = 0, n = _instances.size(); i < n; i++)
    {
        CIMObjectPath localReference_frominstanceNames = CIMObjectPath(
            String(),
            CIMNamespaceName(),
            _instanceNames[i].getClassName(),
            _instanceNames[i].getKeyBindings());

        if (localReference == localReference_frominstanceNames)
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
    for (Uint32 i = 0, n = _instances.size(); i < n; i++)
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
    //if(initFlag == false)
    //    issueSLPRegistrations();

    // begin processing the request
    handler.processing();

    for (Uint32 i = 0, n =_instances.size(); i < n; i++)
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
    if (objectReference.getClassName().equal(SlpTemplateClassName))
    {
        if (methodName.equal("register"))
        {
            if (initFlag == false)
                if (issueSLPRegistrations())
                    response = 0;
                else
                    response = 2;
            else
                response = 1;
        }
        else if (methodName.equal("unregister"))
        {
            if (initFlag == true)
            {
                deregisterSLP();
            }
        }
        else if (methodName.equal("update"))
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

void SLPProvider::terminate()
{
    deregisterSLP();
    delete this;
}

PEGASUS_NAMESPACE_END
    
