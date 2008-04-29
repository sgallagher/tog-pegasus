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

#include "cmpiPerf_TestAssocClassProvider.h"

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/

#define  _ClassName     "cmpiPerf_TestAssocClass"
#define  _RefLeft       "Antecedent"
#define  _RefRight      "Dependent"
#define  _RefLeftClass  "cmpiPerf_TestClassA"
#define  _RefRightClass "cmpiPerf_TestClassB"

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus cmpiPerf_TestAssocClassProviderCleanup(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    CMPIBoolean terminating)
{
    CMReturn(CMPI_RC_OK);
}

CMPIStatus cmpiPerf_TestAssocClassProviderEnumInstanceNames(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref)
{
    CMPIStatus rc    = {CMPI_RC_OK, NULL};
    int        refrc = 0;

    refrc = _assoc_create_inst_1toN(
                _broker,
                ctx,
                rslt,
                ref,
                _ClassName,
                _RefLeftClass,
                _RefRightClass,
                _RefLeft,
                _RefRight,
                1,
                0,
                &rc);

    if (refrc != 0)
    {
        return rc;
    }
    CMReturnDone( rslt );
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderEnumInstances(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref,
    const char ** properties)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    int refrc = 0;

    refrc = _assoc_create_inst_1toN(
                _broker,
                ctx,
                rslt,
                ref,
                _ClassName,
                _RefLeftClass,
                _RefRightClass,
                _RefLeft,
                _RefRight,
                1,
                1,
                &rc);

    if (refrc != 0)
    {
        return rc;
    }
    CMReturnDone( rslt );
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderGetInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char ** properties)
{
    CMPIInstance * ci = NULL;
    CMPIStatus     rc = {CMPI_RC_OK, NULL};

    ci = _assoc_get_inst(_broker,ctx,cop,_ClassName,_RefLeft,_RefRight,&rc);

    if (ci == NULL)
    {
        return rc;
    }

    CMReturnInstance( rslt, ci );
    CMReturnDone(rslt);
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderCreateInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const CMPIInstance * ci)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};


    CMSetStatusWithChars(
        _broker,
        &rc,
        CMPI_RC_ERR_NOT_SUPPORTED,
        "CIM_ERR_NOT_SUPPORTED" );
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderModifyInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const CMPIInstance * ci,
    const char **properties)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMSetStatusWithChars(
        _broker,
        &rc,
        CMPI_RC_ERR_NOT_SUPPORTED,
        "CIM_ERR_NOT_SUPPORTED" );
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderDeleteInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMSetStatusWithChars(
        _broker,
        &rc,
        CMPI_RC_ERR_NOT_SUPPORTED,
        "CIM_ERR_NOT_SUPPORTED" );
    return rc;
}

CMPIStatus cmpiPerf_TestAssocClassProviderExecQuery(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref,
    const char * lang,
    const char * query)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMSetStatusWithChars(
        _broker,
        &rc,
        CMPI_RC_ERR_NOT_SUPPORTED,
        "CIM_ERR_NOT_SUPPORTED" );
    return rc;
}


/* ---------------------------------------------------------------------------*/
/*                    Associator Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus cmpiPerf_TestAssocClassProviderAssociationCleanup(
    CMPIAssociationMI * mi,
    const CMPIContext * ctx,
    CMPIBoolean terminating)
{
    CMReturn(CMPI_RC_OK);
}

CMPIStatus cmpiPerf_TestAssocClassProviderAssociators(
    CMPIAssociationMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char * assocClass,
    const char * resultClass,
    const char * role,
    const char * resultRole,
    const char ** propertyList )
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath * op = NULL;
    int refrc = 0;

    if (assocClass)
    {
        op = CMNewObjectPath(
                 _broker,
                 CMGetCharPtr(CMGetNameSpace(cop,&rc)),
                 _ClassName,
                 &rc );

        if (op==NULL)
        {
            CMSetStatusWithChars(
                _broker,
                &rc,
                CMPI_RC_ERR_FAILED,
                "Create CMPIObjectPath failed." );
            return rc;
        }
    }

    if ((0==assocClass) || (CMClassPathIsA(_broker,op,assocClass,&rc)==1))
    {
        /* check parameters given for validity */
        /* for instance ensure source instance exists */
        if (0==_assoc_check_parameter_const(
                   _broker,
                   cop,
                   _RefLeft,
                   _RefRight,
                   _RefLeftClass,
                   _RefRightClass,
                   resultClass,
                   role,
                   resultRole,
                   &rc ))
        {
            return rc;
        }
        /* create associated instances */
        refrc=_assoc_create_refs_1toN(
                  _broker,
                  ctx,
                  rslt,
                  cop,
                  _ClassName,
                  _RefLeftClass,
                  _RefRightClass,
                  _RefLeft,
                  _RefRight,
                  1,
                  1,
                  &rc);

        if (refrc != 0)
        {
            return rc;
        }
    }
    CMReturnDone( rslt );
    CMReturn(CMPI_RC_OK);
}

