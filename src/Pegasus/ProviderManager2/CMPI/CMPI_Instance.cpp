//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/CIMNameUnchecked.h>
#include <Pegasus/Common/AutoPtr.h>
#include "CMPI_Version.h"

#include "CMPI_Instance.h"
#include "CMPI_Broker.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Mutex.h>
#include <string.h>
#include <new>
#include <Pegasus/Common/Tracer.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C" 
{

    static CMPIStatus instRelease(CMPIInstance* eInst)
    {
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        if (inst)
        {
            delete inst;
            (reinterpret_cast<CMPI_Object*>(eInst))->unlinkAndDelete();
            CMReturn(CMPI_RC_OK);
        }
        else
        {
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
    }

    static CMPIStatus instReleaseNop(CMPIInstance* eInst)
    {
        CMReturn(CMPI_RC_OK);
    }

    static CMPIInstance* instClone(const CMPIInstance* eInst, CMPIStatus* rc) 
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instClone()");
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            PEG_METHOD_EXIT();
            return NULL;
        }
        try
        {
            AutoPtr<CIMInstance> cInst(new CIMInstance(inst->clone()));
            AutoPtr<CMPI_Object> obj(new CMPI_Object(cInst.get()));
            cInst.release();
            obj->unlink();
            CMSetStatus(rc,CMPI_RC_OK);
            CMPIInstance* cmpiInstance = 
                reinterpret_cast<CMPIInstance *>(obj.release());
            PEG_METHOD_EXIT();
            return cmpiInstance;
        }
        catch (const PEGASUS_STD(bad_alloc)&)
        {
            CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
            PEG_METHOD_EXIT();
            return NULL;
        }
    }

    static CMPIData instGetPropertyAt(
        const CMPIInstance* eInst, CMPICount pos, CMPIString** name,
        CMPIStatus* rc)
    {
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        CMPIData data={0,CMPI_nullValue,{0}};

        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }

        if (pos>inst->getPropertyCount())
        {
            CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
            CMPIData data={0,CMPI_nullValue|CMPI_notFound,{0}};
            return data;
        }
        const CIMProperty& p=inst->getProperty(pos);
        const CIMValue& v=p.getValue();
        CIMType pType=p.getType();
        CMPIType t=type2CMPIType(pType,p.isArray());

        value2CMPIData(v,t,&data);

        if (name)
        {
            String str=p.getName().getString();
            *name=(CMPIString*)string2CMPIString(str);
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return data;
    }

    static CMPIData instGetProperty(const CMPIInstance* eInst, 
        const char *name, CMPIStatus* rc)   
    {
        CMPIData data={0,CMPI_nullValue|CMPI_notFound,{0}};

        if (!eInst->hdl)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }
        if (!name)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return data;
        }
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        Uint32 pos=inst->findProperty(String(name));

        if (pos!=PEG_NOT_FOUND)
        {
            CMSetStatus(rc,CMPI_RC_OK);
            return instGetPropertyAt(eInst,pos,NULL,rc);
        }
        CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
        return data;
    }


    static CMPICount instGetPropertyCount(const CMPIInstance* eInst, 
        CMPIStatus* rc) 
    {
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return 0;
        }
        CMSetStatus(rc,CMPI_RC_OK);
        return inst->getPropertyCount();
    }

    static CMPIStatus instSetPropertyWithOrigin(const CMPIInstance* eInst, 
        const char* name, const CMPIValue* data, const CMPIType type,
        const char* origin)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instSetPropertyWithOrigin()");
        CIMInstance *inst=(CIMInstance*)eInst->hdl;
        if (!inst)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        char **list=(char**)(reinterpret_cast<const CMPI_Object*>(eInst))->priv;
        CMPIrc rc;

        if (list)
        {
            while (*list)
            {
                if (System::strcasecmp(name,*list)==0) goto ok;
                list++;
            }
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_OK);
        }

        ok:
        CIMValue v=value2CIMValue(data,type,&rc);
        CIMNameUnchecked sName(name);
        Uint32 pos;
        int count=0;

        if ((pos=inst->findProperty(sName))!=PEG_NOT_FOUND)
        {

            CIMProperty cp=inst->getProperty(pos);


            /* The CMPI interface uses the type "CMPI_instance" to represent 
               both embedded objects and embedded instances. CMPI has no 
               "CMPI_object" type. So when converting a CMPIValue with type 
               "CMPI_instance" to a CIMValue (see value2CIMValue) the type 
               CIMTYPE_OBJECT is always used. If the property's type is 
               CIMTYPE_INSTANCE, and the value's type is CIMTYPE_OBJECT, then
               we convert the value's type to match the property's type.
            */
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
            if (cp.getType() == CIMTYPE_INSTANCE && 
                v.getType() == CIMTYPE_OBJECT)
            {
                if (cp.isArray())
                {
                    if (!v.isArray())
                    {
                        PEG_TRACE((
                            TRC_CMPIPROVIDERINTERFACE,
                            Tracer::LEVEL2,
                            "TypeMisMatch, Expected Type: %s, Actual Type: %s",
                            cimTypeToString(cp.getType()),
                            cimTypeToString(v.getType())));
                        PEG_METHOD_EXIT();
                        CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
                    }
                    Array<CIMObject> tmpObjs;
                    Array<CIMInstance> tmpInsts;
                    v.get(tmpObjs);
                    for (Uint32 i = 0; i < tmpObjs.size() ; ++i)
                    {
                        tmpInsts.append(CIMInstance(tmpObjs[i]));
                    } 
                    v.set(tmpInsts);
                }
                else
                {  
                    CIMObject co;
                    v.get(co);
                    if (co.isInstance())
                        v.set(CIMInstance(co));
                }
            }
#endif
            try
            {
                cp.setValue(v);
                if (origin)
                {
                    CIMName oName(origin);
                    cp.setClassOrigin(oName);
                }
            }
            catch (const TypeMismatchException &)
            {
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL2,
                    " TypeMisMatch exception for: %s",
                    name));
                if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL2,
                        " Aborting because of CMPI_CHECKTYPES..");
                        abort();
                }
                PEG_METHOD_EXIT();
                CMReturn(CMPI_RC_ERR_TYPE_MISMATCH);
            }
            catch (const InvalidNameException &)
            {
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL2,
                    " InvalidName exception for: %s",
                    origin));

                PEG_METHOD_EXIT();
                CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
            }
            catch (const Exception &e)
            {
                PEG_TRACE_STRING(
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL2,
                    "  " + e.getMessage() + " exception for " + name);
                if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL2,
                        " Aborting because of CMPI_CHECKTYPES..");
                        abort();
                }
                PEG_METHOD_EXIT();
                CMReturnWithString(CMPI_RC_ERR_FAILED,
                    reinterpret_cast<CMPIString*>(
                    new CMPI_Object(e.getMessage())));
            }
        }
        else
        {
            if (type==CMPI_ref)
            {
                CIMObjectPath *ref=(CIMObjectPath*)(data->ref->hdl);
                if (origin)
                {
                    CIMName oName(origin);
                    inst->addProperty(CIMProperty(sName,v,count,
                        ref->getClassName(),oName));
                }
                else
                {
                    inst->addProperty(CIMProperty(sName,v,count,
                        ref->getClassName()));
                }
            }
            else
            {
                if (origin)
                {
                    CIMName oName(origin);
                    inst->addProperty(
                        CIMProperty(sName,v,count,CIMName(),oName));
                }
                else
                {
                    inst->addProperty(CIMProperty(sName,v,count));
                }
            }
        }
        PEG_METHOD_EXIT();
        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus instSetProperty(const CMPIInstance* eInst,
        const char *name, const CMPIValue* data, CMPIType type) 
    {
        return instSetPropertyWithOrigin(eInst, name, data, type, NULL);
    }

    static CMPIObjectPath* instGetObjectPath(const CMPIInstance* eInst,
        CMPIStatus* rc) 
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instGetObjectPath()");
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            PEG_METHOD_EXIT();
            return NULL;
        }
        const CIMObjectPath &clsRef=inst->getPath();
        AutoPtr<CIMObjectPath> objPath(NULL);
        AutoPtr<CMPI_Object> obj(NULL);
        try
        {
            /* Check if NameSpace is NULL before calling GetClass. When
               providers run out-of-process and getClass request is made 
               through CIMClient,  CIMOperationRequestEncoder tries to 
               encode the request and finds the namespace is invalid 
               (empty) and throws InvalidNamespaceNameException. 
            */
            if (clsRef.getKeyBindings().size()==0 && 
                !clsRef.getNameSpace().isNull())
            {
                CIMClass *cc=mbGetClass(CMPI_ThreadContext::getBroker(),clsRef);
                /* It seems that when converting the CIMInstnace to XML form, 
                   we miss CIMObjectPath from it. When we don't have namespace 
                   we may not get class, so make ObjectPath with class-name 
                   only.
                   TODO: This will create problems when passing the 
                   EmbeddedInstances as arguements to MethodProviders, where it
                   needs to get ObjectPath from instance. Shall we need to 
                   include CIMObjectPath in CIMInstance while converting to XML 
                   form ??? ...  
                */
                if (!cc)
                {
                    objPath.reset(new CIMObjectPath(clsRef));
                }
                else
                {
                    const CIMObjectPath &ref=inst->buildPath(
                        *(reinterpret_cast<const CIMConstClass*>(cc)));
                    objPath.reset(new CIMObjectPath(ref));
                }
            }
            else
                objPath.reset(new CIMObjectPath(clsRef));
            obj.reset(new CMPI_Object(objPath.get()));
            objPath.release();
            CMSetStatus(rc,CMPI_RC_OK);
            CMPIObjectPath* cmpiObjectPath = 
                reinterpret_cast<CMPIObjectPath*> (obj.release());
            PEG_METHOD_EXIT();
            return cmpiObjectPath; 
        }
        catch (const PEGASUS_STD(bad_alloc)&)
        {
            CMSetStatus(rc, CMPI_RC_ERROR_SYSTEM);
            PEG_METHOD_EXIT();
            return NULL;
        }
    }

    static CMPIStatus instSetObjectPath(
        CMPIInstance* eInst, 
        const CMPIObjectPath *obj)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instSetObjectPath()");
        CIMInstance* inst=(CIMInstance*)eInst->hdl;
        if (inst==NULL)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (obj==NULL)
        {
            PEG_METHOD_EXIT();
            CMReturn ( CMPI_RC_ERR_INVALID_PARAMETER);
        }

        CIMObjectPath &ref = *(CIMObjectPath*)(obj->hdl);
        try
        {
            inst->setPath(ref); 
        }
        catch (const TypeMismatchException &e)
        {
            PEG_METHOD_EXIT();
            CMReturnWithString(CMPI_RC_ERR_FAILED,
                reinterpret_cast<CMPIString*>(
                    new CMPI_Object(e.getMessage())));
        }
        PEG_METHOD_EXIT();
        CMReturn ( CMPI_RC_OK);
    }

    static CMPIStatus instSetPropertyFilter(CMPIInstance* eInst,
        const char** propertyList, const char **keys)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instSetPropertyFilter()");
        if (!eInst->hdl)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }

        CMPI_Object *inst=reinterpret_cast<CMPI_Object*>(eInst);
        char **list=(char**)inst->priv;    // Thank you Warren !
        int i,s;

        if (inst->priv)
        {
            while (*list)
            {
                free (*list);
                list++;
            }
            free(inst->priv);
        }
        inst->priv=NULL;

        if (propertyList==NULL) CMReturn(CMPI_RC_OK);
        if (keys==NULL) CMReturn(CMPI_RC_ERR_FAILED);

        for (s=0,i=0; propertyList[i]; i++,s++);
        for (i=0; keys[i]; i++,s++);
        list = (char**) calloc(s+1, sizeof(char*));
        for (s=0,i=0; propertyList[i]; 
              i++,s++) list[s]=strdup(propertyList[i]);
        for (i=0; keys[i]; i++,s++) list[s]=strdup(keys[i]);
        inst->priv=(void*)list;

        PEG_METHOD_EXIT();
        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus instSetPropertyFilterIgnore(CMPIInstance* eInst,
        const char** propertyList, const char **keys)
    {
        /* We ignore it.  */
        CMReturn ( CMPI_RC_OK);
    }


}

