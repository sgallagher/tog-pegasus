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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Heidi Neuman, heidineu@de.ibm.com
//              Angel Nunez Mencias, anunez@de.ibm.com
//              Viktor Mihajlovski, mihajlov@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CmpiInstance.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiDateTime.h"

#include "CmpiBaseMI.h"
#include "CmpiInstanceMI.h"
#include "CmpiAssociationMI.h"
#include "CmpiMethodMI.h"
#include "CmpiPropertyMI.h"
#include "CmpiIndicationMI.h"

#include "CmpiData.h"
#include "CmpiCharData.h"
#include "CmpiBooleanData.h"


//---------------------------------------------------
//--
//	C to C++ base provider function drivers
//--
//---------------------------------------------------

CmpiBaseMI::CmpiBaseMI(const CmpiBroker& mbp, const CmpiContext& ctx)
{
  broker = new CmpiBroker(mbp);
}

CmpiBaseMI::~CmpiBaseMI()
{
  delete broker;
}

CMPIStatus CmpiBaseMI::driveBaseCleanup
      (void* vi, CMPIContext* eCtx) {
  try {
   CMPIInstanceMI *mi=( CMPIInstanceMI*)vi;
   CmpiContext ctx(eCtx);
   CmpiStatus rc(CMPI_RC_OK);
   CmpiProviderBase::setBroker(((CmpiBaseMI*)mi->hdl)->broker->getEnc());
   rc=((CmpiBaseMI*)mi->hdl)->cleanup(ctx);
   delete (CmpiBaseMI*)mi->hdl;
   return rc.status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;

    return stat.status();
  }
}

CmpiStatus CmpiBaseMI::initialize(const CmpiContext& ctx) {
   return CmpiStatus(CMPI_RC_OK); 
}

CmpiStatus CmpiBaseMI::cleanup(CmpiContext& ctx) { 
   return CmpiStatus(CMPI_RC_OK); 
}

//---------------------------------------------------
//--
//	C to C++ instance provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiInstanceMI::driveEnumInstanceNames
   (CMPIInstanceMI* mi,CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop)
{  
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->enumInstanceNames
      (ctx,rslt,cop).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveEnumInstances
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* *properties)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->enumInstances
      (ctx,rslt,cop,(const char**)properties).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveGetInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* *properties)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->getInstance
      (ctx,rslt,cop,(const char**)properties).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveCreateInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, CMPIInstance* eInst)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiInstance inst(eInst);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->createInstance
      (ctx,rslt,cop,inst).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveSetInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, CMPIInstance* eInst, char* *properties)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiInstance inst(eInst);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->setInstance
      (ctx,rslt,cop,inst,(const char**)properties).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveDeleteInstance
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->deleteInstance
      (ctx,rslt,cop).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiInstanceMI::driveExecQuery
   (CMPIInstanceMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, char* language ,char* query)
{
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiInstanceMI*)mi->hdl)->broker->getEnc());
   return ((CmpiInstanceMI*)mi->hdl)->execQuery
    (ctx,rslt,cop,language,query).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}


