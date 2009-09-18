//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SCMOXmlWriter_h
#define Pegasus_SCMOXmlWriter_h

#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/XmlGenerator.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/SCMO.h>
#include <Pegasus/Common/SCMOInstance.h>
#include <Pegasus/Common/CIMDateTimeInline.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE SCMOXmlWriter : public XmlWriter
{
public:

    static void appendValueSCMOInstanceElement(
        Buffer& out,
        const SCMOInstance& scmoInstance)
    {
        out << STRLIT("<VALUE.NAMEDINSTANCE>\n");

        appendInstanceNameElement(out, scmoInstance);
        appendInstanceElement(out, scmoInstance);

        out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
    }

    static void appendInstanceNameElement(
        Buffer& out,
        const SCMOInstance& scmoInstance)
    {
        out << STRLIT("<INSTANCENAME CLASSNAME=\"");
        Uint64 len;
        const char * className = scmoInstance.getClassName_l(len);
        out.append(className,len-1);
        out << STRLIT("\">\n");

        for (Uint32 i = 0, n = scmoInstance.getKeyBindingCount(); i < n; i++)
        {
            const char * kbName;
            const SCMBUnion * kbValue;
            CIMType kbType;
            Uint32 kbNameLen;

            scmoInstance._getKeyBindingDataAtNodeIndex(
                i,
                &kbName,
                kbNameLen,
                kbType,
                &kbValue);

            /* TODO: NEED to handle new _getKeyBindingDataAtNodeIndex()
            out << STRLIT("<KEYBINDING NAME=\"");
            out.append(kbName,kbNameLen-1);
            out << STRLIT("\">\n");

            if (kbType == CIMKeyBinding::REFERENCE)
            {
                TODO: NEED RESOLvE the value down to a SCMO....
                CIMObjectPath ref = keyBindings[i].getValue();
                appendValueReferenceElement(out, ref, true);
            }
            else
            {
                out << STRLIT("<KEYVALUE VALUETYPE=\"");
                out << keyBindingTypeToString(kbType);
                out << STRLIT("\">");

                // fixed the special character problem - Markus

                appendSpecial(out, kbValue, kbValueLen-1);
                out << STRLIT("</KEYVALUE>\n");
            }
            out << STRLIT("</KEYBINDING>\n");
            */
        }
        out << STRLIT("</INSTANCENAME>\n");
    }

//------------------------------------------------------------------------------
//
// appendInstanceElement()
//
//     <!ELEMENT INSTANCE
//         (QUALIFIER*,(PROPERTY|PROPERTY.ARRAY|PROPERTY.REFERENCE)*)>
//     <!ATTLIST INSTANCE
//         %ClassName;>
//
//------------------------------------------------------------------------------
    static void appendInstanceElement(
        Buffer& out,
        const SCMOInstance& scmoInstance)
    {
        // Class opening element:

        out << STRLIT("<INSTANCE CLASSNAME=\"");
        Uint64 len;
        const char * className = scmoInstance.getClassName_l(len);
        out.append(className,len-1);
        out << STRLIT("\" >\n");

        // Append Instance Qualifiers:
        if (scmoInstance.inst.hdr->flags.includeQualifiers)
        {
            SCMBClass_Main *classMain=scmoInstance.inst.hdr->theClass->cls.hdr;
            char* clsbase = scmoInstance.inst.hdr->theClass->cls.base;

            SCMBQualifier *theArray =
                (SCMBQualifier*)&(clsbase[classMain->qualifierArray.start]);
            // need to iterate
            for (Uint32 i=0, n=classMain->numberOfQualifiers;i<n;i++)
            {
                SCMOXmlWriter::appendQualifierElement(out,theArray[i],clsbase);
            }
        }

        // Append Properties:
        for (Uint32 i=0,k=scmoInstance.inst.hdr->numberProperties;i<k;i++)
        {
            if (scmoInstance.inst.hdr->flags.isFiltered &&
                !scmoInstance._isPropertyInFilter(i))
            {
                // Property is filtered, ignore and go to next
                continue;
            }
            else
            {
                SCMOXmlWriter::appendPropertyElement(out,scmoInstance,i);
            }
        }
        // Instance closing element:
        out << STRLIT("</INSTANCE>\n");
    }



//------------------------------------------------------------------------------
//
// appendQualifierElement()
//
//     <!ELEMENT QUALIFIER (VALUE|VALUE.ARRAY)>
//     <!ATTLIST QUALIFIER
//              %CIMName;
//              %CIMType;               #REQUIRED
//              %Propagated;
//              %QualifierFlavor;>
//
//------------------------------------------------------------------------------

    static void appendQualifierElement(
        Buffer& out,
        const SCMBQualifier& theQualifier,
        const char* base)
    {
        out << STRLIT("<QUALIFIER NAME=\"");
        if(theQualifier.name == QUALNAME_USERDEFINED)
        {
            if (theQualifier.userDefName.start)
            {
                out.append(
                    &(base[theQualifier.userDefName.start]),
                    theQualifier.userDefName.length-1);
            }
        }
        else
        {
            out << SCMOClass::qualifierNameStrLit(theQualifier.name);
        }

        out << STRLIT("\" ");

        // Append type
        out << xmlWriterTypeStrings(theQualifier.value.valueType);
        // append PROPAGATED if set
        if (theQualifier.propagated)
        {
            out << STRLIT(" PROPAGATED=\"true\"");
        }
        // append flavor of the qualifier
        SCMOXmlWriter::appendQualifierFlavorEntity(
            out,
            CIMFlavor(theQualifier.flavor));

        out << STRLIT(">\n");
        // append the value of the qualifier

        // TODO: Implement and call appendValueElement
        //SCMOXmlWriter::appendValueElement(out, theQualifier.value, clsbase);

        out << STRLIT("</QUALIFIER>\n");
    }

//------------------------------------------------------------------------------
//
// appendPropertyElement()
//
//     <!ELEMENT PROPERTY (QUALIFIER*,VALUE?)>
//     <!ATTLIST PROPERTY
//              %CIMName;
//              %CIMType;           #REQUIRED
//              %ClassOrigin;
//              %Propagated;>
//
//     <!ELEMENT PROPERTY.ARRAY (QUALIFIER*,VALUE.ARRAY?)>
//     <!ATTLIST PROPERTY.ARRAY
//              %CIMName;
//              %CIMType;           #REQUIRED
//              %ArraySize;
//              %ClassOrigin;
//              %Propagated;>
//
//     <!ELEMENT PROPERTY.REFERENCE (QUALIFIER*,VALUE.REFERENCE?)>
//     <!ATTLIST PROPERTY.REFERENCE
//              %CIMName;
//              %ReferenceClass;
//              %ClassOrigin;
//              %Propagated;>
//
//------------------------------------------------------------------------------

    static void appendPropertyElement(
        Buffer& out,
        const SCMOInstance& scmoInstance,
        Uint32 pos)
    {
        // Get most of the property data from the instance
        const char* propertyName;
        Uint32 propertyNameLen;
        CIMType propertyType;

        // This is an absolute pointer at a SCMBValue
        SCMBValue * propertyValue;
        const char * propertyValueBase;
        // This is an absolute pointer at a SCMBValue
        SCMBClassProperty * propertyDef;
        // This is the absolute pointer at which the class info for the given
        // instance starts
        const char* clsbase = scmoInstance.inst.hdr->theClass->cls.base;

        SCMO_RC rc = scmoInstance.getPropertyAt(
            pos,
            &propertyValue,
            &propertyValueBase,
            &propertyDef);

        propertyType = propertyValue->valueType;


        if (propertyValue->flags.isArray)
        {
/*
            out << STRLIT("<PROPERTY.ARRAY NAME=\"")
                << rep->getName()
                << STRLIT("\" ");

            if (propertyType == CIMTYPE_OBJECT)
            {
                // If the property array type is CIMObject, then
                //    encode the property in CIM-XML as a string array with the
                //    EmbeddedObject attribute (there is not currently a CIM-XML
                //    "object" datatype)

                Array<CIMObject> a;
                rep->getValue().get(a);
                out << STRLIT(" TYPE=\"string\"");
                // If the Embedded Object is an instance, always add the
                // EmbeddedObject attribute.
                if (a.size() > 0 && a[0].isInstance())
                {
                    out << STRLIT(" EmbeddedObject=\"object\""
                                  " EMBEDDEDOBJECT=\"object\"");
                }
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
                else
#endif
                {
                    // Else the Embedded Object is a class, always add the
                    // EmbeddedObject qualifier.  Note that if the macro
                    // PEGASUS_SNIA_INTEROP_COMPATIBILITY is defined, then
                    // the EmbeddedObject qualifier will always be added,
                    // whether it's a class or an instance.
                    if (rep->findQualifier(PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT)
                       == PEG_NOT_FOUND)
                    {
                        // Note that addQualifiers() cannot be called on a const
                        // CIMQualifierRep.In this case we really do want to add
                        // the EmbeddedObject qualifier, so we cast away the
                        // constness.
                        CIMPropertyRep* tmpRep=const_cast<CIMPropertyRep*>(rep);
                        tmpRep->addQualifier(
                            CIMQualifier(PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT,
                                         true));
                    }
                }
            }
            else if (propertyType == CIMTYPE_INSTANCE)
            {
                // If the property array type is CIMInstance, then
                //   encode the property in CIM-XML as a string array with the
                //   EmbeddedObject attribute (there is not currently a CIM-XML
                //   "instance" datatype)

                Array<CIMInstance> a;
                rep->getValue().get(a);
                out << STRLIT(" TYPE=\"string\"");

                // add the EmbeddedObject attribute
                if (a.size() > 0)
                {
                    out << STRLIT(" EmbeddedObject=\"instance\""
                                  " EMBEDDEDOBJECT=\"instance\"");

                    // Note that if the macro PEGASUS_SNIA_INTEROP_COMPATIBILITY
                    // is defined, then the EmbeddedInstance qualifier will be
                    // added
# ifdef PEGASUS_SNIA_INTEROP_COMPATIBILITY
                    if (rep->findQualifier(
                            PEGASUS_QUALIFIERNAME_EMBEDDEDINSTANCE ==
                            PEG_NOT_FOUND)
                    {
                        // Note that addQualifiers() cannot be called on a const
                        // CIMQualifierRep.In this case we really do want to add
                        // the EmbeddedInstance qualifier, so we cast away the
                        // constness.

                        // For now, we assume that all the embedded instances in
                        // the array are of the same type
                        CIMPropertyRep* tmpRep=const_cast<CIMPropertyRep*>(rep);
                        tmpRep->addQualifier(CIMQualifier(
                            PEGASUS_QUALIFIERNAME_EMBEDDEDINSTANCE,
                            a[0].getClassName().getString()));
                    }
# endif
                }
            }
            else
            {
                out.append(' ');
                out << xmlWriterTypeStrings(rep->getValue().getType());
            }

            if (rep->getArraySize())
            {
                char buffer[32];
                sprintf(buffer, "%u", rep->getArraySize());
                out << STRLIT(" ARRAYSIZE=\"") << buffer;
                out.append('"');
            }

            if (!rep->getClassOrigin().isNull())
            {
                out << STRLIT(" CLASSORIGIN=\"") << rep->getClassOrigin();
                out.append('"');
            }

            if (rep->getPropagated())
            {
                out << STRLIT(" PROPAGATED=\"true\"");
            }

            out << STRLIT(">\n");

            for (Uint32 i = 0, n = rep->getQualifierCount(); i < n; i++)
                XmlWriter::appendQualifierElement(out, rep->getQualifier(i));

            XmlWriter::appendValueElement(out, rep->getValue());

            out << STRLIT("</PROPERTY.ARRAY>\n");
*/
        }
        else if (propertyType == CIMTYPE_REFERENCE)
        {
/*
            out << STRLIT("<PROPERTY.REFERENCE"
                          " NAME=\"") << rep->getName() << STRLIT("\" ");

            if (!rep->getReferenceClassName().isNull())
            {
                out << STRLIT(" REFERENCECLASS=\"")
                    << rep->getReferenceClassName();
                out.append('"');
            }

            if (!rep->getClassOrigin().isNull())
            {
                out << STRLIT(" CLASSORIGIN=\"") << rep->getClassOrigin();
                out.append('"');
            }

            if (rep->getPropagated())
            {
                out << STRLIT(" PROPAGATED=\"true\"");
            }

            out << STRLIT(">\n");

            for (Uint32 i = 0, n = rep->getQualifierCount(); i < n; i++)
                XmlWriter::appendQualifierElement(out, rep->getQualifier(i));

            XmlWriter::appendValueElement(out, rep->getValue());

            out << STRLIT("</PROPERTY.REFERENCE>\n");
*/
        }
        else
        {
            // TODO: Optimize the property name using length as input
            out << STRLIT("<PROPERTY NAME=\"");

            out.append(
                &(clsbase[propertyDef->name.start]),
                propertyDef->name.length-1);

            out << STRLIT("\" ");

            if (scmoInstance.inst.hdr->flags.includeClassOrigin)
            {
                if (propertyDef->originClassName.start != 0)
                {
                    out << STRLIT(" CLASSORIGIN=\"");
                    out.append(
                        &(clsbase[propertyDef->originClassName.start]),
                        propertyDef->originClassName.length-1);
                    out.append('"');
                }
            }
            if (propertyDef->flags.propagated)
            {
                out << STRLIT(" PROPAGATED=\"true\"");
            }

            if (propertyType == CIMTYPE_OBJECT)
            {
/*
                // If the property type is CIMObject, then
                //   encode the property in CIM-XML as a string with the
                //   EmbeddedObject attribute (there is not currently a CIM-XML
                //   "object" datatype)

                CIMObject a;
                rep->getValue().get(a);
                out << STRLIT(" TYPE=\"string\"");

                // If the Embedded Object is an instance, always add the
                // EmbeddedObject attribute.
                if (a.isInstance())
                {
                    out << STRLIT(" EmbeddedObject=\"object\""
                                  " EMBEDDEDOBJECT=\"object\"");
                }
                // Else the Embedded Object is a class, always add the
                // EmbeddedObject qualifier.
#ifndef PEGASUS_SNIA_INTEROP_COMPATIBILITY
                else
#endif
                {
                    // Note that if the macro PEGASUS_SNIA_INTEROP_COMPATIBILITY
                    // is defined, then the EmbeddedObject qualifier will always
                    // be added, whether it's a class or an instance.
                    if (rep->findQualifier(PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT)
                        == PEG_NOT_FOUND)
                    {
                        // Note that addQualifiers() cannot be called on a const
                        // CIMQualifierRep.  In this case we really do want to
                        // add the EmbeddedObject qualifier, so we cast away the
                        // constness.
                        CIMPropertyRep* tmpRep=const_cast<CIMPropertyRep*>(rep);
                        tmpRep->addQualifier(
                            CIMQualifier(PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT,
                                         true));
                    }
                }
*/
            }
            else if (propertyType == CIMTYPE_INSTANCE)
            {
/*
                CIMInstance a;
                rep->getValue().get(a);
                out << STRLIT(" TYPE=\"string\""
                              " EmbeddedObject=\"instance\""
                              " EMBEDDEDOBJECT=\"instance\"");

# ifdef PEGASUS_SNIA_INTEROP_COMPATIBILITY
                if (rep->findQualifier(PEGASUS_QUALIFIERNAME_EMBEDDEDOBJECT)
                    == PEG_NOT_FOUND)
                {
                    // Note that addQualifiers() cannot be called on a const
                    // CIMQualifierRep.  In this case we really do want to add
                    // the EmbeddedInstance qualifier, so we cast away the
                    // constness.
                    CIMPropertyRep* tmpRep = const_cast<CIMPropertyRep*>(rep);
                    tmpRep->addQualifier(CIMQualifier(
                        PEGASUS_QUALIFIERNAME_EMBEDDEDINSTANCE,
                        a.getClassName().getString()));
                }
# endif
*/
            }
            else
            {
                out.append(' ');
                out << xmlWriterTypeStrings(propertyType);
            }
            out << STRLIT(">\n");

            // Append Instance Qualifiers:
            if (scmoInstance.inst.hdr->flags.includeQualifiers)
            {
                SCMBQualifier * theArray=
                    (SCMBQualifier*)
                        &(clsbase[propertyDef->qualifierArray.start]);
                // need to iterate
                for (Uint32 i=0, n=propertyDef->numberOfQualifiers;i<n;i++)
                {
                    SCMOXmlWriter::appendQualifierElement(
                        out,
                        theArray[i],
                        clsbase);
                }
            }
            SCMOXmlWriter::appendValueElement(
                out,
                propertyValue,
                propertyValueBase);

            out << STRLIT("</PROPERTY>\n");
        }
    }
//------------------------------------------------------------------------------
//
// appendValueElement()
//
//    <!ELEMENT VALUE (#PCDATA)>
//    <!ELEMENT VALUE.ARRAY (VALUE*)>
//    <!ELEMENT VALUE.REFERENCE
//        (CLASSPATH|LOCALCLASSPATH|CLASSNAME|INSTANCEPATH|LOCALINSTANCEPATH|
//         INSTANCENAME)>
//    <!ELEMENT VALUE.REFARRAY (VALUE.REFERENCE*)>
//
//------------------------------------------------------------------------------

    static void appendValueElement(
        Buffer& out,
        SCMBValue * value,
        const char * base)
    {
        if (value->flags.isNull)
        {
            return;
        }
        if (value->flags.isArray)
        {
/*
            switch (value.getType())
            {
                case CIMTYPE_BOOLEAN:
                {
                    Array<Boolean> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_UINT8:
                {
                    Array<Uint8> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_SINT8:
                {
                    Array<Sint8> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_UINT16:
                {
                    Array<Uint16> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_SINT16:
                {
                    Array<Sint16> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_UINT32:
                {
                    Array<Uint32> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_SINT32:
                {
                    Array<Sint32> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_UINT64:
                {
                    Array<Uint64> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_SINT64:
                {
                    Array<Sint64> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_REAL32:
                {
                    Array<Real32> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_REAL64:
                {
                    Array<Real64> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_CHAR16:
                {
                    Array<Char16> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_STRING:
                {
                    const String* data;
                    Uint32 size;
                    value._get(data, size);
                    _xmlWritter_appendValueArray(out, data, size);
                    break;
                }

                case CIMTYPE_DATETIME:
                {
                    Array<CIMDateTime> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_REFERENCE:
                {
                    Array<CIMObjectPath> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }

                case CIMTYPE_OBJECT:
                {
                    Array<CIMObject> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }
                case CIMTYPE_INSTANCE:
                {
                    Array<CIMInstance> a;
                    value.get(a);
                    _xmlWritter_appendValueArray(out, a.getData(), a.size());
                    break;
                }
                default:
                    PEGASUS_ASSERT(false);
            }
*/
        }
        else if (value->valueType == CIMTYPE_REFERENCE)
        {
/*
            // Has to be separate because it uses VALUE.REFERENCE tag
            CIMObjectPath v;
            value.get(v);
            _xmlWritter_appendValue(out, v);
*/
        }
        else
        {
            out << STRLIT("<VALUE>");

            switch (value->valueType)
            {
                case CIMTYPE_BOOLEAN:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.bin);
                    break;
                }

                case CIMTYPE_UINT8:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.u8);
                    break;
                }

                case CIMTYPE_SINT8:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.s8);
                    break;
                }

                case CIMTYPE_UINT16:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.u16);
                    break;
                }

                case CIMTYPE_SINT16:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.s16);
                    break;
                }

                case CIMTYPE_UINT32:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.u32);
                    break;
                }

                case CIMTYPE_SINT32:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.s32);
                    break;
                }

                case CIMTYPE_UINT64:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.u64);
                    break;
                }

                case CIMTYPE_SINT64:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.s64);
                    break;
                }

                case CIMTYPE_REAL32:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.r32);
                    break;
                }

                case CIMTYPE_REAL64:
                {
                    SCMOXmlWriter::append(out, value->value.simple.val.r64);
                    break;
                }

                case CIMTYPE_CHAR16:
                {
                    SCMOXmlWriter::appendSpecial(
                        out,
                        value->value.simple.val.c16);
                    break;
                }

                case CIMTYPE_STRING:
                {
                    SCMOXmlWriter::appendSpecial(
                        out,
                        &(base[value->value.stringValue.start]),
                        value->value.stringValue.length-1);
                    break;
                }

                case CIMTYPE_DATETIME:
                {
                    // an SCMBDateTime is a CIMDateTimeRep
                    // this should help us to reuse existing optimized Datetime
                    char buffer[26];
                    _DateTimetoCStr(&(value->value.dateTimeValue), buffer);
                    // datetime value is formatted with a \0 at end, ignore
                    out.append(buffer,sizeof(buffer)-1);
                    break;
                }
/*
                case CIMTYPE_OBJECT:
                {
                    CIMObject v;
                    value.get(v);
                    _xmlWritter_appendValue(out, v);
                    break;
                }
                case CIMTYPE_INSTANCE:
                {
                    CIMInstance v;
                    value.get(v);
                    _xmlWritter_appendValue(out, v);
                    break;
                }
                default:
                    PEGASUS_ASSERT(false);
*/
            }
            out << STRLIT("</VALUE>\n");
        }
    }

private:


    SCMOXmlWriter();
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SCMOXmlWriter_h */
