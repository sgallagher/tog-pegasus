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
// Author: Adrian Schuur (schuur@de.ibm.com) - PEP 164
//
// Modified By: Dave Sudlik (dsudlik@us.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "XmlWriter.h"
#include "XmlReader.h"
#include "XmlParser.h"

#include "CIMName.h"
#include "BinaryStreamer.h"
#include "CIMClassRep.h"
#include "CIMInstanceRep.h"
#include "CIMMethodRep.h"
#include "CIMParameterRep.h"
#include "CIMPropertyRep.h"
#include "CIMQualifierRep.h"

#include "CIMValue.h"
#include "CIMValueRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

PEGASUS_EXPORT int removeDescription=-1;

void BinaryStreamer::encode(Array<Sint8>& out, const CIMClass& cls)
{
   toBin(out, cls);
}

void BinaryStreamer::encode(Array<Sint8>& out, const CIMInstance& inst)
{
   toBin(out, inst);
}

void BinaryStreamer::encode(Array<Sint8>& out, const CIMQualifierDecl& qual)
{
   toBin(out, qual);
}

void BinaryStreamer::decode(const Array<Sint8>& in, unsigned int pos, CIMClass& cls)
{
   cls=extractClass(in,pos,"");
}

void BinaryStreamer::decode(const Array<Sint8>& in, unsigned int pos, CIMInstance& inst)
{
   inst=extractInstance(in,pos,"");
}

void BinaryStreamer::decode(const Array<Sint8>& in, unsigned int pos, CIMQualifierDecl& qual)
{
   qual=extractQualifierDecl(in,pos,"");
}



void BinaryStreamer::append(Array<Sint8>& out, const CIMObjectPath &op)
{
   CString ustr=op.toString().getCString();
   Uint16 nl=strlen((const char*)ustr);
   out.append((Sint8*)&nl,sizeof(Uint16));
   out.append((Sint8*)((const char*)ustr),nl);
}

void BinaryStreamer::append(Array<Sint8>& out, const CIMName &cn)
{
   CString ustr=cn.getString().getCString();
   Uint16 nl=strlen((const char*)ustr);
   out.append((Sint8*)&nl,sizeof(Sint16));
   if (nl)
       out.append((Sint8*)((const char*)ustr),nl);
}

void BinaryStreamer::append(Array<Sint8>& out, const CIMType &typ)
{
   Uint16 type=(Uint16)typ;
   out.append((Sint8*)&type,sizeof(Uint16));
}

void BinaryStreamer::append(Array<Sint8>& out, Uint16 ui)
{
   out.append((Sint8*)&ui,sizeof(Uint16));
}

void BinaryStreamer::append(Array<Sint8>& out, Uint32 ui)
{
   out.append((Sint8*)&ui,sizeof(Uint32));
}

void BinaryStreamer::append(Array<Sint8>& out, Boolean b)
{
   Sint8 rs=(b==true);
   out.append(rs);
}



CIMObjectPath BinaryStreamer::extractObjectPath(const Sint8 *ar, Uint32 & pos)
{
   Uint16 sl=*(Uint16*)(ar+pos);
   Uint32 ppos=pos+=sizeof(Uint16);
   pos+=sl;
   return CIMObjectPath(String(((char*)(ar+ppos)),sl));
}

CIMName BinaryStreamer::extractName(const Sint8 *ar, Uint32 & pos)
{
   Uint16 sl=*(Uint16*)(ar+pos);
   Uint32 ppos=pos+=sizeof(Uint16);
   if (sl) {
      pos+=sl;
      return CIMName(String(((char*)(ar+ppos)),sl));
   }
   return CIMName();
}

Uint16 BinaryStreamer::extractUint16(const Sint8 *ar, Uint32 & pos)
{
   Uint16 ui=*(Uint16*)(ar+pos);
   pos+=sizeof(Uint16);
   return ui;
}

CIMType BinaryStreamer::extractType(const Sint8 *ar, Uint32 & pos)
{
   Uint16 ui=*(Uint16*)(ar+pos);
   pos+=sizeof(Uint16);
   CIMType t=(CIMType)ui;
   return t;
}

