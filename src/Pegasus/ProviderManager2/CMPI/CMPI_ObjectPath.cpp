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
#include "CMPISCMOUtilities.h"
#include <Pegasus/Common/Tracer.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C"
{
    static CMPIStatus refRelease(CMPIObjectPath* eRef)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        //fprintf(stderr, "refRelease(%p)\n",ref);
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
        SCMOInstance *ref=(SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received invalid handle in CMPIObjectPath:refClone");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }

        try
        {
            // Since we make no difference between ObjectPath and Instance,
            // we simply clone using the ObjectPathOnly option.
            SCMOInstance* nRef = new SCMOInstance(ref->clone(true));
            CMPIObjectPath* cmpiObjPath =
                reinterpret_cast<CMPIObjectPath *>(new CMPI_Object(nRef));
            CMSetStatus(rc,CMPI_RC_OK);
            return cmpiObjPath;
        }
        catch (const PEGASUS_STD(bad_alloc)&)
        {
            CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
            return NULL;
        }
    }

    static CMPIStatus refSetNameSpace(CMPIObjectPath* eRef, const char *ns)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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

        // --rk-->TBD: Implement this
        /*const char* prevNamespace = ref->getNameSpace();
        const char* className = ref->getClassName();

        if (prevNamespace && 
            0==strcasecmp(prevNamespace,ns)
        {
        }


        ref->setNameSpace(String(ns));*/
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetNameSpace(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetNameSpace");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const char *ns = ref->getNameSpace();
        CMPIString *eNs = string2CMPIString(ns);
        CMSetStatus(rc, CMPI_RC_OK);
        return eNs;
    }

    static CMPIStatus refSetHostname(CMPIObjectPath* eRef, const char *hn)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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
        ref->setHostName(hn);
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetHostname(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetHostName");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const char* hn = ref->getHostName();
        CMPIString *eHn = string2CMPIString(hn); 
        CMSetStatus(rc, CMPI_RC_OK);
        return eHn;
    }

    static CMPIStatus refSetClassName(CMPIObjectPath* eRef, const char *cn)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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
        // --rk-->TBD: Implement this
        //ref->setClassName(String(cn));
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* refGetClassName(
        const CMPIObjectPath* eRef,
        CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetClassName");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }
        const char* cn = ref->getClassName();
        CMPIString* eCn = string2CMPIString(cn); 
        CMSetStatus(rc, CMPI_RC_OK);
        return eCn;
    }


    static CMPIStatus refAddKey(
        CMPIObjectPath* eRef,
        const char *name,
        const CMPIValue* data,
        const CMPIType type)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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


        if ((type & CMPI_ARRAY) || (type == CMPI_instance))
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid type %X for parameter %s in \
                CMPIObjectPath:refAddKey", type, name));
            CMReturn(CMPI_RC_ERR_INVALID_DATA_TYPE);
        }

        CIMType cimType=type2CIMType(type);

        CMPIrc cmpiRC = CMPI_RC_OK;
        SCMBUnion scmoData = value2SCMOValue(data, type);
        if (cmpiRC != CMPI_RC_OK)
        {
            PEG_TRACE((
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Failed to convert CMPIData to SCMOValue in \
                CMPIObjectPath:refAddKey(%d,%s)", type, name));
            CMReturn(cmpiRC);
        }

        SCMO_RC rc = ref->setKeyBinding(name, 
                                        cimType, 
                                        &scmoData);

        switch (rc)
        {
            case SCMO_OK:
                // Just fall through to the end
                break;
            case SCMO_TYPE_MISSMATCH:
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL1,
                    "Received invalid type %d in \
                    CMPIObjectPath:refAddKey", type));
                CMReturn(CMPI_RC_ERR_INVALID_DATA_TYPE);
                break;
            case SCMO_NOT_FOUND:
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL1,
                    "Received invalid parameter %s in \
                    CMPIObjectPath:refAddKey", name));
                CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
                break;
            default:
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL1,
                    "Unknow error %d in \
                    CMPIObjectPath:refAddKey", rc));
                CMReturn(CMPI_RC_ERR_FAILED);
                break;
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIData refGetKey(
        const CMPIObjectPath* eRef,
        const char *name,
        CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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


        const SCMBUnion* keyValue=0;
        CIMType type;

        SCMO_RC src = ref->getKeyBinding(name, type, &keyValue);
        if (src == SCMO_OK)
        {
            CMPIType ct=type2CMPIType(type, false);
            CMPISCMOUtilities::scmoValue2CMPIData( keyValue, ct, &data );
            CMSetStatus(rc, CMPI_RC_OK);
        }
        else
        {
            // Either SCMO_NULL_VALUE or SCMO_NOT_FOUND
            CMSetStatus(rc, CMPI_RC_ERR_NOT_FOUND);
        }

        return data;
    }

    static CMPIData refGetKeyAt(
        const CMPIObjectPath* eRef,
        unsigned pos,
        CMPIString** name,
        CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
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


        const SCMBUnion* keyValue=0;
        const char* keyName=0;
        CIMType type;

        SCMO_RC src = ref->getKeyBindingAt(pos, &keyName, type, &keyValue);
        if (src == SCMO_OK)
        {
            CMPIType ct=type2CMPIType(type, false);
            CMPISCMOUtilities::scmoValue2CMPIData( keyValue, ct, &data );
            CMSetStatus(rc, CMPI_RC_OK);
        }
        else
        {
            // Either SCMO_NULL_VALUE or SCMO_INDEX_OUT_OF_BOUND
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Property Not Found - CMPIObjectPath:refGetKeyAt");
            CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
        }

        if (keyName)
        {
            *name = (CMPIString*)string2CMPIString(keyName);
        }

        return data;
    }

    static CMPICount refGetKeyCount(const CMPIObjectPath* eRef, CMPIStatus* rc)
    {
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refGetKeyCount");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return 0;
        }
        CMSetStatus(rc, CMPI_RC_OK);
        return ref->getKeyBindingCount();
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
        SCMOInstance* ref = (SCMOInstance*)eRef->hdl;
        if (!ref)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL1,
                "Received invalid handle in CMPIObjectPath:refToString");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return NULL;
        }

        // Convert to string using CIMObjectPath to guarantee same string
        // represenation for ObjectPaths.
        CIMObjectPath cimObjPath;
        ref->getCIMObjectPath(cimObjPath);
        String str = cimObjPath.toString();

        CMSetStatus(rc, CMPI_RC_OK);
        return string2CMPIString(str);
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


CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const SCMOInstance& cop)
{
    hdl = (void*)&cop;
    ft = CMPI_ObjectPathOnStack_Ftab;
}

CMPI_ObjectPathOnStack::CMPI_ObjectPathOnStack(const CIMObjectPath& cop)
{
    fprintf(stderr,"Using non SCMO CMPI_ObjectPathOnStack!!!!\n");
    hdl = (void*)&cop;
    ft = CMPI_ObjectPathOnStack_Ftab;
}


PEGASUS_NAMESPACE_END

