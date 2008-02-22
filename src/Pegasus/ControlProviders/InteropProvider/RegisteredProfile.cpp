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


///////////////////////////////////////////////////////////////////////////////
//  Interop Provider - This provider services those classes from the
//  DMTF Interop schema in an implementation compliant with the SMI-S v1.1
//  Server Profile
//
//  Please see PG_ServerProfile20.mof in the directory
//  $(PEGASUS_ROOT)/Schemas/Pegasus/InterOp/VER20 for retails regarding the
//  classes supported by this control provider.
//
//  Interop forces all creates to the PEGASUS_NAMESPACENAME_INTEROP 
//  namespace. There is a test on each operation that returns 
//  the Invalid Class CIMDError
//  This is a control provider and as such uses the Tracer functions
//  for data and function traces.  Since we do not expect high volume
//  use we added a number of traces to help diagnostics.
///////////////////////////////////////////////////////////////////////////////

#include "InteropProvider.h"
#include "InteropProviderUtils.h"
#include "InteropConstants.h"


PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

//
// Values and ValueMap qualifier names
//
static const CIMName VALUES_QUALIFIERNAME("Values");
static const CIMName VALUEMAP_QUALIFIERNAME("ValueMap");


// Property names for RegisteredProfile class
#define REGISTEREDPROFILE_PROPERTY_INSTANCEID COMMON_PROPERTY_INSTANCEID
const CIMName REGISTEREDPROFILE_PROPERTY_ADVERTISETYPES(
    "AdvertiseTypes");
const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDNAME(
    "RegisteredName");
const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDVERSION(
    "RegisteredVersion");
const CIMName REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION(
    "RegisteredOrganization");
const CIMName REGISTEREDPROFILE_PROPERTY_OTHERREGISTEREDORGANIZATION(
    "OtherRegisteredOrganization");

// Property names for Provider Referenced Profiles
const CIMName REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILES(
    "RegisteredProfiles");
const CIMName REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILES(
    "DependentProfiles");
const CIMName REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILEVERSIONS(
    "RegisteredProfileVersions");
const CIMName REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILEVERSIONS(
    "DependentProfileVersions");
const CIMName REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILES(
    "OtherRegisteredProfiles");
const CIMName REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILES(
    "OtherDependentProfiles");
const CIMName
REFERENCEDPROFILES_PROPERTY_OTHERREGISTEREDPROFILEORGANIZATIONS(
    "OtherRegisteredProfileOrganizations");
const CIMName
REFERENCEDPROFILES_PROPERTY_OTHERDEPENDENTPROFILEORGANIZATIONS(
    "OtherDependentProfileOrganizations");

