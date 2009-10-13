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

#include <Pegasus/Common/Config.h>
#include <cstdlib>
#include <cstdio>
#include <Pegasus/Common/SCMOXmlWriter.h>

PEGASUS_NAMESPACE_BEGIN

void SCMOXmlWriter::appendValueSCMOInstanceElement(
    Buffer& out,
    const SCMOInstance& scmoInstance)
{
    out << STRLIT("<VALUE.NAMEDINSTANCE>\n");

    appendInstanceNameElement(out, scmoInstance);
    appendInstanceElement(out, scmoInstance);

    out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
}

void SCMOXmlWriter::appendInstanceNameElement(
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

        out << STRLIT("<KEYBINDING NAME=\"");
        out.append(kbName,kbNameLen-1);
        out << STRLIT("\">\n");

        if (kbType == CIMTYPE_REFERENCE)
        {
            /*
            TODO: NEED RESOLvE the value down to a SCMO....
            CIMObjectPath ref = keyBindings[i].getValue();
            appendValueReferenceElement(out, ref, true);
            */
        }
        else
        {
            out << STRLIT("<KEYVALUE VALUETYPE=\"");
            out << xmlWriterKeyTypeStrings(kbType);
            out << STRLIT("\">");

            SCMOXmlWriter::appendSCMBUnion(
                out,
                *kbValue,
                kbType,
                scmoInstance.inst.base);

            out << STRLIT("</KEYVALUE>\n");
        }
        out << STRLIT("</KEYBINDING>\n");
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
void SCMOXmlWriter::appendInstanceElement(
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
    for (Uint32 i=0,k=scmoInstance.getPropertyCount();i<k;i++)
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

void SCMOXmlWriter::appendQualifierElement(
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
    SCMOXmlWriter::appendValueElement(out, theQualifier.value, base);

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

void SCMOXmlWriter::appendPropertyElement(
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
        out << STRLIT("<PROPERTY.ARRAY NAME=\"");

        out.append(
            &(clsbase[propertyDef->name.start]),
            propertyDef->name.length-1);

        out << STRLIT("\" ");
        if (propertyType == CIMTYPE_OBJECT)
        {
/*          TODO: Implement writing CIM_OBJECT
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
*/            
        }
        else if (propertyType == CIMTYPE_INSTANCE)
        {
/*          TODO: Implement writing embedded instance
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
*/            
        }
        else
        {        
            out.append(' ');
            out << xmlWriterTypeStrings(propertyType);
        }
        
        Uint32 arraySize=propertyValue->valueArraySize;

        if (0 != arraySize)
        {
            out << STRLIT(" ARRAYSIZE=\"");
            SCMOXmlWriter::append(out, arraySize);
            out.append('"');
        }


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

        SCMOXmlWriter::appendValueElement(out,*propertyValue,propertyValueBase);

        out << STRLIT("</PROPERTY.ARRAY>\n");
    }
    else if (propertyType == CIMTYPE_REFERENCE)
    {
        out << STRLIT("<PROPERTY.REFERENCE NAME=\""); 
        out.append(
            &(clsbase[propertyDef->name.start]),
            propertyDef->name.length-1);
        out << STRLIT("\" ");

        if (0 != propertyDef->refClassName.start)
        {
            out << STRLIT(" REFERENCECLASS=\"");
            out.append(
                &(clsbase[propertyDef->refClassName.start]),
                propertyDef->refClassName.length-1);
            out.append('"');
        }

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
        SCMOXmlWriter::appendValueElement(out,*propertyValue,propertyValueBase);
        out << STRLIT("</PROPERTY.REFERENCE>\n");
    }
    else
    {
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
/*          TODO: Implement writing Embedded Object
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
/*              TODO: Implement writing Embedded Instance
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
        SCMOXmlWriter::appendValueElement(out,*propertyValue,propertyValueBase);
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

void SCMOXmlWriter::appendValueElement(
    Buffer& out,
    const SCMBValue & value,
    const char * base)
{
    if (value.flags.isNull)
    {
        return;
    }
    if (value.flags.isArray)
    {
        appendSCMBUnionArray(
            out,
            value.value,
            value.valueType,
            value.valueArraySize,
            base);
    }
    else if (value.valueType == CIMTYPE_REFERENCE)
    {
/*      TODO: Implement writing an object path as value
        // Has to be separate because it uses VALUE.REFERENCE tag
        CIMObjectPath v;
        value.get(v);
        _xmlWritter_appendValue(out, v);
        ----> same as: XmlWriter::appendValueReferenceElement(out, x, true);
*/
    }
    else
    {
        out << STRLIT("<VALUE>");

        // Call function to write a SCMBUnion + type field
        appendSCMBUnion(out,value.value, value.valueType,base);

        out << STRLIT("</VALUE>\n");
    }
}

//------------------------------------------------------------------------------
//
// appendValueReferenceElement()
//
//    <!ELEMENT VALUE.REFERENCE
//        (CLASSPATH|LOCALCLASSPATH|CLASSNAME|INSTANCEPATH|LOCALINSTANCEPATH|
//         INSTANCENAME)>
//
//------------------------------------------------------------------------------

void SCMOXmlWriter::appendValueReferenceElement(
    Buffer& out,
    const SCMOInstance& ref,
    Boolean putValueWrapper)
{
    if (putValueWrapper)
        out << STRLIT("<VALUE.REFERENCE>\n");

    // See if it is a class or instance reference (instance references have
    // key-bindings; class references do not).

    // differentiate between instance and class using the SCMO flag
    if (ref.inst.hdr->flags.isClassOnly)
    {
        // class
        if (0 != ref.inst.hdr->hostName.start)
        {
            appendClassPathElement(out, ref);
        }
/*      TODO: Implement
        else if (!reference.getNameSpace().isNull())
        {
            appendLocalClassPathElement(out, reference);
        }
        else
        {
            appendClassNameElement(out, reference.getClassName());
        }
*/
    }
    else
    {
        // instance
        if (0 != ref.inst.hdr->hostName.start)
        {
            // appendInstancePathElement(out, reference);
        }
/*      TODO: Implement
        else if (!reference.getNameSpace().isNull())
        {
            appendLocalInstancePathElement(out, reference);
        }
        else
        {
            appendInstanceNameElement(out, reference);
        }
*/
    }
    if (putValueWrapper)
        out << STRLIT("</VALUE.REFERENCE>\n");
}

//------------------------------------------------------------------------------
//
// appendClassPathElement()
//
//     <!ELEMENT CLASSPATH (NAMESPACEPATH,CLASSNAME)>
//
//------------------------------------------------------------------------------

void SCMOXmlWriter::appendClassPathElement(
    Buffer& out,
    const SCMOInstance& classPath)
{
    out << STRLIT("<CLASSPATH>\n");

    Uint64 hostnameLength=0;
    const char* hostname=classPath.getHostName_l(hostnameLength);
    Uint64 nsLength=0;
    const char* ns=classPath.getNameSpace_l(nsLength);

    appendNameSpacePathElement(out,hostname,hostnameLength,ns,nsLength);

    Uint64 classNameLength=0;
    const char* className = classPath.getClassName_l(classNameLength);

    appendClassNameElement(out, className, classNameLength-1);
    out << STRLIT("</CLASSPATH>\n");
}

void SCMOXmlWriter::appendSCMBUnion(
    Buffer& out,
    const SCMBUnion & u,
    const CIMType & valueType,
    const char * base)
{
    switch (valueType)
    {
        case CIMTYPE_BOOLEAN:
        {
            SCMOXmlWriter::append(out, u.simple.val.bin);
            break;
        }

        case CIMTYPE_UINT8:
        {
            SCMOXmlWriter::append(out, u.simple.val.u8);
            break;
        }

        case CIMTYPE_SINT8:
        {
            SCMOXmlWriter::append(out, u.simple.val.s8);
            break;
        }

        case CIMTYPE_UINT16:
        {
            SCMOXmlWriter::append(out, u.simple.val.u16);
            break;
        }

        case CIMTYPE_SINT16:
        {
            SCMOXmlWriter::append(out, u.simple.val.s16);
            break;
        }

        case CIMTYPE_UINT32:
        {
            SCMOXmlWriter::append(out, u.simple.val.u32);
            break;
        }

        case CIMTYPE_SINT32:
        {
            SCMOXmlWriter::append(out, u.simple.val.s32);
            break;
        }

        case CIMTYPE_UINT64:
        {
            SCMOXmlWriter::append(out, u.simple.val.u64);
            break;
        }

        case CIMTYPE_SINT64:
        {
            SCMOXmlWriter::append(out, u.simple.val.s64);
            break;
        }

        case CIMTYPE_REAL32:
        {
            SCMOXmlWriter::append(out, u.simple.val.r32);
            break;
        }

        case CIMTYPE_REAL64:
        {
            SCMOXmlWriter::append(out, u.simple.val.r64);
            break;
        }

        case CIMTYPE_CHAR16:
        {
            SCMOXmlWriter::appendSpecial(
                out,
                u.simple.val.c16);
            break;
        }

        case CIMTYPE_STRING:
        {
            SCMOXmlWriter::appendSpecial(
                out,
                &(base[u.stringValue.start]),
                u.stringValue.length-1);
            break;
        }

        case CIMTYPE_DATETIME:
        {
            // an SCMBDateTime is a CIMDateTimeRep
            // this should help us to reuse existing optimized Datetime
            char buffer[26];
            _DateTimetoCStr(&(u.dateTimeValue), buffer);
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
}

void SCMOXmlWriter::appendSCMBUnionArray(
    Buffer& out,
    const SCMBUnion & u,
    const CIMType & valueType,
    Uint32 numElements,
    const char * base)
{
    SCMBUnion* arr = (SCMBUnion*) &(base[u.arrayValue.start]);
    switch (valueType)
    {
        case CIMTYPE_BOOLEAN:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.bin);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
        case CIMTYPE_UINT8:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.u8);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_SINT8:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.s8);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_UINT16:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.u16);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_SINT16:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.s16);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_UINT32:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.u32);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_SINT32:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.s32);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_UINT64:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.u64);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_SINT64:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.s64);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_REAL32:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.r32);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_REAL64:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.r64);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_CHAR16:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::append(out, arr->simple.val.bin);
                SCMOXmlWriter::appendSpecial(out, arr->simple.val.c16);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            break;
        }
    
        case CIMTYPE_STRING:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                SCMOXmlWriter::appendSpecial(
                    out,
                    &(base[arr->stringValue.start]),
                    arr->stringValue.length-1);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");
            
            break;
        }
    
        case CIMTYPE_DATETIME:
        {
            out << STRLIT("<VALUE.ARRAY>\n");
            char buffer[26];
            while (numElements--)
            {
                out << STRLIT("<VALUE>");
                // an SCMBDateTime is a CIMDateTimeRep
                // this should help us to reuse existing optimized Datetime
                _DateTimetoCStr(&(arr->dateTimeValue), buffer);
                // datetime value is formatted with a \0 at end, ignore
                out.append(buffer,sizeof(buffer)-1);
                arr++;
                out << STRLIT("</VALUE>\n");
            }
            out << STRLIT("</VALUE.ARRAY>\n");        
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

}



PEGASUS_NAMESPACE_END
