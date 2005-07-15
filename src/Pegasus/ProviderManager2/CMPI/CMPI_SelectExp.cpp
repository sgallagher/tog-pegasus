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
#include <Pegasus/Provider/CIMOMHandleQueryContext.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/WQL/WQLParser.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define DDD(X)   if (_cmpi_trace) X;

extern int _cmpi_trace;

extern "C"
{

  static CMPIStatus selxRelease (CMPISelectExp * eSx)
  {
 	CMPI_SelectExp *se = (CMPI_SelectExp*)eSx;
        if (!se->persistent) {
		// Do not call unlinkAndDelete - b/c the CMPI_Object::unlinkAndDelete
		// casts the structure to a CMPI_Object and deletes it. But this is a
                // CMPI_SelectExp structure so not all of the variables get deleted. Hence
                // we delete them here.
         	//((CMPI_Object*)se)->unlinkAndDelete();
         	((CMPI_Object*)se)->unlink();
	 	delete se;
        }

    CMReturn (CMPI_RC_OK);
  }

  static CMPISelectExp *selxClone (const CMPISelectExp * eSx, CMPIStatus * rc)
  {
    if (rc)
      CMSetStatus (rc, CMPI_RC_ERR_NOT_SUPPORTED);
    return NULL;
  }

  /* Helper functions */
  static CMPIBoolean _check_WQL (CMPI_SelectExp * sx, CMPIStatus * rc)
  {

    if (sx->wql_stmt == NULL)
      {
        WQLSelectStatement *stmt = new WQLSelectStatement ();
        try
        {
          WQLParser::parse (sx->cond, *stmt);
        }
        catch (const Exception &e) 
	{
	   DDD(cout<<"### exception: _check_WQL - msg: "<<e.getMessage()<<endl);
           if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_INVALID_QUERY,
            	(CMPIString*)string2CMPIString(e.getMessage()));
           delete stmt;
	   return false;
	}
        catch (...)
        {
	  DDD(cout<<"### exception: _check_WQL - ... " <<endl);
          delete stmt;
          if (rc) CMSetStatus (rc, CMPI_RC_ERR_INVALID_QUERY);
          return false;
        }
        /* Only set it for success */
        sx->wql_stmt = stmt;
      }                         /* sx->wql_stmt ... */
    return true;
  }
  static CMPIBoolean _check_CQL (CMPI_SelectExp * sx, CMPIStatus * rc)
  {
    Boolean fail = false;
    if (sx->cql_stmt == NULL)
      {
        /* The constructor should set this to a valid pointer. */
        if (sx->_context == NULL)
          {
            CMSetStatus (rc, CMPI_RC_ERROR_SYSTEM);
            return false;
          }
        CQLSelectStatement *selectStatement =
          new CQLSelectStatement (sx->lang, sx->cond, *sx->_context);
        try
        {
          CQLParser::parse (sx->cond, *selectStatement);

          selectStatement->validate ();
        }
        catch (const Exception &e) 
 	{
	    DDD(cout<<"### exception: _check_CQL - msg: "<<e.getMessage()<<endl);
            if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_INVALID_QUERY,
            		(CMPIString*)string2CMPIString(e.getMessage()));
            fail = true;
        }
        catch (...)
        {
	  DDD(cout<<"### exception: _check_CQL - ... " <<endl);
          if (rc) CMSetStatus (rc, CMPI_RC_ERR_INVALID_QUERY);
	  fail = true;
        }
	if (fail) 
	{
          delete selectStatement;
          return false;
        }
        sx->cql_stmt = selectStatement;
      }
    return true;
  }

  static CMPIBoolean selxEvaluate (const CMPISelectExp * eSx, const CMPIInstance * inst,
                                   CMPIStatus * rc)
  {
    CMPI_SelectExp *sx = (CMPI_SelectExp *) eSx;
    if (!inst)
      {
        if (rc)
          CMSetStatus (rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return false;
      }
    if (!inst->hdl)
      {
        if (rc)
          CMSetStatus (rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return false;
      }
    CIMInstance *instance = (CIMInstance *) inst->hdl;

    /* WQL */
    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) ==
        0)
      {
        if (_check_WQL (sx, rc))
          {
            try
            {
              return sx->wql_stmt->evaluate (*(CIMInstance *) inst->hdl);
            } catch (const Exception &e)
	    {	
	        DDD(cout<<"### exception: selxEvaluate - msg: "<<e.getMessage()<<endl);
                if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_FAILED,
            		(CMPIString*)string2CMPIString(e.getMessage()));
                return false;
	    }
            catch (...)
            {
	      DDD(cout<<"### exception: selxEvaluate - ... " <<endl);
              if (rc) CMSetStatus (rc, CMPI_RC_ERR_FAILED);
              return false;
            }
          }
        else
          return false;
      }
    /* CIM:CQL */
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) ==
        0)
      {
        if (_check_CQL (sx, rc))
          {
            try
            {
              return sx->cql_stmt->evaluate (*instance);
            }
            catch (const Exception &e) 
            {
	        DDD(cout<<"### exception: selxEvaluate - msg: "<<e.getMessage()<<endl);
                if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_FAILED,
            		(CMPIString*)string2CMPIString(e.getMessage()));
		return false;
            }
            catch (...)
            {
	      DDD(cout<<"### exception: selxEvaluate - ... " <<endl);
              if (rc) CMSetStatus (rc, CMPI_RC_ERR_FAILED);
              return false;
            }
          }
        else
          return false;
      }
    /* Tried some other weird query language which we don't support */
    if (rc)
      CMSetStatus (rc, CMPI_RC_ERR_NOT_SUPPORTED);
    return false;
  }

  static CMPIBoolean selxEvaluateUsingAccessor (const CMPISelectExp * eSx,
                                                CMPIAccessor * accessor,
                                                void *parm, CMPIStatus * rc)
  {
    CMPI_SelectExp *sx = (CMPI_SelectExp *) eSx;
    if (!accessor)
      {
        if (rc)
          CMSetStatus (rc, CMPI_RC_ERR_INVALID_PARAMETER);
        return false;
      }

    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) ==
        0)
      {
        if (_check_WQL (sx, rc))
          {
            CMPI_SelectExpAccessor_WQL ips (accessor, parm);
            try
            {
              if (rc)
                CMSetStatus (rc, CMPI_RC_OK);
              return sx->wql_stmt->evaluateWhereClause (&ips);
            }
	    catch (const Exception &e) 
            {
	        DDD(cout<<"### exception: selxEvaluateUsingAccessor - msg: "<<e.getMessage()<<endl);
                if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_FAILED,
            		(CMPIString*)string2CMPIString(e.getMessage()));
		return false;
            }
            catch (...)
            {
	      DDD(cout<<"### exception: selxEvaluateUsingAccessor - ..." << endl);
              if (rc)
                CMSetStatus (rc, CMPI_RC_ERR_FAILED);
              return false;
            }
          }
        else
          return false;
      }
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) ==
        0)
      {
        if (_check_CQL (sx, rc))
          {
            CMPI_SelectExpAccessor_CQL ips (accessor, parm, sx->cql_stmt,
                                            sx->classNames[0]);
            try
            {
              if (rc)
                CMSetStatus (rc, CMPI_RC_OK);
              return sx->cql_stmt->evaluate (ips.getInstance ());
            }
            catch (const Exception &e)
            {
	        DDD(cout<<"### exception: selxEvaluateUsingAccessor - msg: "<<e.getMessage()<<endl);
                if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_FAILED,
            		(CMPIString*)string2CMPIString(e.getMessage()));
		return false;
	    }
            catch (...)
            {
	      DDD(cout<<"### exception: selxEvaluateUsingAccessor - ..." << endl);
              if (rc)
                CMSetStatus (rc, CMPI_RC_ERR_FAILED);
              return false;
            }
          }
        else
          return false;
      }
    return false;
  }

  static CMPIString *selxGetString (const CMPISelectExp * eSx, CMPIStatus * rc)
  {
    CMPI_SelectExp *sx = (CMPI_SelectExp *) eSx;
    if (rc)
      CMSetStatus (rc, CMPI_RC_OK);
    return string2CMPIString (sx->cond);
  }

  static CMPISelectCond *selxGetDOC (const CMPISelectExp * eSx, CMPIStatus * rc)
  {

    CMPI_SelectExp *sx = (CMPI_SelectExp *) eSx;
    CMPISelectCond *sc = NULL;

    if (strncmp (sx->lang.getCString (), CALL_SIGN_WQL, CALL_SIGN_WQL_SIZE) ==
        0)
      {
        if (sx->wql_dnf == NULL)
          {
            sx->wql_dnf = new CMPI_Wql2Dnf (String (sx->cond), String::EMPTY);
            sx->tableau = sx->wql_dnf->getTableau ();
          }
        sc = (CMPISelectCond *) new CMPI_SelectCond (sx->tableau, 0);
      }
    if (strncmp (sx->lang.getCString (), CALL_SIGN_CQL, CALL_SIGN_CQL_SIZE) ==
        0)
      {
        if (sx->cql_dnf == NULL)
          {
            /* The constructor should set this to a valid pointer. */
            if (sx->_context == NULL)
              {
                CMSetStatus (rc, CMPI_RC_ERROR_SYSTEM);
                return NULL;
              }

            CQLSelectStatement selectStatement (sx->lang, sx->cond,
                                                *sx->_context);
            CMPI_Cql2Dnf *dnf = NULL;
            try
            {
              CQLParser::parse (sx->cond, selectStatement);
              dnf = new CMPI_Cql2Dnf (selectStatement);
            }
            catch (const Exception &e)
            {
		 DDD(cout<<"### exception: selxGetDOC - msg: "<<e.getMessage()<<endl);

         	if (rc) CMSetStatusWithString(rc,CMPI_RC_ERR_FAILED,
            		(CMPIString*)string2CMPIString(e.getMessage()));
                if (dnf)
                  delete dnf;
	       return NULL;
            }
            sx->cql_dnf = dnf;
            sx->tableau = sx->cql_dnf->getTableau ();
          }
        sc = (CMPISelectCond *) new CMPI_SelectCond (sx->tableau, 0);
      }
    if (sc)
      {
        if (rc)
          CMSetStatus (rc, CMPI_RC_OK);
        CMPI_Object *obj = new CMPI_Object (sc);
        obj->priv = ((CMPI_SelectCond *) sc)->priv;
        return reinterpret_cast < CMPISelectCond * >(obj);
      }

    /* If the sc was null, we just exit */
    if (rc)
      CMSetStatus (rc, CMPI_RC_ERR_FAILED);
    return NULL;
  }

  static CMPISelectCond *selxGetCOD (const CMPISelectExp * eSx, CMPIStatus * rc)
  {
    if (rc)
      CMSetStatus (rc, CMPI_RC_ERR_NOT_SUPPORTED);
    return NULL;
  }


}