//
// Method that constructs a CIMInstance object representing an instance of the
// PG_RegisteredProfile or PG_RegisteredSubProfile class (depending on the
// profileClass parameter).
//
CIMInstance InteropProvider::buildRegisteredProfile(
    const String & instanceId,
    const String & profileName,
    const String & profileVersion,
    Uint16         profileOrganization,
    const String & otherProfileOrganization,
    const CIMClass & profileClass)
{
    // Form the skeleton instance
    CIMInstance instance = profileClass.buildInstance(
        false, false, CIMPropertyList());

    // 
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

    // Determine if SLP is currently enabled in the server. If so, specify
    // SLP as the advertise type.
    Array<Uint16> advertiseTypes;
    ConfigManager* configManager = ConfigManager::getInstance();
    if (String::equal(configManager->getCurrentValue("slp"), "true"))
    {
        advertiseTypes.append(3); // Advertised via SLP
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


//
// Generic method for retrieving instances of profile-related classes. This is
// currently used for enumerating the RegisteredProfle, RegisteredSubprofile,
// and SubprofileRequiresProfile classes.
//
Array<CIMInstance> InteropProvider::getProfileInstances(
    const CIMName & profileType, const Array<String> & defaultSniaProfiles)
{
    Array<CIMInstance> instances;
    bool isRequiresProfileOperation = profileType.equal(
        PEGASUS_CLASSNAME_PG_SUBPROFILEREQUIRESPROFILE);
    Array<CIMInstance> profileCapabilities =
        repository->enumerateInstancesForClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_PROVIDERPROFILECAPABILITIES,
            false);
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

    //
    // First build instances based on vendor-created
    // ProviderProfileCapabilities instances.
    //
    Uint32 i = 0;
    Uint32 n = profileCapabilities.size();
    for(; i < n; ++i)
    {
        // Extract the useful properties
        String profileName;
        Uint16 profileOrganization = 0;
        String profileVersion;
        String profileOrganizationName;
        bool getRegisteredProfileInfo = profileType.equal(
            PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE);
        CIMInstance & currentCapabilities = profileCapabilities[i];
        Array<String> profileNames;
        Array<String> profileVersions;
        Array<Uint16> profileOrganizations;
        Array<String> profileOrganizationNames;
        String profileId = extractProfileInfo(currentCapabilities,
            profileCapabilitiesClass, registeredProfileClass, profileName,
            profileVersion, profileOrganization, profileOrganizationName,
            profileNames, profileVersions, profileOrganizations,
            profileOrganizationNames, getRegisteredProfileInfo);
        Array<String> tmpInstanceIds;

        if(getRegisteredProfileInfo)
        {
            tmpInstanceIds.append(profileId);
            profileNames.append(profileName);
            profileVersions.append(profileVersion);
            profileOrganizations.append(profileOrganization);
            profileOrganizationNames.append(profileOrganizationName);
        }
        else
        {
            for(Uint32 j = 0, m = profileNames.size(); j < m; ++j)
            {
                tmpInstanceIds.append(buildProfileInstanceId(
                    profileOrganizationNames[j], profileNames[j],
                    profileVersions[j]));
            }
        }

        for(Uint32 j = 0, m = tmpInstanceIds.size(); j < m; ++j)
        {
            //See if we've already retrieved an equivalent RegisteredSubProfile
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
                    String subprofileVersion = profileVersion;
                    if(profileVersions.size() >= j)
                    {
                        subprofileVersion = profileVersions[j];
                    }
                    instances.append(buildRegisteredProfile(tmpId,
                        profileNames[j], subprofileVersion,
                        profileOrganizations[j],
                        profileOrganizationNames[j],
                        registeredProfileClass));
                }
                instanceIDs.append(tmpId);
            }
        }
    }

    //
    // Now build instances for the Profiles and/or Subprofiles that Pegasus
    // implements in this provider.
    //
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

            //Add instances for SMI-S version 1.2.0
            static String serverProfileId1(buildProfileInstanceId(
                SNIA_NAME, 
                "Server", 
                SNIA_VER_120));
            subprofileId = buildProfileInstanceId(
                SNIA_NAME, 
                defaultSniaProfiles[i],
                SNIA_VER_120);
            compoundId = serverProfileId1 + ":" + subprofileId;
            unique = true;
            for (Uint32 k = 0, x = instanceIDs.size(); k < x; ++k)
            {
                if (instanceIDs[k] == compoundId)
                {
                    unique = false;
                    break;
                }
            }

            if (unique)
            {
                instances.append(buildDependencyInstance(
                    serverProfileId1, 
                    PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                    subprofileId,
                    PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
                    subprofileReqProfileClass));
            }
        }
        else
        {
            const String & currentProfile = defaultSniaProfiles[i];
            String instanceId;
            bool defaultProfileUnique = true;

            //Add profile registration profile instance.
            if (String::equal(
              defaultSniaProfiles[i], 
              String("Profile Registration")))
            {
                instanceId = buildProfileInstanceId(
                    SNIA_NAME,
                    defaultSniaProfiles[i], 
                    SNIA_VER_100);
                for (Uint32 j = 0, m = instanceIDs.size(); j < m; ++j)
                {
                    if (instanceIDs[j] == instanceId)
                    {
                        defaultProfileUnique = false;
                        break;
                    }
                }

                if (defaultProfileUnique)
                {
                    instances.append(
                        buildRegisteredProfile(
                            instanceId,
                            currentProfile, 
                            SNIA_VER_100, 11 /*"SNIA"*/, 
                            String::EMPTY,
                            registeredProfileClass));
                    instanceIDs.append(instanceId);
                }
                continue;
            }

            //Add instances for SMI-S version 1.1.0.
            defaultProfileUnique = true;
            if (String::equal(defaultSniaProfiles[i], String("Server")) ||
                String::equal(defaultSniaProfiles[i], String("Indication")) ||
                String::equal(defaultSniaProfiles[i], String("Software")))
            {
                instanceId = buildProfileInstanceId(
                    SNIA_NAME,
                    defaultSniaProfiles[i],  
                    SNIA_VER_110);
                for (Uint32 j = 0, m = instanceIDs.size(); j < m; ++j)
                {
                    if (instanceIDs[j] == instanceId)
                    {
                        defaultProfileUnique = false;
                        break;
                    }
                }

                if (defaultProfileUnique)
                {
                    instances.append(
                        buildRegisteredProfile(
                            instanceId,
                            currentProfile, 
                            SNIA_VER_110, 11 /*"SNIA"*/, 
                            String::EMPTY,
                            registeredProfileClass));
                    instanceIDs.append(instanceId);
                }
            }

            //Add instances for SMI-S version 1.2.0.
            defaultProfileUnique = true;
            instanceId = buildProfileInstanceId(
                SNIA_NAME,
                defaultSniaProfiles[i],
                SNIA_VER_120);
            defaultProfileUnique = true;
            for (Uint32 j = 0, m = instanceIDs.size(); j < m; ++j)
            {
                if (instanceIDs[j] == instanceId)
                {
                    defaultProfileUnique = false;
                    break;
                }
            }
            if (defaultProfileUnique)
            {
                instances.append(
                    buildRegisteredProfile(
                        instanceId,
                        currentProfile, 
                        SNIA_VER_120, 11 /*"SNIA"*/,
                        String::EMPTY,
                        registeredProfileClass));
                instanceIDs.append(instanceId);
            }
        }
    }

    return instances;
}

