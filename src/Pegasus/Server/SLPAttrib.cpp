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
//%/////////////////////////////////////////////////////////////////////////////

#include "SLPAttrib.h"
#include <string.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/InternalException.h>

PEGASUS_NAMESPACE_BEGIN

//property names

static const String PROP_ELEMENTNAME = "ElementName";
static const String PROP_DESCRIPTION = "Description";
static const String PROP_NAME = "Name";
static const String PROP_CLASSINFO = "Classinfo";
static const String PROP_COMMUNICATIONMECHANISM = "CommunicationMechanism";
static const String PROP_OTHERCOMMUNICATIONMECHANISMDESCRIPTION =
    "OtherCommunicationMechanismDescription";
static const String PROP_INTEROPSCHEMANAMESPACE = "InteropSchemaNamespace";
static const String PROP_VERSION = "Version";
static const String PROP_FUNCTIONALPROFILESSUPPORTED =
    "FunctionalProfilesSupported";
static const String PROP_FUNCTIONALPROFILEDESCRIPTIONS =
    "FunctionalProfileDescriptions";
static const String PROP_MULTIPLEOPERATIONSSUPPORTED =
    "MultipleOperationsSupported";
static const String PROP_AUTHENTICATIONMECHANISMSSUPPORTED =
    "AuthenticationMechanismsSupported";
static const String PROP_AUTHENTICATIONMECHANISMDESCRIPTIONS =
    "AuthenticationMechanismDescriptions";
static const String PROP_NAMESPACETYPE = "namespaceType";
static const String PROP_IPADDRESS = "IPAddress";

static const String CIM_CLASSNAME_REGISTEREDPROFILE = "CIM_RegisteredProfile";
static const String PROP_OTHERREGISTEREDORGANIZATION =
    "OtherRegisteredOrganization";
static const String PROP_REGISTEREDNAME = "RegisteredName";
static const String PROP_ADVERTISETYPES = "AdvertiseTypes";

//constructor
SLPAttrib::SLPAttrib():serviceType(PEGASUS_SLP_SERVICE_TYPE)
{
}

SLPAttrib::~SLPAttrib()
{
}

String SLPAttrib::getAttributes(void) const
{
    return attributes;
}

String SLPAttrib::getServiceType(void) const
{
    return serviceType;
}

String SLPAttrib::getServiceUrl(void) const
{
    return serviceUrl;
}