static CMPISelectExpFT selx_FT = {
  CMPICurrentVersion,
  selxRelease,
  selxClone,
  selxEvaluate,
  selxGetString,
  selxGetDOC,
  selxGetCOD,
  selxEvaluateUsingAccessor
};

CMPISelectExpFT *CMPI_SelectExp_Ftab = &selx_FT;

CMPI_SelectExp::~CMPI_SelectExp()
{
  delete wql_stmt;
  delete cql_stmt;
  delete wql_dnf;
  delete cql_dnf;
}
CMPI_SelectExp::CMPI_SelectExp (const OperationContext & ct,
                                QueryContext * context, String cond_,
                                String lang_):
ctx (ct),
cond (cond_),
lang (lang_),
_context (context),
persistent(true)
{
  // We do NOT add ourselves to the CMPI_Object as this is a persitent object.
  // Look at the other construtors.
  props = NULL;
  ft = CMPI_SelectExp_Ftab;
  wql_dnf = NULL;
  cql_dnf = NULL;
  wql_stmt = NULL;
  cql_stmt = NULL;
  tableau = NULL;
}

CMPI_SelectExp::CMPI_SelectExp (WQLSelectStatement * st):ctx (OperationContext ()),
wql_stmt (st), persistent(false)
{
  // Adding the object to the garbage collector.
  CMPI_ThreadContext::addObject ((CMPI_Object *)this);
  hdl = NULL;
  ft = CMPI_SelectExp_Ftab;
  props = NULL;
  wql_dnf = NULL;
  cql_dnf = NULL;
  cql_stmt = NULL;
  tableau = NULL;
  _context = NULL;
  cond = st->getQuery ();
  lang = CALL_SIGN_WQL;
}

CMPI_SelectExp::CMPI_SelectExp (CQLSelectStatement * st):ctx (OperationContext ()),
cql_stmt (st), persistent(false)
{
  // Adding the object to the garbage collector.
  CMPI_ThreadContext::addObject ((CMPI_Object *)this);
  hdl = NULL;
  ft = CMPI_SelectExp_Ftab;
  props = NULL;
  wql_dnf = NULL;
  cql_dnf = NULL;
  wql_stmt = NULL;
  tableau = NULL;
  _context = NULL;
  cond = st->getQuery ();
  lang = CALL_SIGN_CQL;
  classNames = st->getClassPathList ();
}
PEGASUS_NAMESPACE_END