//---------------------------------------------------
//--
//	Default Instance provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiInstanceMI::enumInstanceNames
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::enumInstances
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
	       const char* *properties){
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::getInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
	       const char* *properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::createInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
               const CmpiInstance& inst) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::setInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
              const  CmpiInstance& inst, const char* *properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::deleteInstance
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiInstanceMI::execQuery
              (const CmpiContext& ctx, CmpiResult& rslt, const CmpiObjectPath& cop,
               const char* language, const char* query) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ association provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiAssociationMI::driveAssociators
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole, char** properties) {
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   CmpiProviderBase::setBroker(((CmpiAssociationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiAssociationMI*)mi->hdl)->associators
      (ctx,rslt,cop,
       (const char*)assocClass,resultClass,
       role,resultRole,(const char**)properties).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiAssociationMI::driveAssociatorNames
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole) {
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   CmpiProviderBase::setBroker(((CmpiAssociationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiAssociationMI*)mi->hdl)->associatorNames
      (ctx,rslt,cop,
       assocClass,resultClass,
       role,resultRole).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiAssociationMI::driveReferences
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, const char* resultClass, const char* role ,
       char** properties) {
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   CmpiProviderBase::setBroker(((CmpiAssociationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiAssociationMI*)mi->hdl)->references
      (ctx,rslt,cop,
       resultClass,role,(const char**)properties).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiAssociationMI::driveReferenceNames
      (CMPIAssociationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eOp, const char* resultClass, const char* role) {
  try {
   CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   CmpiObjectPath cop(eOp);
   CmpiProviderBase::setBroker(((CmpiAssociationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiAssociationMI*)mi->hdl)->referenceNames
      (ctx,rslt,cop,
       resultClass,role).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}


//---------------------------------------------------
//--
//	Default Association provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiAssociationMI::associators
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole, const char** properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::associatorNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* assocClass, const char* resultClass,
       const char* role, const char* resultRole) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::references
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role ,
       const char** properties) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiAssociationMI::referenceNames
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* resultClass, const char* role) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ method provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiMethodMI::driveInvokeMethod
   (CMPIMethodMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPIObjectPath* eCop, const char* methodName,
    CMPIArgs* eIn, CMPIArgs* eOut)
{
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiArgs in(eIn);
   CmpiArgs out(eOut);
   CmpiProviderBase::setBroker(((CmpiMethodMI*)mi->hdl)->broker->getEnc());
   return ((CmpiMethodMI*)mi->hdl)->invokeMethod
      (ctx,rslt,cop,methodName,in,out).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}


//---------------------------------------------------
//--
//	Default Method provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiMethodMI::invokeMethod
              (const CmpiContext& ctx, CmpiResult& rslt,
	       const CmpiObjectPath& ref, const char* methodName,
	       const CmpiArgs& in, CmpiArgs& out) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ property provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiPropertyMI::driveSetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eCop, char* name, CMPIData eData) {
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiData data(eData);
   CmpiProviderBase::setBroker(((CmpiPropertyMI*)mi->hdl)->broker->getEnc());
   return ((CmpiPropertyMI*)mi->hdl)->setProperty
      (ctx,rslt,cop,name,data).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiPropertyMI::driveGetProperty
      (CMPIPropertyMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPIObjectPath* eCop, char* name) {
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   CmpiProviderBase::setBroker(((CmpiPropertyMI*)mi->hdl)->broker->getEnc());
   return ((CmpiPropertyMI*)mi->hdl)->getProperty
      (ctx,rslt,cop,name).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}


//---------------------------------------------------
//--
//	Default property provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiPropertyMI::setProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name, const CmpiData& data) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiPropertyMI::getProperty
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiObjectPath& op, const char* name) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


//---------------------------------------------------
//--
//	C to C++ indication provider function drivers
//--
//---------------------------------------------------

CMPIStatus CmpiIndicationMI::driveAuthorizeFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, char* user){
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   CmpiProviderBase::setBroker(((CmpiIndicationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiIndicationMI*)mi->hdl)->authorizeFilter
      (ctx,rslt,se,ns,cop,user).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}

CMPIStatus CmpiIndicationMI::driveMustPoll
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop){
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   CmpiProviderBase::setBroker(((CmpiIndicationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiIndicationMI*)mi->hdl)->mustPoll
      (ctx,rslt,se,ns,cop).status();
  } catch (CmpiStatus& stat) {
    return stat.status();
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
  }
}

CMPIStatus CmpiIndicationMI::driveActivateFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, CMPIBoolean first){
   const CmpiContext ctx(eCtx);
  try {
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   CmpiProviderBase::setBroker(((CmpiIndicationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiIndicationMI*)mi->hdl)->activateFilter
      (ctx,rslt,se,ns,cop,first).status();
  } catch (CmpiStatus& stat) {
    return stat.status();
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
  }
}

CMPIStatus CmpiIndicationMI::driveDeActivateFilter
      (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
       CMPISelectExp* eSe, char* ns, CMPIObjectPath* eCop, CMPIBoolean last){
  try {
   const CmpiContext ctx(eCtx);
   CmpiResult rslt(eRslt);
   const CmpiObjectPath cop(eCop);
   const CmpiSelectExp se(eSe);
   CmpiProviderBase::setBroker(((CmpiIndicationMI*)mi->hdl)->broker->getEnc());
   return ((CmpiIndicationMI*)mi->hdl)->deActivateFilter
      (ctx,rslt,se,ns,cop,last).status();
  } catch (CmpiStatus& stat) {
    std::cerr << "caught status :" << stat.rc() << " "  << stat.msg() << std::endl;
    return stat.status();
  }
}


//---------------------------------------------------
//--
//	Default indication provider functions
//--
//---------------------------------------------------

CmpiStatus CmpiIndicationMI::authorizeFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op, const char* user) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::mustPoll
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::activateFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
       CMPIBoolean first) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiStatus CmpiIndicationMI::deActivateFilter
      (const CmpiContext& ctx, CmpiResult& rslt,
       const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
       CMPIBoolean last) {
   return CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}