// fill all data required for SLP.
// Returns true if specifie
Boolean SLPAttrib::fillData(String protocol)
{
    CIMClient client;
    CIMClass cimClass;
    //
    // open connection to CIMOM
    //
    String hostStr = System::getHostName();

    try
    {
        //
        client.connectLocal();

        //
        Array <CIMInstance> instances;
        CIMInstance saveInstance;
        CIMValue val;
        Uint32 pos;
        Boolean foundProtocol=false;
        Boolean localOnly=true;
        Boolean includeQualifiers=true;
        Boolean includeClassOrigin=false;
        Boolean deepInheritance=true;
        Uint32 len = 0;

        // set to true if there is "Other" property value in
        // FunctionsProfilesSupported value.
        Boolean functionaProfileDescriptions = false;

        instances = client.enumerateInstances (
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_OBJECTMANAGER,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin);

        PEGASUS_ASSERT (instances.size () == 1);

        pos = instances[0].findProperty(PROP_ELEMENTNAME);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = instances[0].getProperty(pos).getValue ();
        serviceHiName = val.toString();

        pos = instances[0].findProperty (PROP_DESCRIPTION);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = instances[0].getProperty(pos).getValue ();
        serviceHiDescription = val.toString();

        pos = instances[0].findProperty (PROP_NAME);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = instances[0].getProperty(pos).getValue ();
        serviceId = val.toString();

        instances = client.enumerateInstances (
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_CIMXMLCOMMUNICATIONMECHANISM,
            deepInheritance,
            localOnly,
            includeQualifiers,
            includeClassOrigin);

        for (Uint32 n=instances.size(),i=0 ; i<n; i++)
        {
            pos = instances[i].findProperty (PROP_NAMESPACETYPE);
            PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
            val = instances[i].getProperty(pos).getValue ();
            if (val.toString() == protocol)
            {
                pos = instances[i].findProperty (PROP_IPADDRESS);
                PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
                val = instances[i].getProperty(pos).getValue ();
                // Save copy of instance of PG_CIMXMLCommunicationMechanism
                // matching protocol for later reference.
                saveInstance = instances[i];
                serviceUrl= PEGASUS_SLP_SERVICE_TYPE;
                serviceUrl.append(":");
                templateUrlSyntax = protocol;
                templateUrlSyntax.append("://");
                templateUrlSyntax.append(val.toString());
                serviceUrl.append(templateUrlSyntax);
                foundProtocol=true;
                break;
            }
        }
        if (!foundProtocol)
        {
            // reset information that was already set
            serviceHiName.clear();
            serviceHiDescription.clear();
            serviceId.clear();
            return false;
        }

        cimClass = client.getClass(PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_CIMNAMESPACE);
        instances = client.enumerateInstances
            (PEGASUS_NAMESPACENAME_INTEROP, PEGASUS_CLASSNAME_CIMNAMESPACE,
             deepInheritance, localOnly,includeQualifiers,includeClassOrigin);

        for (Uint32 n=instances.size(),i=0 ; i<n; i++)
        {
            pos = instances[i].findProperty (PROP_NAME);
            PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
            val = instances[i].getProperty(pos).getValue();
            nameSpaces.append(val.toString());
            nameSpaces.append(",");

            pos = instances[i].findProperty (PROP_CLASSINFO);
            PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
            val = instances[i].getProperty(pos).getValue();
            pos = cimClass.findProperty(PROP_CLASSINFO);
            classes.append(getMappedValue(cimClass.getProperty(pos),val));
            classes.append(",");
        }

        len = nameSpaces.size();
        if (len > 0)
            nameSpaces.remove( len-1,1);

        len = classes.size();
        if (len > 0)
            classes.remove( len-1,1);

        cimClass = client.getClass (PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM);
        instances = client.enumerateInstances (PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_OBJECTMANAGERCOMMUNICATIONMECHANISM,
            deepInheritance,localOnly,includeQualifiers,includeClassOrigin);

        pos = saveInstance.findProperty (PROP_COMMUNICATIONMECHANISM);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = saveInstance.getProperty(pos).getValue();
        pos = cimClass.findProperty(PROP_COMMUNICATIONMECHANISM);
        communicationMechanism = getMappedValue(cimClass.getProperty(pos),val);
        pos = saveInstance.findProperty (
            PROP_OTHERCOMMUNICATIONMECHANISMDESCRIPTION);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

        val = saveInstance.getProperty(pos).getValue ();
        otherCommunicationMechanismDescription = val.toString();

        interopSchemaNamespace = PEGASUS_NAMESPACENAME_INTEROP.getString();

        pos = saveInstance.findProperty (PROP_VERSION);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = saveInstance.getProperty(pos).getValue();
        pos = cimClass.findProperty(PROP_VERSION);
        protocolVersion = getMappedValue(cimClass.getProperty(pos),val);

        pos = saveInstance.findProperty (PROP_FUNCTIONALPROFILESSUPPORTED);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);

        CIMConstProperty constProperty;
        constProperty = saveInstance.getProperty(pos);
        val = constProperty.getValue();
        Array<Uint16> arrayValFPS;
        val.get(arrayValFPS);
        pos = cimClass.findProperty(PROP_FUNCTIONALPROFILESSUPPORTED);
        for (Uint32 n=arrayValFPS.size(),i=0; i<n; i++)
        {
            String profileValue = getMappedValue(cimClass.getProperty(pos),
                                                 CIMValue(arrayValFPS[i]));
            if (profileValue == "Other")
            {
                functionaProfileDescriptions = true;
            }
            functionalProfilesSupported.append(profileValue);
            functionalProfilesSupported.append(",");
        }
        len = functionalProfilesSupported.size();
        if (len > 0)
            functionalProfilesSupported.remove( len-1,1);

       if (functionalProfileDescriptions == true)
       {
           pos = saveInstance.findProperty (PROP_FUNCTIONALPROFILEDESCRIPTIONS);
           PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
           val = saveInstance.getProperty(pos).getValue();
           Array<String> arrayValFPD;
           val.get(arrayValFPD);
           for (Uint32 n=arrayValFPD.size(),i=0; i<n; i++)
           {
               functionalProfileDescriptions.append(arrayValFPD[i]);
               functionalProfileDescriptions.append(",");
           }
           len = functionalProfileDescriptions.size();
           if (len > 0)
               functionalProfileDescriptions.remove( len-1,1);
        }
        pos = saveInstance.findProperty (PROP_MULTIPLEOPERATIONSSUPPORTED);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = saveInstance.getProperty(pos).getValue();
        Boolean value;
        val.get(value);
        if (value == true)
            multipleOperationsSupported.append("TRUE");
        else
            multipleOperationsSupported.append("FALSE");

        pos = saveInstance.findProperty (
            PROP_AUTHENTICATIONMECHANISMSSUPPORTED);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = saveInstance.getProperty(pos).getValue();
        Array<Uint16> arrayValAMS;
        val.get(arrayValAMS);
        pos = cimClass.findProperty (PROP_AUTHENTICATIONMECHANISMSSUPPORTED);
        for (Uint32 n=arrayValAMS.size(),i=0; i<n; i++)
        {
            authenticationMechanismsSupported.append(
                getMappedValue(cimClass.getProperty(pos),
                CIMValue(arrayValAMS[i])));
            authenticationMechanismsSupported.append(",");
        }
        len = authenticationMechanismsSupported.size();
        if (len > 0)
            authenticationMechanismsSupported.remove( len-1,1);

        pos = saveInstance.findProperty (
            PROP_AUTHENTICATIONMECHANISMDESCRIPTIONS);
        PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
        val = saveInstance.getProperty(pos).getValue();
        Array<String> arrayValAMD;
        val.get(arrayValAMD);
        for (Uint32 n=arrayValAMD.size(),i=0; i<n; i++)
        {
            authenticationMechanismDescriptions.append(arrayValAMD[i]);
            authenticationMechanismDescriptions.append(",");
        }
        len = authenticationMechanismDescriptions.size();
        if (len > 0)
            authenticationMechanismDescriptions.remove( len-1,1);
        instances = client.enumerateInstances (PEGASUS_NAMESPACENAME_INTEROP,
            CIM_CLASSNAME_REGISTEREDPROFILE,
            deepInheritance,localOnly,includeQualifiers,includeClassOrigin);

        for (Uint32 n=instances.size(),i=0 ; i<n; i++)
        {
            pos = instances[i].findProperty (PROP_ADVERTISETYPES);
            PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
            CIMValue cAdvTypes = instances[i].getProperty(pos).getValue();
            Array<Uint16> advTypes;
            cAdvTypes.get(advTypes);
            for (Uint32 n=advTypes.size(),j = 0; j < n; j++)
            {
                if(advTypes[j] == 3)
                {
                    pos = instances[i].findProperty (
                        PROP_OTHERREGISTEREDORGANIZATION);
                    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
                    registeredProfiles.append(
                        instances[i].getProperty(pos).getValue().toString());
                    registeredProfiles.append(":");
                    pos = instances[i].findProperty (PROP_REGISTEREDNAME);
                    PEGASUS_ASSERT (pos != PEG_NOT_FOUND);
                    registeredProfiles.append(
                        instances[i].getProperty(pos).getValue().toString());
                    registeredProfiles.append(",");
                    break;
                }
            }
        }

        if (registeredProfiles.size() > 0)
        {
            len = registeredProfiles.size();
            registeredProfiles.remove( len-1,1);
        }
       else
            registeredProfiles.append("");
    }

    catch(Exception& e)
    {
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
            "Server.CIMServer.EXTERNAL_SLP_REGISTRATION_FAILED_EXCEPTION",
            "exception raised during CIMServer registration with"
                " External SLP. $0",
            e.getMessage());
    }

    catch(...)
    {
        Logger::put_l(Logger::STANDARD_LOG, System::CIMSERVER, Logger::WARNING,
           "Server.CIMServer.EXTERNAL_SLP_REGISTRATION_FAILED_EXCEPTION",
           "Exception raised during CIMserver registration with"
               " External SLP.");
    }

    client.disconnect();
    return true;
}

