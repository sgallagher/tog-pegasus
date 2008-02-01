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
#include "CMPI_Version.h"

#include "CMPI_Object.h"
#include "CMPI_Broker.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"
#include "CMPI_SelectExp.h"

#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMPropertyList.h>
#if defined (CMPI_VER_85)
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/LanguageParser.h>
#endif
#if defined(CMPI_VER_100)
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/TraceComponents.h>
#include <Pegasus/Common/Tracer.h>
#endif

#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLParser.h>

#ifndef PEGASUS_DISABLE_CQL
#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#endif

#include <stdarg.h>
#include <string.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# define lloonngg __int64
#else
# define lloonngg long long int
#endif

// Factory section

static String typeToString(CIMType t)
{
    switch (t)
    {
        case CIMTYPE_BOOLEAN:
            return "boolean";
        case CIMTYPE_UINT8:
            return "uint8";
        case CIMTYPE_SINT8:
            return "sint8";
        case CIMTYPE_UINT16:
            return "uint16";
        case CIMTYPE_SINT16:
            return "sint16";
        case CIMTYPE_UINT32:
            return "uint32";
        case CIMTYPE_SINT32:
            return "sint32";
        case CIMTYPE_UINT64:
            return "sint64";
        case CIMTYPE_SINT64:
            return "sint64";
        case CIMTYPE_REAL32:
            return "real32";
        case CIMTYPE_REAL64:
            return "real64";
        case CIMTYPE_CHAR16:
            return "char16";
        case CIMTYPE_STRING:
            return "string";
        case CIMTYPE_DATETIME:
            return "datetime";
        case CIMTYPE_REFERENCE:
            return "reference";
        case CIMTYPE_OBJECT:
            return "object";
#ifdef PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        case CIMTYPE_INSTANCE:
            return "instance";
#endif // PEGASUS_EMBEDDED_INSTANCE_SUPPORT
        default:
            return "???";
    }
}

#define CMPIInstance_str "CMPIInstance"
#define CMPIInstance_str_l 13

#define CMPIObjectPath_str "CMPIObjectPath"
#define CMPIObjectPath_str_l 16

#define CMPIArgs_str "CMPIArgs"
#define CMPIArgs_str_l 8

#define CMPIContext_str "CMPIContext"
#define CMPIContext_str_l 11

#define CMPIResult_str "CMPIResult"
#define CMPIResult_str_l 12

#define CMPIDateTime_str "CMPIDateTime"
#define CMPIDateTime_str_l 12

#define CMPIArray_str "CMPIArray"
#define CMPIArray_str_l 9

#define CMPIString_str "CMPIString"
#define CMPIString_str_l 10

#define CMPISelectExp_str "CMPISelectExp"
#define CMPISelectExp_str_l 13

#define CMPISelectCond_str "CMPISelectCond"
#define CMPISelectCond_str_l 14

#define CMPISubCond_str "CMPISubCond"
#define CMPISubCond_str_l 11

#define CMPIPredicate_str "CMPIPredicate"
#define CMPIPredicate_str_l 13

#define CMPIBroker_str "CMPIBroker"
#define CMPIBroker_str_l 10

#define CMPIEnumeration_str "CMPIEnumeration"
#define CMPIEnumeration_str_l 15


static Formatter::Arg formatValue(va_list *argptr, CMPIStatus *rc, int *err)
{

    CMPIType type = va_arg(*argptr,int);
    CMSetStatus(rc,CMPI_RC_OK);

    if (*err)
    {
        return Formatter::Arg("*failed*");
    }

    switch (type)
    {
        case CMPI_sint8:
        case CMPI_sint16:
        case CMPI_sint32:
            return Formatter::Arg((int)va_arg(*argptr, int));
        case CMPI_uint8:
        case CMPI_uint16:
        case CMPI_uint32:
            return Formatter::Arg((unsigned int)va_arg(*argptr, unsigned int));
        case CMPI_boolean:
            return Formatter::Arg((Boolean)va_arg(*argptr, int));
        case CMPI_real32:
        case CMPI_real64:
            return Formatter::Arg((double)va_arg(*argptr, double));
        case CMPI_sint64:
            return Formatter::Arg((lloonngg)va_arg(*argptr, lloonngg));
        case CMPI_uint64:
            return Formatter::Arg(
                (unsigned lloonngg)va_arg(*argptr, unsigned lloonngg));
        case CMPI_chars:
            return Formatter::Arg((char*)va_arg(*argptr, char*));
        case CMPI_string:
            {
                CMPIString *s = va_arg(*argptr, CMPIString*);
                return Formatter::Arg((char*)CMGetCharsPtr(s,NULL));
            }
        default:
            *err=1;
            if (rc)
            {
                rc->rc=CMPI_RC_ERR_INVALID_PARAMETER;
            }
            return Formatter::Arg("*bad value type*");
    }
}


static inline CIMNamespaceName NameSpaceName(const char *ns)
{
    CIMNamespaceName n;
    if (ns==NULL)
    {
        return n;
    }
    try
    {
        n = CIMNamespaceName(ns);
    }
    catch (...)
    {
        PEG_TRACE_CSTRING(
            TRC_PROVIDERMANAGER,
            Tracer::LEVEL2,
            "Exception: Unknown Exception thrown...");
        // n won't be assigned to anything yet, so it is safe
        // to send it off.
    }
    return n;
}

