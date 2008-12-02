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

#include "CMPI_ObjectPath.h"

#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"
#include <Pegasus/Common/Tracer.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C"
{
    static CMPIStatus refRelease(CMPIObjectPath* eRef)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (ref)
        {
            delete ref;
            (reinterpret_cast<CMPI_Object*>(eRef))->unlinkAndDelete();
            CMReturn(CMPI_RC_OK);
        }
        CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
    }

    static CMPIStatus refReleaseNop(CMPIObjectPath* eRef)
    {
        CMReturn(CMPI_RC_OK);
    }

    static CMPIObjectPath* refClone(const CMPIObjectPath* eRef, CMPIStatus* rc)
    {
        CIMObjectPath *ref=(CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received invalid handle in CMPIObjectPath:refClone");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        CIMObjectPath *nRef = new CIMObjectPath(
            ref->getHost(),
            ref->getNameSpace(),
            ref->getClassName());

        Array<CIMKeyBinding> kb = ref->getKeyBindings();
        nRef->setKeyBindings(kb);
        CMPI_Object* obj = new CMPI_Object(nRef);
        obj->unlink();
        CMPIObjectPath* neRef = reinterpret_cast<CMPIObjectPath*>(obj);
        CMSetStatus(rc, CMPI_RC_OK);
        return neRef;
    }

    static CMPIStatus refSetNameSpace(CMPIObjectPath* eRef, const char *ns)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refSetNameSpace");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (!ns)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid Parameter in \
                CMPIObjectPath:refSetNameSpace");
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        ref->setNameSpace(String(ns));
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetNameSpace(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetNameSpace");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const CIMNamespaceName &ns = ref->getNameSpace();
        CMPIString *eNs = (CMPIString*)string2CMPIString(ns.getString());
        CMSetStatus(rc, CMPI_RC_OK);
        return eNs;
    }

    static CMPIStatus refSetHostname(CMPIObjectPath* eRef, const char *hn)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refSetHostName");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (!hn)
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid parameter %s in \
                CMPIObjectPath:refSetHostName", hn));
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        ref->setHost(String(hn));
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetHostname(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetHostName");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const String &hn = ref->getHost();
        CMPIString *eHn = (CMPIString*)string2CMPIString(hn);
        CMSetStatus(rc, CMPI_RC_OK);
        return eHn;
    }

    static CMPIStatus refSetClassName(CMPIObjectPath* eRef, const char *cn)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refSetClassName");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (!cn)
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid parameter %s in \
                CMPIObjectPath:refSetClassName", cn));
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        ref->setClassName(String(cn));
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetClassName(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetClassName");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const CIMName &cn = ref->getClassName();
        CMPIString* eCn = (CMPIString*)string2CMPIString(cn.getString());
        CMSetStatus(rc, CMPI_RC_OK);
        return eCn;
    }


    static long locateKey(const Array<CIMKeyBinding> &kb, const CIMName &eName)
    {
        for (unsigned long i=0,s=kb.size(); i<s; i++)
        {
            const String &n = kb[i].getName().getString();
            if (String::equalNoCase(n,eName.getString()))
            {
                return i;
            }
        }
        return -1;
    }

    static CMPIStatus refAddKey(
        CMPIObjectPath* eRef,
        const char *name,
        const CMPIValue* data,
        const CMPIType type)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refAddKey");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (!name)
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid parameter %s in \
                CMPIObjectPath:refAddKey", name));
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        Array<CIMKeyBinding> keyBindings = ref->getKeyBindings();
        CIMName key(name);
        CMPIrc rc;

        long i = locateKey(keyBindings,key);
        if (i >= 0)
        {
            keyBindings.remove(i);
        }

        CIMValue val = value2CIMValue(data,type,&rc);
        keyBindings.append(CIMKeyBinding(key,val));
        ref->setKeyBindings(Array<CIMKeyBinding>(keyBindings));
        CMReturn(CMPI_RC_OK);
    }

    static CMPIData refGetKey(
        const CMPIObjectPath* eRef,
        const char *name,
        CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        CMPIData data = {0, CMPI_nullValue | CMPI_notFound, {0}};

        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetKey");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }
        if (!name)
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid parameter %s in \
                CMPIObjectPath:refGetKey", name));
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return data;
        }
        const CIMName eName(name);
        const Array<CIMKeyBinding> &akb = ref->getKeyBindings();
        CMSetStatus(rc, CMPI_RC_OK);

        long i = locateKey(akb,eName);
        if (i >= 0)
        {
            key2CMPIData(akb[i].getValue(),akb[i].getType(),&data);
            return data;
        }
        CMSetStatus(rc, CMPI_RC_ERR_NOT_FOUND);
        return data;
    }

    static CMPIData refGetKeyAt(
        const CMPIObjectPath* eRef,
        unsigned pos,
        CMPIString** name,
        CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        CMPIData data = {0, CMPI_nullValue | CMPI_notFound, {0}};

        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetKeyAt");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }

        const Array<CIMKeyBinding> &akb = ref->getKeyBindings();
        CMSetStatus(rc, CMPI_RC_OK);

        if (pos >= akb.size())
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Property Not Found - CMPIObjectPath:refGetKeyAt");
            CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
            return data;
        }

        key2CMPIData(akb[pos].getValue(),akb[pos].getType(),&data);

        if (name)
        {
            const String &n = akb[pos].getName().getString();
            *name = (CMPIString*)string2CMPIString(n);
        }
        return data;
    }

    static CMPICount refGetKeyCount(const CMPIObjectPath* eRef, CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetKeyCount");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return 0;
        }
        const Array<CIMKeyBinding> &akb = ref->getKeyBindings();
        CMSetStatus(rc, CMPI_RC_OK);
        return akb.size();
    }

    static CMPIStatus refSetNameSpaceFromObjectPath(
        CMPIObjectPath* eRef,
        const CMPIObjectPath* eSrc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        CIMObjectPath* src = (CIMObjectPath*)eSrc->hdl;
        if (!ref || !src)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in \
                CMPIObjectPath:refSetNameSpaceFromObjectPath");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        ref->setNameSpace(src->getNameSpace());
        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus refSetHostAndNameSpaceFromObjectPath(
        CMPIObjectPath* eRef,
        const CMPIObjectPath* eSrc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        CIMObjectPath* src = (CIMObjectPath*)eSrc->hdl;
        if (!ref || !src)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in \
                CMPIObjectPath:refSetHostAndNameSpaceFromObjectPath");
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        ref->setNameSpace(src->getNameSpace());
        ref->setHost(src->getHost());
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString *refToString(const CMPIObjectPath* eRef, CMPIStatus* rc)
    {
        CIMObjectPath* ref = (CIMObjectPath*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refToString");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        String str = ref->toString();
        CMSetStatus(rc, CMPI_RC_OK);
        return reinterpret_cast<CMPIString*>(new CMPI_Object(str));
    }

}

CMPIObjectPathFT objectPath_FT =
{
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
    NULL,
    refToString
};

CMPIObjectPathFT *CMPI_ObjectPath_Ftab = &objectPath_FT;

CMPIObjectPathFT objectPathOnStack_FT =
{
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
    NULL,
    refToString
};

CMPIObjectPathFT *CMPI_ObjectPathOnStack_Ftab = &objectPathOnStack_FT;


CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const CIMObjectPath& cop)
{
    hdl = (void*)&cop;
    ft = CMPI_ObjectPathOnStack_Ftab;
}


PEGASUS_NAMESPACE_END

