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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/ObjectNormalizer.h>

PEGASUS_NAMESPACE_BEGIN

#ifdef PEGASUS_DEBUG
#define DEBUG_PRINT(X) { PEGASUS_STD(cout) << X << PEGASUS_STD(endl); }
#else
#define DEBUG_PRINT(X)
#endif

static String CLASSES_TO_IGNORE[] = {
    "__Namespace"
};

static CIMQualifier _resolveQualifier(
    const CIMQualifier & referenceQualifier,
    const CIMQualifier & cimQualifier)
{
    DEBUG_PRINT("_resolveQualifier(" << referenceQualifier.getName().getString() << ")");

    CIMName qualifierName = referenceQualifier.getName();
    CIMValue qualifierValue = referenceQualifier.getValue();
    CIMFlavor qualifierFlavor = referenceQualifier.getFlavor();
    Boolean propagated = referenceQualifier.getPropagated() == 0 ? false : true;

    CIMQualifier newQualifier(
        qualifierName,
        qualifierValue,
        qualifierFlavor,
        propagated);

    // apply default value
    if(cimQualifier.getValue().isNull() && !referenceQualifier.getValue().isNull())
    {
        newQualifier.setValue(referenceQualifier.getValue());
    }

    return(newQualifier);
}

static CIMProperty _resolveProperty(
    const CIMProperty & referenceProperty,
    const CIMProperty & cimProperty,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin)
{
    DEBUG_PRINT("_resolveProperty(" << referenceProperty.getName().getString() << ")");

    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));
    DEBUG_PRINT("includeClassOrigin = " << (includeClassOrigin == true ? "true" : "false"));

    CIMName propertyName = referenceProperty.getName();
    CIMValue propertyValue = referenceProperty.getValue();
    Uint32 arraySize = referenceProperty.getArraySize();
    CIMName referenceClassName = referenceProperty.getReferenceClassName();
    CIMName classOrigin = referenceProperty.getClassOrigin();
    Boolean propagated = referenceProperty.getPropagated();

    // create "resolve" property
    CIMProperty newProperty(
        propertyName,
        propertyValue,
        arraySize,
        referenceClassName,
        classOrigin,
        propagated);

    // TODO: check type

    // TODO: check override (especially for references)?

    newProperty.setValue(cimProperty.getValue());

    DEBUG_PRINT(newProperty.getName().getString() << " value = " << newProperty.getValue().toString());

    // set class origin
    if(includeClassOrigin)
    {
        newProperty.setClassOrigin(referenceProperty.getClassOrigin());
    }
    else
    {
        // ATTN: this step should be unnecessary because the reference property should
        // have been gathered with the same flags, thus making this redundant. the current
        // repository does not seem to honor this flag.
        newProperty.setClassOrigin(CIMName());
    }

    DEBUG_PRINT(newProperty.getName().getString() << " class origin = " << newProperty.getClassOrigin().getString());

    // add qualifiers
    if(includeQualifiers)
    {
        // apply reference property qualifiers
        for(Uint32 i = 0, n = referenceProperty.getQualifierCount(); i < n;i++)
        {
            // ATTN: convert const qualifier to non const
            CIMQualifier referenceQualifier = referenceProperty.getQualifier(i).clone();

            DEBUG_PRINT("adding qualifier - " << referenceQualifier.getName().getString());

            newProperty.addQualifier(referenceQualifier);
        }

        // apply specified property qualifiers
        for(Uint32 i = 0, n = cimProperty.getQualifierCount(); i < n; i++)
        {
            CIMQualifier cimQualifier = cimProperty.getQualifier(i).clone();

            Uint32 pos = newProperty.findQualifier(cimQualifier.getName());

            if(pos == PEG_NOT_FOUND)
            {
                // ATTN: is it legal to specify qualifiers not in the property definition?
            }

            DEBUG_PRINT("updating qualifier - " << cimQualifier.getName().getString());

            newProperty.getQualifier(pos).setValue(cimQualifier.getValue());
        }
    }

    return(newProperty);
}

