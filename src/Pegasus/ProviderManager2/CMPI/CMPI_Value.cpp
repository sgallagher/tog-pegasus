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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_String.h"
#include "CMPI_Value.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define CopyToArray(pt,ct) { Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=aData[i].value.ct; \
   v.set(ar##pt); }

#define CopyToStringArray(pt,ct) { Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=String(((char*)aData[i].value.ct)); \
   v.set(ar##pt); }

#define CopyCharsptrToStringArray(pt,ct) { Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=String((*(char**)aData[i].value.ct)); \
   v.set(ar##pt); }

#define CopyToEncArray(pt,ct) { Array<pt> ar##pt(aSize); \
   for (int i=0; i<aSize; i++) ar##pt[i]=*((pt*)aData[i].value.ct->hdl); \
   v.set(ar##pt); }

CIMValue value2CIMValue(const CMPIValue* data, const CMPIType type, CMPIrc *rc) {
   CIMValue v;
   if (rc) *rc=CMPI_RC_OK;

   if (type & CMPI_ARRAY) {

      if (data==NULL || data->array==NULL) {
         CMPIType aType=type&~CMPI_ARRAY;
         return CIMValue(type2CIMType(aType),true);
      }

      CMPIArray *ar=data->array;
      CMPIData *aData=(CMPIData*)ar->hdl;
      CMPIType aType=aData->type&~CMPI_ARRAY;
      int aSize=aData->value.sint32;
      aData++;

     if (aType & CMPI_ENC && (data==NULL || data->array==NULL)) {
         if (aType==CMPI_chars || aType==CMPI_string)
            return CIMValue(CIMTYPE_STRING,true);
         if (aType==CMPI_dateTime)
            return CIMValue(CIMTYPE_DATETIME,true);
         if (aType==CMPI_ref)
            return CIMValue(CIMTYPE_REFERENCE,true);
      }

      if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
         switch (aType) {
            case CMPI_sint32: CopyToArray(Sint32,sint32); break;
            case CMPI_sint16: CopyToArray(Sint16,sint16); break;
            case CMPI_sint8:  CopyToArray(Sint8,sint8);   break;
            case CMPI_sint64: CopyToArray(Sint64,sint64); break;
            default: ;
         }
      }

      else if (aType==CMPI_chars)       CopyToStringArray(String,chars)
      else if (aType==CMPI_charsptr)    CopyCharsptrToStringArray(String,chars)
      else if (aType==CMPI_string)      CopyToStringArray(String,string->hdl)

      else if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
         switch (aType) {
            case CMPI_uint32: CopyToArray(Uint32,uint32); break;
            case CMPI_uint16: CopyToArray(Uint16,uint16); break;
            case CMPI_uint8:  CopyToArray(Uint8,uint8);   break;
            case CMPI_uint64: CopyToArray(Uint64,uint64); break;
            default: ;
         }
      }

      else switch (aType) {
         case CMPI_ref:      CopyToEncArray(CIMObjectPath,ref); break;
         case CMPI_dateTime: CopyToEncArray(CIMDateTime,dateTime); break;
         case CMPI_boolean:  CopyToArray(Boolean,boolean); break;
         case CMPI_char16:   CopyToArray(Char16,char16); break;
         case CMPI_real32:   CopyToArray(Real32,real32); break;
         case CMPI_real64:   CopyToArray(Real64,real64); break;
         default:
         if (rc) *rc=CMPI_RC_ERR_NOT_SUPPORTED;
      }
      return CIMValue(v);
   } // end of array processing



   else if (type==CMPI_chars) {
      if (data) v.set(String((char*)data));
      else return CIMValue(CIMTYPE_STRING,false);
   }
   else if (type==CMPI_charsptr) {
      if (data && *(char**)data) v.set(String(*(char**)data));
      else return CIMValue(CIMTYPE_STRING,false);
   }

   else if (type & CMPI_ENC &&
      (data==NULL || data->string==NULL || data->string->hdl==NULL)) {
         if (type==CMPI_string)
            return CIMValue(CIMTYPE_STRING,false);
         if (type==CMPI_dateTime)
            return CIMValue(CIMTYPE_DATETIME,false);
         if (type==CMPI_ref)
            return CIMValue(CIMTYPE_REFERENCE,false);
   }

   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
      switch (type) {
         case CMPI_sint32: v.set((Sint32)data->sint32); break;
         case CMPI_sint16: v.set((Sint16)data->sint16); break;
         case CMPI_sint8:  v.set((Sint8)data->sint8);   break;
         case CMPI_sint64: v.set((Sint64)data->sint64); break;
         default: ;
      }
   }

   else if (type==CMPI_string) v.set(data->string->hdl ?
        String((char*)data->string->hdl) : String::EMPTY);

   else if ((type & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
      switch (type) {
         case CMPI_uint32: v.set((Uint32)data->uint32); break;
         case CMPI_uint16: v.set((Uint16)data->uint16); break;
         case CMPI_uint8:  v.set((Uint8)data->uint8);   break;
         case CMPI_uint64: v.set((Uint64)data->uint64); break;
         default: ;
      }
   }
   else if (type ==CMPI_instance) 
	{
		v.set(*((CIMInstance*) data->inst->hdl));
	}
   else switch (type) {
      case CMPI_ref:      v.set(*((CIMObjectPath*)data->ref->hdl)); break;
      case CMPI_dateTime: v.set(*((CIMDateTime*)data->dateTime->hdl)); break;
      case CMPI_boolean:  v.set((Boolean&)data->boolean); break;
      case CMPI_char16:   v.set((Char16)data->char16); break;
      case CMPI_real32:   v.set((Real32)data->real32); break;
      case CMPI_real64:   v.set((Real64)data->real64); break;
      default:
      if (rc) *rc=CMPI_RC_ERR_NOT_SUPPORTED;
   }
   return CIMValue(v);
}



#define CopyFromArray(pt,ct) { Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) aData[i].value.ct=ar##pt[i]; }

#define CopyFromStringArray(pt,ct) { Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) { \
      aData[i].value.ct=(CMPIString*)new CMPI_Object(ar##pt[i]); } }

#define CopyFromEncArray(pt,ct,cn) { Array<pt> ar##pt; \
   v.get(ar##pt); \
   for (int i=0; i<aSize; i++) { \
     aData[i].value.cn=(ct*)new CMPI_Object(new pt(ar##pt[i])); } }


CMPIrc value2CMPIData(const CIMValue& v, CMPIType t, CMPIData *data) {

   data->type=t;
   data->state=0;
   data->value.uint64=0;

   if (v.isNull()) {
      data->state=CMPI_nullValue;
      return CMPI_RC_OK;
   }

   if (t & CMPI_ARRAY) {
      int aSize=v.getArraySize();
      CMPIType aType=t&~CMPI_ARRAY;
      CMPIData *aData=new CMPIData[aSize+1];
      aData->type=aType;
      aData->value.sint32=aSize;

      for (int i=1; i<aSize+1; i++) {
         aData[i].type=aType;
         aData[i].state=0;
      }
      aData++;

      if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
         switch (aType) {
            case CMPI_sint32: CopyFromArray(Sint32,sint32); break;
            case CMPI_sint16: CopyFromArray(Sint16,sint16); break;
            case CMPI_sint8:  CopyFromArray(Sint8,sint8);   break;
            case CMPI_sint64: CopyFromArray(Sint64,sint64); break;
            default: ;
         }
      }
      else if (aType==CMPI_string) CopyFromStringArray(String,string)
      else if ((aType & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
         switch (aType) {
            case CMPI_uint32: CopyFromArray(Uint32,uint32); break;
            case CMPI_uint16: CopyFromArray(Uint16,uint16); break;
            case CMPI_uint8:  CopyFromArray(Uint8,uint8);   break;
            case CMPI_uint64: CopyFromArray(Uint64,uint64); break;
            default: ;
         }
      }
      else switch (aType) {
         case CMPI_ref:      CopyFromEncArray(CIMObjectPath,CMPIObjectPath,ref); break;
         case CMPI_dateTime: CopyFromEncArray(CIMDateTime,CMPIDateTime,dateTime); break;
         case CMPI_boolean:  CopyFromArray(Boolean,boolean); break;
         case CMPI_char16:   CopyFromArray(Char16,char16); break;
         case CMPI_real32:   CopyFromArray(Real32,real32); break;
         case CMPI_real64:   CopyFromArray(Real64,real64); break;
         default:
         return CMPI_RC_ERR_NOT_SUPPORTED;
      }
      data->value.array=(CMPIArray*)new CMPI_Object(aData-1);
   }  // end of array porocessing

   else if ((t & (CMPI_UINT|CMPI_SINT))==CMPI_SINT) {
      switch (t) {
         case CMPI_sint32: v.get((Sint32&)data->value.sint32); break;
         case CMPI_sint16: v.get((Sint16&)data->value.sint16); break;
         case CMPI_sint8:  v.get((Sint8&)data->value.sint8);   break;
         case CMPI_sint64: v.get((Sint64&)data->value.sint64); break;
         default: ;
      }
   }

   else if (t==CMPI_string) {
      String str;
      v.get(str);
      data->value.string=string2CMPIString(str);
   }

   else if ((t & (CMPI_UINT|CMPI_SINT))==CMPI_UINT) {
      switch (t) {
         case CMPI_uint32: v.get((Uint32&)data->value.uint32); break;
         case CMPI_uint16: v.get((Uint16&)data->value.uint16); break;
         case CMPI_uint8:  v.get((Uint8&)data->value.uint8);   break;
         case CMPI_uint64: v.get((Uint64&)data->value.uint64); break;
         default: ;
      }
   }

   else switch (t) {
   case CMPI_ref: {
         CIMObjectPath ref;
         v.get(ref);
         data->value.ref=(CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(ref));
      }
      break;
   case CMPI_dateTime: {
         CIMDateTime dt;
         v.get(dt);
         data->value.dateTime=(CMPIDateTime*)new CMPI_Object(new CIMDateTime(dt));
      }
      break;
   case CMPI_boolean:      v.get((Boolean&)data->value.boolean); break;
   case CMPI_char16:       v.get((Char16&)data->value.char16); break;
   case CMPI_real32:       v.get((Real32&)data->value.real32); break;
   case CMPI_real64:       v.get((Real64&)data->value.real64); break;
   default:
      return CMPI_RC_ERR_NOT_SUPPORTED;
   }
   return CMPI_RC_OK;
}

CMPIType type2CMPIType(CIMType pt, int array) {
   static CMPIType types[]={
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
    };
    int t=types[pt];
    if (array) t|=CMPI_ARRAY;
    return (CMPIType)t;
}

CIMType type2CIMType(CMPIType pt) {
   switch (pt) {
   case CMPI_null:      return (CIMType)0;
   case CMPI_boolean:   return CIMTYPE_BOOLEAN;
   case CMPI_char16:    return CIMTYPE_CHAR16;

   case CMPI_real32:    return CIMTYPE_REAL32;
   case CMPI_real64:    return CIMTYPE_REAL64;

   case CMPI_uint8:     return CIMTYPE_UINT8;
   case CMPI_uint16:    return CIMTYPE_UINT16;
   case CMPI_uint32:    return CIMTYPE_UINT32;
   case CMPI_uint64:    return CIMTYPE_UINT64;

   case CMPI_sint8:     return CIMTYPE_SINT8;
   case CMPI_sint16:    return CIMTYPE_SINT16;
   case CMPI_sint32:    return CIMTYPE_SINT32;
   case CMPI_sint64:    return CIMTYPE_SINT64;

   case CMPI_string:    return CIMTYPE_STRING;
   case CMPI_chars:     return CIMTYPE_STRING;
   case CMPI_dateTime:  return CIMTYPE_DATETIME;
   case CMPI_ref:       return CIMTYPE_REFERENCE;

   default:             return (CIMType)0;
   }
 }

CMPIrc key2CMPIData(const String& v, CIMKeyBinding::Type t, CMPIData *data) {
   data->state=CMPI_keyValue;
   switch (t) {
   case CIMKeyBinding::NUMERIC: {
 //        const char *vp=v.getCString();
         CString vp=v.getCString();

        //#if defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) || defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
          //data->value.sint64=strtoll((const char*)vp, NULL, 10);
        //#elif defined(PEGASUS_OS_HPUX) || defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
          //data->value.sint64 = 0;
          //sscanf((const char*)vp, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                 //&data->value.sint64);
        //#else
          //data->value.sint64=atoll((const char*)vp);
        //#endif

         data->value.sint64 = 0;
         sscanf((const char*)vp, "%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                 &data->value.sint64);
         data->type=CMPI_sint64;
//         delete vp;
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
      data->value.ref=(CMPIObjectPath*)new CMPI_Object(new CIMObjectPath(v));
      data->type=CMPI_ref;
      break;
   default:
      return CMPI_RC_ERR_NOT_SUPPORTED;
   }
   return CMPI_RC_OK;
}

PEGASUS_NAMESPACE_END