Uint32 BinaryStreamer::extractUint32(const Sint8 *ar, Uint32 & pos)
{
   Uint32 ui=*(Uint32*)(ar+pos);
   pos+=sizeof(Uint32);
   return ui;
}

Boolean BinaryStreamer::extractBoolean(const Sint8 *ar, Uint32 & pos)
{
   return ((*(ar+(pos++)))!=0);
}




void BinaryStreamer::toBin(Array<Sint8>& out, const CIMClass& cls)
{
   CIMClassRep *rep=cls._rep;
   static BINREP_CLASS_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,rep->getClassName());

   append(out,rep->getSuperClassName());

   Uint16 qn=rep->getQualifierCount();
   append(out,qn);
   for (Uint16 i=0; i<qn; i++) {
       const CIMQualifier &cq=rep->getQualifier(i);
       toBin(out,cq);
   }

   Uint16 pn=rep->getPropertyCount();
   append(out,pn);
   for (Uint16 i = 0; i < pn; i++) {
       toBin(out,rep->getProperty(i));
   }

   Uint16 mn=rep->getMethodCount();
   append(out,mn);
   for (Uint16 i = 0; i < mn; i++) {
       toBin(out,rep->getMethod(i));
   }

   append(out,rep->_resolved);
}


CIMClass BinaryStreamer::extractClass(const Array<Sint8>& in, Uint32 & pos, const String &path)
{
   BINREP_RECORD_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   try {
      if (!preamble->endogenous()) {
          throw BinException(BINREP_CLASS,String("Incompatible Binary Repository not supported"));
      }
      if (preamble->type()!=BINREP_CLASS) {
          throw BinException(BINREP_CLASS,String("Expected CIMClass subtype not found"));
      }

      pos+=preamble->size();

      switch (preamble->version()) {
      case BINREP_CLASS_V1: {

         CIMName name=extractName(ar,pos); //if (name=="CIM_Memory") asm("int $3");
         CIMName super=extractName(ar,pos);
         CIMClass cls(name,super);

         Uint16 qn=extractUint16(ar,pos);
         for (Uint16 i=0; i<qn; i++) {
            CIMQualifier q=extractQualifier(in,pos);
            cls.addQualifier(q);
         }

         Uint16 pn=extractUint16(ar,pos);
         for (Uint16 i=0; i<pn; i++) {
            CIMProperty p=extractProperty(in,pos);
            cls.addProperty(p);
         }

         Uint16 mn=extractUint16(ar,pos);
         for (Uint16 i=0; i<mn; i++) {
            CIMMethod m=extractMethod(in,pos);
            cls.addMethod(m);
         }

         cls._rep->_resolved=extractBoolean(ar,pos);

         return cls;
      }
      default:
          throw BinException(BINREP_CLASS,String("CIMClass subtype version ")+
             CIMValue(preamble->version()).toString()+" not supported ");
      }
   }
   catch (BinException &be) {
      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,"Binary Repository integraty failure: "+
         be.message+" - Accessing class: "+path);
   }

   return CIMClass();
}





void BinaryStreamer::toBin(Array<Sint8>& out, const CIMInstance& inst)
{
   CIMInstanceRep *rep=inst._rep;

   static BINREP_INSTANCE_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,rep->getPath());

   Uint16 qn=rep->getQualifierCount();
   append(out,qn);
   for (Uint16 i=0; i<qn; i++) {
       const CIMQualifier &cq=rep->getQualifier(i);
       toBin(out,cq);
   }

   Uint16 pn=rep->getPropertyCount();
   append(out,pn);
   for (Uint16 i = 0; i < pn; i++) {
       toBin(out,rep->getProperty(i));
   }

   append(out,rep->_resolved);
}


