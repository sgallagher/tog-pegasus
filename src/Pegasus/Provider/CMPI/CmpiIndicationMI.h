
/*
 *
 * CmpiIndicationMI.h
 *
 * (C) Copyright IBM Corp. 2002
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:        Adrian Schuur <schuur@de.ibm.com>
 * Contributors:
 *
 * Description: CMPI C++ indication provider wrapper
 *
 */

#ifndef _CmpiIndicationMI_h_
#define _CmpiIndicationMI_h_

#include <iostream>

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiBaseMI.h"
#include "CmpiStatus.h"
#include "CmpiObjectPath.h"
#include "CmpiResult.h"
#include "CmpiContext.h"
#include "CmpiSelectExp.h"
#include "Linkage.h"


class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiIndicationMI : virtual public CmpiBaseMI {
public:
   CmpiIndicationMI(const CmpiBroker &mbp, const CmpiContext& ctx) :
   CmpiBaseMI (mbp,ctx) {
   }

   static CMPIStatus driveAuthorizeFilter
   (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPISelectExp* se, char* ns, CMPIObjectPath* op, char* user);
   static CMPIStatus driveMustPoll
   (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRrslt,
    CMPISelectExp* se, char* ns, CMPIObjectPath* op);
   static CMPIStatus driveActivateFilter
   (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPISelectExp* se, char* ns, CMPIObjectPath* op, CMPIBoolean first);
   static CMPIStatus driveDeActivateFilter
   (CMPIIndicationMI* mi, CMPIContext* eCtx, CMPIResult* eRslt,
    CMPISelectExp* se, char* ns, CMPIObjectPath* op, CMPIBoolean last);

   virtual CmpiStatus authorizeFilter
   (const CmpiContext& ctx, CmpiResult& rslt,
    const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
    const char* user);
   virtual CmpiStatus mustPoll
   (const CmpiContext& ctx, CmpiResult& rslt,
    const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op);
   virtual CmpiStatus activateFilter
   (const CmpiContext& ctx, CmpiResult& rslt,
    const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
    CmpiBoolean first);
   virtual CmpiStatus deActivateFilter
   (const CmpiContext& ctx, CmpiResult& rslt,
    const CmpiSelectExp& se, const char* ns, const CmpiObjectPath& op,
    CmpiBoolean last);
};

#endif

