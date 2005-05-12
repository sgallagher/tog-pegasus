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
// Based on: RT_IndicationProvider.cpp by Carol Ann Krug Graves, Hewlett-Packard Company
//                                        (carolann_graves@hp.com)

// Author: Adrian Schuur, IBM (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include <time.h>

#define CMPI_VER_86 1

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

static CMPIBroker * broker;

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;

static int enabled=0;
static int _nextUID = 0;


static void generateIndication(char *methodname, CMPIContext *ctx)
{

   CMPIInstance *inst;
   CMPIObjectPath *cop;
   CMPIDateTime *dat;
   CMPIArray *ar;
   CMPIStatus rc;

   if (enabled) {

      fprintf(stderr,"+++ generateIndication()\n");

      cop=CMNewObjectPath(broker,"root/cimv2","CMPI_RT_SampleIndication",&rc);
      inst=CMNewInstance(broker,cop,&rc);

      char buffer[32];
      sprintf(buffer, "%d", _nextUID++);
      CMSetProperty(inst,"IndicationIdentifier",buffer,CMPI_chars);

      dat=CMNewDateTime(broker,&rc);
      CMSetProperty(inst,"IndicationTime",&dat,CMPI_dateTime);

      CMSetProperty(inst,"MethodName",methodname,CMPI_chars);

      ar=CMNewArray(broker,0,CMPI_string,&rc);
      CMSetProperty(inst,"CorrelatedIndications",&ar,CMPI_stringA);

      CMAddContextEntry(ctx,"SnmpTrapOidContainer","1.3.6.1.4.1.900.2.3.9002.9600",CMPI_chars);

      CBDeliverIndication(broker,ctx,"root/cimv2",inst);
   }
   fprintf(stderr,"+++ generateIndication() done\n");
}

//----------------------------------------------------------
//---
//	Method Provider
//---
//----------------------------------------------------------


CMPIStatus testProvMethodCleanup(CMPIMethodMI* cThis, CMPIContext *ctx) {
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvInvokeMethod
                (CMPIMethodMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, char* method, CMPIArgs *in, CMPIArgs *out) {

   fprintf(stderr,"+++ testProvInvokeMethod()\n");

   if (enabled==0)
      fprintf(stderr,"+++ PROVIDER NOT ENABLED\n");

   else generateIndication(method,ctx);

   CMReturn(CMPI_RC_OK);
}

//----------------------------------------------------------
//---
//	Indication Provider
//---
//----------------------------------------------------------


CMPIStatus testProvIndicationCleanup(CMPIIndicationMI *cThis, CMPIContext *ctx) {
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvAuthorizeFilter
                (CMPIIndicationMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPISelectExp *filter, char *indType, CMPIObjectPath *classPath,
		 char *owner) {

   CMReturnData(rslt,&CMPI_true,CMPI_boolean);
   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvMustPoll
                (CMPIIndicationMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPISelectExp *filter, char *indType, CMPIObjectPath *classPath) {

   CMReturnData(rslt,&CMPI_false,CMPI_boolean);
   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvActivateFilter
                (CMPIIndicationMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPISelectExp *exp, char *indType, CMPIObjectPath *classPath,
		 CMPIBoolean firstActivation) {
/*
   CMPISelectCond* cond;
   CMPISubCond *subc;
   CMPIPredicate *prd;
   CMPIString* filter,*lhs,*rhs;
   CMPICount c;
   int type;

   fprintf(stderr,"+++ testProvActivateFilter()\n");

   filter=CMGetSelExpString(exp,NULL);
   fprintf(stderr,"--- %s\n",CMGetCharsPtr(filter,NULL));

   cond=CMGetDoc(exp,NULL);
   c=CMGetSubCondCountAndType(cond,&type,NULL);
   fprintf(stderr,"--- Count: %d type: %d\n",c,type);

//   for (int i=0,m=c; i<m; i++) {
      subc=CMGetSubCondAt(cond,0,NULL);
//   }
      fprintf(stderr,"--- Count: %d\n",CMGetPredicateCount(subc,NULL));
   prd=CMGetPredicateAt(subc,0,NULL);

   CMGetPredicateData(prd,NULL,NULL,&lhs,&rhs);
   fprintf(stderr,"--- %s %s\n",CMGetCharsPtr(lhs,NULL),CMGetCharsPtr(rhs,NULL));
*/

   fprintf(stderr,"+++ testProvActivateFilter()\n");

   enabled++;
   fprintf(stderr,"--- enabled: %d\n",enabled);

   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvDeActivateFilter
                (CMPIIndicationMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPISelectExp *filter, char *indType, CMPIObjectPath *classPath,
		 CMPIBoolean lastActivation) {
   fprintf(stderr,"+++ testProvDeActivateFilter\n");
   CMReturn(CMPI_RC_OK);
}

void testProvEnableIndications(CMPIIndicationMI* cThis) {
   fprintf(stderr,"+++ testProvEnableIndications\n");
}

void testProvDisableIndications(CMPIIndicationMI* cThis) {
   fprintf(stderr,"+++ testProvDisableIndications\n");
}


//----------------------------------------------------------
//---
//	Provider Factory Stubs
//---
//----------------------------------------------------------

CMMethodMIStub(testProv,CMPI_RT_SampleProvider,broker,CMNoHook);
//   fprintf(stderr,"+++ RT_Sample_Create_MethodMI(): called\n"));

//----------------------------------------------------------

CMIndicationMIStub(testProv,CMPI_RT_SampleProvider,broker,CMNoHook);
//   fprintf(stderr,"+++ RT_Sample_Create_IndicationMI(): called\n"));

//----------------------------------------------------------