static inline CIMName Name(const char *n)
{
    CIMName name;
    if (n==NULL)
    {
        return name;
    }
    try
    {
        name = CIMNameUnchecked(n);
    }
    catch ( ...)
    {
        PEG_TRACE_CSTRING(
            TRC_PROVIDERMANAGER,
            Tracer::LEVEL2,
            "Exception: Unknown Exception thrown...");
    }
    return name;
}


extern "C"
{

    static CMPIString* mbEncToString(
        const CMPIBroker*,
        const void *o,
        CMPIStatus *rc);

    static CMPIInstance* mbEncNewInstance(
        const CMPIBroker* mb,
        const CMPIObjectPath* eCop,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewInstance()");
        if (!eCop)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Parameter in CMPI_BrokerEnc:mbEncToString");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            PEG_METHOD_EXIT();
            return NULL;
        }
        CIMObjectPath* cop = (CIMObjectPath*)eCop->hdl;
        if (!cop)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid handle in CMPI_BrokerEnc:mbEncToString");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            PEG_METHOD_EXIT();
            return NULL;
        }

        CIMClass *cls = mbGetClass(mb,*cop);
        CIMInstance *ci = NULL;

        if (cls)
        {
            const CMPIContext *ctx = CMPI_ThreadContext::getContext();
            CMPIFlags flgs=ctx->ft->getEntry(
                ctx,CMPIInvocationFlags,rc).value.uint32;
            CIMInstance newInst = cls->buildInstance(
                Boolean(flgs & CMPI_FLAG_IncludeQualifiers),
                false,
                CIMPropertyList());

            ci = new CIMInstance(newInst);
        }
        else
        {
            CMSetStatus(rc, CMPI_RC_ERR_NOT_FOUND);
            PEG_METHOD_EXIT();
            return NULL;
        }

        ci->setPath(*cop);
        CMPIInstance* neInst =
            reinterpret_cast<CMPIInstance*>(new CMPI_Object(ci));
        CMSetStatus(rc, CMPI_RC_OK);
   //   CMPIString *str=mbEncToString(mb,neInst,NULL);
        PEG_METHOD_EXIT();
        return neInst;
    }

    static CMPIObjectPath* mbEncNewObjectPath(
        const CMPIBroker* mb,
        const char *ns,
        const char *cls,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewObjectPath()");
        Array<CIMKeyBinding> keyBindings;
        String host;
        CIMName className;
        if (cls)
        {
            className=Name(cls);
        }
        else
        {
            className=Name("");
        }
        CIMNamespaceName nameSpace;
        if (ns)
        {
            nameSpace =NameSpaceName(ns);
        }
        else
        {
            nameSpace=NameSpaceName("");
        }
        CIMObjectPath *cop = new CIMObjectPath(
            host,
            nameSpace,
            className,
            keyBindings);
        CMPIObjectPath *nePath = reinterpret_cast<CMPIObjectPath*>(
            new CMPI_Object(cop));
        CMSetStatus(rc, CMPI_RC_OK);
        PEG_METHOD_EXIT();
        return nePath;
    }

    static CMPIArgs* mbEncNewArgs(const CMPIBroker* mb, CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewArgs()");
        CMSetStatus(rc,CMPI_RC_OK);
        CMPIArgs* cmpiArgs = reinterpret_cast<CMPIArgs*>(
            new CMPI_Object(new Array<CIMParamValue>()));
        PEG_METHOD_EXIT();
        return cmpiArgs;
    }

    static CMPIString* mbEncNewString(
        const CMPIBroker* mb,
        const char *cStr,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_Broker:mbEncNewString()");
        CMSetStatus(rc,CMPI_RC_OK);
        if (cStr == NULL)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Parameter in CMPI_BrokerEnc:mbEncNewString");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            PEG_METHOD_EXIT();
            return NULL;
        }
        CMPIString* cmpiString = 
            reinterpret_cast<CMPIString*>(new CMPI_Object(cStr));
        PEG_METHOD_EXIT();
        return cmpiString;
    }

    CMPIArray* mbEncNewArray(
        const CMPIBroker* mb,
        CMPICount count,
        CMPIType type,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewArray()");
        CMSetStatus(rc,CMPI_RC_OK);
        CMPIData *dta=new CMPIData[count+1];
        dta->type=type;
        dta->value.uint32=count;
        for (unsigned int i=1; i<=count; i++)
        {
            dta[i].type=type;
            dta[i].state=CMPI_nullValue;
            dta[i].value.uint64=0;
        }
        CMPIArray* cmpiArray = 
            reinterpret_cast<CMPIArray*>(new CMPI_Object(dta));
        PEG_METHOD_EXIT();
        return cmpiArray;
    }

    extern CMPIDateTime *newDateTime();

    static CMPIDateTime* mbEncNewDateTime(const CMPIBroker* mb, CMPIStatus *rc)
    {
        CMSetStatus(rc,CMPI_RC_OK);
        return newDateTime();
    }

    extern CMPIDateTime *newDateTimeBin(CMPIUint64,CMPIBoolean);

    static CMPIDateTime* mbEncNewDateTimeFromBinary(
        const CMPIBroker* mb,
        CMPIUint64 time,
        CMPIBoolean interval,
        CMPIStatus *rc)
    {
        CMSetStatus(rc,CMPI_RC_OK);
        return newDateTimeBin(time, interval);
    }

    extern CMPIDateTime *newDateTimeChar(const char*);

    static CMPIDateTime* mbEncNewDateTimeFromString(
        const CMPIBroker* mb,
        const char *t,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewDateTimeFromString()");
        CMPIDateTime *date = NULL;
        CMSetStatus(rc,CMPI_RC_OK);
        date=newDateTimeChar(t);
        if (!date)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Parameter in CMPI_BrokerEnc:newDateTimeChar");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        }
        PEG_METHOD_EXIT();
        return date;
    }