CIMInstance BinaryStreamer::extractInstance(const Array<Sint8>& in, Uint32 & pos,
        const String & path)
{
   BINREP_RECORD_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   try {
      if (!preamble->endogenous()) {
          throw BinException(BINREP_INSTANCE,String("Incompatible Binary Repository not supported"));
      }
      if (preamble->type()!=BINREP_INSTANCE) {
          throw BinException(BINREP_INSTANCE,String("Expected CIMInstance subtype not found"));
      }

      pos+=preamble->size();

      switch (preamble->version()) {
      case BINREP_INSTANCE_V1: {

         CIMObjectPath op=extractObjectPath(ar,pos);
         CIMInstance inst(op.getClassName());
         inst.setPath(op);

         Uint16 qn=extractUint16(ar,pos);
         for (Uint16 i=0; i<qn; i++) {
            CIMQualifier q=extractQualifier(in,pos);
            inst.addQualifier(q);
         }

         Uint16 pn=extractUint16(ar,pos);
         for (Uint16 i=0; i<pn; i++) {
            CIMProperty p=extractProperty(in,pos);
            inst.addProperty(p);
         }

         inst._rep->_resolved=extractBoolean(ar,pos);

         return inst;
      }
      default:
          throw BinException(BINREP_INSTANCE,String("CIMInstance subtype version ")+
             CIMValue(preamble->version()).toString()+" not supported ");
      }
   }
   catch (BinException &be) {
      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,"Binary Repository integraty failure: "+
         be.message+" - Accessing instance: "+path);
   }
   return CIMInstance();
}






void BinaryStreamer::toBin(Array<Sint8>& out, const CIMQualifierDecl& qdc)
{
   static BINREP_QUALIFIERDECL_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,qdc.getName());

   toBin(out,qdc.getValue());

   toBin(out,qdc.getScope());

   toBin(out,qdc.getFlavor());

   append(out,qdc.getArraySize());

}


CIMQualifierDecl BinaryStreamer::extractQualifierDecl(const Array<Sint8>& in, Uint32 & pos,
        const String &path)
{
   BINREP_RECORD_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   try {
      if (!preamble->endogenous()) {
          throw BinException(BINREP_QUALIFIERDECL,String
             ("Incompatible Binary Repository not supported"));
      }
      if (preamble->type()!=BINREP_QUALIFIERDECL) {
          throw BinException(BINREP_QUALIFIERDECL,String(
             "Expected CIMQualifierDecl subtype not found"));
      }

      pos+=preamble->size();

      switch (preamble->version()) {
      case BINREP_INSTANCE_V1: {

         CIMName name=extractName(ar,pos);
         CIMValue val=extractValue(in,pos);
         CIMScope scp=extractScope(in,pos);
         CIMFlavor fl=extractFlavor(in,pos);
         Uint32    as=extractUint32(ar,pos);

         CIMQualifierDecl qdl(name,val,scp,fl,as);

         return qdl;
      }
      default:
          throw BinException(BINREP_INSTANCE,String("CIMInstance subtype version ")+
             CIMValue(preamble->version()).toString()+" not supported ");
      }
   }
   catch (BinException &be) {
      throw PEGASUS_CIM_EXCEPTION_L(CIM_ERR_FAILED,"Binary Repository integraty failure: "+
         be.message+" - Accessing instance: "+path);
   }
   return CIMQualifierDecl();
}





void BinaryStreamer::toBin(Array<Sint8>& out, const CIMMethod &meth)
{
   CIMMethodRep *rep=meth._rep;

   static BINREP_METHOD_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,rep->getName());

   append(out,rep->getType());

   append(out,rep->getClassOrigin());

   append(out,rep->getPropagated());

   Uint16 qn=rep->getQualifierCount();
   append(out,qn);
   for (Uint16 i=0; i<qn; i++) {
       const CIMQualifier &cq=rep->getQualifier(i);
       toBin(out,cq);
   }

    Uint16 pn=rep->getParameterCount();
    out.append((Sint8*)&pn,sizeof(Uint16));
    for (Uint16 i = 0; i < pn; i++) {
       toBin(out,rep->getParameter(i));
    }
}


