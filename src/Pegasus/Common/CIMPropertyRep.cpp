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

#include <cstdio>
#include "CIMPropertyRep.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"
#include "MofWriter.h"
#include "DeclContext.h"
#include "StrLit.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

CIMPropertyRep::CIMPropertyRep()
{
}

CIMPropertyRep::CIMPropertyRep(
    const CIMPropertyRep& x,
    Boolean propagateQualifiers)
    :
    Sharable(),
    _name(x._name),
    _value(x._value),
    _arraySize(x._arraySize),
    _referenceClassName(x._referenceClassName),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    if (propagateQualifiers)
        x._qualifiers.cloneTo(_qualifiers);
}

CIMPropertyRep::CIMPropertyRep(
    const CIMName& name,
    const CIMValue& value,
    Uint32 arraySize,
    const CIMName& referenceClassName,
    const CIMName& classOrigin,
    Boolean propagated)
    :
    _name(name), _value(value), _arraySize(arraySize),
    _referenceClassName(referenceClassName), _classOrigin(classOrigin),
    _propagated(propagated)
{
    // ensure name is not null
    if (name.isNull())
    {
        throw UninitializedObjectException();
    }

    if ((arraySize != 0) &&
        (!value.isArray() || value.getArraySize() != arraySize))
    {
        throw TypeMismatchException();
    }

    // A CIM Property may not be of reference array type
    if (value.isArray() && (value.getType() == CIMTYPE_REFERENCE))
    {
        throw TypeMismatchException();
    }

    // if referenceClassName exists, must be CIMType REFERENCE.
    if (!referenceClassName.isNull())
    {
        if (_value.getType() != CIMTYPE_REFERENCE)
        {
            throw TypeMismatchException();
        }
    }

    // Can a property be of reference type with a null referenceClassName?
    // The DMTF says yes if it is a property of an instance; no if it is a
    // property of a class.  We'll allow it here, but check in the CIMClass
    // addProperty() method.
}

CIMPropertyRep::~CIMPropertyRep()
{
}

void CIMPropertyRep::setName(const CIMName& name)
{
    // ensure name is not null
    if (name.isNull())
    {
        throw UninitializedObjectException();
    }

    _name = name;
}

void CIMPropertyRep::setClassOrigin(const CIMName& classOrigin)
{
    _classOrigin = classOrigin;
}

void CIMPropertyRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace,
    Boolean isInstancePart,
    const CIMConstProperty& inheritedProperty,
    Boolean propagateQualifiers)
{
    PEGASUS_ASSERT(!inheritedProperty.isUninitialized());

    // Check the type:

    if (!inheritedProperty.getValue().typeCompatible(_value))
    {
        if (!(
            (inheritedProperty.getValue().getType() == CIMTYPE_OBJECT) &&
            (_value.getType() == CIMTYPE_STRING) &&
            (_qualifiers.find(CIMName("EmbeddedObject")) != PEG_NOT_FOUND) &&
            (inheritedProperty.getValue().isArray() == _value.isArray())
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            ) &&
            !(
            (inheritedProperty.getValue().getType() == CIMTYPE_INSTANCE) &&
            (_value.getType() == CIMTYPE_STRING) &&
            (_qualifiers.find(CIMName("EmbeddedInstance")) != PEG_NOT_FOUND) &&
            (inheritedProperty.getValue().isArray() == _value.isArray())
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            ))
        {
            throw TypeMismatchException();
        }
    }

    // Validate the qualifiers of the property (according to
    // superClass's property with the same name). This method
    // will throw an exception if the validation fails.

    CIMScope scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMTYPE_REFERENCE)
        scope = CIMScope::REFERENCE;

    // Test the reference class name against the inherited property
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    if (_value.getType() == CIMTYPE_REFERENCE || 
        _value.getType() == CIMTYPE_INSTANCE)