//---------------------------------------------------
//--
//	CmpiArray member functions
//--
//---------------------------------------------------

void *CmpiArray::makeArray(CMPIBroker *mb, CMPICount max, CMPIType type) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *array=mb->eft->newArray(mb,max,type,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return array;
}

CMPICount CmpiArray::size() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPICount c=getEnc()?getEnc()->ft->getSize(getEnc(),&rc):0;
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return c;
}

//---------------------------------------------------
//--
//	CmpiArrayIdx member functions
//--
//---------------------------------------------------


CmpiArrayIdx& CmpiArrayIdx::operator=(const CmpiData& v) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   if (ar.getEnc()->ft->getSimpleType(ar.getEnc(),&rc)!=v.data.type)
      throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
   rc=ar.getEnc()->ft->setElementAt(ar.getEnc(),idx,(CMPIValue*)&v.data.value,
				    v.data.type);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return *this;
}


CmpiData CmpiArrayIdx::getData() const{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIData d;
   d=ar.getEnc()->ft->getElementAt(ar.getEnc(),idx,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return d;
}

CmpiArrayIdx::operator CmpiString() const{
   return getData();
}

CmpiArrayIdx::operator const char* () const{
   return getData();
}

CmpiArrayIdx::operator CmpiDateTime() const{
   return getData();
}

CmpiArrayIdx::operator CmpiObjectPath() const{
   return getData();
}

CmpiArrayIdx::operator CmpiInstance() const{
   return getData();
}

CmpiArrayIdx::operator CMPIUint8() const{
   return getData();
}

CmpiArrayIdx::operator CMPIUint16() const{
   return getData();
}

CmpiArrayIdx::operator CMPIUint32() const{
   return getData();
}

CmpiArrayIdx::operator CMPIUint64() const{
   return getData();
}

CmpiArrayIdx::operator CMPISint8() const{
   return getData();
}

CmpiArrayIdx::operator CMPISint16() const{
   return getData();
}

CmpiArrayIdx::operator CMPISint32() const{
   return getData();
}

CmpiArrayIdx::operator CMPISint64() const{
   return getData();
}

CmpiArrayIdx::operator CMPIReal32() const{
   return getData();
}

CmpiArrayIdx::operator CMPIReal64() const{
   return getData();
}


//---------------------------------------------------
//--
//	CmpiData member functions
//--
//---------------------------------------------------

CmpiData::CmpiData(const CmpiObjectPath& d) {
  data.value.ref=(CMPIObjectPath*)d.getEnc(); 
  data.type=CMPI_ref; 
}

CmpiData::operator CmpiInstance() const{
   if (data.type!=CMPI_instance)
      throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
   else return CmpiInstance(data.value.inst);
}

CmpiData::operator CmpiObjectPath() const {
   if (data.type!=CMPI_ref)
      throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
   else return CmpiObjectPath(data.value.ref);
}

CmpiData::operator CmpiArray() const {
   if (!(data.type&CMPI_ARRAY))
      throw CmpiStatus(CMPI_RC_ERR_TYPE_MISMATCH);
   else return CmpiArray(data.value.array);
}


//---------------------------------------------------
//--
//	CmpiInstance member functions
//--
//---------------------------------------------------


void *CmpiInstance::makeInstance(CMPIBroker *mb, const CmpiObjectPath& cop) {
   CMPIStatus rc={CMPI_RC_OK,NULL};

   fprintf(stderr,"CmpiInstance::makeInstance(CMPIBroker *mb, const CmpiObjectPath& cop) called\n");
   fprintf(stderr," CMPIBroker *mb %p\n",mb);

   void *inst=mb->eft->newInstance(mb,((CmpiObjectPath&)cop).getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);

   fprintf(stderr,"CmpiInstance::makeInstance(CMPIBroker *mb, const CmpiObjectPath& cop) exited\n");
   return inst;
}

CmpiBoolean CmpiInstance::doInstanceIsA(CMPIBroker *mb, const char *className) {
   CmpiObjectPath cop=getObjectPath();
   return cop.doClassPathIsA(mb,className);
}

CmpiData CmpiInstance::getProperty(const char* name) const{
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   d.data=getEnc()->ft->getProperty(getEnc(),name,&rc);
   if (rc.rc!=CMPI_RC_OK) {
     if (rc.msg)
       throw CmpiStatus(rc);
     else
       throw CmpiStatus(rc.rc,name);
   }
   return d;
}

CmpiData CmpiInstance::getProperty(const int pos, CmpiString *name) {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s;
   d.data=getEnc()->ft->getPropertyAt(getEnc(),pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) {
     if (rc.msg)
       rc.msg=name->getEnc();
     throw CmpiStatus(rc);
   }
   if (name) *name=*(new CmpiString(s));
   return d;
};

unsigned int CmpiInstance::getPropertyCount() {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   unsigned int c=getEnc()->ft->getPropertyCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return c;
}

void CmpiInstance::setProperty(const char* name, const CmpiData data) {
   CmpiData dt=CmpiData(data);
   CMPIStatus rc=getEnc()->ft->setProperty(getEnc(),name,
      dt.data.type!=CMPI_chars ? &dt.data.value : (CMPIValue*)dt.data.value.chars,
      dt.data.type);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}


static const char * _nullkeys[] = {0};

void CmpiInstance::setPropertyFilter(const char** properties, const char** keys) {
  if (keys==0) keys = _nullkeys;
  CMPIStatus rc=getEnc()->ft->setPropertyFilter(getEnc(),(char**)properties, 
						 (char**)keys);
  if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiObjectPath CmpiInstance::getObjectPath() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CmpiObjectPath cop(getEnc()->ft->getObjectPath(getEnc(),&rc));
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return cop;
}


//---------------------------------------------------
//--
//	CmpiStatus member functions
//--
//---------------------------------------------------


CmpiStatus::CmpiStatus() {
   st.rc=CMPI_RC_OK;
   st.msg=NULL;
}

CmpiStatus::CmpiStatus(CMPIrc rcp) {
   st.rc=rcp;
   st.msg=NULL;
}

CmpiStatus::CmpiStatus(CMPIStatus stat) {
   st=stat;
}

//---------------------------------------------------
//--
//	CmpiObjectPath member functions
//--
//---------------------------------------------------



void *CmpiObjectPath::makeObjectPath(CMPIBroker *mb, const char *ns, const char *cls) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *op=mb->eft->newObjectPath(mb,ns,cls,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return op;
}

void *CmpiObjectPath::makeObjectPath(CMPIBroker *mb, const CmpiString& ns, const char *cls) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *op=mb->eft->newObjectPath(mb,CMGetCharPtr(ns.getEnc()),cls,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return op;
}

CmpiBoolean CmpiObjectPath::doClassPathIsA(CMPIBroker *mb, const char *className) const {
   CMPIStatus rc = {CMPI_RC_OK,NULL};
   CmpiBoolean bv=mb->eft->classPathIsA(mb,getEnc(),className,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return bv;
}

CmpiString CmpiObjectPath::getNameSpace() const{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s=getEnc()->ft->getNameSpace(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return *new CmpiString(s);
}

void CmpiObjectPath::setNameSpace(const char* ns) {
   CMPIStatus rc=getEnc()->ft->setNameSpace(getEnc(),ns);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiString CmpiObjectPath::getHostname() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s=getEnc()->ft->getHostname(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return *new CmpiString(s);
}

void CmpiObjectPath::setHostname(const char* hn) {
   CMPIStatus rc=getEnc()->ft->setHostname(getEnc(),hn);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiString CmpiObjectPath::getClassName() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s=getEnc()->ft->getClassName(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return *new CmpiString(s);
}

void CmpiObjectPath::setClassName(const char* cn) {
   CMPIStatus rc=getEnc()->ft->setClassName(getEnc(),cn);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiData CmpiObjectPath::getKey(const char* name) const {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   d.data=getEnc()->ft->getKey(getEnc(),name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return d;
}

unsigned int CmpiObjectPath::getKeyCount() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   unsigned int c=getEnc()->ft->getKeyCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return c;
}

CmpiData CmpiObjectPath::getKey(const int pos, CmpiString *name) {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s;
   d.data=getEnc()->ft->getKeyAt(getEnc(),(int)pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   if (name) *name=*(new CmpiString(s));
   return d;
}

void CmpiObjectPath::setKey(const char* name, const CmpiData data) {
   CmpiData dt=CmpiData(data);
   CMPIStatus rc=getEnc()->ft->addKey(getEnc(),name,
      dt.data.type!=CMPI_chars ? &dt.data.value : (CMPIValue*)dt.data.value.chars,
      dt.data.type);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}


//---------------------------------------------------
//--
//	CmpiResult member functions
//--
//---------------------------------------------------



void CmpiResult::returnData(const CmpiData& d) {
   CMPIStatus rc=getEnc()->ft->returnData(getEnc(),
      &((CmpiData)d).data.value,d.data.type);
      if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

void CmpiResult::returnData(const CmpiInstance& d) {
   CMPIStatus rc=getEnc()->ft->returnInstance(getEnc(),d.getEnc());
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

void CmpiResult::returnData(const CmpiObjectPath& d) {
   CMPIStatus rc=getEnc()->ft->returnObjectPath(getEnc(),d.getEnc());
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

void CmpiResult::returnDone() {
   CMPIStatus rc=getEnc()->ft->returnDone(getEnc());
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}



//---------------------------------------------------
//--
//	CmpiBroker member functions
//--
//---------------------------------------------------


void CmpiBroker::deliverIndication(const CmpiContext& ctx, const char*,
                                   const CmpiInstance& inst)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

     // class 1 services
CmpiEnumeration CmpiBroker::enumInstanceNames(const CmpiContext& ctx,
                                              const CmpiObjectPath& cop)
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIEnumeration* en=getEnc()->bft->enumInstanceNames
      (getEnc(),ctx.getEnc(),cop.getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiEnumeration(en);
}

CmpiInstance CmpiBroker::getInstance(const CmpiContext& ctx,
                         const CmpiObjectPath& cop,
                         const char** properties)
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIInstance* ci=getEnc()->bft->getInstance
     (getEnc(),ctx.getEnc(),cop.getEnc(),(char **)properties,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiInstance(ci);
}


     // class 2 services
CmpiObjectPath CmpiBroker::createInstance(const CmpiContext& ctx,
                              const CmpiObjectPath& cop,
                              const CmpiInstance& inst)
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIObjectPath* co=getEnc()->bft->createInstance
     (getEnc(),ctx.getEnc(),cop.getEnc(),inst.getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiObjectPath(co);
}

void CmpiBroker::setInstance(const CmpiContext& ctx, const CmpiObjectPath& cop,
                             const CmpiInstance& inst)
{
   CMPIStatus rc=getEnc()->bft->setInstance
     (getEnc(),ctx.getEnc(),cop.getEnc(),inst.getEnc());
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

void CmpiBroker::deleteInstance(const CmpiContext& ctx, const CmpiObjectPath& cop)
{
   CMPIStatus rc=getEnc()->bft->deleteInstance
     (getEnc(),ctx.getEnc(),cop.getEnc());
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiEnumeration CmpiBroker::execQuery(const CmpiContext& ctx,
                          const  CmpiObjectPath& cop,
                          const char* query, const char* language)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}


CmpiEnumeration CmpiBroker::enumInstances(const CmpiContext& ctx,
                              const CmpiObjectPath& cop,
                              const char** properties)
{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIEnumeration* en=getEnc()->bft->enumInstances
      (getEnc(),ctx.getEnc(),cop.getEnc(),(char**)properties,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiEnumeration(en);
}

CmpiEnumeration CmpiBroker::associators(const CmpiContext& ctx,
                            const CmpiObjectPath& cop,
                            const char* assocClass, const char* resultClass,
		            const char* role, const char* resultRole,
			    const char** properties)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::associatorNames(const CmpiContext& ctx,
                                const CmpiObjectPath& cop,
                                const char* assocClass, const char* resultClass,
		                const char* role, const char* resultRole)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::references(const CmpiContext& ctx,
                           const CmpiObjectPath& cop,
                           const char* resultClass, const char* role,
			   const char** properties)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiEnumeration CmpiBroker::referenceNames(const CmpiContext& ctx,
                               const CmpiObjectPath& cop,
                               const char* resultClass, const char* role)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiData CmpiBroker::invokeMethod(const CmpiContext& ctx, const CmpiObjectPath& cop,
                 const char* methName, const CmpiArgs& in, CmpiArgs& out)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

void CmpiBroker::setProperty(const CmpiContext& ctx, const CmpiObjectPath& cop,
                 const char* name, const CmpiData& data)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}

CmpiData CmpiBroker::getProperty(const CmpiContext& ctx, const CmpiObjectPath& cop,
                  const char* name)
{
   throw CmpiStatus(CMPI_RC_ERR_NOT_SUPPORTED);
}




//---------------------------------------------------
//--
//	CmpiString member functions
//--
//---------------------------------------------------




CmpiString::CmpiString(const CmpiString& s) {
  enc=CMNewString(CmpiProviderBase::getBroker(),(char*)s.charPtr(),NULL);
}



//---------------------------------------------------
//--
//	CmpiArgs member functions
//--
//---------------------------------------------------




void *CmpiArgs::makeArgs(CMPIBroker *mb) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *args=mb->eft->newArgs(mb,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return args;
}

void CmpiArgs::setArg(const char* name, CmpiData& data) {
   CMPIStatus rc=getEnc()->ft->addArg(getEnc(),name,
         data.data.type!=CMPI_chars ? &data.data.value
	                            : (CMPIValue*)data.data.value.chars,
         data.data.type);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
}

CmpiData CmpiArgs::getArg(const int pos, CmpiString *name) const {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *s;
   d.data=getEnc()->ft->getArgAt(getEnc(),(int)pos,&s,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   if (name) *name=*(new CmpiString(s));
   return d;
}

CmpiData CmpiArgs::getArg(const char* name) const {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   d.data=getEnc()->ft->getArg(getEnc(),name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return d;
}

unsigned int CmpiArgs::getArgCount() const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   unsigned int c=getEnc()->ft->getArgCount(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return c;
}



//---------------------------------------------------
//--
//	CmpiObject member functions
//--
//---------------------------------------------------




CmpiString CmpiObject::doToString(CMPIBroker *mb) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIString *str=mb->eft->toString(mb,enc,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiString(str);
}

CmpiBoolean CmpiObject::doIsA(CMPIBroker *mb, const char *typeName) const {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CmpiBoolean bv=mb->eft->isOfType(mb,enc,typeName,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return bv;
}

//---------------------------------------------------
//--
//	CmpiEnumeration member functions
//--
//---------------------------------------------------


CmpiBoolean CmpiEnumeration::hasNext() {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CmpiBoolean bv=getEnc()->ft->hasNext(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return bv;
}

CmpiData CmpiEnumeration::getNext() {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIData d=getEnc()->ft->getNext(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiData(d);
}

CmpiData CmpiEnumeration::toArray() {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIArray* a=getEnc()->ft->toArray(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return CmpiData(CmpiArray(a));
}


//---------------------------------------------------
//--
//	CmpiContext member functions
//--
//---------------------------------------------------



const char *CmpiContext::invocationFlags=CMPIInvocationFlags;

CmpiData CmpiContext::getEntry(const char* name) {
   CmpiData d;
   CMPIStatus rc={CMPI_RC_OK,NULL};
   d.data=getEnc()->ft->getEntry(getEnc(),name,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return d;
}



//---------------------------------------------------
//--
//	CmpiDateTime member functions
//--
//---------------------------------------------------


void *CmpiDateTime::makeDateTime(CMPIBroker *mb) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *dt=mb->eft->newDateTime(mb,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return dt;
}

void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const char* utcTime) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *dt=mb->eft->newDateTimeFromChars(mb,(char*)utcTime,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return dt;
}

void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const CMPIUint64 binTime,
                                 const CmpiBoolean interval) {
   CMPIStatus rc={CMPI_RC_OK,NULL};
   void *dt=mb->eft->newDateTimeFromBinary(mb,binTime,interval,&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return dt;
}

CmpiBoolean CmpiDateTime::isInterval() const{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CmpiBoolean bv=getEnc()->ft->isInterval(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return bv;
}

CMPIUint64 CmpiDateTime::getDateTime() const{
   CMPIStatus rc={CMPI_RC_OK,NULL};
   CMPIUint64 rv=getEnc()->ft->getBinaryFormat(getEnc(),&rc);
   if (rc.rc!=CMPI_RC_OK) throw CmpiStatus(rc);
   return rv;
};


CmpiBooleanData CmpiTrue(true);
CmpiBooleanData CmpiFalse(false);

//-----------------------------------------------------------
//--
//      per thread support
//--
//-----------------------------------------------------------

static pthread_once_t brokerOnce = PTHREAD_ONCE_INIT;
static pthread_key_t brokerKey;
static void init_key()
{
  pthread_key_create(&brokerKey,NULL);
}

CMPIBroker* CmpiProviderBase::
   getBroker()
{
  return (CMPIBroker*) pthread_getspecific(brokerKey);
}

void CmpiProviderBase::
    setBroker(const CMPIBroker *mb)
{
  pthread_once(&brokerOnce,init_key);
  pthread_setspecific(brokerKey,mb);
}