static CIMParameter _resolveParameter(
    const CIMParameter & referenceParameter,
    const CIMParameter & cimParameter,
    const Boolean includeQualifiers)
{
    DEBUG_PRINT("_resolveParameter(" << referenceParameter.getName().getString() << ")");

    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));

    CIMParameter newParameter;

    return(newParameter);
}

static CIMMethod _resolveMethod(
    const CIMMethod & referenceMethod,
    const CIMMethod & cimMethod,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin)
{
    DEBUG_PRINT("_resolveMethod(" << referenceMethod.getName().getString() << ")");

    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));
    DEBUG_PRINT("includeClassOrigin = " << (includeClassOrigin == true ? "true" : "false"));

    CIMMethod newMethod;

    return(newMethod);
}

static CIMClass _resolveClass(
    const CIMClass & referenceClass,
    const CIMClass & cimClass,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("_resolveClass(" << referenceClass.getClassName().getString() << " : " << referenceClass.getSuperClassName().getString() << ")");

    DEBUG_PRINT("localOnly = " << (localOnly == true ? "true" : "false"));
    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));
    DEBUG_PRINT("includeClassOrigin = " << (includeClassOrigin == true ? "true" : "false"));

    // get object path elements
    String hostName = referenceClass.getPath().getHost();
    CIMNamespaceName nameSpace = referenceClass.getPath().getNameSpace();
    CIMName className = referenceClass.getPath().getClassName();
    CIMName superClassName = referenceClass.getSuperClassName();

    // create "resolved" class
    CIMClass newClass(className);

    // set object path
    newClass.setPath(CIMObjectPath(hostName, nameSpace, className));

    // set super class name
    newClass.setSuperClassName(referenceClass.getSuperClassName());

    if(includeQualifiers)
    {
        // apply reference class qualifiers
        for(Uint32 i = 0, n = referenceClass.getQualifierCount(); i < n;i++)
        {
            // ATTN: convert const qualifier to non const
            CIMQualifier referenceQualifier = referenceClass.getQualifier(i).clone();

            DEBUG_PRINT("adding qualifier - " << referenceQualifier.getName().getString());

            newClass.addQualifier(referenceQualifier);
        }

        // apply specified class qualifiers
        for(Uint32 i = 0, n = cimClass.getQualifierCount(); i < n; i++)
        {
            CIMQualifier cimQualifier = cimClass.getQualifier(i).clone();

            Uint32 pos = newClass.findQualifier(cimQualifier.getName());

            if(pos == PEG_NOT_FOUND)
            {
                // ATTN: is it legal to specify qualifiers not in the property definition?
            }

            DEBUG_PRINT("updating qualifier - " << cimQualifier.getName().getString());

            newClass.getQualifier(pos).setValue(cimQualifier.getValue());
        }
    }

    // apply reference class properties
    for(Uint32 i = 0, n = referenceClass.getPropertyCount(); i < n; i++)
    {
        // ATTN: convert const property to non const
        CIMProperty referenceProperty = referenceClass.getProperty(i).clone();

        DEBUG_PRINT("adding property - " << referenceProperty.getName().getString());

        CIMProperty cimProperty = referenceClass.getProperty(i).clone();

        newClass.addProperty(_resolveProperty(referenceProperty, cimProperty, includeQualifiers, includeClassOrigin));
    }

    // apply reference class methods
    for(Uint32 i = 0, n = referenceClass.getMethodCount(); i < n; i++)
    {
        // ATTN: convert const method to non const
        CIMMethod referenceMethod = referenceClass.getMethod(i).clone();

        DEBUG_PRINT("adding method - " << referenceMethod.getName().getString());

        CIMMethod cimMethod = referenceClass.getMethod(i).clone();

        newClass.addMethod(_resolveMethod(referenceMethod, cimMethod, includeQualifiers, includeClassOrigin));
    }

    // TODO: check for properties in the specified class that do not exist in the reference class

    /*
    // apply specified class properties
    for(Uint32 i = 0, n = cimClass.getPropertyCount(); i < n; i++)
    {
        if(localOnly && !cimClass.getProperty(i).isPropagated())
        {
            Uint32 pos = cimClass.findProperty(cimClass.getProperty(i).getName());

            if(pos == PEG_NOT_FOUND)
            {
                // throw invalid property
            }

            CIMProperty referenceProperty = newClass.getProperty(pos);

            // ATTN: temporarily remove property until a better way to update is implemented.
            newClass.removeProperty(pos);

            // ATTN: convert const property to non const
            CIMProperty cimProperty = cimClass.getProperty(i).clone();

            DEBUG_PRINT("updating property - " << cimProperty.getName().getString());

            newClass.addProperty(_resolveProperty(referenceProperty, cimProperty, includeQualifiers, includeClassOrigin));
        }
    }

    // apply specified class methods
    for(Uint32 i = 0, n = referenceClass.getMethodCount(); i < n; i++)
    {
        // TODO: needs implementation

        // TODO: do parameters have default values? can they be changed? what else can be overriden/added?
    }
    */

    // ATTN: work around bug in CIMClass (null class origin is automatically updated)
    if(!includeClassOrigin)
    {
        for(Uint32 i = 0, n = newClass.getPropertyCount(); i < n; i++)
        {
            newClass.getProperty(i).setClassOrigin(CIMName());
        }

        // methods seem to work fine
    }

    return(newClass);
}

