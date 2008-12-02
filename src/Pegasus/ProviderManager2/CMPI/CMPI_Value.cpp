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

#include "CMPI_Version.h"

#include "CMPI_String.h"
#include "CMPI_Value.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define CopyToArray(pt,ct) \
    { \
        Array<pt> ar##pt(aSize); \
        for (int i=0; i<aSize; i++) \
        { \
            ar##pt[i]=aData[i].value.ct; \
        } \
        v.set(ar##pt); \
    }

#define CopyToStringArray(pt,ct) \
    { \
        Array<pt> ar##pt(aSize); \
        for (int i=0; i<aSize; i++) \
        { \
            ar##pt[i]=String(((char*)aData[i].value.ct)); \
        } \
        v.set(ar##pt); \
    }

#define CopyCharsptrToStringArray(pt,ct) \
    { \
        Array<pt> ar##pt(aSize); \
        for (int i=0; i<aSize; i++) \
        { \
            ar##pt[i]=String((*(char**)aData[i].value.ct)); \
        }\
        v.set(ar##pt); \
    }

#define CopyToEncArray(pt,ct) \
    { \
        Array<pt> ar##pt(aSize); \
        for (int i=0; i<aSize; i++) \
        { \
            ar##pt[i]=*((pt*)aData[i].value.ct->hdl); \
        } \
        v.set(ar##pt); \
    }

#define CopyFromArray(pt,ct) \
    { \
        Array<pt> ar##pt; \
        v.get(ar##pt); \
        for (int i=0; i<aSize; i++) \
        { \
            aData[i].value.ct=ar##pt[i]; \
        } \
    }

#define CopyFromStringArray(pt,ct) \
    { \
        Array<pt> ar##pt; \
        v.get(ar##pt); \
        for (int i=0; i<aSize; i++) \
        { \
             aData[i].value.ct=reinterpret_cast<CMPIString*>(\
                 new CMPI_Object(ar##pt[i])); \
        } \
    }

#define CopyFromEncArray(pt,ct,cn) \
    { \
        Array<pt> ar##pt; \
        v.get(ar##pt); \
        for (int i=0; i<aSize; i++) \
        { \
            aData[i].value.cn=reinterpret_cast<ct*>(\
                new CMPI_Object(new pt(ar##pt[i]))); \
        } \
    }

/**
  Function to convert CMPIValue to CIMValue
*/
CIMValue value2CIMValue(const CMPIValue* data, const CMPIType type, CMPIrc *rc)
{
    CIMValue v;
    if( rc )
    {
        *rc=CMPI_RC_OK;
    }

/**
   check data for NULL if type is not CMPIArray.
*/
    if( !(type & CMPI_ARRAY) && !data )
    {
        return CIMValue(type2CIMType(type), false);
    }
    /**
       Case when type is CMPIArray
   */
    if( type & CMPI_ARRAY )
    {
        //Return if data itself is NULL or Array is NULL
        if( data == NULL || data->array == NULL )
        {
            CMPIType aType=type&~CMPI_ARRAY;
            return CIMValue(type2CIMType(aType),true);
        }
        // When data is not NULL and data->array is also set
        CMPIArray *ar=data->array;
        CMPIData *aData=(CMPIData*)ar->hdl;

        //Get the type of the elements in the array
        CMPIType aType=aData->type&~CMPI_ARRAY;

        int aSize=aData->value.sint32;
        aData++;

        // Checks for Signed Integers
        if( (aType & (CMPI_UINT|CMPI_SINT))==CMPI_SINT )
        {
            switch( aType )
            {
                case CMPI_sint32:
                    CopyToArray(Sint32,sint32);
                    break;

                case CMPI_sint16:
                    CopyToArray(Sint16,sint16);
                    break;

                case CMPI_sint8:
                    CopyToArray(Sint8,sint8);
                    break;

                case CMPI_sint64:
                    CopyToArray(Sint64,sint64);
                    break;

                default: ;
            }
        }
        else
        if( aType == CMPI_chars )
        {
            CopyToStringArray(String,string->hdl)
        }
        else 
        if( aType == CMPI_charsptr )
        {
            CopyCharsptrToStringArray(String,chars)
        }
        else 
        if( aType == CMPI_string )
        {
            CopyToStringArray(String,string->hdl)
        }


        // Checks for Unsigned Integers
        else
        if( (aType & (CMPI_UINT|CMPI_SINT))==CMPI_UINT )
        {
            switch( aType )
            {
                case CMPI_uint32:
                    CopyToArray(Uint32,uint32);
                    break;
                case CMPI_uint16:
                    CopyToArray(Uint16,uint16);
                    break;
                case CMPI_uint8:
                    CopyToArray(Uint8,uint8);
                    break;
                case CMPI_uint64:
                    CopyToArray(Uint64,uint64);
                    break;
                default: ;
            }


        }
        else
        {
            switch( aType )
            {
                case CMPI_ref:
                    CopyToEncArray(CIMObjectPath,ref);
                    break;

                case CMPI_dateTime:
                    CopyToEncArray(CIMDateTime,dateTime);
                    break;
                case CMPI_instance:
                    CopyToEncArray(CIMObject,inst);
                    break;
                case CMPI_boolean:
                    CopyToArray(Boolean,boolean);
                    break;

                case CMPI_char16:
                    CopyToArray(Char16,char16);
                    break;

                case CMPI_real32:
                    CopyToArray(Real32,real32);
                    break;

                case CMPI_real64:
                    CopyToArray(Real64,real64);
                    break;

                default:
                    if( rc )
                    {
                        *rc=CMPI_RC_ERR_NOT_SUPPORTED;
                    }
            }
        }
        return CIMValue(v);
    } // end of array processing
    //Start of non - array types processing
    else
    if( type == CMPI_chars )
    {
        v.set(String((char*)data));
    }
    else
    if( type == CMPI_charsptr )
    {
        if( data && *(char**)data )
        {
            v.set(String(*(char**)data));
        }
        else
        {
            return CIMValue(CIMTYPE_STRING,false);
        }
    }

    //Checks for Signed integers
    else
    if( (type & (CMPI_UINT|CMPI_SINT))==CMPI_SINT )
    {
        switch( type )
        {
            case CMPI_sint32:
                v.set((Sint32)data->sint32);
                break;

            case CMPI_sint16:
                v.set((Sint16)data->sint16);
                break;

            case CMPI_sint8:
                v.set((Sint8)data->sint8);
                break;

            case CMPI_sint64:
                v.set((Sint64)data->sint64);
                break;
            default: ;
        }
    }
    else
    if( type == CMPI_string )
    {
        if( data->string && data->string->hdl )
        {
            v.set(String((char*)data->string->hdl));
        }
        else
        {
            return CIMValue(CIMTYPE_STRING,false);
        }
    }

    //Checks for Unsigned Integers
    else
    if( (type & (CMPI_UINT|CMPI_SINT))==CMPI_UINT )
    {
        switch( type )
        {
            case CMPI_uint32:
                v.set((Uint32)data->uint32);
                break;

            case CMPI_uint16:
                v.set((Uint16)data->uint16);
                break;

            case CMPI_uint8:
                v.set((Uint8)data->uint8);
                break;

            case CMPI_uint64:
                v.set((Uint64)data->uint64);
                break;

            default: ;
        }
    }

    //Checks for remaining Encapsulated and non-encapsulated types
    else
        switch( type )
        {
            case CMPI_instance:
                if( data->inst && data->inst->hdl )
                {
                    v.set(*((CIMObject*) data->inst->hdl));
                }
                else
                {
                    return CIMValue(CIMTYPE_OBJECT, false);
                }   
                break;

            case CMPI_ref:      
                if( data->ref && data->ref->hdl )
                {
                    v.set(*((CIMObjectPath*)data->ref->hdl));
                }
                else
                {
                    return CIMValue(CIMTYPE_REFERENCE, false);
                }
                break;
            case CMPI_dateTime: 
                if( data->dateTime && data->dateTime->hdl )
                {
                    v.set(*((CIMDateTime*)data->dateTime->hdl)); 
                }
                else
                {
                    return CIMValue(CIMTYPE_DATETIME, false);
                }
                break;

            case CMPI_boolean:
                v.set((Boolean&)data->boolean);
                break;

            case CMPI_char16:
                v.set((Char16)data->char16);
                break;

            case CMPI_real32:
                v.set((Real32)data->real32);
                break;

            case CMPI_real64:
                v.set((Real64)data->real64);
                break;

            default:
                if( rc )
                {
                    *rc=CMPI_RC_ERR_NOT_SUPPORTED;
                }
        }
    return CIMValue(v);
}

//Function to convert CIMValue to CMPIData
CMPIrc value2CMPIData(const CIMValue& v, CMPIType t, CMPIData *data)
{
    //Initialize CMPIData object
    data->type=t;
    data->state=0;
    data->value.uint64=0;

    //Check for NULL CIMValue
    if( v.isNull() )
    {
        data->state=CMPI_nullValue;
        return CMPI_RC_OK;
    }

    //Start of CMPIArray processing
    if( t & CMPI_ARRAY )
    {
        //Get the size of the array
        int aSize=v.getArraySize();

        //Get the type of the element of the CMPIArray
        CMPIType aType=t&~CMPI_ARRAY;
        CMPIData *aData=new CMPIData[aSize+1];
        aData->type=aType;
        aData->value.sint32=aSize;

        //Set the type and state for all array elements
        for( int i=1; i<aSize+1; i++ )
        {
            aData[i].type=aType;
            aData[i].state=0;
        }
        aData++;

        //Check for Signed Integers
        if( (aType & (CMPI_UINT|CMPI_SINT))==CMPI_SINT )
        {
            switch( aType )
            {
                case CMPI_sint32:
                    CopyFromArray(Sint32,sint32);
                    break;

                case CMPI_sint16:
                    CopyFromArray(Sint16,sint16);
                    break;

                case CMPI_sint8:
                    CopyFromArray(Sint8,sint8);
                    break;

                case CMPI_sint64:
                    CopyFromArray(Sint64,sint64);
                    break;

                default: ;
            }
        }
        //Check for CMPI_string data type
        else
        if( aType == CMPI_string )
        {
            CopyFromStringArray(String,string)
        }

        // Check for Unsigned Integers
        else
        if( (aType & (CMPI_UINT|CMPI_SINT))==CMPI_UINT )
        {
            switch( aType )
            {
                case CMPI_uint32:
                    CopyFromArray(Uint32,uint32);
                    break;

                case CMPI_uint16:
                    CopyFromArray(Uint16,uint16);
                    break;

                case CMPI_uint8:
                    CopyFromArray(Uint8,uint8);
                    break;

                case CMPI_uint64:
                    CopyFromArray(Uint64,uint64);
                    break;

                default: ;
            }
        }
        else
            switch( aType )
            {
                case CMPI_ref:
                    CopyFromEncArray(CIMObjectPath,CMPIObjectPath,ref);
                    break;

                case CMPI_dateTime:
                    CopyFromEncArray(CIMDateTime,CMPIDateTime,dateTime);
                    break;
                case CMPI_instance:
                    if (v.getType() == CIMTYPE_OBJECT)
                    {
                        Array<CIMObject> tmpObjs;
                        v.get(tmpObjs);
                        for (int i = 0; i < aSize ; ++i)
                        {
                          aData[i].value.inst = reinterpret_cast<CMPIInstance*>(
                                new CMPI_Object(new CIMInstance(tmpObjs[i])));
                        } 
                    }
                    else
                    {
                        CopyFromEncArray(CIMInstance,CMPIInstance,inst);
                    }
                    break;
                case CMPI_boolean:
                    CopyFromArray(Boolean,boolean);
                    break;

                case CMPI_char16:
                    CopyFromArray(Char16,char16);
                    break;

                case CMPI_real32:
                    CopyFromArray(Real32,real32);
                    break;

                case CMPI_real64:
                    CopyFromArray(Real64,real64);
                    break;

                default:
                    // Not supported for this CMPItype
                    delete [] aData;
                    return CMPI_RC_ERR_NOT_SUPPORTED;
            }
        data->value.array = reinterpret_cast<CMPIArray*>(
        new CMPI_Object(aData-1));
    }  // end of array porocessing

    //Start of non-array processing
    //Ckecking for Signed integers
    else 
    if( (t & (CMPI_UINT|CMPI_SINT)) == CMPI_SINT )
    {
        switch( t )
        {
            case CMPI_sint32:
                v.get((Sint32&)data->value.sint32);
                break;

            case CMPI_sint16:
                v.get((Sint16&)data->value.sint16);
                break;

            case CMPI_sint8:
                v.get((Sint8&)data->value.sint8);
                break;

            case CMPI_sint64:
                v.get((Sint64&)data->value.sint64);
                break;

            default: ;
        }
    }

    //Check for CMPI_string data type
    else 
    if( t == CMPI_string )
    {
        String str;
        v.get(str);
        data->value.string=string2CMPIString(str);
    }

    //Check for Unsigned Integers
    else
    if( (t & (CMPI_UINT|CMPI_SINT)) == CMPI_UINT )
    {
        switch( t )
        {
            case CMPI_uint32:
                v.get((Uint32&)data->value.uint32);
                break;

            case CMPI_uint16:
                v.get((Uint16&)data->value.uint16);
                break;

            case CMPI_uint8:
                v.get((Uint8&)data->value.uint8);
                break;

            case CMPI_uint64:
                v.get((Uint64&)data->value.uint64);
                break;

            default: ;
        }
    }

    //Checking for remaining encapulated and simple types
    else
        switch( t )
        {
            case CMPI_ref:
                {
                    CIMObjectPath ref;
                    v.get(ref);
                    data->value.ref = reinterpret_cast<CMPIObjectPath*>(
                    new CMPI_Object(new CIMObjectPath(ref)));
                }
                break;

            case CMPI_instance:
                {
                    CIMInstance inst;
                    if( v.getType() == CIMTYPE_OBJECT )
                    {
                        CIMObject tmpObj;
                        v.get(tmpObj);
                        inst = CIMInstance(tmpObj);
                    }
                    else
                    {
                        v.get(inst);
                    }

                    data->value.inst = reinterpret_cast<CMPIInstance*>(
                    new CMPI_Object(new CIMInstance(inst)));
                }
                break;

            case CMPI_dateTime:
                {
                    CIMDateTime dt;
                    v.get(dt);
                    data->value.dateTime = reinterpret_cast<CMPIDateTime*>(
                    new CMPI_Object(new CIMDateTime(dt)));
                }
                break;

            case CMPI_boolean:
                v.get((Boolean&)data->value.boolean);
                break;

            case CMPI_char16:
                v.get((Char16&)data->value.char16);
                break;

            case CMPI_real32:
                v.get((Real32&)data->value.real32);
                break;

            case CMPI_real64:
                v.get((Real64&)data->value.real64);
                break;

            default:
                return CMPI_RC_ERR_NOT_SUPPORTED;
        }
    return CMPI_RC_OK;
}

//Function to convert CIMType to CMPIType
CMPIType type2CMPIType(CIMType pt, int array)
{
    static CMPIType types[] =
    {
        CMPI_boolean,    // BOOLEAN,
        CMPI_uint8,      // UINT8,
        CMPI_sint8,      // SINT8,
        CMPI_uint16,     // UINT16,
        CMPI_sint16,     // SINT16,
        CMPI_uint32,     // UINT32,
        CMPI_sint32,     // SINT32,
        CMPI_uint64,     // UINT64,
        CMPI_sint64,     // SINT64,
        CMPI_real32,     // REAL32,
        CMPI_real64,     // REAL64,
        CMPI_char16,     // CHAR16,
        CMPI_string,     // STRING,
        CMPI_dateTime,   // DATETIME,
        CMPI_ref,        // REFERENCE
        CMPI_instance,    // Embedded Object
        CMPI_instance  // Embedded Instance
    };
    int t=types[pt];
    if( array )
    {
        t|=CMPI_ARRAY;
    }
    return(CMPIType)t;
}

//Function to convert CMPIType to CIMType
CIMType type2CIMType(CMPIType pt)
{
    switch( pt )
    {
        case CMPI_null:
            return(CIMType)0;

        case CMPI_boolean:
            return CIMTYPE_BOOLEAN;

        case CMPI_char16:
            return CIMTYPE_CHAR16;

        case CMPI_real32:
            return CIMTYPE_REAL32;

        case CMPI_real64:
            return CIMTYPE_REAL64;

        case CMPI_uint8:
            return CIMTYPE_UINT8;

        case CMPI_uint16:
            return CIMTYPE_UINT16;

        case CMPI_uint32:
            return CIMTYPE_UINT32;

        case CMPI_uint64:
            return CIMTYPE_UINT64;

        case CMPI_sint8:
            return CIMTYPE_SINT8;

        case CMPI_sint16:
            return CIMTYPE_SINT16;

        case CMPI_sint32:
            return CIMTYPE_SINT32;

        case CMPI_sint64:
            return CIMTYPE_SINT64;

        case CMPI_string:
            return CIMTYPE_STRING;

        case CMPI_chars:
            return CIMTYPE_STRING;

        case CMPI_charsptr:
            return CIMTYPE_STRING;

        case CMPI_dateTime:
            return CIMTYPE_DATETIME;

        case CMPI_ref:
            return CIMTYPE_REFERENCE;
        case CMPI_instance:
            return CIMTYPE_INSTANCE;
        default:
            return(CIMType)0;
    }
}

//Function to convert CIMKeyBindings to CMPIData
CMPIrc key2CMPIData(const String& v, CIMKeyBinding::Type t, CMPIData *data)
{
    data->state=CMPI_keyValue;
    switch( t )
    {
        case CIMKeyBinding::NUMERIC:
            {
                CString vp=v.getCString();

                data->value.sint64 = 0;
                if( *((const char*)vp)=='-' )
                {
                    sscanf(
                        (const char*)vp,
                        "%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                    &data->value.sint64);
                    data->type=CMPI_sint64;
                }
                else
                {
                    sscanf(
                        (const char*)vp,
                        "%" PEGASUS_64BIT_CONVERSION_WIDTH "u",
                    &data->value.uint64);
                    data->type=CMPI_uint64;
                }
            }
            break;

        case CIMKeyBinding::STRING:
            data->value.string=string2CMPIString(v);
            data->type=CMPI_string;
            break;

        case CIMKeyBinding::BOOLEAN:
            data->value.boolean=(String::equalNoCase(v,"true"));
            data->type=CMPI_boolean;
            break;

        case CIMKeyBinding::REFERENCE:
            data->value.ref=reinterpret_cast<CMPIObjectPath*>(
            new CMPI_Object(new CIMObjectPath(v)));
            data->type=CMPI_ref;
            break;

        default:
            return CMPI_RC_ERR_NOT_SUPPORTED;
    }
    return CMPI_RC_OK;
}

PEGASUS_NAMESPACE_END

