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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_ObjectPath.h"

#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

static CMPIStatus refRelease(CMPIObjectPath* eRef) {
//   cout<<"--- refRelease()"<<endl;
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   if (ref) {
      delete ref;
      ((CMPI_Object*)eRef)->unlinkAndDelete();
   }
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus refReleaseNop(CMPIObjectPath* eRef) {
   CMReturn(CMPI_RC_OK);
}

static CMPIObjectPath* refClone(CMPIObjectPath* eRef, CMPIStatus* rc) {
//   cout<<"--- refClone()"<<endl;
   CIMObjectPath *ref=(CIMObjectPath*)eRef->hdl;
   CIMObjectPath *nRef=new CIMObjectPath(ref->getHost(),
                                          ref->getNameSpace(),
					  ref->getClassName());
   Array<KeyBinding> kb=ref->getKeyBindings();
   nRef->setKeyBindings(kb);
   CMPIObjectPath* neRef=(CMPIObjectPath*)new CMPI_Object(nRef,CMPI_ObjectPath_Ftab);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return neRef;
}

static CMPIStatus refSetNameSpace(CMPIObjectPath* eRef, char* ns) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   ref->setNameSpace(String(ns));
   CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetNameSpace(CMPIObjectPath* eRef, CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const CIMNamespaceName &ns=ref->getNameSpace();
   CMPIString *eNs=(CMPIString*)string2CMPIString(ns.getString());
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return eNs;
}

static CMPIStatus refSetHostname(CMPIObjectPath* eRef, char* hn) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   ref->setHost(String(hn));
   CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetHostname(CMPIObjectPath* eRef, CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const String &hn=ref->getHost();
   CMPIString *eHn=(CMPIString*)string2CMPIString(hn);
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return eHn;
}

static CMPIStatus refSetClassName(CMPIObjectPath*,char*) {
   CMReturn(CMPI_RC_OK);
}

static CMPIString* refGetClassName(CMPIObjectPath* eRef, CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const CIMName &cn=ref->getClassName();
   CMPIString* eCn=(CMPIString*)string2CMPIString(cn.getString());
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return eCn;
}


static long locateKey(const Array<KeyBinding> &kb, const CIMName &eName) {
   for (ulong i=0,s=kb.size(); i<s; i++) {
      const String &n=kb[i].getName();
      if (String::equalNoCase(n,eName)) return i;
   }
   return -1;
}

static CMPIStatus refAddKey(CMPIObjectPath* eRef, char* name,
          CMPIValue* data, CMPIType type) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   Array<KeyBinding> keyBindings=ref->getKeyBindings();
   CIMName key(name);
   CMPIrc rc;

   long i=locateKey(keyBindings,key);
   if (i>=0) keyBindings.remove(i);

   CIMValue val=value2CIMValue(data,type,&rc);
   keyBindings.append(KeyBinding(key,val));
   ref->setKeyBindings(Array<KeyBinding>(keyBindings));
   CMReturn(CMPI_RC_OK);
}

static CMPIData refGetKey(CMPIObjectPath* eRef, char* name, CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const CIMName eName(name);
   const Array<KeyBinding> &akb=ref->getKeyBindings();
   CMPIData data={0,0,{0}};
   if (rc) CMSetStatus(rc,CMPI_RC_OK);

   long i=locateKey(akb,eName);
   if (i>=0)  {
       key2CMPIData(akb[i].getValue(),akb[i].getType(),&data);
       return data;
   }
   if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
   return data;
}

static CMPIData refGetKeyAt(CMPIObjectPath* eRef, unsigned pos, CMPIString** name,
          CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const Array<KeyBinding> &akb=ref->getKeyBindings();
   CMPIData data={0,0,{0}};
   if (rc) CMSetStatus(rc,CMPI_RC_OK);

   if (pos>=akb.size()) {
      if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_FOUND);
      return data;
   }

   key2CMPIData(akb[pos].getValue(),akb[pos].getType(),&data);

   if (name) {
      const String &n=akb[pos].getName();
      *name=(CMPIString*)string2CMPIString(n);
   }
   return data;
}

static CMPICount refGetKeyCount(CMPIObjectPath* eRef, CMPIStatus* rc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   const Array<KeyBinding> &akb=ref->getKeyBindings();
   if (rc) CMSetStatus(rc,CMPI_RC_OK);
   return akb.size();
}

static CMPIStatus refSetNameSpaceFromObjectPath(CMPIObjectPath* eRef,
          CMPIObjectPath* eSrc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   CIMObjectPath* src=(CIMObjectPath*)eSrc->hdl;
   ref->setNameSpace(src->getNameSpace());
   CMReturn(CMPI_RC_OK);
}

static CMPIStatus refSetHostAndNameSpaceFromObjectPath(CMPIObjectPath* eRef,
          CMPIObjectPath* eSrc) {
   CIMObjectPath* ref=(CIMObjectPath*)eRef->hdl;
   CIMObjectPath* src=(CIMObjectPath*)eSrc->hdl;
   ref->setNameSpace(src->getNameSpace());
   ref->setHost(src->getHost());
   CMReturn(CMPI_RC_OK);
}

CMPIObjectPathFT objectPath_FT={
     CMPICurrentVersion,
     refRelease,
     refClone,
     refSetNameSpace,
     refGetNameSpace,
     refSetHostname,
     refGetHostname,
     refSetClassName,
     refGetClassName,
     refAddKey,
     refGetKey,
     refGetKeyAt,
     refGetKeyCount,
     refSetNameSpaceFromObjectPath,
     refSetHostAndNameSpaceFromObjectPath,
     NULL,
     NULL,
     NULL,
     NULL
};

CMPIObjectPathFT *CMPI_ObjectPath_Ftab=&objectPath_FT;

CMPIObjectPathFT objectPathOnStack_FT={
     CMPICurrentVersion,
     refReleaseNop,
     refClone,
     refSetNameSpace,
     refGetNameSpace,
     refSetHostname,
     refGetHostname,
     refSetClassName,
     refGetClassName,
     refAddKey,
     refGetKey,
     refGetKeyAt,
     refGetKeyCount,
     refSetNameSpaceFromObjectPath,
     refSetHostAndNameSpaceFromObjectPath,
     NULL,
     NULL,
     NULL,
     NULL
};

CMPIObjectPathFT *CMPI_ObjectPathOnStack_Ftab=&objectPathOnStack_FT;


CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const CIMObjectPath& cop) {
      hdl=(void*)&cop;
      ft=CMPI_ObjectPathOnStack_Ftab;
   }


PEGASUS_NAMESPACE_END