CIMMethod BinaryStreamer::extractMethod(const Array<Sint8>& in, Uint32 & pos)
{
   BINREP_SUBTYPE_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   if (preamble->type()!=BINREP_METHOD) {
       throw BinException(BINREP_METHOD,String("Expected CIMMethod subtype not found"));
   }

   pos+=preamble->size();

   switch (preamble->version()) {
   case BINREP_METHOD_V1: {

      CIMName name=extractName(ar,pos);
      CIMType type=extractType(ar,pos);
      CIMName orig=extractName(ar,pos);
      Boolean prpg=extractBoolean(ar,pos);

      CIMMethod meth(name,type,orig,prpg);

      Uint16 qn=extractUint16(ar,pos);
      for (Uint16 i=0; i<qn; i++) {
         CIMQualifier q=extractQualifier(in,pos);
         meth.addQualifier(q);
      }

      Uint16 pn=extractUint16(ar,pos);
      for (Uint16 i=0; i<pn; i++) {
         CIMParameter p=extractParameter(in,pos);
         meth.addParameter(p);
      }

      return meth;
   }
   default: ;
       throw BinException(BINREP_METHOD,String("CIMMethod subtype version ")+
          CIMValue(preamble->version()).toString()+" not supported ");
   }
   return CIMMethod();
}





void BinaryStreamer::toBin(Array<Sint8>& out, const CIMParameter& prm)
{
   CIMParameterRep *rep=prm._rep;

   static BINREP_PARAMETER_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,rep->getName());

   append(out,rep->getType());

   append(out,rep->isArray());

   append(out,rep->getArraySize());

   append(out,rep->getReferenceClassName());

   Uint16 qn=rep->getQualifierCount();
   append(out,qn);
   for (Uint16 i=0; i<qn; i++) {
       const CIMQualifier &cq=rep->getQualifier(i);
       toBin(out,cq);
   }
}

CIMParameter BinaryStreamer::extractParameter(const Array<Sint8>& in, Uint32 &pos)
{
   BINREP_SUBTYPE_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   if (preamble->type()!=BINREP_PARAMETER) {
       throw BinException(BINREP_PARAMETER,String("Expected CIMParameter subtype not found"));
  }

   pos+=preamble->size();

   switch (preamble->version()) {
   case BINREP_PARAMETER_V1: {

      CIMName name=extractName(ar,pos);
      CIMType type=extractType(ar,pos);
      Boolean isAr=extractBoolean(ar,pos);
      Uint32    as=extractUint32(ar,pos);
      CIMName clsr=extractName(ar,pos);

      CIMParameter parm(name,type,isAr,as,clsr);

      Uint16 qn=extractUint16(ar,pos);
      for (Uint16 i=0; i<qn; i++) {
         CIMQualifier q=extractQualifier(in,pos);
         parm.addQualifier(q);
      }

      return parm;
   }
   default: ;
       throw BinException(BINREP_PARAMETER,String("CIMParameter subtype version ")+
          CIMValue(preamble->version()).toString()+" not supported ");
   }
   return CIMParameter();
}




void BinaryStreamer::toBin(Array<Sint8>& out, const CIMProperty& prop)
{
   CIMPropertyRep *rep=prop._rep;

   static BINREP_PROPERTY_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,rep->getName());

   toBin(out,rep->getValue());

   append(out,rep->getArraySize());

   append(out,rep->getReferenceClassName());

   append(out,rep->getClassOrigin());

   append(out,rep->getPropagated());

   Uint16 qn=rep->getQualifierCount();
   append(out,qn);
   for (Uint16 i=0; i<qn; i++) {
       const CIMQualifier &cq=rep->getQualifier(i);
       toBin(out,cq);
   }
}


CIMProperty BinaryStreamer::extractProperty(const Array<Sint8>& in, Uint32 &pos)
{
   BINREP_SUBTYPE_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   if (preamble->type()!=BINREP_PROPERTY) {
       throw BinException(BINREP_PROPERTY,String("Expected CIMProperty subtype not found"));
   }

   pos+=preamble->size();

   switch (preamble->version()) {
   case BINREP_PROPERTY_V1: {

      CIMName name=extractName(ar,pos);
      CIMValue val=extractValue(in,pos);
      Uint32    as=extractUint32(ar,pos);
      CIMName clsr=extractName(ar,pos);
      CIMName orig=extractName(ar,pos);
      Boolean prpg=extractBoolean(ar,pos);

      CIMProperty prop(name,val,as,clsr,orig,prpg);

      Uint16 qn=extractUint16(ar,pos);
      for (Uint16 i=0; i<qn; i++) {
         CIMQualifier q=extractQualifier(in,pos);
         prop.addQualifier(q);
      }

      return prop;
   }
   default: ;
       throw BinException(BINREP_PROPERTY,String("CIMProperty subtype version ")+
          CIMValue(preamble->version()).toString()+" not supported ");
   }
   return CIMProperty();
}





