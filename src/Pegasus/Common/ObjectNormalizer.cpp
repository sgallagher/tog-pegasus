//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
    const CIMQualifier & referenceQualifier,
    const CIMQualifier & cimQualifier)
{
    // check name
    if(!referenceQualifier.getName().equal(cimQualifier.getName()))
    {
        String message = cimQualifier.getName().getString() + String(" invalid qualifier name.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // check type
    if(referenceQualifier.getType() != cimQualifier.getType())
    {
        String message = cimQualifier.getName().getString() + String(" incorrect qualifier type.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMQualifier normalizedQualifier(
        referenceQualifier.getName(),
        referenceQualifier.getValue(),  // default value
        referenceQualifier.getFlavor(),
        referenceQualifier.getPropagated() == 0 ? false : true);

    // update value
    if(!cimQualifier.getValue().isNull())
    {
        normalizedQualifier.setValue(cimQualifier.getValue());
    }

    return(normalizedQualifier);
}

static CIMProperty _processProperty(
    const CIMProperty & referenceProperty,
    const CIMProperty & cimProperty,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin)
{
    // check name
    if(!referenceProperty.getName().equal(cimProperty.getName()))
    {
        String message = cimProperty.getName().getString() + String(" invalid property name.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    // check type
    if(referenceProperty.getType() != cimProperty.getType())
    {
        String message = cimProperty.getName().getString() + String(" incorrect property type.");

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
        for(Uint32 i = 0, n = referenceProperty.getQualifierCount(); i < n;i++)
        {
            CIMConstQualifier referenceQualifier = referenceProperty.getQualifier(i);

            normalizedProperty.addQualifier(referenceQualifier.clone());
        }

        // update qualifier values from property or add any new ones
        for(Uint32 i = 0, n = cimProperty.getQualifierCount(); i < n; i++)
        {
            CIMConstQualifier cimQualifier = cimProperty.getQualifier(i);

            // attempt to find existing qualifier
            Uint32 pos = normalizedProperty.findQualifier(cimQualifier.getName());

            if(pos == PEG_NOT_FOUND)
            {
                // add qualifier

                // TODO: ensure the qualifier is exists and is valid in this scope

                normalizedProperty.addQualifier(cimQualifier.clone());
            }
            else
            {
                // update qualifier

                // TODO: normalize qualifier fisrt to check type, etc.?

                normalizedProperty.getQualifier(pos).setValue(cimQualifier.getValue());
            }
        }
    }

    return(normalizedProperty);
}

ObjectNormalizer::ObjectNormalizer(
    const CIMClass & cimClass,
    const Boolean localOnly,
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin)
    :
    _cimClass(cimClass),
    _localOnly(localOnly),
    _includeQualifiers(includeQualifiers),
    _includeClassOrigin(includeClassOrigin)
{
}

CIMObjectPath ObjectNormalizer::processClassObjectPath(const CIMObjectPath & cimObjectPath) const
{
    // pre-check
    if(_cimClass.isUninitialized())
    {
        // do nothing
        return(cimObjectPath);
    }

    if(cimObjectPath.getClassName().isNull())
    {
        throw CIMException(CIM_ERR_FAILED, "uninitialized object path");
    }

    // check class name
    if(!_cimClass.getClassName().equal(cimObjectPath.getClassName()))
    {
        String message = cimObjectPath.getClassName().getString() + String(" invalid class name.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMObjectPath normalizedObjectPath(
        _cimClass.getPath().getHost(),
        _cimClass.getPath().getNameSpace(),
        _cimClass.getClassName());      // use reference class name to preserve case

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

    if(cimObjectPath.getClassName().isNull())
    {
        throw CIMException(CIM_ERR_FAILED, "uninitialized object path");
    }

    // check class name
    if(!_cimClass.getClassName().equal(cimObjectPath.getClassName()))
    {
        String message = cimObjectPath.getClassName().getString() + String(" invalid class name.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMObjectPath normalizedObjectPath(
        _cimClass.getPath().getHost(),
        _cimClass.getPath().getNameSpace(),
        _cimClass.getClassName());      // use reference class name to preserve case

    // get the key property names from the class
    Array<CIMKeyBinding> keys;

    // identify and complete the keys
    for(Uint32 i = 0, n = _cimClass.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty cimProperty = _cimClass.getProperty(i);

        Uint32 pos = cimProperty.findQualifier("key");

        if((pos != PEG_NOT_FOUND) && (cimProperty.getQualifier(pos).getValue().equal(CIMValue(true))))
        {
            // get the property name from the class to preserve case
            CIMName name = cimProperty.getName();

            // ATTN: there is no way to determine the differenciate a null string and a
            // zero length string. the former is invalid, while the later is valid. as
            // a lame workaround, the string value of a key is set to the literal '<null>'
            // if there is no default value. this allows a missing key without a default value
            // to be detected. hopefully, '<null>' is never used as a literal key value.

            // get the string value regardless of type
            String value = cimProperty.getValue().isNull() ? "<null>" : cimProperty.getValue().toString();

            // override the value from the specified object
            for(Uint32 j = 0, m = cimObjectPath.getKeyBindings().size(); j < m; j++)
            {
                if(name.equal(cimObjectPath.getKeyBindings()[j].getName()))
                {
                    // TODO: convert to value to check type compatibility
                    value = cimObjectPath.getKeyBindings()[j].getValue();

                    break;
                }
            }

            // no default and not overriden by specified object path
            if(value == "<null>")
            {
                String message = name.getString() + String(" key property missing from object path.");

                throw CIMException(CIM_ERR_FAILED, message);
            }

            CIMKeyBinding::Type type;

            switch(_cimClass.getProperty(_cimClass.findProperty(name)).getType())
            {
            case CIMTYPE_BOOLEAN:
                type = CIMKeyBinding::BOOLEAN;

                break;
            case CIMTYPE_UINT8:
            case CIMTYPE_SINT8:
            case CIMTYPE_UINT16:
            case CIMTYPE_SINT16:
            case CIMTYPE_UINT32:
            case CIMTYPE_SINT32:
            case CIMTYPE_UINT64:
            case CIMTYPE_SINT64:
            case CIMTYPE_REAL32:
            case CIMTYPE_REAL64:
                type = CIMKeyBinding::NUMERIC;

                break;
            case CIMTYPE_CHAR16:
            case CIMTYPE_STRING:
            case CIMTYPE_DATETIME:
                type = CIMKeyBinding::STRING;

                break;
            case CIMTYPE_REFERENCE:
                type = CIMKeyBinding::REFERENCE;

                break;
            case CIMTYPE_OBJECT:
            default:
                throw CIMException(CIM_ERR_FAILED, "invalid key type.");
                break;
            }

            keys.append(CIMKeyBinding(name, value, type));
        }
    }

    normalizedObjectPath.setKeyBindings(keys);

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

    if(cimInstance.isUninitialized())
    {
        throw CIMException(CIM_ERR_FAILED, "unintialized instance object.");
    }

    // check class name
    if(!_cimClass.getClassName().equal(cimInstance.getClassName()))
    {
        String message = cimInstance.getClassName().getString() + String(" invalid class name.");

        throw CIMException(CIM_ERR_FAILED, message);
    }

    CIMInstance normalizedInstance(_cimClass.getClassName());   // use the original class name to preserve case

    // process instance qualifiers
    if(_includeQualifiers)
    {
        // propagate class qualifiers
        for(Uint32 i = 0, n = _cimClass.getQualifierCount(); i < n; i++)
        {
            CIMConstQualifier referenceQualifier = _cimClass.getQualifier(i);

            // ATTN: all class qualifiers are propagated to instance because making it
            // work as it should (only pass qualifiers with TOINSTANCE) would break
            // existing behavior.
            CIMQualifier cimQualifier(
                referenceQualifier.getName(),
                referenceQualifier.getValue(),
                referenceQualifier.getFlavor(),
                false);

            normalizedInstance.addQualifier(cimQualifier);
        }

        // update qualifier values from instance or add any new ones
        for(Uint32 i = 0, n = cimInstance.getQualifierCount(); i < n; i++)
        {
            CIMConstQualifier cimQualifier = cimInstance.getQualifier(i);

            // attempt to find existing qualifier
            Uint32 pos = normalizedInstance.findQualifier(cimQualifier.getName());

            if(pos == PEG_NOT_FOUND)
            {
                // add

                // TODO: ensure the qualifier is exists and is valid in this scope

                normalizedInstance.addQualifier(cimQualifier.clone());
            }
            else
            {
                // update

                // ATTN: normalize qualifier fisrt to check type, etc.?

                normalizedInstance.getQualifier(pos).setValue(cimQualifier.getValue());
            }
        }
    }

    // 3) check property names and types. any properties in the instance but not in the
    // class are implictly dropped (normalized).
    for(Uint32 i = 0, n = cimInstance.getPropertyCount(); i < n; i++)
    {
        CIMConstProperty cimProperty = cimInstance.getProperty(i);

        Uint32 pos = _cimClass.findProperty(cimProperty.getName());

        if(pos != PEG_NOT_FOUND)
        {
            CIMConstProperty referenceProperty = _cimClass.getProperty(pos);

            // 4) if local only is true, only process properties defined in the class. otherwise, do all properties
            if(!_localOnly || (_localOnly && _cimClass.getClassName().equal(referenceProperty.getClassOrigin())))
            {
                CIMProperty normalizedProperty =
                    _processProperty(
                        referenceProperty.clone(),
                        cimProperty.clone(),
                        _includeQualifiers,
                        _includeClassOrigin);

                normalizedInstance.addProperty(normalizedProperty);
            }
        }
    }

    // update object path
    CIMObjectPath cimObjectPath(cimInstance.getPath());

    cimObjectPath.setClassName(_cimClass.getClassName());

    // add keys from instance if none specified
    if(cimInstance.getPath().getKeyBindings().size() == 0)
    {
        Array<CIMKeyBinding> keys;

        Array<CIMName> keyNames;

        _cimClass.getKeyNames(keyNames);

        for(Uint32 i = 0, n = keyNames.size(); i < n; i++)
        {
            Uint32 pos = cimInstance.findProperty(keyNames[i]);

            if(pos != PEG_NOT_FOUND)
            {
                keys.append(CIMKeyBinding(keyNames[i], cimInstance.getProperty(pos).getValue()));   // get key name from class to preserve case
            }
        }

        cimObjectPath.setKeyBindings(keys);
    }

    normalizedInstance.setPath(processInstanceObjectPath(cimObjectPath));

    return(normalizedInstance);
}

PEGASUS_NAMESPACE_END
