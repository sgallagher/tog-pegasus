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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/ObjectNormalizer.h>

PEGASUS_NAMESPACE_BEGIN

static CIMQualifier _processQualifier(
    CIMConstQualifier & referenceQualifier,
    CIMConstQualifier & cimQualifier)
{
    // check name
    if(!referenceQualifier.getName().equal(cimQualifier.getName()))
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_QUALIFIER_NAME",
            "Invalid qualifier name: $0",
            cimQualifier.getName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // check type
    if(referenceQualifier.getType() != cimQualifier.getType())
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_QUALIFIER_TYPE",
            "Invalid qualifier type: $0",
            cimQualifier.getName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMQualifier normalizedQualifier(
        referenceQualifier.getName(),
        referenceQualifier.getValue(),  // default value
        referenceQualifier.getFlavor(),
        referenceQualifier.getPropagated() == 0 ? false : true);

    // TODO: check override

    // update value
    if(!cimQualifier.getValue().isNull())
    {
        normalizedQualifier.setValue(cimQualifier.getValue());
    }

    return(normalizedQualifier);
}

static CIMProperty _processProperty(
    CIMConstProperty & referenceProperty,
    CIMConstProperty & cimProperty,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
{
    // check name
    if(!referenceProperty.getName().equal(cimProperty.getName()))
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_PROPERTY_NAME",
            "Invalid property name: $0",
            cimProperty.getName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // check type
    if(referenceProperty.getType() != cimProperty.getType())
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_PROPERTY_TYPE",
            "Invalid property type: $0",
            cimProperty.getName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // TODO: check array size?

    CIMProperty normalizedProperty(
        referenceProperty.getName(),
        referenceProperty.getValue(),   // default value
        referenceProperty.getArraySize(),
        referenceProperty.getReferenceClassName(),
        CIMName(),
        false);

    // TODO: check override (especially for references)?

    // update value
    if(!cimProperty.getValue().isNull())
    {
        normalizedProperty.setValue(cimProperty.getValue());
    }

    // update class origin
    if(includeClassOrigin)
    {
        normalizedProperty.setClassOrigin(referenceProperty.getClassOrigin());
    }

    // add qualifiers
    if(includeQualifiers)
    {
        // propagate class property qualifiers
        for(Uint32 i = 0, n = referenceProperty.getQualifierCount(); i < n; i++)
        {
            CIMConstQualifier referenceQualifier = referenceProperty.getQualifier(i);

            Uint32 pos = cimProperty.findQualifier(referenceQualifier.getName());

            // update value if qualifier is present in the specified property
            if(pos != PEG_NOT_FOUND)
            {
                CIMConstQualifier cimQualifier = cimProperty.getQualifier(pos);

                CIMQualifier normalizedQualifier =
                    _processQualifier(
                        referenceQualifier,
                        cimQualifier);

                normalizedProperty.addQualifier(normalizedQualifier);
            }
            else
            {
                normalizedProperty.addQualifier(referenceQualifier.clone());
            }
        }
    }

    return(normalizedProperty);
}

ObjectNormalizer::ObjectNormalizer(void)
    : _includeQualifiers(false),
    _includeClassOrigin(false)
{
}

ObjectNormalizer::ObjectNormalizer(
    const CIMClass & cimClass,
    Boolean includeQualifiers,
    Boolean includeClassOrigin)
    : _cimClass(cimClass),
    _includeQualifiers(includeQualifiers),
    _includeClassOrigin(includeClassOrigin)
{
    if(!_cimClass.isUninitialized())
    {
        // ATTN: the following code is intended to expedite normalizing instances and instance object
        // paths by establishing the keys once now rather than multiple times later. it is biased
        // toward providers that return many instances with many properties.

        // build a reference object path within the class
        Array<CIMKeyBinding> keys;

        for(Uint32 i = 0, n = _cimClass.getPropertyCount(); i < n; i++)
        {
            CIMConstProperty referenceProperty = _cimClass.getProperty(i);

            Uint32 pos = referenceProperty.findQualifier("key");

            if((pos != PEG_NOT_FOUND) && (referenceProperty.getQualifier(pos).getValue().equal(CIMValue(true))))
            {
                if(referenceProperty.getType() == CIMTYPE_REFERENCE)
                {
                    // ATTN: a fake reference is inserted in the key so that the _BubbleSort() method
                    // in CIMObjectPath does not throw and exception. It implicitly validates keys of
                    // type REFERENCE so just place a dummy value for now. The value will be replaced
                    // by the normalized object later.
                    keys.append(CIMKeyBinding(referenceProperty.getName(), "class.key=\"value\"", CIMKeyBinding::REFERENCE));
                }
                else
                {
                    keys.append(CIMKeyBinding(referenceProperty.getName(), referenceProperty.getValue()));
                }
            }
        }

        // update class object path
        CIMObjectPath cimObjectPath(_cimClass.getPath());

        cimObjectPath.setKeyBindings(keys);

        _cimClass.setPath(cimObjectPath);
    }
}

CIMObjectPath ObjectNormalizer::processClassObjectPath(const CIMObjectPath & cimObjectPath) const
{
    // pre-check
    if(_cimClass.isUninitialized())
    {
        // do nothing
        return(cimObjectPath);
    }

    /*
    // ATTN: moving similar logic to the response handlers because this type of error should
    // be checked regardless with or without normalization enabled.
    if(cimObjectPath.getClassName().isNull())
    {
        throw CIMException(CIM_ERR_FAILED, "uninitialized object path");
    }
    */

    /*
    // ATTN: The following code is currently redundant because the CIMName object validates
    // legal names when it is constructed. It is included here for completeness.
    // check class name
    if(!CIMName(cimObjectPath.getClassName()).legal())
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_CLASS_NAME",
            "Invalid class name: $0",
            cimObjectPath.getClassName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }
    */

    // check class type
    if(!_cimClass.getClassName().equal(cimObjectPath.getClassName()))
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_CLASS_TYPE",
            "Invalid class type: $0",
            cimObjectPath.getClassName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMObjectPath normalizedObjectPath(
        _cimClass.getPath().getHost(),
        _cimClass.getPath().getNameSpace(),
        _cimClass.getClassName());

    // ignore any keys, they are not part of a class object path

    return(normalizedObjectPath);
}

CIMObjectPath ObjectNormalizer::processInstanceObjectPath(const CIMObjectPath & cimObjectPath) const
{
    // pre-check
    if(_cimClass.isUninitialized())
    {
        // do nothing
        return(cimObjectPath);
    }

    /*
    // ATTN: moving similar logic to the response handlers because this type of error should
    // be checked regardless with or without normalization enabled.
    if(cimObjectPath.getClassName().isNull())
    {
        throw CIMException(CIM_ERR_FAILED, "uninitialized object path");
    }
    */

    /*
    // ATTN: The following code is currently redundant because the CIMName object validates
    // legal names when it is constructed. It is included here for completeness.
    // check class name
    if(!CIMName(cimObjectPath.getClassName()).legal())
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_CLASS_NAME",
            "Invalid class name: $0",
            cimObjectPath.getClassName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }
    */

    // check class type
    if(!_cimClass.getClassName().equal(cimObjectPath.getClassName()))
    {
        MessageLoaderParms message(
            "Common.ObjectNormalizer.INVALID_CLASS_TYPE",
            "Invalid class type: $0",
            cimObjectPath.getClassName().getString());

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMObjectPath normalizedObjectPath(
        _cimClass.getPath().getHost(),
        _cimClass.getPath().getNameSpace(),
        _cimClass.getClassName());

    Array<CIMKeyBinding> normalizedKeys;

    Array<CIMKeyBinding> referenceKeys = _cimClass.getPath().getKeyBindings();
    Array<CIMKeyBinding> cimKeys = cimObjectPath.getKeyBindings();

    for(Uint32 i = 0, n = referenceKeys.size(); i < n; i++)
    {
        CIMKeyBinding key;

        // override the value from the specified object
        for(Uint32 j = 0, m = cimKeys.size(); j < m; j++)
        {
            if(referenceKeys[i].getName().equal(cimKeys[j].getName()))
            {
                // check type
                if(referenceKeys[i].getType() != cimKeys[j].getType())
                {
                    MessageLoaderParms message(
                        "Common.ObjectNormalizer.INVALID_KEY_TYPE",
                        "Invalid key type: $0",
                        referenceKeys[i].getName().getString());

                    throw CIMException(CIM_ERR_FAILED, message);
                }

                key = CIMKeyBinding(referenceKeys[i].getName(), cimKeys[j].getValue(), referenceKeys[i].getType());

                break;
            }
        }

        // key not found
        if(key.getName().isNull())
        {
            MessageLoaderParms message(
                "Common.ObjectNormalizer.MISSING_KEY",
                "Missing key: $0",
                referenceKeys[i].getName().getString());

            throw CIMException(CIM_ERR_FAILED, message);
        }

        normalizedKeys.append(key);
    }

    normalizedObjectPath.setKeyBindings(normalizedKeys);

    return(normalizedObjectPath);
}

CIMInstance ObjectNormalizer::processInstance(const CIMInstance & cimInstance) const
{
    // pre-checks
    if(_cimClass.isUninitialized())
    {
        // do nothing
        return(cimInstance);
    }

    /*
    // ATTN: moving similar logic to the response handlers because this type of error should
    // be checked regardless with or without normalization enabled.
    if(cimInstance.isUninitialized())
    {
        throw CIMException(CIM_ERR_FAILED, "unintialized instance object.");
    }
    */

    CIMInstance normalizedInstance(_cimClass.getClassName());

    // proces object path
    normalizedInstance.setPath(processInstanceObjectPath(cimInstance.getPath()));

    // process instance qualifiers
    if(_includeQualifiers)
    {
        // propagate class qualifiers
        for(Uint32 i = 0, n = _cimClass.getQualifierCount(); i < n; i++)
        {
            CIMConstQualifier referenceQualifier = _cimClass.getQualifier(i);

            Uint32 pos = cimInstance.findQualifier(referenceQualifier.getName());

            // update value if qualifier is present in the specified property
            if(pos != PEG_NOT_FOUND)
            {
                CIMConstQualifier cimQualifier = cimInstance.getQualifier(pos);

                CIMQualifier normalizedQualifier =
                    _processQualifier(
                        referenceQualifier,
                        cimQualifier);

                normalizedInstance.addQualifier(normalizedQualifier);
            }
            else
            {
                normalizedInstance.addQualifier(referenceQualifier.clone());
            }
        }
    }

    // check property names and types. any properties in the class but not in the instance
    // are implicitly dropped.
    for(Uint32 i = 0, n = _cimClass.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty referenceProperty = _cimClass.getProperty(i);

        Uint32 pos = cimInstance.findProperty(referenceProperty.getName());

        if(pos != PEG_NOT_FOUND)
        {
            CIMConstProperty cimProperty = cimInstance.getProperty(pos);

            CIMProperty normalizedProperty =
                _processProperty(
                    referenceProperty,
                    cimProperty,
                    _includeQualifiers,
                    _includeClassOrigin);

            normalizedInstance.addProperty(normalizedProperty);
        }
    }

    return(normalizedInstance);
}

PEGASUS_NAMESPACE_END