//
// Retrieve the RegisteredProfile instances, making use of the generic
// getProfileInstances function above.
//
Array<CIMInstance> InteropProvider::enumRegisteredProfileInstances()
{
    static String serverProfileName("Server");
    static String profileRegistrationProfileName("Profile Registration");
    static String SMISVersionProfileName("SMI-S");
    Array<String> defaultSubprofiles;
    defaultSubprofiles.append(profileRegistrationProfileName);
    defaultSubprofiles.append(SMISVersionProfileName);
    defaultSubprofiles.append(serverProfileName);

    return getProfileInstances(PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
        defaultSubprofiles);
}

const String indicationProfileName("Indication");
const String softwareProfileName("Software");

//
// Retrieve the RegisteredSubProfile instances, making use of the generic
// getProfileInstances function above.
//
Array<CIMInstance> InteropProvider::enumRegisteredSubProfileInstances()
{
    Array<String> defaultSubprofiles;
    defaultSubprofiles.append(indicationProfileName);
    defaultSubprofiles.append(softwareProfileName);
    return getProfileInstances(PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE,
        defaultSubprofiles);
}

//
// Retrieve the SubProfileRequiresProfile instances, making use of the generic
// getProfileInstances function above.
//
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

    //
    // Retrieve all of the ProviderReferencedProfiles provider registration
    // instances. Those instances contain the lists used to create the
    // ReferencedProfiles associations.
    //
    Array<CIMInstance> referencedProfiles =
        repository->enumerateInstancesForClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES,
            false);

    CIMClass providerRefProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_PROVIDERREFERENCEDPROFILES,
            false, true, false);
    CIMClass referencedProfileClass = repository->getClass(
            PEGASUS_NAMESPACENAME_INTEROP,
            PEGASUS_CLASSNAME_PG_REFERENCEDPROFILE,
            false, true, false);

    Array<String> instanceIds;
    for(unsigned int i = 0, n = referencedProfiles.size(); i < n; ++i)
    {
        //
        // Retrieve the required properties linking profile instances via
        // this association.
        //
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

        //
        // Retrieve the "other" information about profiles. This is used when
        // a provider supports a profile not currently listed in the Pegasus
        // provider registration schema.
        //
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

        //
        // Loop through the registered profile and dependent profile
        // information gathered above.
        //
        for(Uint32 j = 0; j < m; ++j)
        {
            Uint16 currentProfile = registeredProfiles[j];
            Uint16 currentDependent = dependentProfiles[j];
            String profileName;
            String dependentName;
            String profileOrgName;
            String dependentOrgName;

            //
            // Get information about the scoping/antecedent profile
            //
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
                profileName = translateValue(currentProfile,
                    REFERENCEDPROFILES_PROPERTY_REGISTEREDPROFILES,
                    VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME,
                    providerRefProfileClass);
                Uint32 index = profileName.find(Char16(':'));
                PEGASUS_ASSERT(index != PEG_NOT_FOUND);
                profileOrgName = profileName.subString(0, index);
                profileName = profileName.subString(index+1);
            }

            //
            // Get information about the referencing/dependent profile
            //
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
                dependentName = translateValue(currentDependent,
                    REFERENCEDPROFILES_PROPERTY_DEPENDENTPROFILES,
                    VALUEMAP_QUALIFIERNAME, VALUES_QUALIFIERNAME,
                    providerRefProfileClass);
                Uint32 index = dependentName.find(Char16(':'));
                PEGASUS_ASSERT(index != PEG_NOT_FOUND);
                dependentOrgName = dependentName.subString(0, index);
                dependentName = dependentName.subString(index+1);
            }

            //
            // Create the instanceID's for the profile and dependent profile
            // and determine if this ReferencedProfile association is unique
            // or if it's already been created.
            //
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
                // This ReferencedProfile association hasn't been created yet.
                // Adding this to the list of instanceIds ensures that a
                // duplicate won't be created later.
                instanceIds.append(instanceId);

                // Now find out whether the profile and dependent profiles are
                // RegisteredProfile or RegisteredSubProfile instances.
                CIMName profileType;
                if(currentProfile >= 1000)
                    profileType = PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE;
                else
                    profileType = PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE;

                CIMName dependentType;
                if(currentDependent >= 1000)
                    dependentType = PEGASUS_CLASSNAME_PG_REGISTEREDSUBPROFILE;
                else
                    dependentType = PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE;

                //
                // Create the actual ReferencedProfile association instance.
                //
                instances.append(buildDependencyInstance(
                    profileId, profileType, dependentId, dependentType,
                    referencedProfileClass));
            }
        }
    }
    //Add a referencedprofile association instance between
    // the server profile and the profile registration profile.
    String profileId = buildProfileInstanceId(
        SNIA_NAME, 
        "Server", 
        SNIA_VER_120);
    String dependentId = buildProfileInstanceId(
        SNIA_NAME, 
        "Profile Registration",
        SNIA_VER_100);
    String instanceId = profileId + ":" + dependentId;
    bool unique = true;
    for (Uint32 k = 0, x = instanceIds.size(); k < x; ++k)
    {
        if (instanceIds[k] == instanceId)
        {
            unique = false;
            break;
        }
    }

    if (unique)
    {
        // This ReferencedProfile association hasn't been created yet.
        // Adding this to the list of instanceIds ensures that a
        // duplicate won't be created later.
        instanceIds.append(instanceId);

        //
        // Create the actual ReferencedProfile association instance.
        //
        instances.append(
            buildDependencyInstance(
                profileId, 
                PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE, 
                dependentId, 
                PEGASUS_CLASSNAME_PG_REGISTEREDPROFILE,
                referencedProfileClass));
    }
    return instances;
}