void BinaryStreamer::toBin(Array<Sint8>& out, const CIMFlavor& flav)
{
   out.append((Sint8*)&flav.cimFlavor,sizeof(flav.cimFlavor));
}


CIMFlavor BinaryStreamer::extractFlavor(const Array<Sint8>& in, Uint32 & pos)
{
   CIMFlavor flav;
   flav.cimFlavor=extractUint32(in.getData(),pos);
   return flav;
}





void BinaryStreamer::toBin(Array<Sint8>& out, const CIMScope& scp)
{
   out.append((Sint8*)&scp.cimScope,sizeof(scp.cimScope));
}


CIMScope BinaryStreamer::extractScope(const Array<Sint8>& in, Uint32 & pos)
{
   CIMScope scp;
   scp.cimScope=extractUint32(in.getData(),pos);
   return scp;
}


 

void BinaryStreamer::toBin(Array<Sint8>& out, const CIMQualifier& qual)
{
   CIMQualifierRep *rep=qual._rep;

   static BINREP_QUALIFIER_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   CIMName name=rep->getName();
   append(out,name);

   if (removeDescription>0 && name=="description") {
      CIMValue val(CIMTYPE_STRING,false);
      val.set(String("*REMOVED*"));
      toBin(out,val);
   }
   else toBin(out,rep->getValue());

   toBin(out,rep->getFlavor());

   append(out,rep->getPropagated());
}


CIMQualifier BinaryStreamer::extractQualifier(const Array<Sint8>& in, Uint32 & pos)
{
   BINREP_SUBTYPE_IN(preamble,in,pos);
   const Sint8 *ar=in.getData();

   if (preamble->type()!=BINREP_QUALIFIER) {
       throw BinException(BINREP_QUALIFIER,String("Expected CIMQualifier subtype not found"));
   }

  pos+=preamble->size();

   switch (preamble->version()) {
   case BINREP_QUALIFIER_V1: {

      CIMName name=extractName(ar,pos);
      CIMValue val=extractValue(in,pos);
      CIMFlavor fl=extractFlavor(in,pos);
      Boolean prpg=extractBoolean(ar,pos);

      CIMQualifier q(name,val,fl,prpg);

      return q;
   }
   default: ;
       throw BinException(BINREP_QUALIFIER,String("CIMQualifier subtype version ")+
          CIMValue(preamble->version()).toString()+" not supported ");
   }
   return CIMQualifier();
}




