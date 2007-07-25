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

#include <Pegasus/Common/CIMError.h>
#include "CMPI_Version.h"

#include "CMPI_Error.h"
#include "CMPI_Ftabs.h"
#include "CMPI_String.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

extern "C" 
{

    CMPIError *newCMPIError(
        const char* owner, const char* msgID, 
        const char * msg,
        const CMPIErrorSeverity sev, 
        const CMPIErrorProbableCause pc, 
        const CMPIrc cimStatusCode)
    {
        CIMError::PerceivedSeverityEnum pgSev = 
            (CIMError::PerceivedSeverityEnum)sev;
        CIMError::ProbableCauseEnum pgPc = (CIMError::ProbableCauseEnum)pc;
        CIMError::CIMStatusCodeEnum pgSc = 
            (CIMError::CIMStatusCodeEnum)cimStatusCode;

        CIMError *cer=new CIMError(owner, msgID, msg, pgSev, pgPc, pgSc);
        return reinterpret_cast<CMPIError*>(new CMPI_Object(cer));
    }

    static CMPIError* errClone(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }
        CIMError* cErr=new CIMError(*cer);
        CMPI_Object* obj=new CMPI_Object(cErr);
        obj->unlink();
        CMPIError* neErr=reinterpret_cast<CMPIError*>(obj);
        CMSetStatus(rc,CMPI_RC_OK);
        return neErr;
    }

    static CMPIStatus errRelease(CMPIError* eErr)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (cer)
        {
            delete cer;
            (reinterpret_cast<CMPI_Object*>(eErr))->unlinkAndDelete();
        }
        CMReturn(CMPI_RC_OK);
    }

    static CMPIErrorType errGetErrorType(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return UnknownErrorType;
        }

        CMPIBoolean notNull;
        CIMError::ErrorTypeEnum pgErrorType;

        try
        {
            notNull = cer->getErrorType(pgErrorType);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return UnknownErrorType;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return UnknownErrorType;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return(CMPIErrorType)pgErrorType;
    }

    static CMPIString* errGetOtherErrorType(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgOtherErrorType;

        try
        {
            notNull = cer->getOtherErrorType(pgOtherErrorType);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgOtherErrorType);
    }

    static CMPIString* errGetOwningEntity(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgOwningEntity;

        try
        {
            notNull = cer->getOwningEntity(pgOwningEntity);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgOwningEntity);
    }

    static CMPIString* errGetMessageID(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgMessageID;

        try
        {
            notNull = cer->getMessageID(pgMessageID);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgMessageID);
    }

    static CMPIString* errGetMessage(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgMessage;

        try
        {
            notNull = cer->getMessage(pgMessage);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgMessage);
    }

    static CMPIErrorSeverity errGetPerceivedSeverity(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return ErrorSevUnknown;
        }

        CMPIBoolean notNull;
        CIMError::PerceivedSeverityEnum pgPerceivedSeverity;

        try
        {
            notNull = cer->getPerceivedSeverity(pgPerceivedSeverity);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return ErrorSevUnknown;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return ErrorSevUnknown;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return(CMPIErrorSeverity)pgPerceivedSeverity;
    }

    static CMPIErrorProbableCause errGetProbableCause(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return ErrorProbCauseUnknown;
        }

        CMPIBoolean notNull;
        CIMError::ProbableCauseEnum pgProbableCause;

        try
        {
            notNull = cer->getProbableCause(pgProbableCause);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return ErrorProbCauseUnknown;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return ErrorProbCauseUnknown;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return(CMPIErrorProbableCause)pgProbableCause;
    }

    static CMPIString* errGetProbableCauseDescription(
        const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgProbCauseDesc;

        try
        {
            notNull = cer->getProbableCauseDescription(pgProbCauseDesc);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgProbCauseDesc);
    }

    static CMPIArray* errGetRecommendedActions(
        const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        Array<String> pgRecommendedActions;

        try
        {
            notNull = cer->getRecommendedActions(pgRecommendedActions);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMPIUint32 arrSize = pgRecommendedActions.size();
        CMPIData *dta=new CMPIData[arrSize+1];
        // first element reserved for type and size
        dta->type=CMPI_string;
        dta->value.uint32=arrSize;
        for (unsigned int i=1; i<=arrSize; i++)
        {
            dta[i].type=CMPI_string;
            dta[i].state=CMPI_goodValue;
            String s = pgRecommendedActions[i-1];
            dta[i].value.string=string2CMPIString(s);
        }
        CMSetStatus(rc,CMPI_RC_OK);
        return reinterpret_cast<CMPIArray*>(new CMPI_Object(dta));
    }

    static CMPIString* errGetErrorSource(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgErrorSource;

        try
        {
            notNull = cer->getErrorSource(pgErrorSource);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgErrorSource);
    }

    static CMPIErrorSrcFormat errGetErrorSourceFormat(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return CMPIErrSrcUnknown;
        }

        CMPIBoolean notNull;
        CIMError::ErrorSourceFormatEnum pgErrorSourceFormat;

        try
        {
            notNull = cer->getErrorSourceFormat(pgErrorSourceFormat);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return CMPIErrSrcUnknown;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return CMPIErrSrcUnknown;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return(CMPIErrorSrcFormat)pgErrorSourceFormat;
    }

    static CMPIString* errGetOtherErrorSourceFormat(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgOtherErrorSourceFormat;

        try
        {
            notNull = cer->getOtherErrorSourceFormat(pgOtherErrorSourceFormat);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgOtherErrorSourceFormat);
    }

    static CMPIrc errGetCIMStatusCode(const CMPIError* eErr, CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return CMPI_RC_ERR_INVALID_PARAMETER;
        }

        CMPIBoolean notNull;
        CIMError::CIMStatusCodeEnum pgCIMStatusCode;

        try
        {
            notNull = cer->getCIMStatusCode(pgCIMStatusCode);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return CMPI_RC_ERR_INVALID_PARAMETER;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return CMPI_RC_ERR_FAILED;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return(CMPIrc)pgCIMStatusCode;
    }

    static CMPIString* errGetCIMStatusCodeDescription(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        String pgCIMStatusCodeDescription;

        try
        {
            notNull = cer->getCIMStatusCodeDescription(
                pgCIMStatusCodeDescription);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMSetStatus(rc,CMPI_RC_OK);
        return string2CMPIString(pgCIMStatusCodeDescription);
    }

    static CMPIArray* errGetMessageArguments(
        const CMPIError* eErr, 
        CMPIStatus* rc)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
            return NULL;
        }

        CMPIBoolean notNull;
        Array<String> pgMessageArguments;

        try
        {
            notNull = cer->getMessageArguments(pgMessageArguments);
            if (!notNull)
            {
                CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
                return NULL;
            }
        }
        catch (...)
        {
            CMSetStatus(rc, CMPI_RC_ERR_FAILED);
            return NULL;
        }

        CMPIUint32 arrSize = pgMessageArguments.size();
        CMPIData *dta=new CMPIData[arrSize+1];
        // first element reserved for type and size
        dta->type=CMPI_string;
        dta->value.uint32=arrSize;
        for (unsigned int i=1; i<=arrSize; i++)
        {
            dta[i].type=CMPI_string;
            dta[i].state=CMPI_goodValue;
            String s = pgMessageArguments[i-1];
            dta[i].value.string=string2CMPIString(s);
        }
        CMSetStatus(rc,CMPI_RC_OK);
        return reinterpret_cast<CMPIArray*>(new CMPI_Object(dta));
    }

    static CMPIStatus errSetErrorType(
        CMPIError* eErr, 
        const CMPIErrorType errorType)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        CIMError::ErrorTypeEnum pgErrorType;
        pgErrorType = (CIMError::ErrorTypeEnum)errorType;

        try
        {
            cer->setErrorType(pgErrorType);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetOtherErrorType(
        CMPIError* eErr, 
        const char* otherErrorType)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        String pgOtherErrorType(otherErrorType);

        try
        {
            cer->setOtherErrorType(pgOtherErrorType);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetProbableCauseDescription(
        CMPIError* eErr, 
        const char* probableCauseDescription)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        String pgProbableCauseDescription(probableCauseDescription);

        try
        {
            cer->setProbableCauseDescription(pgProbableCauseDescription);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetRecommendedActions(
        CMPIError* eErr, 
        const CMPIArray* recommendedActions)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        Array<String> pgRecommendedActions;

        CMPIData* dta=(CMPIData*)recommendedActions->hdl;
        if (!dta)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        try
        {
            for (unsigned int i=0; i<dta->value.uint32; i++)
            {
                CMPIData arrEl;
                const char * arrElStr;
                CMPIStatus rc = {CMPI_RC_OK,NULL};

                if (dta[i].type!=CMPI_string)
                {
                    CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
                }

                arrEl = CMGetArrayElementAt(recommendedActions, i, &rc);
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }

                arrElStr = CMGetCharsPtr(arrEl.value.string, &rc);
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }

                pgRecommendedActions.append(String(arrElStr));
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }
            }

            cer->setRecommendedActions(pgRecommendedActions);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetErrorSource(
        CMPIError* eErr, 
        const char* errorSource)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        String pgErrorSource(errorSource);

        try
        {
            cer->setErrorSource(pgErrorSource);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetErrorSourceFormat(
        CMPIError* eErr, 
        const CMPIErrorSrcFormat errorSrcFormat)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        CIMError::ErrorSourceFormatEnum pgErrorSourceFormat;
        pgErrorSourceFormat = (CIMError::ErrorSourceFormatEnum)errorSrcFormat;

        try
        {
            cer->setErrorSourceFormat(pgErrorSourceFormat);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetOtherErrorSourceFormat(
        CMPIError* eErr, 
        const char* otherErrorSourceFormat)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        String pgOtherErrorSourceFormat(otherErrorSourceFormat);

        try
        {
            cer->setOtherErrorSourceFormat(pgOtherErrorSourceFormat);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetCIMStatusCodeDescription(
        CMPIError* eErr, 
        const char* cimStatusCodeDescription)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        String pgCIMStatusCodeDescription(cimStatusCodeDescription);

        try
        {
            cer->setCIMStatusCodeDescription(pgCIMStatusCodeDescription);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

    static CMPIStatus errSetMessageArguments(
        CMPIError* eErr, 
        CMPIArray* messageArguments)
    {
        CIMError* cer=(CIMError*)eErr->hdl;
        if (!cer)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        Array<String> pgMessageArguments;

        CMPIData* dta=(CMPIData*)messageArguments->hdl;
        if (!dta)
        {
            CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
        }

        try
        {
            for (unsigned int i=0; i<dta->value.uint32; i++)
            {
                CMPIData arrEl;
                const char * arrElStr;
                CMPIStatus rc = {CMPI_RC_OK,NULL};

                if (dta[i].type!=CMPI_string)
                {
                    CMReturn(CMPI_RC_ERR_INVALID_PARAMETER);
                }

                arrEl = CMGetArrayElementAt(messageArguments, i, &rc);
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }

                arrElStr = CMGetCharsPtr(arrEl.value.string, &rc);
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }

                pgMessageArguments.append(String(arrElStr));
                if (rc.rc != CMPI_RC_OK)
                {
                    return rc;
                }
            }

            cer->setMessageArguments(pgMessageArguments);
        }
        catch (...)
        {
            CMReturn(CMPI_RC_ERR_FAILED);
        }

        CMReturn(CMPI_RC_OK);
    }

}

static CMPIErrorFT error_FT = 
{
    CMPICurrentVersion,
    errRelease,
    errClone,
    errGetErrorType,
    errGetOtherErrorType,
    errGetOwningEntity,
    errGetMessageID,
    errGetMessage,
    errGetPerceivedSeverity,
    errGetProbableCause,
    errGetProbableCauseDescription,
    errGetRecommendedActions,
    errGetErrorSource,
    errGetErrorSourceFormat,
    errGetOtherErrorSourceFormat,
    errGetCIMStatusCode,
    errGetCIMStatusCodeDescription,
    errGetMessageArguments,
    errSetErrorType,
    errSetOtherErrorType,
    errSetProbableCauseDescription,
    errSetRecommendedActions,
    errSetErrorSource,
    errSetErrorSourceFormat,
    errSetOtherErrorSourceFormat,
    errSetCIMStatusCodeDescription,
    errSetMessageArguments,
};

CMPIErrorFT *CMPI_Error_Ftab=&error_FT;

PEGASUS_NAMESPACE_END

    