static CIMInstance _resolveInstance(
    const CIMInstance & referenceInstance,
    const CIMInstance & cimInstance,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("_resolveInstance(" << referenceInstance.getClassName().getString() << ")");

    DEBUG_PRINT("localOnly = " << (localOnly == true ? "true" : "false"));
    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));
    DEBUG_PRINT("includeClassOrigin = " << (includeClassOrigin == true ? "true" : "false"));

    // get object path elements
    String hostName = referenceInstance.getPath().getHost();
    CIMNamespaceName nameSpace = referenceInstance.getPath().getNameSpace();
    CIMName className = referenceInstance.getPath().getClassName();

    // create "resolved" instance
    CIMInstance newInstance(className);

    newInstance.setPath(CIMObjectPath(hostName, nameSpace, className));

    if(includeQualifiers)
    {
        // apply reference instance qualifiers
        for(Uint32 i = 0, n = referenceInstance.getQualifierCount(); i < n;i++)
        {
            // ATTN: convert const qualifier to non const
            CIMQualifier referenceQualifier = referenceInstance.getQualifier(i).clone();

            DEBUG_PRINT("adding qualifier - " << referenceQualifier.getName().getString());

            newInstance.addQualifier(referenceQualifier);
        }

        // apply specified instance qualifiers
        for(Uint32 i = 0, n = cimInstance.getQualifierCount(); i < n; i++)
        {
            CIMQualifier cimQualifier = cimInstance.getQualifier(i).clone();

            Uint32 pos = newInstance.findQualifier(cimQualifier.getName());

            if(pos == PEG_NOT_FOUND)
            {
                // ATTN: is it legal to specify qualifiers not in the property definition?
            }

            DEBUG_PRINT("updating qualifier - " << cimQualifier.getName().getString());

            newInstance.getQualifier(pos).setValue(cimQualifier.getValue());
        }
    }

    // apply reference instance properties
    for(Uint32 i = 0, n = referenceInstance.getPropertyCount(); i < n; i++)
    {
        // ATTN: convert const property to non const
        CIMProperty referenceProperty = referenceInstance.getProperty(i).clone();

        DEBUG_PRINT("adding property - " << referenceProperty.getName().getString());

        newInstance.addProperty(_resolveProperty(referenceProperty, referenceProperty, includeQualifiers, includeClassOrigin));
    }

    // apply specified instance properties
    for(Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
        CIMProperty cimProperty = cimInstance.getProperty(i).clone();

        Uint32 pos = newInstance.findProperty(cimProperty.getName());

        if(pos == PEG_NOT_FOUND)
        {
            // throw invalid property
        }

        DEBUG_PRINT("updating property - " << cimProperty.getName().getString());

        newInstance.getProperty(pos).setValue(cimProperty.getValue());
    }

    // update keys
    Array<CIMKeyBinding> keys = referenceInstance.getPath().getKeyBindings();

    for(Uint32 i = 0, n = keys.size(); i < n; i++)
    {
        Uint32 pos = newInstance.findProperty(keys[i].getName());

        if(pos == PEG_NOT_FOUND)
        {
            // throw missing key. it must be present to create a valid object path
        }

        CIMValue value = newInstance.getProperty(pos).getValue();

        // ATTN: check type

        keys[i].setValue(value.toString());
    }

    // udpate object path
    CIMObjectPath path = newInstance.getPath();

    path.setKeyBindings(keys);

    newInstance.setPath(path);

    return(newInstance);
}