// Map Values to ValueMap
String SLPAttrib::getMappedValue(const CIMProperty& cimProperty, CIMValue value)
{
    String retValue;
    Uint16 localValue;

    if (strcmp(cimTypeToString(value.getType()),"string") == 0)
    {
        value.get(retValue);
    }
    else
    {
        value.get(localValue);
        retValue = Formatter::format("$0",localValue);
    }

    Uint32 posValueMap;

    if ((posValueMap = cimProperty.findQualifier(CIMName("ValueMap")))
        == PEG_NOT_FOUND)
    {
        return retValue;
    }
    CIMConstQualifier qValueMap = cimProperty.getQualifier(posValueMap);
    if (!qValueMap.isArray() || (qValueMap.getType() != CIMTYPE_STRING))
    {
        return retValue;
    }

    CIMValue q1 = qValueMap.getValue();
    Array<String> val;
    q1.get(val);

    Uint32 posValue;
    if ((posValue = cimProperty.findQualifier("Values")) == PEG_NOT_FOUND)
    {
        return retValue;
    }

    CIMConstQualifier qValue = cimProperty.getQualifier(posValue);
    if (!qValue.isArray() || (qValue.getType() != CIMTYPE_STRING))
    {
        return retValue;
    }

    CIMValue q2 = qValue.getValue();
    Array<String> va2;
    q2.get(va2);

    if (va2.size() != val.size())
    {
        return retValue;
    }

    for (Uint32 n=val.size(),i = 0; i < n; i++)
    {
        if(retValue == val[i])
        {
            return (va2[i]);
        }
    }
        return retValue;
}

