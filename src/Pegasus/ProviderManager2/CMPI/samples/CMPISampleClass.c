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
// Author: Adrian Schuur, IBM (schuur@de.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#define CMStringCharPtr(s) CMGetCharsPtr(s,NULL)

#include <string.h>
#include <time.h>

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>

unsigned char CMPI_true=1;
unsigned char CMPI_false=0;

int dataNext=0;
typedef struct {
   CMPIString *key;
   CMPIString *data;
} Data;

Data store[64];

CMPIBroker* broker;

int addToStore(CMPIString *k, CMPIString *d)
{
   int i;
   for (i=0; i<dataNext; i++)
      if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) return 0;
   store[dataNext].key=k;
   store[dataNext++].data=d;
   return 1;
}

CMPIString* getFromStore(CMPIString *k)
{
   int i;
   for (i=0; i<dataNext; i++)
      if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) return store[i].data;
   return NULL;
}

int replInStore(CMPIString *k, CMPIString *d)
{
   int i;
   for (i=0; i<dataNext; i++)
      if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) {
         store[i].data=d;
	 return 1;
      }
   return 0;
}

int remFromStore(CMPIString *k)
{
   int i;
   for (i=0; i<dataNext; i++)
     if (strcmp(CMStringCharPtr(k),CMStringCharPtr(store[i].key))==0) {
	for (; i<dataNext; i++) {
	  store[i].key=store[i+1].key;
	  store[i].data=store[i+1].data;
	}
	--dataNext;
	return 1;
     }
   return 0;
}



//----------------------------------------------------------
//---
//	Instance Provider
//---
//----------------------------------------------------------


CMPIStatus testProvCleanup(CMPIInstanceMI* cThis, CMPIContext *ctx) {
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvEnumInstanceNames
                (CMPIInstanceMI* cThis, CMPIContext* ctx,CMPIResult* rslt,
                 CMPIObjectPath* ref)
{
   int i;
   CMPIStatus rc;
   CMPIObjectPath *cop, *copClone;
   fprintf(stderr,"+++ testProvEnumInstanceNames()\n");

   cop=CMNewObjectPath(broker,CMGetCharPtr(CMGetNameSpace(ref,&rc)),
			      CMGetCharPtr(CMGetClassName(ref,&rc)),&rc);
   for (i=0; i<dataNext; i++)
   {
      copClone=CMClone(cop,&rc);
      CMAddKey(copClone,"Identifier",&store[i].key,CMPI_string);
      CMReturnObjectPath(rslt,copClone);
   }
   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvEnumInstances
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* ref, char **properties) {
   int i;
   CMPIStatus rc;
   CMPIObjectPath *cop, *copClone;
   CMPIInstance * inst;
   fprintf(stderr,"+++ testProvEnumInstances()\n");

   cop=CMNewObjectPath(broker,CMGetCharPtr(CMGetNameSpace(ref,&rc)),
			      CMGetCharPtr(CMGetClassName(ref,&rc)),&rc);
   for (i=0; i<dataNext; i++)
   {
      copClone=CMClone(cop,&rc);
      CMAddKey(copClone,"Identifier",&store[i].key,CMPI_string);
      inst=CMNewInstance(broker,copClone,&rc);
      CMSetProperty(inst,"Identifier",&store[i].key,CMPI_string);
      CMSetProperty(inst,"data",&store[i].data,CMPI_string);
      CMReturnInstance(rslt,inst);
   }
   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvGetInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, char **properties) {
   CMPIString *d,*k;
   CMPIStatus rc;
   CMPIInstance *inst;

   k=cop->ft->getKey(cop,"Identifier",&rc).value.string;
   fprintf(stderr,"+++ testProvGetInstance()\n");

   if ((d=getFromStore(k))!=NULL) {
      inst=CMNewInstance(broker,cop,&rc);
      CMSetProperty(inst,"Identifier",&k,CMPI_string);
      CMSetProperty(inst,"data",&d,CMPI_string);
      CMReturnInstance(rslt,inst);
   }
   else CMReturn(CMPI_RC_ERR_NOT_FOUND);

   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvCreateInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, CMPIInstance* inst) {
   CMPIString *k,*d;
   CMPIStatus rc;

   fprintf(stderr,"+++ testProvCreateInstance()\n");

   k=CMGetProperty(inst,"Identifier",&rc).value.string;
   d=CMGetProperty(inst,"data",&rc).value.string;

   if (addToStore(k,d)==0){
      fprintf(stderr,"+++ testProvCreateInstance() already exists\n");
      CMReturn(CMPI_RC_ERR_ALREADY_EXISTS);
   }

   CMReturnObjectPath(rslt,cop);
   CMReturnDone(rslt);
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvSetInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, CMPIInstance* inst, char **properties) {
   CMReturn(CMPI_RC_OK);
}

CMPIStatus testProvDeleteInstance
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop) {
   CMPIString *k;
   CMPIStatus rc;

   k=CMGetKey(cop,"Identifier",&rc).value.string;
   if (remFromStore(k)) { CMSetStatus(&rc,CMPI_RC_OK); }
   else CMSetStatus(&rc,CMPI_RC_ERR_NOT_FOUND);

   return rc;
}

CMPIStatus testProvExecQuery
                (CMPIInstanceMI* cThis, CMPIContext* ctx, CMPIResult* rslt,
                 CMPIObjectPath* cop, char* lang, char* query) {
   CMReturn(CMPI_RC_OK);
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
   CMPIStatus rc;
   char data[128];
   fprintf(stderr,"+++ testProvInvokeMethod()\n");

   strcpy(data,"hallo! dir: ");
   strcat(data,(char*)(CMGetArg(in,"dir",&rc).value.string->hdl));
   strcat(data," type: ");
   strcat(data,(char*)(CMGetArg(in,"type",&rc).value.string->hdl));

   CMReturnData(rslt,data,CMPI_chars);
   CMReturnDone(rslt);

   CMReturn(CMPI_RC_OK);
}

//----------------------------------------------------------
//---
//	Provider Factory Stubs
//---
//----------------------------------------------------------

CMInstanceMIStub(testProv,CMPISample,broker,CMNoHook);
//   fprintf(stderr,"+++ CMPISample_Create_InstanceMI(): called\n"));

//----------------------------------------------------------

CMMethodMIStub(testProv,CMPISample,broker,CMNoHook);
//   fprintf(stderr,"+++ CMPISample_Create_MethodMI(): called\n"));

//----------------------------------------------------------//----------------------------------------------------------



