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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_SelectExp.h"
#include "CMPI_Ftabs.h"
#include "CMPI_Value.h"
#include "CMPI_String.h"
#include "CMPI_SelectExpAccessor_WQL.h"
#include "CMPI_SelectExpAccessor_CQL.h"

#include <Pegasus/WQL/WQLInstancePropertySource.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Repository/RepositoryQueryContext.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/WQL/WQLParser.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define CALL_SIGN_WQL "WQL"
#define CALL_SIGN_WQL_SIZE 3
#define CALL_SIGN_CQL "CIM:CQL"
#define CALL_SIGN_CQL_SIZE 7
extern "C" {

   static CMPIStatus selxRelease(CMPISelectExp* eSx) {
      CMReturn(CMPI_RC_OK);
   }

   static CMPISelectExp* selxClone(CMPISelectExp* eSx, CMPIStatus* rc) {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return NULL;
   }

	/* Helper functions */
   static CMPIBoolean _check_WQL(CMPI_SelectExp *sx, CMPIStatus *rc) {

	if (sx->wql_stmt == NULL)
	  {
	    WQLSelectStatement *stmt = new WQLSelectStatement ();
	    try
	    {
	      WQLParser::parse (sx->cond, *stmt);
	    }
	    catch (...)
	    {
		  delete stmt;
	      CMSetStatus (rc, CMPI_RC_ERR_FAILED);
	      return false;
	    }
		/* Only set it for success */
		sx->wql_stmt = stmt;
	  }			/* sx->wql_stmt ... */
	  return true;
   }
   static CMPIBoolean _check_CQL(CMPI_SelectExp *sx, CMPIStatus *rc) {

		if (sx->cql_stmt == NULL)
	      {
			CIMNamespaceName _ns (sx->_namespace);
		if (_ns.isNull ())
		{
		  // Take that! It should never happend, but you never known.
		  CMSetStatus (rc, CMPI_RC_ERR_INVALID_NAMESPACE);
		  return false;
		}
		/* The constructor should set this to a valid pointer. */
		if (sx->_repository == NULL) {
			CMSetStatus (rc, CMPI_RC_ERROR_SYSTEM);
			return false;
		}
		RepositoryQueryContext rep_ctx (_ns, sx->_repository);

		CQLSelectStatement *selectStatement =
		  new CQLSelectStatement (sx->lang, sx->cond, rep_ctx);
		try
		{
		  CQLParser::parse (sx->cond, *selectStatement);

		  selectStatement->validate ();
		}
		catch (...)
		{
		  delete selectStatement;
		  CMSetStatus (rc, CMPI_RC_ERR_FAILED);
		  return false;
		}

		sx->cql_stmt = selectStatement;
		}
	 return true;
   }

   static CMPIBoolean selxEvaluate(CMPISelectExp* eSx, CMPIInstance* inst, CMPIStatus* rc) {
      CMPI_SelectExp *sx=(CMPI_SelectExp*)eSx;
	  if (!inst) {
		if (rc) CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
		return false;
	  }
	  if (!inst->hdl) {
		if (rc) CMSetStatus(rc,CMPI_RC_ERR_INVALID_PARAMETER);
		return false;
	  }
	CIMInstance *instance = (CIMInstance *) inst->hdl;

    /* WQL */
    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) ==
	0)
      {
		if (_check_WQL(sx, rc))
		{
			try
			{
	  			return sx->wql_stmt->evaluate (*(CIMInstance *) inst->hdl);
			}
			catch (...)
			{
	  			CMSetStatus (rc, CMPI_RC_ERR_FAILED);
	  			return false;
			}
      	}
		else
			return false;
	}
    /* CIM:CQL */
#if 0
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) ==
	0)
      {
	   if (_check_CQL(sx, rc)) {	
	    try
	    {
	      return sx->cql_stmt->evaluate (*instance);
	    }
	    catch (...)
	    {
	      CMSetStatus (rc, CMPI_RC_ERR_FAILED);
	      return false;
	    }
	  }	else
		return false;	
     } 
#endif
    /* Tried some other weird query language which we don't support */
    if (rc) CMSetStatus (rc, CMPI_RC_ERR_NOT_SUPPORTED);
    return false;
   }

   static CMPIBoolean selxEvaluateUsingAccessor(CMPISelectExp* eSx,
               CMPIAccessor* accessor, void *parm, CMPIStatus* rc) {
      CMPI_SelectExp *sx=(CMPI_SelectExp*)eSx;
      if (!accessor) {
		if (rc) CMSetStatus(rc, CMPI_RC_ERR_INVALID_PARAMETER);
	    return false;
      }

    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) == 0)
	{
	 if (_check_WQL(sx, rc)) 
	  {
      	CMPI_SelectExpAccessor_WQL ips(accessor,parm);
      	try {
	     	if (rc) CMSetStatus(rc,CMPI_RC_OK);
         	return sx->wql_stmt->evaluateWhereClause(&ips);
      	}
      	catch (...) {
	     if (rc) CMSetStatus(rc,CMPI_RC_ERR_FAILED);
         return false;
      	}
	  }
	 else
		return false;
	}