#if defined(CMPI_VER_200)
    extern CMPIError *newCMPIError(
        const char*,
        const char*,
        const char*,
        const CMPIErrorSeverity,
        const CMPIErrorProbableCause,
        const CMPIrc);

    static CMPIError* mbEncNewCMPIError(
        const CMPIBroker* mb,
        const char* owner,
        const char* msgID,
        const char* msg,
        const CMPIErrorSeverity sev,
        const CMPIErrorProbableCause pc,
        const CMPIrc cimStatusCode, CMPIStatus* rc)
    {
        CMPIError* cmpiError;
        if (rc)
        {
            CMSetStatus(rc, CMPI_RC_OK);
        }
        cmpiError = newCMPIError(owner, msgID, msg, sev, pc, cimStatusCode);
        if (!cmpiError)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
        }
        return cmpiError;
    }
#endif

    static CMPIString* mbEncToString(
        const CMPIBroker*,
        const void *o,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncToString()");
        CMPI_Object *obj = (CMPI_Object*)o;
        String str;
        char msg[128];

        CMSetStatus(rc,CMPI_RC_OK);
        if (obj==NULL)
        {
            sprintf(msg,"** Null object ptr (%p) **",o);
            CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
            CMPIString* cmpiString =
                reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
            PEG_METHOD_EXIT();
            return cmpiString;
        }

        if (obj->getHdl()==NULL)
        {
            sprintf(msg,"** Null object hdl (%p) **",o);
            CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
            CMPIString* cmpiString =
                reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
            PEG_METHOD_EXIT();
            return cmpiString;
        }

        if (obj->getFtab() == (void*)CMPI_Instance_Ftab ||
            obj->getFtab() == (void*)CMPI_InstanceOnStack_Ftab)
        {
            CIMInstance *ci=(CIMInstance*)obj->getHdl();
            str="Instance of "+ci->getClassName().getString()+" {\n";
            for (int i=0,m=ci->getPropertyCount(); i<m; i++)
            {
                CIMConstProperty p = ci->getProperty(i);
                str.append("  "+typeToString(p.getType())+
                    " "+p.getName().getString()+
                    " = "+p.getValue().toString()+";\n");
            }
            str.append("};\n");
        }
        else if (obj->getFtab() == (void*)CMPI_ObjectPath_Ftab ||
            obj->getFtab() == (void*)CMPI_ObjectPathOnStack_Ftab)
        {
            str = ((CIMObjectPath*)obj->getHdl())->toString();
        }
        else if (obj->getFtab()==(void*)CMPI_String_Ftab)
        {
            str = String((const char*)obj->getHdl());
        }
        else if (obj->getFtab() == (void*)CMPI_Args_Ftab ||
            obj->getFtab() == (void*)CMPI_ArgsOnStack_Ftab)
        {
            const Array<CIMParamValue>* arg =
                (Array<CIMParamValue>*)obj->getHdl();
            for (int i=0,m=arg->size(); i < m; i++)
            {
                const CIMParamValue &p = (*arg)[i];
                str.append(
                    p.getParameterName()+":"+p.getValue().toString()+"\n");
            }
        }
        else
        {
            sprintf(msg,"** Object not recognized (%p) **",o);
            CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
            CMPIString* cmpiString =
                reinterpret_cast<CMPIString*>(new CMPI_Object(msg));
            PEG_METHOD_EXIT();
            return cmpiString;
        }

        sprintf(msg,"%p: ",o);
        CMPIString* cmpiString = 
            reinterpret_cast<CMPIString*>(new CMPI_Object(String(msg)+str));
        PEG_METHOD_EXIT();
        return cmpiString;
    }

    static CMPIBoolean mbEncClassPathIsA(
        const CMPIBroker *mb,
        const CMPIObjectPath *eCp,
        const char *type,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncClassPathIsA()");

        CMSetStatus(rc,CMPI_RC_OK);
        if ((eCp==NULL) || (type==NULL))
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Parameter - eCp || type in \
                CMPI_BrokerEnc:mbEncClassPathIsA");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            PEG_METHOD_EXIT();
            return false;
        }
        if (CIMName::legal(type) == false)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Class in \
                CMPI_BrokerEnc:mbEncClassPathIsA");
            CMSetStatus(rc,CMPI_RC_ERR_INVALID_CLASS);
            PEG_METHOD_EXIT();
            return 0;
        }
        CIMObjectPath* cop = (CIMObjectPath*)eCp->hdl;

        const CIMName tcn(type);

        if (tcn == cop->getClassName())
        {
            PEG_METHOD_EXIT();
            return 1;
        }

        CIMClass *cc = mbGetClass(mb,*cop);
        if (cc == NULL)
        {
            PEG_METHOD_EXIT();
            return 0;
        }
        CIMObjectPath  scp(*cop);
        scp.setClassName(cc->getSuperClassName());

        for (; !scp.getClassName().isNull(); )
        {
            cc = mbGetClass(mb,scp);
            if (cc == NULL)
            {
                PEG_METHOD_EXIT();
                return 0;
            }
            if (cc->getClassName() == tcn)
            {
                PEG_METHOD_EXIT();
                return 1;
            }
            scp.setClassName(cc->getSuperClassName());
        };
        PEG_METHOD_EXIT();
        return 0;
    }

    static CMPIBoolean mbEncIsOfType(
        const CMPIBroker *mb,
        const void *o,
        const char *type,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncIsOfType()");
        CMPI_Object *obj = (CMPI_Object*)o;
        char msg[128];
        void *Ftab = NULL;

        if (obj==NULL)
        {
            sprintf(msg,"** Null object ptr (%p) **",o);
            if (rc)
            {
                CMSetStatusWithChars(mb, rc, CMPI_RC_ERR_FAILED, msg);
            }
            PEG_METHOD_EXIT();
            return 0;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        Ftab = obj->getFtab();

        if (((Ftab == (void*)CMPI_Instance_Ftab) ||
            (Ftab == (void*)CMPI_InstanceOnStack_Ftab)) &&
            strncmp(type, CMPIInstance_str, CMPIInstance_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (((Ftab == (void*)CMPI_ObjectPath_Ftab) ||
            (Ftab == (void*)CMPI_ObjectPathOnStack_Ftab)) &&
            strncmp(type, CMPIObjectPath_str, CMPIObjectPath_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (((Ftab == (void*)CMPI_Args_Ftab) ||
            (Ftab == (void*)CMPI_ArgsOnStack_Ftab)) &&
            strncmp(type, CMPIArgs_str, CMPIArgs_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (((Ftab == (void*)CMPI_Context_Ftab) ||
            (Ftab == (void*)CMPI_ContextOnStack_Ftab)) &&
            strncmp(type, CMPIContext_str, CMPIContext_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (((Ftab == (void*)CMPI_ResultRefOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultInstOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultData_Ftab) ||
            (Ftab == (void*)CMPI_ResultMethOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultResponseOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultExecQueryOnStack_Ftab)) &&
            strncmp(type, CMPIResult_str, CMPIResult_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_DateTime_Ftab &&
            strncmp(type, CMPIDateTime_str, CMPIDateTime_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_Array_Ftab &&
            strncmp(type, CMPIArray_str, CMPIArray_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_String_Ftab &&
            strncmp(type, CMPIString_str, CMPIString_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_SelectExp_Ftab &&
            strncmp(type, CMPISelectExp_str, CMPISelectExp_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_SelectCond_Ftab &&
            strncmp(type, CMPISelectCond_str, CMPISelectCond_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_SubCond_Ftab &&
            strncmp(type, CMPISubCond_str, CMPISubCond_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_Predicate_Ftab &&
            strncmp(type, CMPIPredicate_str, CMPIPredicate_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (Ftab == (void*)CMPI_Broker_Ftab &&
            strncmp(type, CMPIBroker_str, CMPIBroker_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }
        if (((Ftab == (void*)CMPI_ObjEnumeration_Ftab) ||
            (Ftab == (void*)CMPI_InstEnumeration_Ftab) ||
            (Ftab == (void*)CMPI_OpEnumeration_Ftab)) &&
            strncmp(type, CMPIEnumeration_str, CMPIEnumeration_str_l) == 0)
        {
            PEG_METHOD_EXIT();
            return 1;
        }

        sprintf(msg, "** Object not recognized (%p) **", o);
        if (rc)
        {
            CMSetStatusWithChars(mb, rc, CMPI_RC_ERR_FAILED, msg);
        }
        PEG_METHOD_EXIT();
        return 0;
    }

    static CMPIString* mbEncGetType(
        const CMPIBroker *mb,
        const  void* o,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncGetType()");
        CMPI_Object *obj = (CMPI_Object*)o;
        char msg[128];
        void *Ftab= NULL;

        if (obj==NULL)
        {
            sprintf(msg, "** Null object ptr (%p) **", o);
            if (rc)
            {
                CMSetStatusWithChars(mb, rc, CMPI_RC_ERR_FAILED, msg);
            }
            PEG_METHOD_EXIT();
            return 0;
        }

        CMSetStatus(rc, CMPI_RC_OK);
        Ftab = obj->getFtab();

        if ((Ftab == (void*)CMPI_Instance_Ftab) ||
            (Ftab == (void*)CMPI_InstanceOnStack_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIInstance_str, rc);
        }
        if ((Ftab == (void*)CMPI_ObjectPath_Ftab) ||
            (Ftab == (void*)CMPI_ObjectPathOnStack_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIObjectPath_str, rc);
        }
        if ((Ftab == (void*)CMPI_Args_Ftab) ||
            (Ftab == (void*)CMPI_ArgsOnStack_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIArgs_str, rc);
        }
        if ((Ftab == (void*)CMPI_Context_Ftab) ||
            (Ftab == (void*)CMPI_ContextOnStack_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIContext_str, rc);
        }
        if ((Ftab == (void*)CMPI_ResultRefOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultInstOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultData_Ftab) ||
            (Ftab == (void*)CMPI_ResultMethOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultResponseOnStack_Ftab) ||
            (Ftab == (void*)CMPI_ResultExecQueryOnStack_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIResult_str, rc);
        }
        if (Ftab == (void*)CMPI_DateTime_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIDateTime_str, rc);
        }
        if (Ftab == (void*)CMPI_Array_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIArray_str, rc);
        }
        if (Ftab == (void*)CMPI_String_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIString_str, rc);
        }
        if (Ftab == (void*)CMPI_SelectExp_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPISelectExp_str, rc);
        }
        if (Ftab == (void*)CMPI_SelectCond_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPISelectCond_str, rc);
        }
        if (Ftab == (void*)CMPI_SubCond_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPISubCond_str, rc);
        }
        if (Ftab == (void*)CMPI_Predicate_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIPredicate_str, rc);
        }
        if (Ftab == (void*)CMPI_Broker_Ftab)
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIBroker_str, rc);
        }
        if ((Ftab == (void*)CMPI_ObjEnumeration_Ftab) ||
            (Ftab == (void*)CMPI_InstEnumeration_Ftab) ||
            (Ftab == (void*)CMPI_OpEnumeration_Ftab))
        {
            PEG_METHOD_EXIT();
            return mb->eft->newString(mb, CMPIEnumeration_str, rc);
        }
        sprintf(msg, "** Object not recognized (%p) **", o);
        if (rc)
        {
            CMSetStatusWithChars(mb, rc, CMPI_RC_ERR_FAILED, msg);
        }
        PEG_METHOD_EXIT();
        return 0;
    }

#if defined (CMPI_VER_85)

    static CMPIString* mbEncGetMessage(
        const CMPIBroker *mb,
        const char *msgId,
        const char *defMsg,
        CMPIStatus* rc,
        CMPICount count,
        ...)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncGetMessage()");
        MessageLoaderParms parms(msgId, defMsg);
        int err = 0;
        if (rc)
        {
            rc->rc = CMPI_RC_OK;
        }
        if (count > 0)
        {
            va_list argptr;
            va_start(argptr, count);
            for (;;)
            {
                if (count > 0)
                {
                    parms.arg0 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 1)
                {
                    parms.arg1 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 2)
                {
                    parms.arg2 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 3)
                {
                    parms.arg3 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 4)
                {
                    parms.arg4 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 5)
                {
                    parms.arg5 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 6)
                {
                    parms.arg6 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 7)
                {
                    parms.arg7 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 8)
                {
                    parms.arg8 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 9)
                {
                    parms.arg9 = formatValue(&argptr, rc, &err);
                }
                break;
            }
            va_end(argptr);
        }
        String nMsg = MessageLoader::getMessage(parms);
        PEG_METHOD_EXIT();
        return string2CMPIString(nMsg);
    }
#endif

#ifdef CMPI_VER_200
    static CMPIStatus mbEncOpenMessageFile(
        const CMPIBroker* mb,
        const char* msgFile,
        CMPIMsgFileHandle* msgFileHandle)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncOpenMessageFile()");
        CMPIStatus rc = { CMPI_RC_OK, NULL };
        AutoPtr<MessageLoaderParms> parms(new MessageLoaderParms());
        parms->msg_src_path = msgFile;
        // Get the AcceptLanguage entry
        const CMPIContext *ctx = CMPI_ThreadContext::getContext();
        CMPIData data = ctx->ft->getEntry(ctx, CMPIAcceptLanguage, &rc);
        if (rc.rc != CMPI_RC_ERR_NO_SUCH_PROPERTY)
        {
            if (rc.rc == CMPI_RC_OK)
            {
                parms->acceptlanguages =
                    LanguageParser::parseAcceptLanguageHeader(
                        CMGetCharPtr(data.value.string));
            }
            else
            {
                PEG_METHOD_EXIT();
                return rc; // should be CMPI_RC_ERR_INVALID_HANDLE
            }
        }
        MessageLoader::openMessageFile(*parms.get());

        ContentLanguageList cll = parms->contentlanguages;
        // Check that we have at least one content language (ie. matching
        // resource bundle was found) before adding to Invocation Context.
        if (cll.size() > 0)
        {
            ctx->ft->addEntry(
                ctx,
                CMPIContentLanguage,
                (CMPIValue*)(const char*)
                    LanguageParser::
                        buildContentLanguageHeader(cll).getCString(),
                CMPI_chars);
        }

        *msgFileHandle = (void *)parms.release();
        PEG_METHOD_EXIT();
        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus mbEncCloseMessageFile(
        const CMPIBroker *mb,
        const CMPIMsgFileHandle msgFileHandle)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncCloseMessageFile()");
        MessageLoaderParms* parms;
        parms = (MessageLoaderParms*)msgFileHandle;
        MessageLoader::closeMessageFile(*parms);
        delete parms;
        PEG_METHOD_EXIT();
        CMReturn(CMPI_RC_OK);
    }

    static CMPIString* mbEncGetMessage2(
        const CMPIBroker *mb,
        const char *msgId,
        const CMPIMsgFileHandle msgFileHandle,
        const char *defMsg,
        CMPIStatus* rc,
        CMPICount count,
        ...)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncGetMessage2()");
        MessageLoaderParms* parms;
        parms = (MessageLoaderParms*)msgFileHandle;
        parms->msg_id = String(msgId);
        parms->default_msg = String(defMsg);
        int err = 0;
        if (rc)
        {
            rc->rc=CMPI_RC_OK;
        }

        if (count > 0)
        {
            va_list argptr;
            va_start(argptr, count);
            for (;;)
            {
                if (count > 0)
                {
                     parms->arg0 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 1)
                {
                    parms->arg1 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 2)
                {
                    parms->arg2 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 3)
                {
                    parms->arg3 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 4)
                {
                    parms->arg4 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 5)
                {
                    parms->arg5 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 6)
                {
                    parms->arg6 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 7)
                {
                    parms->arg7 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 8)
                {
                    parms->arg8 = formatValue(&argptr, rc, &err);
                }
                else
                {
                    break;
                }
                if (count > 9)
                {
                    parms->arg9 = formatValue(&argptr, rc, &err);
                }
                break;
            }
            va_end(argptr);
        }
        String nMsg = MessageLoader::getMessage2(*parms);
        PEG_METHOD_EXIT();
        return string2CMPIString(nMsg);
    }
#endif

#if defined(CMPI_VER_100)
    CMPIStatus mbEncLogMessage(
        const CMPIBroker*,
        CMPISeverity severity,
        const char *id,
        const char *text,
        const CMPIString *string)
    {
        if (!id || !(text || string))
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        String logString = id;
        Uint32 logSeverity = Logger::INFORMATION;
        logString.append(":");

        if (string)
        {
            logString.append(( char*)CMGetCharsPtr(string,NULL));
        }
        else
        {
            logString.append(text);
        }
        // There are no notion in CMPI spec about what 'severity' means.
        // So we are going to try to map
        if (severity <= 1)
        {
            logSeverity = Logger::INFORMATION;
        }
        else if (severity == 2)
        {
            logSeverity = Logger::WARNING;
        }
        else if (severity == 3)
        {
            logSeverity = Logger::SEVERE;
        }
        else if (severity == 4)
        {
            logSeverity = Logger::FATAL;
        }
        Logger::put(Logger::STANDARD_LOG, id, logSeverity, logString);
        CMReturn ( CMPI_RC_OK);
    }

    inline Uint32 mapTraceLevel(CMPILevel level)
    {
        // There are no notion in CMPI spec about what 'level' means.
        // So we are going to try to map . We don't want to map to LEVEL1
        // as it requires the PEG_METHOD_ENTER and PEG_METHOD_EXIT macros.
        switch (level)
        {
            case CMPI_LEV_INFO:
                return Tracer::LEVEL2;
                break;
            case CMPI_LEV_WARNING:
                return Tracer::LEVEL3;
                break;
            case CMPI_LEV_VERBOSE:
                return Tracer::LEVEL4;
                break;
            default:
                return Tracer::LEVEL4;
        }
    }

    inline const char* decideTraceString(
        const char *text,
        const CMPIString *string)
    {
        if (text)
        {
            return text;
        }
        return (const char*) CMGetCharsPtr(string, NULL);
    }

    CMPIStatus mbEncTracer(
        const CMPIBroker*,
        CMPILevel level,
        const char *component,
        const char *text,
        const CMPIString *string)
    {
        if (!component || !(text || string))
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }
        // emptiness of component checked at begin of mbEncTracer
        // don't have to expect a null pointer here
        // through the first check in mbEncTracer
        // we know that return from decideTraceString is not null
        PEG_TRACE(
            (TRC_CMPIPROVIDER,
                mapTraceLevel(level),
                "%s: %s",
                component,
                decideTraceString(text, string)));

        CMReturn ( CMPI_RC_OK);
    }
#endif

    static CMPISelectExp *mbEncNewSelectExp(
        const CMPIBroker *mb,
        const char *query,
        const char *lang,
        CMPIArray ** projection,
        CMPIStatus * st)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncNewSelectExp()");
        int exception = 1;
        int useShortNames = 0;
        CMPIStatus rc = { CMPI_RC_OK, NULL };

        if (strncmp (lang, CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) == 0)
        {
            // Get the namespace.
            const CMPIContext *ctx = CMPI_ThreadContext::getContext ();

            CMPIData data = ctx->ft->getEntry (ctx, CMPIInitNameSpace, &rc);
            if (rc.rc != CMPI_RC_OK)
            {
                CMSetStatus (st, CMPI_RC_ERR_FAILED);
                PEG_METHOD_EXIT();
                return NULL;
            }

            // Create the CIMOMHandle wrapper.
            CIMOMHandle *cm_handle = CM_CIMOM (mb);
            CIMOMHandleQueryContext qcontext(
                CIMNamespaceName(CMGetCharPtr(data.value.string)),
                *cm_handle);

            String sLang (lang);
            String sQuery (query);

            WQLSelectStatement *stmt = new WQLSelectStatement (sLang,
                                                               sQuery,
                                                               qcontext);
            try
            {
                WQLParser::parse(query, *stmt);
                exception = 0;
            }
            catch (const ParseError &)
            {
                if (st)
                {
                    CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
                }
            }
            catch (const MissingNullTerminator &)
            {
                if (st)
                {
                    CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
                }
            }

            if (exception)
            {
                delete stmt;
                if (projection)
                {
                    *projection = NULL;
                }
                PEG_METHOD_EXIT();
                return NULL;
            }
            if (projection)
            {
                if (stmt->getAllProperties ())
                {
                    *projection = NULL;
                }
                else
                {
                    *projection =
                        mbEncNewArray(
                            mb,
                            stmt->getSelectPropertyNameCount(),
                            CMPI_string,
                            NULL);
                    for (int i = 0, m = stmt->getSelectPropertyNameCount();
                        i < m; i++)
                    {
                        const CIMName &n = stmt->getSelectPropertyName(i);
                        /* cerr << "Property: " << n.getString() << endl;
                           Since the array and the CIMName disappear when this
                           function exits we use CMPI data storage - the
                           CMPI_Object keeps a list of data and cleans it up
                           when the provider API function is exited.
                        */
                        CMPIString *str_data =
                            reinterpret_cast<CMPIString *>(
                                new CMPI_Object(n.getString()));
                        CMPIValue value;
                        value.string = str_data;

                        rc = CMSetArrayElementAt(
                            *projection,
                            i,
                            &value,
                            CMPI_string);
                        if (rc.rc != CMPI_RC_OK)
                        {
                            if (st)
                            {
                                CMSetStatus (st, rc.rc);
                            }
                            PEG_METHOD_EXIT();
                            return NULL;
                        }
                    }
                }
            }
            stmt->hasWhereClause ();
            if (st)
            {
                CMSetStatus (st, CMPI_RC_OK);
            }
            CMPISelectExp* cmpiSelectExp = new CMPI_SelectExp(stmt);

            PEG_METHOD_EXIT();
            return (cmpiSelectExp);
        }
#ifndef PEGASUS_DISABLE_CQL
        if ((strncmp (lang, CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0) ||
            (strncmp (lang, "CIMxCQL", 7) == 0) ||
            (strncmp (lang, "CIM:CQL", 7) == 0))
        {
            /* IBMKR: This will have to be removed when the CMPI spec is
               updated with a clear explanation of what properties array
               can have as strings. For right now, if useShortNames is set
               to true, _only_ the last chained identifier is used.
            */
            if (strncmp (lang, CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0)
            {
                useShortNames = 1;
            }
            // Get the namespace.
            const CMPIContext *ctx = CMPI_ThreadContext::getContext ();

            CMPIData data = ctx->ft->getEntry (ctx, CMPIInitNameSpace, &rc);
            if (rc.rc != CMPI_RC_OK)
            {
                if (st)
                {
                    CMSetStatus (st, CMPI_RC_ERR_FAILED);
                }
                PEG_METHOD_EXIT();
                return NULL;
            }

            // Create the CIMOMHandle wrapper.
            CIMOMHandle *cm_handle = CM_CIMOM (mb);
            CIMOMHandleQueryContext qcontext(
                CIMNamespaceName(CMGetCharPtr(data.value.string)),
                *cm_handle);

            String sLang (lang);
            String sQuery (query);

            CQLSelectStatement *selectStatement =
                new CQLSelectStatement(sLang, sQuery, qcontext);
            try
            {
                CQLParser::parse (query, *selectStatement);
                selectStatement->validate ();
                exception = 0;
            }
            catch (...)
            {
                PEG_TRACE_CSTRING(
                    TRC_CMPIPROVIDERINTERFACE,
                    Tracer::LEVEL2,
                    "Exception: Unknown Exception received...");
                if (st)
                {
                    PEG_TRACE_CSTRING(
                        TRC_CMPIPROVIDERINTERFACE,
                        Tracer::LEVEL2,
                        "Exception: Invalid Query Exception received...");
                    CMSetStatus (st, CMPI_RC_ERR_INVALID_QUERY);
                }
            }

            if (exception)
            {
                delete selectStatement;
                if (projection)
                {
                    *projection = NULL;
                }
                PEG_METHOD_EXIT();
                return NULL;
            }
            else
            {
                if (projection)
                {
                    Array <CQLChainedIdentifier> select_Array =
                        selectStatement->getSelectChainedIdentifiers();

                    // Special check. Remove it when useShortNames is not
                    // neccessary
                    if ((select_Array.size() == 1) &&
                        (useShortNames) &&
                        (select_Array[0].getLastIdentifier().getName() \
                         .getString() == String::EMPTY))
                    {
                        *projection= NULL;

                    }
                    else
                    {
                        *projection =
                            mbEncNewArray(
                                mb,
                                select_Array.size(),
                                CMPI_string,
                                NULL);

                        CQLIdentifier identifier;
                        String name;

                        for (Uint32 i = 0; i < select_Array.size (); i++)
                        {
                            if (useShortNames)
                            {
                                identifier =
                                    select_Array[i].getLastIdentifier();
                                name = identifier.getName ().getString ();
                            }
                            else
                            {
                                name = select_Array[i].toString ();
                            }
                            /* Since the array and the CIMName disappear when
                               this function exits we use CMPI data storage -
                               the CMPI_Object keeps a list of data and cleans
                               it up when the provider API function is exited.
                               cerr << "Property: " << name << endl;
                            */
                            CMPIString *str_data =
                                reinterpret_cast<CMPIString *>(
                                    new CMPI_Object (name));
                            CMPIValue value;
                            value.string = str_data;

                            rc = CMSetArrayElementAt(
                                *projection,
                                i,
                                &value,
                                CMPI_string);

                            if (rc.rc != CMPI_RC_OK)
                            {
                                if (st)
                                {
                                    CMSetStatus (st, rc.rc);
                                }
                                PEG_METHOD_EXIT();
                                return NULL;
                            }
                        }
                    }
                }
            }
            if (st)
            {
                CMSetStatus (st, CMPI_RC_OK);
            }
            CMPI_SelectExp* cmpiSelectExp = new CMPI_SelectExp(
                selectStatement, false, qcontext.clone());
            PEG_METHOD_EXIT();
            return (cmpiSelectExp);
        }
#endif
        if (st)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Query Language is currently NOT SUPPORTED");
            CMSetStatus (st, CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED);
        }
        return NULL;
    }

#if defined (CMPI_VER_90) && !defined(CMPI_VER_100)

    static CMPIArray *mbEncGetKeyList(
        CMPIBroker *mb,
        CMPIContext *ctx,
        CMPIObjectPath *cop,
        CMPIStatus *rc)
    {
        PEG_METHOD_ENTER(
            TRC_CMPIPROVIDERINTERFACE,
            "CMPI_BrokerEnc:mbEncGetKeyList()");
        if ((cop==NULL) || (ctx==NULL))
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Invalid Parameter cop || ctx in \
                CMPI_BrokerEnc:mbEncGetKeyList");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            PEG_METHOD_EXIT();
            return NULL;
        }
        CIMObjectPath *op = (CIMObjectPath*)cop->hdl;

        if (!op)
        {
            PEG_TRACE_CSTRING(
                TRC_CMPIPROVIDERINTERFACE,
                Tracer::LEVEL2,
                "Received Invalid Handle cop->handle in \
                CMPI_BrokerEnc:mbEncGetKeyList");
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_HANDLE);
            PEG_METHOD_EXIT();
            return NULL;
        }
        CIMClass *cls = mbGetClass(mb,*op);
        Array<String> keys;
        for (int i=0,m=cls->getPropertyCount(); i<m; i++)
        {
            CIMConstProperty p = cls->getProperty(i);
            Uint32 k = p.findQualifier("key");
            if (k != PEG_NOT_FOUND)
            {
                keys.append(p.getName().getString());
            }
        }
        CMPIArray *ar = mb->eft->newArray(mb, keys.size(), CMPI_string, NULL);
        for (Uint32 i=0,m=keys.size(); i<m; i++)
        {
            String s = keys[i];
            CMPIString *str = string2CMPIString(s);
            ar->ft->setElementAt(ar, i, (CMPIValue*)&str, CMPI_string);
        }
        CMSetStatus(rc, CMPI_RC_OK);
        PEG_METHOD_EXIT();
        return ar;
    }


#endif

}

static CMPIBrokerEncFT brokerEnc_FT =
{
    CMPICurrentVersion,
    mbEncNewInstance,
    mbEncNewObjectPath,
    mbEncNewArgs,
    mbEncNewString,
    mbEncNewArray,
    mbEncNewDateTime,
    mbEncNewDateTimeFromBinary,
    mbEncNewDateTimeFromString,
    mbEncNewSelectExp,
    mbEncClassPathIsA,
    mbEncToString,
    mbEncIsOfType,
    mbEncGetType,
#if defined (CMPI_VER_85)
    mbEncGetMessage,
#endif
#if defined (CMPI_VER_90) && !defined(CMPI_VER_100)
    mbEncGetKeyList,
#endif
#if defined (CMPI_VER_100)
    mbEncLogMessage,
    mbEncTracer,
#endif
#if defined (CMPI_VER_200)
    mbEncNewCMPIError,
    mbEncOpenMessageFile,
    mbEncCloseMessageFile,
    mbEncGetMessage2,
#endif
};

CMPIBrokerEncFT *CMPI_BrokerEnc_Ftab = &brokerEnc_FT;


PEGASUS_NAMESPACE_END



