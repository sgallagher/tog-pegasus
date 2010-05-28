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

#include <Pegasus/Common/AutoPtr.h>
#include "CMPI_Version.h"

#include "CMPI_Instance.h"
#include "CMPI_Broker.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"

#include <Pegasus/Common/CIMNameCast.h>
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
            CMPIData retData={0,CMPI_nullValue|CMPI_notFound,{0}};
            return retData;
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
        CMPIrc rc;
        CIMValue v=value2CIMValue(data,type,&rc);
        Uint32 pos;
        int count=0;

        if ((pos=inst->findProperty(CIMNameCast(name)))!=PEG_NOT_FOUND)
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
            if (cp.getType() == CIMTYPE_INSTANCE &&
                v.getType() == CIMTYPE_OBJECT)
            {
                if (cp.isArray())
                {
                    if (!v.isArray())
                    {
                        PEG_TRACE((
                            TRC_CMPIPROVIDERINTERFACE,
                            Tracer::LEVEL1,
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
                    Tracer::LEVEL1,
                    " TypeMisMatch exception for: %s",
                    name));
                if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL1,
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
                    Tracer::LEVEL1,
                    " InvalidName exception for: %s",
                    origin));

                PEG_METHOD_EXIT();
                CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
            }
            catch (const Exception &e)
            {
                PEG_TRACE((TRC_CMPIPROVIDERINTERFACE,Tracer::LEVEL1,
                    "Exception for %s: %s",name,
                    (const char*)e.getMessage().getCString()));
                if (getenv("PEGASUS_CMPI_CHECKTYPES")!=NULL)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL1,
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
            PEG_TRACE((TRC_CMPIPROVIDERINTERFACE,Tracer::LEVEL3,
                       "Property %s not set on created instance."
                           "Either the property is not part of the class or"
                               "not part of the property list.",
                       name));
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
                    objPath->setHost(clsRef.getHost());
                    objPath->setNameSpace(clsRef.getNameSpace());
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

    /* The function removes all properties not part of the given
       array of property names(property lists).
       Function can handle second list being set or not set.
       This is an internal function used by instSetPropertyFilter()
    */
    static void filterCIMInstance(
        const char** listroot1,
        const char** listroot2,
        CIMInstance & inst)
    {
        /* listroot1 never can be 0, this function is only called by
           instSetPropertyFilter() and the check is done there already */

        /* determine number of properties on the instance */
        int propertyCount = inst.getPropertyCount();

        /* number of properties in each property list */
        int numProperties1 = 0;
        int numProperties2 = 0;
        /* Masks for property lists
           0 = list element not yet found
           1 = list element already found)
        */
        char * plMask1=0;
        char * plMask2=0;
        /* end of array of property list names */
        const char **listend1 = listroot1;
        /* place end pointer at end of array and count number of properties
           named in the first list */
        while (*listend1)
        {
            listend1++;
        }
        /* former while loop steps one field to far, step one back */
        listend1--;
        /* calculate last valid index of a property */
        numProperties1 = listend1 - listroot1;
        /* reserver memory for index + 1 property mask fields */
        plMask1 = (char*) calloc(1, numProperties1+1);

        /* special dish for two lists, need to check those at same time */
        /* variable to hold end of array of property list names */
        const char **listend2 = listroot2;
        /* place end pointer at end of array and count number of properties
           named in the first list */
        if (listroot2)
        {
            while (*listend2)
            {
                listend2++;
            }
            /* former while loop steps one field to far, step one back */
            listend2--;
            /* calculate last valid index of a property */
            numProperties2 = listend2 - listroot2;
            /* reserver memory for index + 1 property mask fields */
            plMask2 = (char*) calloc(1, numProperties2+1);
        }

        /* for each property on the instance */
        /* use reverse order, so we don't change index of properties not yet
           checked when removing a property */
        for (int idx=propertyCount-1; idx >= 0; idx--)
        {
            CIMConstProperty prop = inst.getProperty(idx);
            CString cName = prop.getName().getString().getCString();
            const char* pName = (const char*)cName;

            /* work the property list backward too, as often times
               properties on the instance and in the list are ordered in the
               same way, this helps reduce number of required strcasecmp */
            int found = false;
            /* use temporary list1 to step through the list */
            const char **list1 = listend1;

            /* steps through the entire property list and does compare
               the name in the property list with the currently investigated
               name of the property, except one of the following conditions
               is true:
                 1.) the property list element has already been found as
                     indicated by the property list mask
                 2.) the property is found, mark it as found in the mask and
                     leave the for-loop
            */
            for (int pos1=numProperties1; pos1 >= 0; pos1--,list1--)
            {
                if (!plMask1[pos1])
                {
                    if (System::strcasecmp(pName, *list1)==0)
                    {
                        found = true;
                        plMask1[pos1] = 1;
                        break;
                    }
                }
            }
            /* Do the same algorithm for the second list too,
               except if we found the property already */
            if (listroot2 && !found)
            {
                /* use temporary list2 to step through the list */
                const char **list2 = listend2;
                for (int pos2=numProperties2; pos2 >= 0; pos2--,list2--)
                {
                    if (!plMask2[pos2])
                    {
                        if (System::strcasecmp(pName, *list2)==0)
                        {
                            found = true;
                            plMask2[pos2] = 1;
                            break;
                        }
                    }
                }
            }
            /* If the property could not be found in either property list,
               remove the property from the instance */
            if (!found)
            {
                inst.removeProperty(idx);
            }
        }
        free(plMask1);
        if (listroot2)
        {
            free(plMask2);
        }
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
        /* The property list is to be applied on the given instance.
           Currently CMPI provider have to call instSetPropertyFilter to honor
           property filters or have to filter for themselves.
           Removing properties from the CIMInstance here helps to effectively
           avoid the need to carry a property list around in the CMPI layer.

           A (propertyList == 0) means the property list is null and there
           should be no filtering.

           An empty propertylist(no property to be returned) is represented by
           a valid propertyList pointer pointing to a null pointer, i.e.
           (*propertyList == 0)
        */

        CIMInstance& inst=*(CIMInstance*)(eInst->hdl);
        if (propertyList)
        {
            if (!keys)
            {
                filterCIMInstance(propertyList, 0, inst);
            }
            else
            {
                filterCIMInstance(propertyList, keys, inst);
            }
        };
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