CMPIStatus cmpiPerf_TestAssocClassProviderAssociatorNames(
    CMPIAssociationMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char * assocClass,
    const char * resultClass,
    const char * role,
    const char * resultRole)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath * op = NULL;
    int refrc = 0;

    if (assocClass)
    {
        op = CMNewObjectPath(
                 _broker,
                 CMGetCharPtr(CMGetNameSpace(cop,&rc)),
                 _ClassName,
                 &rc );
        if (op==NULL)
        {
            CMSetStatusWithChars(
                _broker,
                &rc,
                CMPI_RC_ERR_FAILED,
                "Create CMPIObjectPath failed.");
            return rc;
        }
    }

    if ((0==assocClass) || (CMClassPathIsA(_broker,op,assocClass,&rc)==1))
    {
        if (0==_assoc_check_parameter_const(
                   _broker,
                   cop,
                   _RefLeft,
                   _RefRight,
                   _RefLeftClass,
                   _RefRightClass,
                   resultClass,
                   role,
                   resultRole,
                   &rc))
        {
            return rc;
        }

        refrc=_assoc_create_refs_1toN(
                  _broker,
                  ctx,
                  rslt,
                  cop,
                  _ClassName,
                  _RefLeftClass,
                  _RefRightClass,
                  _RefLeft,
                  _RefRight,
                  0,
                  1,
                  &rc);

        if (refrc != 0)
        {
            return rc;
        }
    }
    CMReturnDone(rslt);
    CMReturn(CMPI_RC_OK);
}

CMPIStatus cmpiPerf_TestAssocClassProviderReferences(
    CMPIAssociationMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char * assocClass,
    const char * role,
    const char ** propertyList)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath * op = NULL;
    int refrc = 0;

    if (assocClass)
    {
        op=CMNewObjectPath(
               _broker,
               CMGetCharPtr(CMGetNameSpace(cop,&rc)),
               _ClassName,
               &rc);

        if (op==NULL)
        {
            CMSetStatusWithChars(
                _broker,
                &rc,
                CMPI_RC_ERR_FAILED,
                "Create CMPIObjectPath failed.");
            return rc;
        }
    }

    if ((0==assocClass) || (CMClassPathIsA(_broker,op,assocClass,&rc)==1))
    {
        if (0 == _assoc_check_parameter_const(
                     _broker,
                     cop,
                     _RefLeft,
                     _RefRight,
                     _RefLeftClass,
                     _RefRightClass,
                     0,
                     role,
                     0,
                     &rc))
        {
            return rc;
        }

        refrc=_assoc_create_refs_1toN(
                  _broker,
                  ctx,
                  rslt,
                  cop,
                  _ClassName,
                  _RefLeftClass,
                  _RefRightClass,
                  _RefLeft,
                  _RefRight,
                  1,
                  0,
                  &rc);
        if (refrc != 0)
        {
            return rc;
        }
    }
    CMReturnDone( rslt );
    CMReturn(CMPI_RC_OK);
}

CMPIStatus cmpiPerf_TestAssocClassProviderReferenceNames(
    CMPIAssociationMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char * assocClass,
    const char * role)
{
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath * op = NULL;
    int refrc = 0;

    if (assocClass)
    {
        op=CMNewObjectPath(
               _broker,
               CMGetCharPtr(CMGetNameSpace(cop,&rc)),
               _ClassName,
               &rc);
        if (op==NULL)
        {
            CMSetStatusWithChars(
                _broker,
                &rc,
                CMPI_RC_ERR_FAILED,
                "Create CMPIObjectPath failed." );
            return rc;
        }
    }
    if ((0==assocClass) || (CMClassPathIsA(_broker,op,assocClass,&rc)==1))
    {
        if (0==_assoc_check_parameter_const(
                   _broker,
                   cop,
                   _RefLeft,
                   _RefRight,
                   _RefLeftClass,
                   _RefRightClass,
                   0,
                   role,
                   0,
                   &rc))
        {
            return rc;
        }
        refrc=_assoc_create_refs_1toN(
                  _broker,
                  ctx,
                  rslt,
                  cop,
                  _ClassName,
                  _RefLeftClass,
                  _RefRightClass,
                  _RefLeft,
                  _RefRight,
                  0,
                  0,
                  &rc);
        if (refrc != 0)
        {
            return rc;
        }
    }
    CMReturnDone( rslt );
    CMReturn(CMPI_RC_OK);
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/
CMInstanceMIStub(cmpiPerf_TestAssocClassProvider,
                 cmpiPerf_TestAssocClassProvider,
                 _broker,
                 CMNoHook)

CMAssociationMIStub(cmpiPerf_TestAssocClassProvider,
                    cmpiPerf_TestAssocClassProvider,
                    _broker,
                    CMNoHook)
/* ---------------------------------------------------------------------------*/
/*                 end of cmpiPerf_TestAssocClassProvider                   */
/* ---------------------------------------------------------------------------*/
