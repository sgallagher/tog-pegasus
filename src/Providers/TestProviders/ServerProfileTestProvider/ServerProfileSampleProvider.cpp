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
// Author: Alex Dunfey (dunfey_alexander@emc.com)
//
// Modified By:
//
//
//
//%////////////////////////////////////////////////////////////////////////////

#include "ServerProfileSampleProvider.h"

PEGASUS_USING_PEGASUS;

Boolean processAssocOperationForInst(
    const CIMObjectPath & ref,
    const CIMInstance & assocInstance,
    const CIMName & associationClass,
    const CIMName & remoteClass,
    const String & role,
    const String & remoteRole,
    CIMObjectPath & remoteRef)
{
    // For now we'll cheat, since we know that we're only working on the
    // ElementConformsToProfile association
    static CIMName MANAGED_ELEMENT_PROPERTY("ManagedElement");
    static CIMName CONFORMANT_STANDARD_PROPERTY("ConformantStandard");
    static CIMName ELEMENT_CONFORMS_TO_PROFILE_CLASS(
        "PG_SampleElementConformsToProfile");
    static CIMName COMPUTER_SYSTEM1_CLASS(
        "PG_SampleComputerSystem1");
    static CIMName COMPUTER_SYSTEM2_CLASS(
            "PG_SampleComputerSystem2");
    static CIMName REGISTERED_PROFILE_CLASS(
        "PG_RegisteredProfile");
    if(ref.getClassName() != COMPUTER_SYSTEM1_CLASS &&
        ref.getClassName() != COMPUTER_SYSTEM2_CLASS)
    {
        return false;
    }

    if(!associationClass.isNull() &&
        associationClass != ELEMENT_CONFORMS_TO_PROFILE_CLASS)
    {
        return false;
    }

    if(!remoteClass.isNull() &&
        remoteClass != REGISTERED_PROFILE_CLASS)
    {
        return false;
    }

    if(role.size() > 0 && role != MANAGED_ELEMENT_PROPERTY)
    {
        return false;
    }

    if(remoteRole.size() > 0 && remoteRole != CONFORMANT_STANDARD_PROPERTY)
    {
        return false;
    }

    Uint32 managedElementIndex = assocInstance.findProperty(
        MANAGED_ELEMENT_PROPERTY);
    Uint32 conformantStandardIndex = assocInstance.findProperty(
        CONFORMANT_STANDARD_PROPERTY);
    if(managedElementIndex == PEG_NOT_FOUND ||
        conformantStandardIndex == PEG_NOT_FOUND)
    {
        return false;
    }

    CIMObjectPath managedElementPath;
    assocInstance.getProperty(managedElementIndex).getValue().get(
        managedElementPath);

    CIMObjectPath conformantStandardPath;
    assocInstance.getProperty(conformantStandardIndex).getValue().get(
        conformantStandardPath);

    managedElementPath.setHost(ref.getHost());
    conformantStandardPath.setHost(ref.getHost());
    managedElementPath.setNameSpace(ref.getNameSpace());
    conformantStandardPath.setNameSpace(ref.getNameSpace());

    if(managedElementPath.identical(ref))
    {
        remoteRef = conformantStandardPath;
        return true;
    }

    return false;
}

/**
 * PG_SampleComputerSystem instances must be filled with the following key and
 * required properties:
 *      Name
 *      CreationClassName
 *      Description
 *      ElementName
 *      OperationalStatus
 *      NameFormat
 *      providerType (Key for PG_SampleComputerSystem class)
 */