static CIMInstance _resolveIndication(
    const CIMIndication & referenceIndication,
    const CIMIndication & cimIndication,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("_resolveIndication(" << referenceIndication.getClassName().getString() << ")");

    CIMNamespaceName nameSpace = referenceIndication.getPath().getNameSpace();
    CIMName className = referenceIndication.getPath().getClassName();

    DEBUG_PRINT("localOnly = " << (localOnly == true ? "true" : "false"));
    DEBUG_PRINT("includeQualifiers = " << (includeQualifiers == true ? "true" : "false"));
    DEBUG_PRINT("includeClassOrigin = " << (includeClassOrigin == true ? "true" : "false"));

    CIMIndication newIndication;

    return(newIndication);
}

static CIMMethod _resolveMethod(
    const CIMMethod & referenceMethod,
    const CIMMethod & cimMethod,
    const Uint32 flags)
{
    DEBUG_PRINT("_resolveMethod(" << referenceMethod.getName().getString() << ")");

    CIMMethod newMethod;

    return(newMethod);
}

static CIMParameter _resolveParameter(
    const CIMParameter & referenceParameter,
    const CIMParameter & cimParameter,
    const Uint32 flags)
{
    DEBUG_PRINT("_resolveParameter(" << referenceParameter.getName().getString() << ")");

    CIMParameter newParameter;

    return(newParameter);
}

ObjectNormalizer::ObjectNormalizer(CIMRepository & repository) : _repository(repository)
{
}

CIMClass ObjectNormalizer::normalizeClass(
    const CIMClass & cimClass,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("ObjectNormalizer::normalizeClass()");

    CIMNamespaceName nameSpace = cimClass.getPath().getNameSpace();
    CIMName className = cimClass.getPath().getClassName();

    CIMClass referenceClass =
        _repository.getClass(
            nameSpace,
            className,
            false,
            true,
            true,
            propertyList);

    // propagate namespace in object
    referenceClass.setPath(CIMObjectPath("", nameSpace, className));

    return(_resolveClass(referenceClass, cimClass, localOnly, includeQualifiers, includeClassOrigin, propertyList));
}

CIMInstance ObjectNormalizer::normalizeInstance(
    const CIMInstance & cimInstance,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("ObjectNormalizer::normalizeInstance()");

    Array<CIMInstance> cimInstances;

    cimInstances.append(cimInstance);

    Array<CIMInstance> resolvedInstances =
        normalizeInstances(
            cimInstances,
            localOnly,
            includeQualifiers,
            includeClassOrigin,
            propertyList);

    if(resolvedInstances.size() != 1)
    {
        throw CIMException(CIM_ERR_FAILED);
    }

    return(resolvedInstances[0]);
}

