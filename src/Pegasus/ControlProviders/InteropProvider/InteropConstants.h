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
//=============================================================================

#ifndef InteropConstants_h
#define InteropConstants_h

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/PegasusVersion.h>

PEGASUS_NAMESPACE_BEGIN

//
// Some Commonly shared property names
//
const CIMName COMMON_PROPERTY_NAME("Name");
const CIMName COMMON_PROPERTY_CREATIONCLASSNAME("CreationClassName");
const CIMName COMMON_PROPERTY_SYSTEMNAME("SystemName");
const CIMName COMMON_PROPERTY_SYSTEMCREATIONCLASSNAME(
        "SystemCreationClassName");
const CIMName COMMON_PROPERTY_ELEMENTNAME("ElementName");
const CIMName COMMON_PROPERTY_CAPTION("Caption");
const CIMName COMMON_PROPERTY_INSTANCEID("InstanceId");

// Generic property names for Dependency and its subclasses
const CIMName PROPERTY_ANTECEDENT("Antecedent");
const CIMName PROPERTY_DEPENDENT("Dependent");

// Property names for ProviderProfileCapabilities
const CIMName PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE(
        "RegisteredProfile");
const CIMName PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES(
        "RegisteredSubprofiles");
const CIMName PROFILECAPABILITIES_PROPERTY_PROFILEVERSION("ProfileVersion");
const CIMName PROFILECAPABILITIES_PROPERTY_SUBPROFILEVERSIONS(
        "SubprofileVersions");
const CIMName PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDPROFILE(
        "OtherRegisteredProfile");
const CIMName PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDSUBPROFILES(
        "OtherRegisteredSubprofiles");
const CIMName PROFILECAPABILITIES_PROPERTY_OTHERPROFILEORGANIZATION(
        "OtherProfileOrganization");
const CIMName PROFILECAPABILITIES_PROPERTY_OTHERSUBPROFILEORGANIZATIONS(
        "OtherSubprofileOrganizations");
const CIMName PROFILECAPABILITIES_PROPERTY_CONFORMINGELEMENTS(
        "ConformingElements");

// Property names for ProviderModule class
#define PROVIDERMODULE_PROPERTY_NAME  COMMON_PROPERTY_NAME
const CIMName PROVIDERMODULE_PROPERTY_VENDOR("Vendor");
const CIMName PROVIDERMODULE_PROPERTY_VERSION("Version");
const CIMName PROVIDERMODULE_PROPERTY_INTERFACETYPE("InterfaceType");
const CIMName PROVIDERMODULE_PROPERTY_MAJORVERSION("MajorVersion");
const CIMName PROVIDERMODULE_PROPERTY_MINORVERSION("MinorVersion");
const CIMName PROVIDERMODULE_PROPERTY_REVISIONNUMBER("RevisionNumber");
const CIMName PROVIDERMODULE_PROPERTY_BUILDNUMBER("BuildNumber");

// Property names for Provider class
#define PROVIDER_PROPERTY_NAME  COMMON_PROPERTY_NAME
#define PROVIDER_PROPERTY_ELEMENTNAME  COMMON_PROPERTY_ELEMENTNAME
#define PROVIDER_PROPERTY_CAPTION  COMMON_PROPERTY_CAPTION
const CIMName PROVIDER_PROPERTY_PROVIDERMODULENAME("ProviderModuleName");

// Property names for PG_Capabilities class
#define CAPABILITIES_PROPERTY_PROVIDERMODULENAME \
    PROVIDER_PROPERTY_PROVIDERMODULENAME
const CIMName CAPABILITIES_PROPERTY_PROVIDERNAME("ProviderName");

// Property names for PG_ProviderCapabilities
#define PROVIDERCAPABILITIES_PROPERTY_PROVIDERMODULENAME \
    PROVIDER_PROPERTY_PROVIDERMODULENAME
#define PROVIDERCAPABILITIES_PROPERTY_PROVIDERNAME \
    CAPABILITIES_PROPERTY_PROVIDERNAME
const CIMName PROVIDERCAPABILITIES_PROPERTY_CLASSNAME("ClassName");
const CIMName PROVIDERCAPABILITIES_PROPERTY_NAMESPACES("Namespaces");

// Property names for ElementConformsToProfile
const CIMName ELEMENTCONFORMSTOPROFILE_PROPERTY_CONFORMANTSTANDARD(
    "ConformantStandard");
const CIMName ELEMENTCONFORMSTOPROFILE_PROPERTY_MANAGEDELEMENT(
    "ManagedElement");

// Some property names for ObjectManager Class
#define OM_PROPERTY_NAME COMMON_PROPERTY_NAME
const CIMName OM_PROPERTY_GATHERSTATISTICALDATA(
    "GatherStatisticalData");

// Property names for InstalledSoftwareIdentity
const CIMName INSTALLEDSOFTWAREIDENTITY_PROPERTY_SYSTEM("System");
const CIMName INSTALLEDSOFTWAREIDENTITY_PROPERTY_INSTALLEDSOFTWARE(
    "InstalledSoftware");

// Implementation-specific constant strings
const String SNIA_NAME("SNIA");
const String SNIA_VER_110("1.1.0");
const String SNIA_VER_100("1.0.0");
const String SNIA_VER_120("1.2.0");

const String INTEROP_PROVIDER_NAME("Interoperability Provider");
const String PEGASUS_MODULE_NAME = String(PEGASUS_CIMOM_GENERIC_NAME) +
    String(" ") + String(PEGASUS_PRODUCT_NAME);
const String PEGASUS_INTERNAL_PROVIDER_TYPE(
    "Internal Control Provider");
const String PEGASUS_DYNAMIC("__DYNAMIC_");
const Uint32 PEGASUS_DYNAMIC_LEN(PEGASUS_DYNAMIC.size());
const CIMNamespaceName PEGASUS_NAMESPACENAME_ROOT("root");

#define thisProvider "InteropProvider"

PEGASUS_NAMESPACE_END

#endif // InteropConstants_h