void buildCSInstance(
    const CIMClass & csClass,
    const String & name,
    Uint16 providerType,
    CIMInstance & newInst)
{
    const String creationClassName(csClass.getClassName().getString());
    const String description("Computer System for Server Profile Sample Provider");
    const String elementName("Sample ComputerSystem");
    Array<Uint16> operationalStatus;
    operationalStatus.append(Uint16(2));
    const String nameFormat("Other");
    newInst = csClass.buildInstance(true, true, CIMPropertyList());

    Uint32 nameIndex = newInst.findProperty("Name");
    CIMProperty nameProp = newInst.getProperty(nameIndex);
    nameProp.setValue(CIMValue(name));
    newInst.removeProperty(nameIndex);
    newInst.addProperty(nameProp);

    Uint32 creationClassNameIndex = newInst.findProperty("CreationClassName");
    CIMProperty creationClassNameProp = newInst.getProperty(creationClassNameIndex);
    creationClassNameProp.setValue(CIMValue(creationClassName));
    newInst.removeProperty(creationClassNameIndex);
    newInst.addProperty(creationClassNameProp);

    Uint32 descriptionIndex = newInst.findProperty("Description");
    CIMProperty descriptionProp = newInst.getProperty(descriptionIndex);
    descriptionProp.setValue(CIMValue(description));
    newInst.removeProperty(descriptionIndex);
    newInst.addProperty(descriptionProp);

    Uint32 elementNameIndex = newInst.findProperty("ElementName");
    CIMProperty elementNameProp = newInst.getProperty(elementNameIndex);
    elementNameProp.setValue(CIMValue(elementName));
    newInst.removeProperty(elementNameIndex);
    newInst.addProperty(elementNameProp);

    Uint32 operationalStatusIndex = newInst.findProperty("OperationalStatus");
    CIMProperty operationalStatusProp = newInst.getProperty(operationalStatusIndex);
    operationalStatusProp.setValue(CIMValue(operationalStatus));
    newInst.removeProperty(operationalStatusIndex);
    newInst.addProperty(operationalStatusProp);

    Uint32 nameFormatIndex = newInst.findProperty("NameFormat");
    CIMProperty nameFormatProp = newInst.getProperty(nameFormatIndex);
    nameFormatProp.setValue(CIMValue(nameFormat));
    newInst.removeProperty(nameFormatIndex);
    newInst.addProperty(nameFormatProp);

    Uint32 providerTypeIndex = newInst.findProperty("providerType");
    CIMProperty providerTypeProp = newInst.getProperty(providerTypeIndex);
    providerTypeProp.setValue(CIMValue(providerType));
    newInst.removeProperty(providerTypeIndex);
    newInst.addProperty(providerTypeProp);

    newInst.setPath(newInst.buildPath(csClass));
}

void buildElementConformsInstance(
    const CIMClass & ecClass,
    const CIMObjectPath & conformantStandard,
    const CIMObjectPath & managedElement,
    CIMInstance & newInst)
{
    newInst = ecClass.buildInstance(true, true, CIMPropertyList());

    Uint32 conformantStandardIndex = newInst.findProperty("ConformantStandard");
    CIMProperty conformantStandardProp = newInst.getProperty(conformantStandardIndex);
    conformantStandardProp.setValue(CIMValue(conformantStandard));
    newInst.removeProperty(conformantStandardIndex);
    newInst.addProperty(conformantStandardProp);

    Uint32 managedElementIndex = newInst.findProperty("ManagedElement");
    CIMProperty managedElementProp = newInst.getProperty(managedElementIndex);
    managedElementProp.setValue(CIMValue(managedElement));
    newInst.removeProperty(managedElementIndex);
    newInst.addProperty(managedElementProp);

    newInst.setPath(newInst.buildPath(ecClass));
}

void ServerProfileSampleProvider::initialize(
        CIMOMHandle & cimom)
{
    static CIMNamespaceName sampleNamespace("test/TestProvider");
    OperationContext context;
    CIMClass computerSystemClass;
    if(type == 1)
    {
        computerSystemClass = cimom.getClass(context,
            sampleNamespace, CIMName("PG_SampleComputerSystem1"), false, true,
            false, CIMPropertyList());
    }
    else
    {
        computerSystemClass = cimom.getClass(context,
            sampleNamespace, CIMName("PG_SampleComputerSystem2"), false, true,
            false, CIMPropertyList());
    }

    CIMInstance inst1;
    buildCSInstance(computerSystemClass, "CSInst1", type, inst1);
    CIMInstance inst2;
    buildCSInstance(computerSystemClass, "CSInst2", type, inst2);

    instances.append(inst1);
    instances.append(inst2);

    if(type == 2)
    {
        CIMInstance inst3;
        buildCSInstance(computerSystemClass, "CSInst3", type, inst3);
        CIMInstance inst4;
        buildCSInstance(computerSystemClass, "CSInst4", type, inst4);

        instances.append(inst3);
        instances.append(inst4);

        // Create the ElementConformsToProfile instances
        CIMNamespaceName interopNamespace("root/PG_InterOp");
        CIMClass elementConformsClass = cimom.getClass(context,
            sampleNamespace, CIMName("PG_SampleElementConformsToProfile"),
            false, true, false, CIMPropertyList());
        Array<CIMName> regProfilePropList;
        regProfilePropList.append(CIMName("RegisteredName"));
        Array<CIMInstance> registeredProfiles =
            cimom.enumerateInstances(context, interopNamespace,
                CIMName("PG_RegisteredProfile"), true, false, false, false,
                regProfilePropList);
        for(Uint32 i = 0, n = registeredProfiles.size(); i < n; ++i)
        {
            CIMInstance & currentProfile = registeredProfiles[i];
            // We had 1 property in the property list, so there should be
            // exactly 1 property in the property list for the instance
            if(currentProfile.getPropertyCount() == 1)
            {
                String registeredName;
                currentProfile.getProperty(0).getValue().get(registeredName);
                CIMInstance elementConformsInst;
                if(registeredName == "SampleProfile")
                {
                    buildElementConformsInstance(elementConformsClass,
                        currentProfile.getPath(),
                        inst4.getPath(),
                        elementConformsInst);
                    instances.append(elementConformsInst);
                }
                else if(registeredName == "Array")
                {
                    buildElementConformsInstance(elementConformsClass,
                        currentProfile.getPath(),
                        inst2.getPath(),
                        elementConformsInst);
                    instances.append(elementConformsInst);
                }
            }
            else
            {
                String errMessage(
                    "Unexpected number of properties found for instance ");
                throw CIMException(CIM_ERR_FAILED, errMessage +
                    currentProfile.getPath().toString());
            }
        }
    }
}