// Form the attributes String.
void SLPAttrib::formAttributes()
{
    attributes.append(Formatter::format(
        "(template-url-syntax=$0),(service-hi-name=$1),"
        "(service-hi-description=$2),(service-id=$3),"
        "(Namespace=$4),(Classinfo=$5),(CommunicationMechanism=$6),"
        "(OtherCommunicationMechanismDescription=$7),"
        "(InteropSchemaNamespace=$8),(ProtocolVersion=$9),",
        templateUrlSyntax,serviceHiName,
        serviceHiDescription,serviceId,
        nameSpaces,classes,
        communicationMechanism,
        otherCommunicationMechanismDescription,
        interopSchemaNamespace,
        protocolVersion));
    
    attributes.append(Formatter::format(
        "(FunctionalProfilesSupported=$0),"
        "(FunctionalProfileDescriptions=$1),"
        "(MultipleOperationsSupported=$2),"
        "(AuthenticationMechanismsSupported=$3),"
        "(AuthenticationMechanismDescriptions=$4),"
        "(RegisteredProfilesSupported=$5)",
        functionalProfilesSupported,
        functionalProfileDescriptions,
        multipleOperationsSupported,
        authenticationMechanismsSupported,
        authenticationMechanismDescriptions,
        registeredProfiles));

    if (attributes.size() > 7900)
    {
        //truncate the attributes
        attributes = attributes.subString(0,7900);
        Uint32 index = attributes.reverseFind(')');
        attributes= attributes.subString(0,index+1);
    }
    return;
}
PEGASUS_NAMESPACE_END