void BinaryStreamer::toBin(Array<Sint8> & out, const CIMValue& val)
{
   CIMValueRep *_rep=val._rep;

   static BINREP_VALUE_PREAMBLE_V1(preamble);
   out.append((Sint8*)&preamble,sizeof(preamble));

   append(out,val.getType());

   Boolean isArray=val.isArray();
   append(out,isArray);

   Uint32 as=0;

   if (isArray) {
      as=val.getArraySize();
      append(out,as);
   }

   Boolean isNull=val.isNull();
   append(out,isNull);

   if (!isNull) {
      if (isArray) {
         switch (val.getType()) {
         case CIMTYPE_BOOLEAN: {
               out.append((Sint8*)_rep->_u._booleanArray->getData(),sizeof(Boolean)*as);
            }
            break;
         case CIMTYPE_UINT8: {
               out.append((Sint8*)_rep->_u._uint8Array->getData(),sizeof(Uint8)*as);
            }
            break;
         case CIMTYPE_SINT8: {
               out.append((Sint8*)_rep->_u._sint8Array->getData(),sizeof(Sint8)*as);
            }
            break;
         case CIMTYPE_UINT16: {
               out.append((Sint8*)_rep->_u._uint16Array->getData(),sizeof(Uint16)*as);
            }
            break;
         case CIMTYPE_SINT16: {
               out.append((Sint8*)_rep->_u._sint16Array->getData(),sizeof(Sint16)*as);
            }
            break;
         case CIMTYPE_UINT32: {
               out.append((Sint8*)_rep->_u._uint32Array->getData(),sizeof(Uint32)*as);
            }
            break;
         case CIMTYPE_SINT32: {
               out.append((Sint8*)_rep->_u._sint32Array->getData(),sizeof(Sint32)*as);
            }
            break;
         case CIMTYPE_UINT64: {
               out.append((Sint8*)_rep->_u._uint64Array->getData(),sizeof(Uint64)*as);
            }
            break;
         case CIMTYPE_SINT64: {
               out.append((Sint8*)_rep->_u._sint64Array->getData(),sizeof(Uint64)*as);
            }
            break;
         case CIMTYPE_REAL32: {
               out.append((Sint8*)_rep->_u._real32Array->getData(),sizeof(Real32)*as);
            }
            break;
         case CIMTYPE_REAL64: {
               out.append((Sint8*)_rep->_u._real64Array->getData(),sizeof(Real64)*as);
            }
            break;
         case CIMTYPE_CHAR16: {
               out.append((Sint8*)_rep->_u._char16Array->getData(),sizeof(Char16)*as);
            }
            break;
         case CIMTYPE_STRING: {
               for (Uint32 i=0; i<as; i++) {
                  CString ustr=(*(_rep->_u._stringArray))[i].getCString();
                  Uint32 sz=strlen((const char*)ustr);
                  out.append((Sint8*)&sz,sizeof(Uint32));
                  out.append((Sint8*)((const char*)ustr),sz);
               }
            }
            break;
         case CIMTYPE_DATETIME: {
               for (Uint32 i=0; i<as; i++) {
                  String dts=(*(_rep->_u._dateTimeArray))[i].toString();
                  Sint32 dtl=dts.size();
                  out.append((Sint8*)&dtl,sizeof(Sint32));
                  out.append((Sint8*)dts.getChar16Data(),dtl*sizeof(Char16));
               }
            }
            break;
         case CIMTYPE_REFERENCE: {
                for (Uint32 i=0; i<as; i++) {
                  String rfs=(*(_rep->_u._referenceArray))[i].toString();
                  Sint32 rfl=rfs.size();
                  out.append((Sint8*)&rfl,sizeof(Sint32));
                  out.append((Sint8*)rfs.getChar16Data(),rfl*sizeof(Char16));
               }
            }
            break;
         }
      }

      else switch (val.getType()) {
      case CIMTYPE_BOOLEAN:
         out.append((Sint8*)&_rep->_u,sizeof(Boolean));  break;
      case CIMTYPE_UINT8:
         out.append((Sint8*)&_rep->_u,sizeof(Uint8));    break;
      case CIMTYPE_SINT8:
         out.append((Sint8*)&_rep->_u,sizeof(Sint8));    break;
      case CIMTYPE_UINT16:
         out.append((Sint8*)&_rep->_u,sizeof(Uint16));   break;
      case CIMTYPE_SINT16:
         out.append((Sint8*)&_rep->_u,sizeof(Sint16));   break;
      case CIMTYPE_UINT32:
         out.append((Sint8*)&_rep->_u,sizeof(Uint32));   break;
      case CIMTYPE_SINT32:
         out.append((Sint8*)&_rep->_u,sizeof(Sint32));   break;
      case CIMTYPE_UINT64:
         out.append((Sint8*)&_rep->_u,sizeof(Uint64));   break;
      case CIMTYPE_SINT64:
         out.append((Sint8*)&_rep->_u,sizeof(Uint64));   break;
      case CIMTYPE_REAL32:
         out.append((Sint8*)&_rep->_u,sizeof(Real32));   break;
      case CIMTYPE_REAL64:
         out.append((Sint8*)&_rep->_u,sizeof(Real64));   break;
      case CIMTYPE_CHAR16:
         out.append((Sint8*)&_rep->_u,sizeof(Char16));   break;
      case CIMTYPE_STRING: {
            CString ustr=_rep->_u._stringValue->getCString();
            Uint32 sz=strlen((const char*)ustr);
            out.append((Sint8*)&sz,sizeof(Uint32));
            out.append((Sint8*)((const char*)ustr),sz);
         }
         break;
      case CIMTYPE_DATETIME: {
            String dts=_rep->_u._dateTimeValue->toString();
            Sint32 dtl=dts.size();
            out.append((Sint8*)&dtl,sizeof(Sint32));
            out.append((Sint8*)dts.getChar16Data(),dtl*sizeof(Char16));
         }
         break;
      case CIMTYPE_REFERENCE: {
            String rfs=_rep->_u._referenceValue->toString();
            Sint32 rfl=rfs.size();
            out.append((Sint8*)&rfl,sizeof(Sint32));
            out.append((Sint8*)rfs.getChar16Data(),rfl*sizeof(Char16));
         }
         break;
      }
   }
   else {
   }
}