void ServerProfileSampleProvider::getInstance(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    handler.processing();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMInstance & currentInst = instances[i];
        CIMObjectPath currentPath = currentInst.getPath();
        currentPath.setNameSpace(ref.getNameSpace());
        currentPath.setHost(ref.getHost());
        if(currentPath.identical(ref))
        {
            handler.deliver(currentInst);
            handler.complete();
            return;
        }
    }

    throw CIMException(CIM_ERR_NOT_FOUND);
}

void ServerProfileSampleProvider::enumerateInstances(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    InstanceResponseHandler & handler)
{
    handler.processing();
    CIMName className = ref.getClassName();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMInstance & currentInst = instances[i];
        if(currentInst.getClassName() == className)
        {
            handler.deliver(currentInst);
        }
    }
    handler.complete();
}

void ServerProfileSampleProvider::enumerateInstanceNames(
    const OperationContext & context,
    const CIMObjectPath & ref,
    ObjectPathResponseHandler & handler)
{
    handler.processing();
    CIMName className = ref.getClassName();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMInstance & currentInst = instances[i];
        if(currentInst.getClassName() == className)
        {
            handler.deliver(currentInst.getPath());
        }
    }
    handler.complete();
}

void ServerProfileSampleProvider::modifyInstance(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const CIMInstance & obj,
    const Boolean includeQualifiers,
    const CIMPropertyList & propertyList,
    ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ServerProfileSampleProvider::createInstance(
    const OperationContext & context,
    const CIMObjectPath & ref,
    const CIMInstance & obj,
    ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

void ServerProfileSampleProvider::deleteInstance(
    const OperationContext & context,
    const CIMObjectPath & ref,
    ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
}

// CIMAssociationProvider methods
void ServerProfileSampleProvider::associators(
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
    handler.processing();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMObjectPath resultPath;
        CIMInstance & currentInst = instances[i];
        if(processAssocOperationForInst(objectName, currentInst, associationClass,
           resultClass, role, resultRole, resultPath))
        {
            for(Uint32 j = 0, m = instances.size(); j < m; ++j)
            {
                if(j == i)
                    continue;
                CIMInstance & remoteInst = instances[j];
                if(remoteInst.getPath().identical(resultPath))
                {
                    CIMInstance filteredInst = remoteInst.clone();
                    filteredInst.filter(includeQualifiers, includeClassOrigin,
                        propertyList);
                    handler.deliver(filteredInst);
                    break;
                }
            }
        }
    }
    handler.complete();
}

void ServerProfileSampleProvider::associatorNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & associationClass,
    const CIMName & resultClass,
    const String & role,
    const String & resultRole,
    ObjectPathResponseHandler & handler)
{
    handler.processing();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMObjectPath resultPath;
        CIMInstance & currentInst = instances[i];
        if(processAssocOperationForInst(objectName, currentInst, associationClass,
           resultClass, role, resultRole, resultPath))
        {
            handler.deliver(resultPath);
        }
    }
    handler.complete();
}

void ServerProfileSampleProvider::references(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList,
    ObjectResponseHandler & handler)
{
    handler.processing();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMObjectPath tmpPath;
        CIMInstance & currentInst = instances[i];
        if(processAssocOperationForInst(objectName, currentInst, resultClass,
            CIMName(), role, String(), tmpPath))
        {
            CIMInstance filteredInst = currentInst.clone();
            filteredInst.filter(
                includeQualifiers, includeClassOrigin, propertyList);
            handler.deliver(filteredInst);
        }
    }
    handler.complete();
}

void ServerProfileSampleProvider::referenceNames(
    const OperationContext & context,
    const CIMObjectPath & objectName,
    const CIMName & resultClass,
    const String & role,
    ObjectPathResponseHandler & handler)
{
    handler.processing();
    for(Uint32 i = 0, n = instances.size(); i < n; ++i)
    {
        CIMObjectPath tmpPath;
        CIMInstance & currentInst = instances[i];
        if(processAssocOperationForInst(objectName, currentInst, resultClass,
           CIMName(), role, String(), tmpPath))
        {
            handler.deliver(currentInst.getPath());
        }
    }
    handler.complete();
}