Array<CIMInstance> ObjectNormalizer::normalizeInstances(
    const Array<CIMInstance> & cimInstances,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("ObjectNormalizer::normalizeInstances()");

    // TODO: ensure array is not empty
    // TODO: ensure objects in the array are initialized
    // TODO: ensure all class names are the same

    String hostName = cimInstances[0].getPath().getHost();
    CIMNamespaceName nameSpace = cimInstances[0].getPath().getNameSpace();
    CIMName className = cimInstances[0].getPath().getClassName();

    // ignore "special" objects (like __Namespace, which does not have a class definition)
    for(Uint32 i = 0, n = sizeof(CLASSES_TO_IGNORE) / sizeof(CLASSES_TO_IGNORE[0]); i < n; i++)
    {
        if(String::equalNoCase(className.getString(), CLASSES_TO_IGNORE[i]))
        {
            return(cimInstances);
        }
    }

    // ATTN: get the complete instance and use this resolver to remove extraneous information.
    CIMClass referenceClass =
        _repository.getClass(
            nameSpace,
            className,
            false,
            true,
            true,
            CIMPropertyList());

    // make the reference class a reference instance
    CIMInstance referenceInstance(className);

    // build a reference object path for the reference instance
    Array<CIMName> keyNames;

    referenceClass.getKeyNames(keyNames);

    Array<CIMKeyBinding> keyBindings;

    for(Uint32 i = 0, n = keyNames.size(); i < n; i++)
    {
        CIMProperty referenceProperty = referenceClass.getProperty(referenceClass.findProperty(keyNames[i]));

        keyBindings.append(CIMKeyBinding(referenceProperty.getName(), referenceProperty.getValue()));
    }

    // propagate host and namespace in object
    referenceInstance.setPath(CIMObjectPath(hostName, nameSpace, className, keyBindings));

    // copy qualifiers (if includeQualfiers == false, the class should have none
    // the following code will do nothing)
    for(Uint32 i = 0, n = referenceClass.getQualifierCount(); i < n; i++)
    {
        CIMQualifier referenceQualifier = referenceClass.getQualifier(i).clone();

        if(referenceQualifier.getFlavor().hasFlavor(CIMFlavor::TOINSTANCE))
        {
            referenceInstance.addQualifier(referenceQualifier);
        }
    }

    // copy properties (if localOnly == true, the class should only have
    // local properties and the following code will operate correctly)
    for(Uint32 i = 0, n = referenceClass.getPropertyCount(); i < n; i++)
    {
        CIMProperty referenceProperty = referenceClass.getProperty(i).clone();

        referenceInstance.addProperty(referenceProperty);
    }

    // TODO: copy methods ???

    Array<CIMInstance> resolvedInstances;

    for(Uint32 i = 0, n = cimInstances.size(); i < n; i++)
    {
        CIMInstance cimInstance =
            _resolveInstance(
                referenceInstance,
                cimInstances[i],
                localOnly,
                includeQualifiers,
                includeClassOrigin,
                propertyList);

        resolvedInstances.append(cimInstance);
    }

    return(resolvedInstances);
}

CIMIndication ObjectNormalizer::normalizeIndication(
    const CIMIndication & cimIndication,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList & propertyList)
{
    DEBUG_PRINT("ObjectNormalizer::normalizeIndication()");

    String hostName = cimIndication.getPath().getHost();
    CIMNamespaceName nameSpace = cimIndication.getPath().getNameSpace();
    CIMName className = cimIndication.getPath().getClassName();

    // ATTN: get the complete instance and use this resolver to remove extraneous information.
    CIMClass referenceClass =
        _repository.getClass(
            nameSpace,
            className,
            false,
            true,
            true,
            CIMPropertyList());

    CIMIndication referenceIndication(className);

    // propagate host and namespace in object
    referenceIndication.setPath(CIMObjectPath(hostName, nameSpace, className));

    return(_resolveIndication(referenceIndication, cimIndication, localOnly, includeQualifiers, includeClassOrigin, propertyList));
}

PEGASUS_NAMESPACE_END