#if 0	
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0)
	{
		if (_check_CQL(sx, rc)) 
		{
			CMPI_SelectExpAccessor_CQL ips(accessor, parm, sx->cql_stmt,sx->classNames[0]);
			try {
				if (rc) CMSetStatus(rc, CMPI_RC_OK);
				return sx->cql_stmt->evaluate(ips.getInstance());	
			} catch (...) {
				if (rc) CMSetStatus(rc, CMPI_RC_ERR_FAILED);
				return false;
			}
		}
		  else
			return false;
	}
#endif
    return false;
   }

   static CMPIString* selxGetString(CMPISelectExp* eSx, CMPIStatus* rc) {
      CMPI_SelectExp *sx=(CMPI_SelectExp*)eSx;
	  if (rc) CMSetStatus(rc,CMPI_RC_OK);
      return string2CMPIString(sx->cond);
   }

   static CMPISelectCond* selxGetDOC(CMPISelectExp* eSx, CMPIStatus* rc) {

    CMPI_SelectExp *sx = (CMPI_SelectExp *) eSx;
    CMPISelectCond *sc = NULL;

    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) == 0)
      {
		if (sx->wql_dnf == NULL)
	  	{
	    	sx->wql_dnf = new CMPI_Wql2Dnf (String (sx->cond), String::EMPTY);
	    	sx->tableau = sx->wql_dnf->getTableau ();
	  	}
		sc = (CMPISelectCond *) new CMPI_SelectCond (sx->tableau, 0);
      }
#if 0
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) == 0)
      {
		if (sx->cql_dnf == NULL)
		{
		/* Extract the namespace string. This value is defined by the constructor. */
			CIMNamespaceName _ns (sx->_namespace);

			if (_ns.isNull ()) {
				CMSetStatus (rc, CMPI_RC_ERR_INVALID_NAMESPACE);
				return NULL;
			}

			/* The constructor should set this to a valid pointer. */
			if (sx->_repository == NULL) {
				CMSetStatus (rc, CMPI_RC_ERROR_SYSTEM);
				return NULL;
			}
		
			RepositoryQueryContext rep_ctx (_ns, sx->_repository);
			CQLSelectStatement selectStatement (sx->lang, sx->cond, rep_ctx);
			CMPI_Cql2Dnf *dnf = NULL;
			try
			{
	  			CQLParser::parse (sx->cond, selectStatement);
	  			selectStatement.validate ();
	  			selectStatement.applyContext ();
	  			selectStatement.normalizeToDOC ();
	  			dnf = new CMPI_Cql2Dnf (selectStatement);
			}
			catch (Exception & e)
			{
	  			if (dnf)
					delete dnf;
	  			if (rc) CMSetStatus (rc, CMPI_RC_ERR_FAILED);
			}
			sx->cql_dnf = dnf;
			sx->tableau = sx->cql_dnf->getTableau ();
		}
	  	sc = (CMPISelectCond *) new CMPI_SelectCond (sx->tableau, 0);
	}     
#endif

    if (sc)
    {
		if (rc) CMSetStatus (rc, CMPI_RC_OK);
	  	CMPI_Object *obj = new CMPI_Object(sc);
	  	obj->priv = ((CMPI_SelectCond *)sc)->priv;
	  	return reinterpret_cast<CMPISelectCond *>(obj); 
    }

	/* If the sc was null, we just exit */
    if (rc) CMSetStatus (rc, CMPI_RC_ERR_FAILED);
    return NULL;
   }
   
   static CMPISelectCond* selxGetCOD(CMPISelectExp* eSx, CMPIStatus* rc) {
         if (rc) CMSetStatus(rc,CMPI_RC_ERR_NOT_SUPPORTED);
         return NULL;
   }
   

}

static CMPISelectExpFT selx_FT={
     CMPICurrentVersion,
     selxRelease,
     selxClone,
     selxEvaluate,
     selxGetString,
     selxGetDOC,
     selxGetCOD,
	 selxEvaluateUsingAccessor
};

CMPISelectExpFT *CMPI_SelectExp_Ftab=&selx_FT;

CMPI_SelectExp::CMPI_SelectExp(const OperationContext& ct,CIMRepository *repository, String _ns, String cond_, String lang_)
  :  ctx(ct),  _namespace(_ns), cond(cond_), lang(lang_), _repository(repository) {
   props=NULL;
   ft=CMPI_SelectExp_Ftab;
   wql_dnf=NULL;
   cql_dnf=NULL;
   wql_stmt=NULL;
   cql_stmt=NULL;
   tableau=NULL;
}

CMPI_SelectExp::CMPI_SelectExp(WQLSelectStatement* st)
  : ctx(OperationContext()), wql_stmt(st) {
   CMPI_ThreadContext::addObject((CMPI_Object*)this);
   hdl=NULL;
   ft=CMPI_SelectExp_Ftab;
   props=NULL;
   wql_dnf=NULL;
   cql_dnf=NULL;
   wql_stmt=st;
   cql_stmt=NULL;
   tableau=NULL;
}

PEGASUS_NAMESPACE_END
