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
#include "CMPISCMOUtilities.h"

#include <Pegasus/Common/CIMNameCast.h>
#include <Pegasus/Common/InternalException.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Mutex.h>
#include <string.h>
#include <new>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/SCMODump.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C"
{

    static CMPIStatus instRelease(CMPIInstance* eInst)
    {
        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
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
        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            PEG_METHOD_EXIT();
            return NULL;
        }
        try
        {
            SCMOInstance* cInst = new SCMOInstance(inst->clone());
            CMPIInstance* cmpiInstance =
                reinterpret_cast<CMPIInstance *>
                (new CMPI_Object(cInst,CMPI_Object::ObjectTypeInstance));
            CMSetStatus(rc,CMPI_RC_OK);
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
        CMPIData data={0,CMPI_badValue,{0}};

        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }

        const SCMBUnion* value = 0;
        Boolean isArray = 0;
        Uint32 size = 0;
        CIMType type = (CIMType)0;
        const char* pName=0;

        SCMO_RC src = inst->getPropertyAt((Uint32)pos,
                                          &pName,
                                          type,
                                          &value,
                                          isArray,
                                          size);
        switch(src)
        {
            case SCMO_OK:
            {
                CMPIType ct=type2CMPIType(type, isArray);
                CMPISCMOUtilities::scmoValue2CMPIData( value, ct, &data );
                if ((ct&~CMPI_ARRAY) == CMPI_string)
                {
                    // We always receive strings as an array of pointers
                    // with at least one element, which needs to be released
                    // after it was converted to CMPIData
                    free((void*)value);
                }
                break;
            }
            case SCMO_INDEX_OUT_OF_BOUND:
            {
                CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
                CMPIData rdata={0,CMPI_nullValue|CMPI_notFound,{0}};
                return rdata;
                break;
            }

            case SCMO_NULL_VALUE:
            {
                // A NullValue does not indicate an error, but simply that
                // no value has been set for the property.
                data.type = type2CMPIType(type, isArray);
                data.state = CMPI_nullValue;
                data.value.uint64 = 0;
                break;
            }

            default:
            {
                // Other return codes should not appear here, but are possible
                // code wise (i.e. SCMO_NOT_FOUND etc.)
                PEG_TRACE((
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL2,
                    "Unexpected RC from SCMOInstance.instGetPropertyAt: %d",
                    src));
                CMSetStatus(rc, CMPI_RC_ERR_FAILED);
                return data;
            }
            break;
        }


        // Returning the property name as CMPI String
        if (name)
        {
            *name=string2CMPIString(pName);
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return data;
    }

    static CMPIData instGetProperty(const CMPIInstance* eInst,
        const char *name, CMPIStatus* rc)
    {
        CMPIData data={0,CMPI_badValue,{0}};

        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return data;
        }

        if (!name)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return data;
        }

        const SCMBUnion* value = 0;
        Boolean isArray = 0;
        Uint32 size = 0;
        CIMType type = (CIMType)0;

        SCMO_RC src = inst->getProperty(name, type, &value, isArray, size);
        if (src != SCMO_OK)
        {
            switch(src)
            {
            case SCMO_NOT_FOUND:
                {
                    CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
                    return data;
                }
                break;

            case SCMO_NULL_VALUE:
                {
                    // A NullValue does not indicate an error, but simply that
                    // no value has been set for the property.

                    // TBD: Though the CMPI specification mandates to return a 
                    // nullvalue when a property exists on an instance but has 
                    // not yet been assigned a value, for compatibility with 
                    // previous versions we return CMPI_RC_ERR_NO_SUCH_PROPERTY 
                    // in this case.
                    // If SCMO would distinguish between nullvalues and values
                    // that have not been set at all on an instance, we could
                    // be more precise here.
                    /*
                         // Correct code for nullvalues
                         data.type = type2CMPIType(type, isArray);
                         data.state = CMPI_nullValue;
                         data.value.uint64 = 0;
                    */
                    // Code for properties that have not been set
                    CMSetStatus(rc, CMPI_RC_ERR_NO_SUCH_PROPERTY);
                    return data;

                }
                break;

            default:
                {
                    PEG_TRACE((
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL1,
                        "Unexpected RC from SCMOInstance.instGetPropertyAt: %d",
                        src));
                    CMSetStatus(rc, CMPI_RC_ERR_FAILED);
                    return data;
                }
                break;
            }
        }
        else
        {
            CMPIType ct=type2CMPIType(type, isArray);
            CMPISCMOUtilities::scmoValue2CMPIData(value, ct, &data, size);
            if ((ct&~CMPI_ARRAY) == CMPI_string)
            {
                // We always receive strings as an array of pointers
                // with at least one element, which needs to be released
                // after it was converted to CMPIData
                free((void*)value);
            }
        }


        CMSetStatus(rc,CMPI_RC_OK);
        return data;
    }


    static CMPICount instGetPropertyCount(const CMPIInstance* eInst,
        CMPIStatus* rc)
    {
        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            return 0;
        }
        CMSetStatus(rc,CMPI_RC_OK);
        return inst->getPropertyCount();
    }

    static CMPIStatus instSetPropertyWithOrigin(
        const CMPIInstance* eInst,
        const char* name, 
        const CMPIValue* data, 
        const CMPIType type,
        const char* origin)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Instance:instSetPropertyWithOrigin()");

        SCMOInstance *inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }

        CMPIStatus cmpiRC = {CMPI_RC_OK,0};
        SCMO_RC rc;

        if (!(type&CMPI_ARRAY))
        {
            CIMType cimType=type2CIMType(type);
            SCMBUnion scmoData = value2SCMOValue(data, type);

            rc = inst->setPropertyWithOrigin(name,
                                             cimType,
                                             &scmoData,
                                             false,  // isArray
                                             0,      // arraySize
                                             origin);
        }
        else
        {
            //Get the type of the elements in the array
            CMPIType aType=type&~CMPI_ARRAY;
            CIMType cimType=type2CIMType(aType);

            if( data == NULL || data->array == NULL )
            {
                // In this case just set a NULL Value
                rc = inst->setPropertyWithOrigin(name,cimType,0,true,0,origin);
            }
            else
            {
                // When data is not NULL and data->array is also set
                CMPI_Array* ar = (CMPI_Array*)data->array->hdl;
                CMPIData* arrData = (CMPIData*)ar->hdl;


                Uint32 arraySize = arrData->value.uint32;


                // Convert the array of CMPIData to an array of SCMBUnion
                SCMBUnion scmbArray[arraySize];
                for (unsigned int x=0; x<arraySize; x++)
                {
                    scmbArray[x] = value2SCMOValue(&(arrData[x].value), type);
                }

                rc = inst->setPropertyWithOrigin(name,
                                                 cimType,
                                                 &(scmbArray[0]),
                                                 true,          // isArray
                                                 arraySize,
                                                 origin);
            }
        }

        if (rc != SCMO_OK)
        {
            PEG_TRACE((TRC_CMPIPROVIDERINTERFACE,Tracer::LEVEL3,
                       "Property %s not set on created instance."
                           "Either the property is not part of the class or"
                       "not part of the property list. SCMO_RC=%d",
                       name,
                       rc));

            switch (rc)
            {
                case SCMO_NOT_SAME_ORIGIN:
                    cmpiRC.rc = CMPI_RC_ERR_INVALID_PARAMETER;
                    break;
                case SCMO_NOT_FOUND:
                    //TBD: Should return an error here, but previous impl.
                    //     returned OK. Is this correct?
                    //cmpiRC.rc = CMPI_RC_ERR_NO_SUCH_PROPERTY;
                    cmpiRC.rc = CMPI_RC_OK;
                    break;
                case SCMO_WRONG_TYPE:
                    cmpiRC.rc = CMPI_RC_ERR_INVALID_DATA_TYPE;
                    break;
                case SCMO_NOT_AN_ARRAY:
                    cmpiRC.rc = CMPI_RC_ERR_INVALID_DATA_TYPE;
                    break;
                case SCMO_IS_AN_ARRAY:
                    cmpiRC.rc = CMPI_RC_ERR_INVALID_DATA_TYPE;
                    break;
                default:
                    cmpiRC.rc = CMPI_RC_ERR_FAILED;
                    break;
        }
        }

        PEG_METHOD_EXIT();
        return(cmpiRC);
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

        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        if (!inst)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            PEG_METHOD_EXIT();
            return NULL;
        }

        try
        {
            // Generate keys from instance
            inst->buildKeyBindingsFromProperties();

            // Since we make no difference between ObjectPath and Instance,
            // we simply clone using the ObjectPathOnly option.
            SCMOInstance* cInst = new SCMOInstance(inst->clone(true));

            CMPIObjectPath* cmpiObjPath =
                reinterpret_cast<CMPIObjectPath *>
                (new CMPI_Object(cInst,CMPI_Object::ObjectTypeObjectPath));
            CMSetStatus(rc,CMPI_RC_OK);
            PEG_METHOD_EXIT();
            return cmpiObjPath;
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

        SCMOInstance* prevInst=(SCMOInstance*)eInst->hdl;
        if (prevInst==NULL)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_HANDLE);
        }
        if (obj==NULL)
        {
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        SCMOInstance* ref = (SCMOInstance*)(obj->hdl);
        if (ref->isSame(*prevInst))
        {
            // Since we represent CMPIObjectPath as well as CMPIInstance
            // through SCMOInstance, in this case both point to the same
            // physical SCMB and the objectPath is already set.
            // So this path is a nop.
            PEG_METHOD_EXIT();
            CMReturn(CMPI_RC_OK);
        }
        else
        {
            // It is not possible to have an instance or objectPath in
            // this implementation without a classname or namespace.
            const char* nsRef = ref->getNameSpace();
            const char* clsRef = ref->getClassName();

            if (0 == strcasecmp(clsRef, prevInst->getClassName()))
            {
                // Set the new namespace
                prevInst->setNameSpace(nsRef);

                // Now we try to copy the key properties from the given
                // ObjectPath
                // TODO: Remove the previously set key properties.
                //       Set isSet to false and remove userdefined keys.
                CMPIrc rc = CMPISCMOUtilities::copySCMOKeyProperties(
                    ref,            // source
                    prevInst);      // target
                if (rc != CMPI_RC_OK)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL1,
                        "Failed to copy key bindings");
                    PEG_METHOD_EXIT();
                    CMReturn(CMPI_RC_ERR_FAILED);
                }
            }
            else
            {
                // Uurrgh, changing class on an existing
                // CMPIInstance is a prohibited change.
                // Simply returning an error
                PEG_TRACE_CSTRING(
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL1,
                    "Cannot set objectpath because it would change classname"
                    "or namespace of instance");
                PEG_METHOD_EXIT();
                CMReturnWithString(
                    CMPI_RC_ERR_FAILED,
                    string2CMPIString("Incompatible ObjectPath"));
            }
        }

        PEG_METHOD_EXIT();
        CMReturn(CMPI_RC_OK);
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
           Removing properties from the SCMOInstance here helps to effectively
           avoid the need to carry a property list around in the CMPI layer.

           A (propertyList == 0) means the property list is null and there
           should be no filtering.

           An empty propertylist(no property to be returned) is represented by
           a valid propertyList pointer pointing to a null pointer, i.e.
           (*propertyList == 0)
        */

        SCMOInstance* inst=(SCMOInstance*)eInst->hdl;
        inst->setPropertyFilter(propertyList);

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


CMPI_InstanceOnStack::CMPI_InstanceOnStack(const SCMOInstance& ci)
{
    PEG_METHOD_ENTER(
        TRC_CMPIPROVIDERINTERFACE,
        "CMPI_InstanceOnStack::CMPI_InstanceOnStack()");

    hdl=(void*)&ci;
    ft=CMPI_InstanceOnStack_Ftab;
    PEG_METHOD_EXIT();
}


PEGASUS_NAMESPACE_END