static CMPIInstanceFT instance_FT={
    CMPICurrentVersion,
    instRelease,
    instClone,
    instGetProperty,
    instGetPropertyAt,
    instGetPropertyCount,
    instSetProperty,
    instGetObjectPath,
    instSetPropertyFilter,
#if defined(CMPI_VER_100)
    instSetObjectPath,
#endif
#if defined(CMPI_VER_200)
    instSetPropertyWithOrigin,
#endif
};

static CMPIInstanceFT instanceOnStack_FT={
    CMPICurrentVersion,
    instReleaseNop,
    instClone,
    instGetProperty,
    instGetPropertyAt,
    instGetPropertyCount,
    instSetProperty,
    instGetObjectPath,
    instSetPropertyFilterIgnore,
#if defined(CMPI_VER_100)
    instSetObjectPath,
#endif
#if defined(CMPI_VER_200)
    instSetPropertyWithOrigin,
#endif
};

CMPIInstanceFT *CMPI_Instance_Ftab=&instance_FT;
CMPIInstanceFT *CMPI_InstanceOnStack_Ftab=&instanceOnStack_FT;


CMPI_InstanceOnStack::CMPI_InstanceOnStack(const CIMInstance& ci)
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPI_InstanceOnStack::CMPI_InstanceOnStack()");

    hdl=(void*)&ci;
    ft=CMPI_InstanceOnStack_Ftab;
    PEG_METHOD_EXIT();
}


PEGASUS_NAMESPACE_END
    