#else // !PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    if (_value.getType() == CIMTYPE_REFERENCE)
#endif // end PEGASUS_EMBEDDED_INSTANCE_SUPPORT
    {
        CIMName inheritedClassName;
        Array<CIMName> classNames;

#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        if (_value.getType() == CIMTYPE_INSTANCE)
        {
            Uint32 pos = inheritedProperty.findQualifier("EmbeddedInstance");
            if (pos != PEG_NOT_FOUND)
            {
                String qualStr;
                inheritedProperty.getQualifier(pos).getValue().get(qualStr);
                inheritedClassName = qualStr;
            }

            if (_value.isArray())
            {
                Array<CIMInstance> embeddedInstances;
                _value.get(embeddedInstances);
                for (Uint32 i = 0, n = embeddedInstances.size(); i < n; ++i)
                {
                    classNames.append(embeddedInstances[i].getClassName());
                }
            }
            else
            {
                CIMInstance embeddedInst;
                _value.get(embeddedInst);
                classNames.append(embeddedInst.getClassName());
            }
        }
        else
#endif // end PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        {
            CIMName referenceClass;
            if (_referenceClassName.isNull())
            {
                CIMObjectPath reference;
                _value.get(reference);
                referenceClass = reference.getClassName();
            }
            else
            {
                referenceClass = _referenceClassName;
            }

            inheritedClassName = inheritedProperty.getReferenceClassName();
            classNames.append(referenceClass);
        }

        // This algorithm is friendly to arrays of embedded instances. It
        // remembers the class names that are found to be subclasses of the
        // inherited class name retrieved from the inherited property. This
        // ensures that any branch in the inheritance hierarchy will only be
        // traversed once. This provides significant optimization given that
        // most elements of an array of embedded instances will probably be of
        // very closely related types.
        Array<CIMName> successTree;
        successTree.append(inheritedClassName);
        for (Uint32 i = 0, n = classNames.size(); i < n; ++i)
        {
            Array<CIMName> traversalHistory;
            CIMName currentName = classNames[i];
            Boolean found = false;
            while (!found && !currentName.isNull())
            {
                for (Uint32 j = 0, m = successTree.size(); j < m; ++j)
                {
                    if (currentName == successTree[j])
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    traversalHistory.append(currentName);
                    CIMClass currentClass = declContext->lookupClass(
                            nameSpace, currentName);
                    if (currentClass.isUninitialized())
                    {
                        throw PEGASUS_CIM_EXCEPTION(
                                CIM_ERR_NOT_FOUND, currentName.getString());
                    }
                    currentName = currentClass.getSuperClassName();
                }
            }

            if (!found)
            {
                throw TypeMismatchException();
            }

            successTree.appendArray(traversalHistory);
        }
    }

    _qualifiers.resolve(
        declContext, nameSpace, scope, isInstancePart,
        inheritedProperty._rep->_qualifiers, propagateQualifiers);

    _classOrigin = inheritedProperty.getClassOrigin();
}

void CIMPropertyRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace,
    Boolean isInstancePart,
    Boolean propagateQualifiers)
{
    CIMQualifierList dummy;

    CIMScope scope = CIMScope::PROPERTY;

    if (_value.getType() == CIMTYPE_REFERENCE)
        scope = CIMScope::REFERENCE;

    _qualifiers.resolve(
        declContext,
        nameSpace,
        scope,
        isInstancePart,
        dummy,
        propagateQualifiers);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMPropertyRep::toXml(Buffer& out) const
{
    if (_value.isArray())
    {
        out << STRLIT("<PROPERTY.ARRAY NAME=\"") << _name << STRLIT("\" ");

        if (_value.getType() == CIMTYPE_OBJECT)
        {
            // If the property array type is CIMObject, then
            //     encode the property in CIM-XML as a string array with the
            //     EMBEDDEDOBJECT attribute (there is not currently a CIM-XML
            //     "object" datatype)

            Array<CIMObject> a;
            _value.get(a);
            out << STRLIT(" TYPE=\"string\"");
            // If the Embedded Object is an instance, always add the
            // EMBEDDEDOBJECT attribute.
            if (a.size() > 0 && a[0].isInstance())
                out << STRLIT(" EMBEDDEDOBJECT=\"object\"");
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
            else
            {
#endif
                // Else the Embedded Object is a class, always add the
                // EmbeddedObject qualifier.  Note that if the macro
                // PEGASUS_SNIA_INTEROP_COMPATIBILITY is defined, then
                // the EmbeddedObject qualifier will always be added,
                // whether it's a class or an instance.
                if (_qualifiers.find(CIMName("EmbeddedObject")) ==
                    PEG_NOT_FOUND)
                {
                    // Note that _qualifiers is not defined as const, and
                    // neither is add(), but this method toXml() *is* const.
                    // However, in this case we really do want to add the
                    // EmbeddedObject qualifier, so we cast away the implied
                    // const-ness of _qualifiers.
                    CIMQualifierList * tmpQualifiers =
                        const_cast<CIMQualifierList *>(&_qualifiers);
                    tmpQualifiers->add(
                        CIMQualifier(CIMName("EmbeddedObject"), true));
                }
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
            }
#endif
        }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        else if (_value.getType() == CIMTYPE_INSTANCE)
        {
            // If the property array type is CIMInstance, then
            //   encode the property in CIM-XML as a string array with the
            //   EMBEDDEDOBJECT attribute (there is not currently a CIM-XML
            //   "instance" datatype)

            Array<CIMInstance> a;
            _value.get(a);
            out << " TYPE=\"string\"";

            // add the EMBEDDEDOBJECT attribute
            if (a.size() > 0)
            {
                out << " EMBEDDEDOBJECT=\"instance\"";

                // Note that if the macro PEGASUS_SNIA_INTEROP_COMPATIBILITY is
                // defined, then the EmbeddedInstance qualifier will be added
#ifdef PEGASUS_SNIA_INTEROP_COMPATIBILITY
                if (_qualifiers.find(CIMName("EmbeddedInstance")) ==
                    PEG_NOT_FOUND)
                {
                    // Note that _qualifiers is not defined as const, and
                    // neither is add(), but this method toXml() *is* const.
                    // However, in this case we really do want to add the
                    // EmbeddedObject qualifier, so we cast away the implied
                    // const-ness of _qualifiers.

                    // For now, we assume that all the embedded instances in
                    // the array are of the same type
                    CIMQualifierList * tmpQualifiers =
                        const_cast<CIMQualifierList *>(&_qualifiers);
                    tmpQualifiers->add(CIMQualifier(
                        CIMName("EmbeddedInstance"),
                        a[0].getClassName().getString()));
                }
#endif
            }
        }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        else
        {
            out << STRLIT(" TYPE=\"") << cimTypeToString(_value.getType ());
            out.append('"');
        }

        if (_arraySize)
        {
            char buffer[32];
            sprintf(buffer, "%d", _arraySize);
            out << STRLIT(" ARRAYSIZE=\"") << buffer;
            out.append('"');
        }

        if (!_classOrigin.isNull())
        {
            out << STRLIT(" CLASSORIGIN=\"") << _classOrigin;
            out.append('"');
        }

        if (_propagated != false)
        {
            out << STRLIT(" PROPAGATED=\"") << _toString(_propagated);
            out.append('"');
        }

        out << STRLIT(">\n");

        _qualifiers.toXml(out);

        XmlWriter::appendValueElement(out, _value);

        out << STRLIT("</PROPERTY.ARRAY>\n");
    }
    else if (_value.getType() == CIMTYPE_REFERENCE)
    {
        out << STRLIT("<PROPERTY.REFERENCE");

        out << STRLIT(" NAME=\"") << _name << STRLIT("\" ");

        if (!_referenceClassName.isNull())
        {
            out << STRLIT(" REFERENCECLASS=\"") << _referenceClassName;
            out.append('"');
        }

        if (!_classOrigin.isNull())
        {
            out << STRLIT(" CLASSORIGIN=\"") << _classOrigin;
            out.append('"');
        }

        if (_propagated != false)
        {
            out << STRLIT(" PROPAGATED=\"") << _toString(_propagated);
            out.append('"');
        }

        out << STRLIT(">\n");

        _qualifiers.toXml(out);

        XmlWriter::appendValueElement(out, _value);

        out << STRLIT("</PROPERTY.REFERENCE>\n");
    }
    else
    {
        out << STRLIT("<PROPERTY NAME=\"") << _name << STRLIT("\" ");

        if (!_classOrigin.isNull())
        {
            out << STRLIT(" CLASSORIGIN=\"") << _classOrigin;
            out.append('"');
        }

        if (_propagated != false)
        {
            out << STRLIT(" PROPAGATED=\"") << _toString(_propagated);
            out.append('"');
        }

        if (_value.getType() == CIMTYPE_OBJECT)
        {
            // If the property type is CIMObject, then
            //   encode the property in CIM-XML as a string with the
            //   EMBEDDEDOBJECT attribute (there is not currently a CIM-XML
            //   "object" datatype)

            CIMObject a;
            _value.get(a);
            out << STRLIT(" TYPE=\"string\"");

            // If the Embedded Object is an instance, always add the
            // EMBEDDEDOBJECT attribute.
            if (a.isInstance())
                out << STRLIT(" EMBEDDEDOBJECT=\"object\"");
            // Else the Embedded Object is a class, always add the
            // EmbeddedObject qualifier.
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
            else
            {
#endif
                // Note that if the macro PEGASUS_SNIA_INTEROP_COMPATIBILITY
                // is defined, then the EmbeddedObject qualifier will always
                // be added, whether it's a class or an instance.
                if (_qualifiers.find(CIMName("EmbeddedObject")) ==
                    PEG_NOT_FOUND)
                {
                    // Note that _qualifiers is not defined as const, and
                    // neither is add(), but this method toXml() *is* const.
                    // However, in this case we really do want to add the
                    // EmbeddedObject qualifier, so we cast away the implied
                    // const-ness of _qualifiers.
                    CIMQualifierList * tmpQualifiers =
                        const_cast<CIMQualifierList *>(&_qualifiers);
                    tmpQualifiers->add(
                        CIMQualifier(CIMName("EmbeddedObject"), true));
                }
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
            }
#endif
        }
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        else if (_value.getType() == CIMTYPE_INSTANCE)
        {
            CIMInstance a;
            _value.get(a);
            out << " TYPE=\"string\"";
            out << " EMBEDDEDOBJECT=\"instance\"";

#ifdef PEGASUS_SNIA_INTEROP_COMPATIBILITY
            if (_qualifiers.find(CIMName("EmbeddedObject")) == PEG_NOT_FOUND)
            {
                // Note that _qualifiers is not defined as const, and neither
                // is add(), but this method toXml() *is* const. However, in
                // this case we really do want to add the EmbeddedObject
                // qualifier, so we cast away the implied const-ness of
                // _qualifiers.
                CIMQualifierList * tmpQualifiers =
                    const_cast<CIMQualifierList *>(&_qualifiers);
                tmpQualifiers->add(
                  CIMQualifier(CIMName("EmbeddedInstance"),
                  a.getClassName.getString()));
            }
#endif
        }
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        else
        {
            out << STRLIT(" TYPE=\"") << cimTypeToString(_value.getType());
            out.append('"');
        }

        out << STRLIT(">\n");

        _qualifiers.toXml(out);

        XmlWriter::appendValueElement(out, _value);

        out << STRLIT("</PROPERTY>\n");
    }
}

/** toMof - returns the MOF for the CIM Property Object in the parameter.
    The MOF for property declaration in a class and value presentation in
    an instance are different.

    The BNF for the property Declaration MOF is:
    <pre>
    propertyDeclaration     =   [ qualifierList ] dataType propertyName
                                [ array ] [ defaultValue ] ";"

    array                   =   "[" [positiveDecimalValue] "]"

    defaultValue            =   "=" initializer
    </pre>
    Format with qualifiers on one line and declaration on another. Start
    with newline but none at the end.

    Note that instances have a different format that propertyDeclarations:
    instanceDeclaration = [ qualifiersList ] INSTANCE OF className | alias
         "["valueInitializer "]" ";"
    valueInitializer = [ qualifierList ] [ propertyName | referenceName ] "="
                       initializer ";"
*/
void CIMPropertyRep::toMof(Boolean isDeclaration, Buffer& out) const
{
    //Output the qualifier list
    if (_qualifiers.getCount())
        out.append('\n');
    _qualifiers.toMof(out);

    // Output the Type and name on a new line
    out << '\n';
    if (isDeclaration)
    {
        out << cimTypeToString(_value.getType ());
        out.append(' ');
    }
    out << _name;

    // If array put the Array indicator "[]" and possible size after name.
    if (isDeclaration)
    {
        if (_value.isArray())
        {
            if (_arraySize)
            {
                char buffer[32];
                int n = sprintf(buffer, "[%d]", _arraySize);
                out.append(buffer, n);
            }
            else
                out << STRLIT("[]");
        }
    }

    // If the property value is not Null, add value after "="
    if (!_value.isNull())
    {
        out << STRLIT(" = ");
        if (_value.isArray())
        {
            // Insert any property values
            MofWriter::appendValueElement(out, _value);
        }
        else if (_value.getType() == CIMTYPE_REFERENCE)
        {
            MofWriter::appendValueElement(out, _value);
        }
        else
        {
            MofWriter::appendValueElement(out, _value);
        }
    }
    else if (!isDeclaration)
            out << STRLIT(" = NULL");

    // Close the property MOF
    out.append(';');

}

Boolean CIMPropertyRep::identical(const CIMPropertyRep* x) const
{
    if (!_name.equal (x->_name))
        return false;

    if (_value != x->_value)
        return false;

    if (!_referenceClassName.equal (x->_referenceClassName))
        return false;

    if (!_qualifiers.identical(x->_qualifiers))
        return false;

    if (!_classOrigin.equal (x->_classOrigin))
        return false;

    if (_propagated != x->_propagated)
        return false;

    return true;
}

void CIMPropertyRep::setValue(const CIMValue& value)
{
    // CIMType of value is immutable:

    if (!value.typeCompatible(_value))
        throw TypeMismatchException();

    if (_arraySize && _arraySize != value.getArraySize())
        throw TypeMismatchException();

    // A CIM Property may not be of reference array type
    if (value.isArray() && (value.getType() == CIMTYPE_REFERENCE))
    {
        throw TypeMismatchException();
    }

    _value = value;
}

PEGASUS_NAMESPACE_END