//
// Given an instance of PG_ProviderProfileCapabilities, this method retrieves
// the values necessary for constructing instances of PG_RegisteredProfile,
// PG_RegisteredSubProfile, and all of their associations.
//
String extractProfileInfo(const CIMInstance & profileCapabilities,
                          const CIMClass & capabilitiesClass,
                          const CIMClass & profileClass,
                          String & name,
                          String & version,
                          Uint16 & organization,
                          String & organizationName,
                          Array<String> & subprofileNames,
                          Array<String> & subprofileVersions,
                          Array<Uint16> & subprofileOrganizations,
                          Array<String> & subprofileOrganizationNames,
                          bool noSubProfileInfo)
{
    Uint16 registeredProfile = getRequiredValue<Uint16>(profileCapabilities,
        PROFILECAPABILITIES_PROPERTY_REGISTEREDPROFILE);

    // Retrieve information about the RegisteredProfile
    if(registeredProfile == 0) // Other
    {
        name = getRequiredValue<String>(profileCapabilities,
            PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDPROFILE);
        organizationName = getRequiredValue<String>(profileCapabilities,
            PROFILECAPABILITIES_PROPERTY_OTHERPROFILEORGANIZATION);
    }
    else
    {
        // Retrieve the profile and organization name from the ValueMap
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

    // Translate the organization name into the organization ValueMap value
    // that will be used to create a PG_RegisteredProfile instance for this
    // profile.
    String organizationMapping = translateValue(organizationName,
        REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION,
        VALUES_QUALIFIERNAME, VALUEMAP_QUALIFIERNAME, profileClass);
    if(organizationMapping.size() == 0)
    {
        organization = 1;
    }
    else
    {
        organization = atoi((const char *)organizationMapping.getCString());
    }


    // Check whether information about the subprofiles associated to the
    // registered profile is requested.
    if(!noSubProfileInfo)
    {
        // Retrieve the ValueMap values for the subprofiles associated with the
        // RegisteredProfile.
        Array<Uint16> registeredSubprofiles =
            getRequiredValue<Array<Uint16> >(profileCapabilities,
                PROFILECAPABILITIES_PROPERTY_REGISTEREDSUBPROFILES);

        // It is possible that a subprofile could contain a different version
        // number than its parent profile, so retrieve the list. If the
        // SubprofileVersions property isn't present, is NULL, or is an empty
        // array, then the version from the parent profile will be used.
        Uint32 subprofileVersionsIndex = profileCapabilities.findProperty(
            PROFILECAPABILITIES_PROPERTY_SUBPROFILEVERSIONS);
        if(subprofileVersionsIndex != PEG_NOT_FOUND)
        {
            CIMValue val = profileCapabilities.getProperty(
                subprofileVersionsIndex).getValue();
            if(!val.isNull())
            {
                val.get(subprofileVersions);
                Uint32 numVersions = subprofileVersions.size();
                if (numVersions != 0 &&
                    numVersions != registeredSubprofiles.size())
                {
                    throw CIMOperationFailedException(
                        profileCapabilities.getPath().toString() +
                        " does not contain enough entries in property " +
                        PROFILECAPABILITIES_PROPERTY_SUBPROFILEVERSIONS
                            .getString());
                }
            }
        }

        // Either none were supplied or the property wasn't supplied, so
        // use the version value from the scoping profile.
        if(subprofileVersions.size() == 0)
        {
            // Add a version string for each registered subprofile
            for(unsigned int i = 0, n = registeredSubprofiles.size();
                i < n; ++i)
            {
                subprofileVersions.append(version);
            }
        }

        // Retrieve any specified "Other" Registered Subprofiles.
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

        Array<String> otherSubprofileOrganizations;
        Uint32 otherOrganizationsIndex = profileCapabilities.findProperty(
            PROFILECAPABILITIES_PROPERTY_OTHERSUBPROFILEORGANIZATIONS);
        Uint32 numOrgs = 0;
        if(otherOrganizationsIndex != PEG_NOT_FOUND)
        {
            CIMValue val = profileCapabilities.getProperty(
                otherOrganizationsIndex).getValue();
            if(!val.isNull())
            {
                val.get(otherSubprofileOrganizations);
                numOrgs = otherSubprofileOrganizations.size();
            }
        }

        // There must be corresponding entries in the
        // OtherRegisteredSubprofiles and OtherSubprofileOrganizations
        // properties
        if(numOrgs != numOtherSubprofiles)
        {
            throw CIMOperationFailedException(
                profileCapabilities.getPath().toString() +
                " does not contain enough entries in property " +
                PROFILECAPABILITIES_PROPERTY_OTHERSUBPROFILEORGANIZATIONS
                    .getString());
        }

        // Now loop through all of the retrieved subprofile information and
        // set the output parameters.
        otherSubprofileIndex = 0;
        for(Uint32 k = 0, x = registeredSubprofiles.size(); k < x; ++k)
        {
            Uint16 subprofileMapping = registeredSubprofiles[k];
            String subprofileName;
            String subprofileOrg;
            if(subprofileMapping == 0) // "Other"
            {
                // Retrieve the subprofile name and organization from the
                // arrays containing the "other" information
                if(otherSubprofileIndex == numOtherSubprofiles)
                {
                    throw CIMOperationFailedException(
                        profileCapabilities.getPath().toString() +
                        " does not contain enough entries in property " +
                        PROFILECAPABILITIES_PROPERTY_OTHERREGISTEREDSUBPROFILES
                            .getString());
                }
                subprofileName =
                    otherRegisteredSubprofiles[otherSubprofileIndex];
                subprofileOrg =
                    otherSubprofileOrganizations[otherSubprofileIndex++];
            }
            else
            {
                // Retrieve the subprofile name and organization from the
                // ValueMap value.
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
                {
                    subprofileOrg = subprofileName.subString(0, orgIndex);
                    subprofileName = subprofileName.subString(orgIndex+1);
                }
                else
                {
                    subprofileOrg = organizationName;
                }
            }

            subprofileNames.append(subprofileName);
            subprofileOrganizationNames.append(subprofileOrg);

            // Translate the organization name into an integral value
            String orgMapping = translateValue(organizationName,
                REGISTEREDPROFILE_PROPERTY_REGISTEREDORGANIZATION,
                VALUES_QUALIFIERNAME, VALUEMAP_QUALIFIERNAME, profileClass);
            if(organizationMapping.size() == 0)
            {
                subprofileOrganizations.append(Uint16(1)); // "Other"
            }
            else
            {
                subprofileOrganizations.append(
                    atoi((const char *)organizationMapping.getCString()));
            }
        }
    }

    return buildProfileInstanceId(organizationName, name, version);
}

PEGASUS_NAMESPACE_END

// END_OF_FILE