CIMValue BinaryStreamer::extractValue(const Array<Sint8>& in, Uint32 & pos)
{
    BINREP_SUBTYPE_IN(preamble,in,pos);
    const Sint8 *ar=in.getData();

   if (preamble->type()!=BINREP_VALUE) {
       throw BinException(BINREP_VALUE,String("Expected CIMValue subtype not found"));
   }

    pos+=preamble->size();

    Uint32 as=0;

    switch (preamble->version()) {
    case BINREP_VALUE_V1: {

      const Sint8 *ar=in.getData();

      CIMType type=extractType(ar,pos);

      Boolean isArray=extractBoolean(ar,pos);
      if (isArray)
          as=extractUint32(ar,pos);

      Boolean isNull=extractBoolean(ar,pos);

      if (!isNull) {
         if (isArray) {

            CIMValue val(type,isArray,as);

            switch (type) {
            case CIMTYPE_BOOLEAN: {
                  val.set(Array<Boolean>((Boolean*)(ar+pos),as));
                  pos+=sizeof(Boolean)*as;
               }
               break;
            case CIMTYPE_UINT8: {
                  val.set(Array<Uint8>((Uint8*)(ar+pos),as));
                  pos+=sizeof(Uint8)*as;
               }
               break;
            case CIMTYPE_SINT8: {
                  val.set(Array<Sint8>((Sint8*)(ar+pos),as));
                  pos+=sizeof(Sint8)*as;
               }
               break;
            case CIMTYPE_UINT16: {
                  val.set(Array<Uint16>((Uint16*)(ar+pos),as));
                  pos+=sizeof(Uint16)*as;
               }
               break;
            case CIMTYPE_SINT16: {
                  val.set(Array<Sint16>((Sint16*)(ar+pos),as));
                  pos+=sizeof(Sint16)*as;
               }
               break;
            case CIMTYPE_UINT32: {
                  val.set(Array<Uint32>((Uint32*)(ar+pos),as));
                  pos+=sizeof(Uint32)*as;
               }
               break;
            case CIMTYPE_SINT32: {
                  val.set(Array<Sint32>((Sint32*)(ar+pos),as));
                  pos+=sizeof(Sint32)*as;
               }
               break;
            case CIMTYPE_UINT64: {
                  val.set(Array<Uint64>((Uint64*)(ar+pos),as));
                  pos+=sizeof(Uint64)*as;
               }
               break;
            case CIMTYPE_SINT64: {
                  val.set(Array<Sint64>((Sint64*)(ar+pos),as));
                  pos+=sizeof(Sint64)*as;
               }
               break;
            case CIMTYPE_REAL32: {
                  val.set(Array<Real32>((Real32*)(ar+pos),as));
                  pos+=sizeof(Real32)*as;
               }
               break;
            case CIMTYPE_REAL64: {
                  val.set(Array<Real64>((Real64*)(ar+pos),as));
                  pos+=sizeof(Real64)*as;
               }
               break;
            case CIMTYPE_CHAR16: {
                  val.set(Array<Char16>((Char16*)(ar+pos),as));
                  pos+=sizeof(Char16)*as;
               }
               break;
            case CIMTYPE_STRING: {
                  Array<String> sar;
                  for (Uint32 i=0; i<as; i++) {
                     Uint32 sl=*(Uint32*)(ar+pos);
                     pos+=sizeof(Uint32);
                     sar.append(String(((char*)(ar+pos)),sl));
                     pos+=sl;
                  }
                  val.set(sar);
               }
               break;
            case CIMTYPE_DATETIME: {
                  Array<CIMDateTime> dar;
                  for (Uint32 i=0; i<as; i++) {
                     Uint32 sl=*(Uint32*)(ar+pos);
                     pos+=sizeof(Uint32);
                     dar.append(CIMDateTime(String(((Char16*)(ar+pos)),sl)));
                     pos+=sl*sizeof(Char16);
                  }
                  val.set(dar);
               }
               break;
            case CIMTYPE_REFERENCE: {
                  Array<CIMObjectPath> rar;
                  for (Uint32 i=0; i<as; i++) {
                     Uint32 sl=*(Uint32*)(ar+pos);
                     pos+=sizeof(Uint32);
                     rar.append(CIMObjectPath(String(((Char16*)(ar+pos)),sl)));
                     pos+=sl*sizeof(Char16);
                  }
                  val.set(rar);
               }
               break;
            default:
               PEGASUS_ASSERT(false);
            }
            return val;
         }

         else {

            CIMValue val(type,isArray);

            switch (type) {
            case CIMTYPE_BOOLEAN:
               val.set(*(Boolean*)(ar+pos));
               pos++;
               break;
            case CIMTYPE_SINT8:
               val.set(*(Sint8*)(ar+pos));
               pos++;
               break;
            case CIMTYPE_UINT8:
               val.set(*(Uint8*)(ar+pos));
               pos++;
               break;
            case CIMTYPE_UINT16:
               val.set(*(Uint16*)(ar+pos));
               pos+=sizeof(Uint16);
               break;
            case CIMTYPE_SINT16:
               val.set(*(Sint16*)(ar+pos));
               pos+=sizeof(Sint16);
               break;
            case CIMTYPE_CHAR16:
               val.set(*(Char16*)(ar+pos));
               pos+=sizeof(Char16);
               break;
            case CIMTYPE_UINT32:
               val.set(*(Uint32*)(ar+pos));
               pos+=sizeof(Uint32);
               break;
            case CIMTYPE_SINT32:
               val.set(*(Sint32*)(ar+pos));
               pos+=sizeof(Sint32);
               break;
            case CIMTYPE_REAL32:
               val.set(*(Real32*)(ar+pos));
               pos+=sizeof(Real32);
               break;
            case CIMTYPE_UINT64:
               val.set(*(Uint64*)(ar+pos));
               pos+=sizeof(Uint64);
               break;
            case CIMTYPE_SINT64:
               val.set(*(Sint64*)(ar+pos));
               pos+=sizeof(Sint64);
               break;
            case CIMTYPE_REAL64:
               val.set(*(Real64*)(ar+pos));
               pos+=sizeof(Real64);
               break;
            case CIMTYPE_STRING: {
                  Uint32 sl=*(Uint32*)(ar+pos);
                  pos+=sizeof(Uint32);
                  val.set(String(((char*)(ar+pos)),sl));
                  pos+=sl;
               }
               break;
            case CIMTYPE_DATETIME: {
                  Uint32 dtl=*(Uint32*)(ar+pos);
                  pos+=sizeof(Uint32);
                  val.set(CIMDateTime(String(((Char16*)(ar+pos)),dtl)));
                  pos+=dtl*sizeof(Char16);
               }
               break;
            case CIMTYPE_REFERENCE: {
                  Uint32 rfl=*(Uint32*)(ar+pos);
                  pos+=sizeof(Uint32);
                  val.set(CIMObjectPath(String(((Char16*)(ar+pos)),rfl)));
                  pos+=rfl*sizeof(Char16);
               }
               break;
            default:
               PEGASUS_ASSERT(false);
            }
            return val;
         }
      }
      else {
         CIMValue val;
         val.setNullValue(type,isArray,as);
         return val;
      }
      break;
   }
   default:
       throw BinException(BINREP_VALUE,String("CIMValue subtype version ")+
          CIMValue(preamble->version()).toString()+" not supported ");
   }
   return CIMValue();
}


PEGASUS_NAMESPACE_END
