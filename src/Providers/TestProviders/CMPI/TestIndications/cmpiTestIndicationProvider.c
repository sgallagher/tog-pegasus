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
// Author: Konrad Rzeszutek <konradr@us.ibm.com>
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Pegasus/Provider/CMPI/cmpi_cql.h>
#include <Pegasus/Provider/CMPI/cmpios.h>

#if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#else
#include <sys/time.h>
#endif

#define _IndClassName "TestCMPI_Indication"
#define _Namespace    "test/TestProvider"
#define _ProviderLocation "/src/Providers/TestProviders/CMPI/TestIndications/tests/"
#define _LogExtension ".log"

#ifdef CMPI_VER_100
static const CMPIBroker *_broker;
#else
static CMPIBroker *_broker;
#endif
static CMPI_MUTEX_TYPE _mutex;
static CMPI_COND_TYPE _cond;
static char *_ns;
static int _thread_runs = 0;
static int _thread_active = 0;
static CMPISelectExp *_se;

unsigned char CMPI_true = 1;
unsigned char CMPI_false = 0;
static FILE *fd = NULL;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
/* ---------------------------------------------------------------------------*/
#if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)

struct timeval
{
	long int tv_sec;      
	long int tv_usec;
};

void gettimeofday(struct timeval* t,void* timezone)
{       struct _timeb timebuffer;
        _ftime( &timebuffer );
        t->tv_sec=timebuffer.time;
        t->tv_usec=1000*timebuffer.millitm;
}
#endif

void
PROV_LOG (const char *fmt, ...)
{

  va_list ap;
  if (!fd)
    fd = stderr;

  fprintf (fd, " ");
  va_start (ap, fmt);
  vfprintf (fd, fmt, ap);
  va_end (ap);

  fprintf (fd, "\n");
  fflush (fd);
}

void
PROV_LOG_CLOSE ()
{
  if (fd != stderr)
    fclose (fd);
  fd = stderr;
}

void
PROV_LOG_OPEN (const char *file, const char *location)
{
  char *path = NULL;
  const char *env;
  size_t i = 0;
  size_t j = 0;
  size_t len = strlen (file);
  size_t env_len = 0;
  size_t loc_len = strlen (location);
  size_t ext_len = strlen (_LogExtension);

  env = getenv ("PEGASUS_ROOT");
  if (env)
    env_len = strlen (env);

  path = (char *) malloc (env_len + len + loc_len + ext_len);

  strncpy (path, env, env_len);

  path[env_len] = 0;
  strncat (path, location, loc_len);
  for (i = 0; i < len; i++)
    /* Only use good names. */
    if (isalpha (file[i]))
      {
        path[j + env_len + loc_len] = file[i];
        j++;
      }
  path[j + env_len + loc_len] = 0;
  strncat (path, _LogExtension, ext_len);
  path[j + env_len + loc_len + ext_len] = 0;

  fd = fopen (path, "a+");
  if (fd == NULL)
    fd = stderr;
  free (path);
}

/* ---------------------------------------------------------------------------*/
/*                       Helper functions                        */
/* ---------------------------------------------------------------------------*/
const char *
strCMPIStatus (CMPIStatus rc)
{


  switch (rc.rc)
    {

    case CMPI_RC_OK:
      return "CMPI_RC_OK";
    case CMPI_RC_ERR_FAILED:
      return "CMPI_RC_ERR_FAILED";
    case CMPI_RC_ERR_ACCESS_DENIED:
      return "CMPI_RC_ERR_ACCESS_DENIED";
    case CMPI_RC_ERR_INVALID_NAMESPACE:
      return "CMPI_RC_ERR_INVALID_NAMESPACE";
    case CMPI_RC_ERR_INVALID_PARAMETER:
      return "CMPI_RC_ERR_INVALID_PARAMETER";
    case CMPI_RC_ERR_INVALID_CLASS:
      return "CMPI_RC_ERR_INVALID_CLASS";
    case CMPI_RC_ERR_NOT_FOUND:
      return "CMPI_RC_ERR_NOT_FOUND";
    case CMPI_RC_ERR_NOT_SUPPORTED:
      return "CMPI_RC_ERR_NOT_SUPPORTED";
    case CMPI_RC_ERR_CLASS_HAS_CHILDREN:
      return "CMPI_RC_ERR_CLASS_HAS_CHILDREN";
    case CMPI_RC_ERR_CLASS_HAS_INSTANCES:
      return "CMPI_RC_ERR_CLASS_HAS_INSTANCES";
    case CMPI_RC_ERR_INVALID_SUPERCLASS:
      return "CMPI_RC_ERR_INVALID_SUPERCLASS";
    case CMPI_RC_ERR_ALREADY_EXISTS:
      return "CMPI_RC_ERR_ALREADY_EXISTS";
    case CMPI_RC_ERR_NO_SUCH_PROPERTY:
      return "CMPI_RC_ERR_NO_SUCH_PROPERTY";
    case CMPI_RC_ERR_TYPE_MISMATCH:
      return "CMPI_RC_ERR_TYPE_MISMATCH";
    case CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED:
      return "CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED";
    case CMPI_RC_ERR_INVALID_QUERY:
      return "CMPI_RC_ERR_INVALID_QUERY";
    case CMPI_RC_ERR_METHOD_NOT_AVAILABLE:
      return "CMPI_RC_ERR_METHOD_NOT_AVAILABLE";
    case CMPI_RC_ERR_METHOD_NOT_FOUND:
      return "CMPI_RC_ERR_METHOD_NOT_FOUND";
    case CMPI_RC_ERROR_SYSTEM:
      return "CMPI_RC_ERROR_SYSTEM";
    case CMPI_RC_ERROR:
      return "CMPI_RC_ERROR";
    default:
      return "Unknown error.";
    }

  return "";
}

const char *
strCMPIType (CMPIType type)
{

  switch (type)
    {
    case CMPI_null:
      return "CMPI_null";
    case CMPI_SIMPLE:
      return "CMPI_SIMPLE";
    case CMPI_char16:
      return "CMPI_char16";
    case CMPI_REAL:
      return "CMPI_REAL";
    case CMPI_real64:
      return "CMPI_real64";
    case CMPI_UINT:
      return "CMPI_UINT";
    case CMPI_uint16:
      return "CMPI_uint16";
    case CMPI_uint32:
      return "CMPI_uint32";
    case CMPI_uint64:
      return "CMPI_uint64";
    case CMPI_sint16:
      return "CMPI_sint16";
    case CMPI_sint32:
      return "CMPI_sint32";
    case CMPI_ENC:
      return "CMPI_ENC";
    case CMPI_ref:
      return "CMPI_ref";
    case CMPI_args:
      return "CMPI_args";
    case CMPI_class:
      return "CMPI_class";
    case CMPI_filter:
      return "CMPI_filter";
    case CMPI_enumeration:
      return "CMPI_enumeration";
    case CMPI_string:
      return "CMPI_string";
    case CMPI_chars:
      return "CMPI_chars";
    case CMPI_dateTime:
      return "CMPI_dateTime";
    case CMPI_ptr:
      return "CMPI_ptr";
    case CMPI_charsptr:
      return "CMPI_charsptr";
    case CMPI_ARRAY:
      return "CMPI_ARRAY";
    case CMPI_SIMPLEA:
      return "CMPI_SIMPLEA";
    case CMPI_char16A:
      return "CMPI_char16A";
    case CMPI_REALA:
      return "CMPI_REALA";
    case CMPI_real64A:
      return "CMPI_real64A";
    case CMPI_uint16A:
      return "CMPI_uint16A";
    case CMPI_uint32A:
      return "CMPI_uint32A";
    case CMPI_uint64A:
      return "CMPI_uint64A";
    case CMPI_SINTA:
      return "CMPI_SINTA";
    case CMPI_sint16A:
      return "CMPI_sint16A";
    case CMPI_sint32A:
      return "CMPI_sint32A";
    case CMPI_sint64A:
      return "CMPI_sint64A";
    case CMPI_ENCA:
      return "CMPI_ENCA";
    case CMPI_stringA:
      return "CMPI_stringA";
    case CMPI_charsA:
      return "CMPI_charsA";
    case CMPI_dateTimeA:
      return "CMPI_dateTimeA";
    case CMPI_refA:
      return "CMPI_refA";
    case CMPI_charsptrA:
      return "CMPI_charsptrA";
    case CMPI_keyInteger:
      return "CMPI_keyInteger";
    case CMPI_integerString:
      return "CMPI_integerString";
    case CMPI_realString:
      return "CMPI_realString";
    case CMPI_numericString:
      return "CMPI_numericString";
    case CMPI_booleanString:
      return "CMPI_booleanString";
    case CMPI_dateTimeString:
      return "CMPI_dateTimeString";
    default:
      return "Unknown";

    }
  return "";
}

const char *
strCMPIValueState (CMPIValueState state)
{
  switch (state)
    {
    case CMPI_goodValue:
      return "CMPI_goodValue";
    case CMPI_nullValue:
      return "CMPI_nullValue";
    case CMPI_keyValue:
      return "CMPI_keyValue";
    case CMPI_notFound:
      return "CMPI_notFound";
    case CMPI_badValue:
      return "CMPI_badValue";
    default:
      return "Unknown state";

    }
  return "";

}

const char *
strCMPIPredOp (CMPIPredOp op)
{
  switch (op)
    {
    case CMPI_PredOp_Equals:
      return " CMPI_PredOp_Equals ";
    case CMPI_PredOp_NotEquals:
      return " CMPI_PredOp_NotEquals ";
    case CMPI_PredOp_LessThan:
      return " CMPI_PredOp_LessThan ";
    case CMPI_PredOp_GreaterThanOrEquals:
      return " CMPI_PredOp_GreaterThanOrEquals ";
    case CMPI_PredOp_GreaterThan:
      return " CMPI_PredOp_GreaterThan ";
    case CMPI_PredOp_LessThanOrEquals:
      return " CMPI_PredOp_LessThanOrEquals ";
    case CMPI_PredOp_Isa:
      return " CMPI_PredOp_Isa ";
    case CMPI_PredOp_NotIsa:
      return " CMPI_PredOp_NotIsa ";
    case CMPI_PredOp_Like:
      return " CMPI_PredOp_Like ";
    case CMPI_PredOp_NotLike:
      return " CMPI_PredOp_NotLike ";
    default:
      return "Unknown operation";
    }
}
const char *
strCMPIValue (CMPIValue value)
{
  /* This function only handles string values */
  if (value.string != NULL)
    return CMGetCharPtr (value.string);
  return "No value";
}

/* ---------------------------------------------------------------------------*/
/*                       CMPI Helper function                        */
/* ---------------------------------------------------------------------------*/

CMPIObjectPath *
make_ObjectPath (const char *ns, const char *clss)
{
  CMPIObjectPath *objPath = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG ("--- make_ObjectPath: CMNewObjectPath");
  objPath = CMNewObjectPath (_broker, ns, clss, &rc);

  PROV_LOG ("----- %s", strCMPIStatus (rc));

  return objPath;
}

/* ---------------------------------------------------------------------------*/
/*                       CMPI Accessor function                        */
/* ---------------------------------------------------------------------------*/

CMPIData
instance_accessor (const char *name, void *param)
{

  CMPIData data = { 0, CMPI_null, {0} };
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG ("-- #4.2 instance_accessor");
  PROV_LOG ("-- Property: %s", name);
/* SELECT * FROM CIM_InstModification WHERE (SourceInstance="Boo" OR SourceInstance="Hello") AND PreviousInstance !="Hello again"]
*/
  if (strcmp ("PropertyA", name) == 0)
    {
      data.type = CMPI_string;
      data.state = CMPI_goodValue;
      data.value.string = CMNewString (_broker, "AccessorPropertyA", &rc);
    }
  if (strcmp ("PropertyB", name) == 0)
    {
      data.type = CMPI_string;
      data.state = CMPI_goodValue;
      data.value.string = CMNewString (_broker, "AccessorPropertyB", &rc);
    }
  if (strcmp ("n", name) == 0)
    {
      data.type = CMPI_uint64;
      data.state = CMPI_goodValue;
      data.value.uint64 = 42;
    }
  if (strcmp ("f", name) == 0)
    {
      data.type = CMPI_real64;
      data.state = CMPI_goodValue;
      data.value.real64 = 0.42;
    }
  if (strcmp ("s", name) == 0)
    {
      data.type = CMPI_string;
      data.state = CMPI_goodValue;
      data.value.string = CMNewString (_broker, "s", &rc);
    }

  PROV_LOG ("----  (%s)", strCMPIStatus (rc));
  return data;
}

/* ---------------------------------------------------------------------------*/
/*                       Thread function                                      */
/* ---------------------------------------------------------------------------*/

static CMPI_THREAD_RETURN CMPI_THREAD_CDECL
thread (void *context)
{
  CMPIString *str = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Eval = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Clone = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Inst = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Pred = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Array = { CMPI_RC_OK, NULL };
  CMPIStatus rc_CMGetPropertyAt = { CMPI_RC_OK, NULL };

  CMPIString *type = NULL;
  CMPISelectExp *clone = NULL;
  CMPIBoolean evalRes;
  CMPIInstance *inst = NULL;
  CMPIObjectPath *objPath = NULL;
  CMPIString *name = NULL;
  CMPIData prop_data = { 0, CMPI_null, {0} };
  CMPIData data = { 0, CMPI_null, {0} };
  CMPIArray *projection = NULL;
  /* General purpose counters */
  unsigned int idx;
  CMPICount cnt;
  /* Select Condition and sub conditions */
  CMPISelectCond *cond = NULL;
  int sub_type;
  CMPISubCond *subcnd = NULL;
  /* Predicate operations */
  CMPICount pred_cnt;
  unsigned int pred_idx;
  CMPIPredicate *pred = NULL;
  CMPIType pred_type;
  CMPIPredOp pred_op;
  CMPIString *left_side = NULL;
  CMPIString *right_side = NULL;
  CMPIContext *ctx;

  // Get the CMPIContext and attach it to this thread.
  ctx = (CMPIContext *) context;
  rc = CBAttachThread (_broker, ctx);

  while (_thread_runs == 0)
    {
      // Wait until we get the message that we can start running
      _broker->xft->lockMutex (_mutex);
      // Wait until we get a condition change, which is happening only when
      // _thread_runs is changed
      _broker->xft->condWait (_cond, _mutex);
      _broker->xft->unlockMutex (_mutex);
    }
  _thread_active = 1;

  //PROV_LOG_OPEN (_IndClassName, _ProviderLocation);
  PROV_LOG ("--- CBAttachThread callled.");
  PROV_LOG ("---- %s", strCMPIStatus (rc));

  PROV_LOG ("--- %s CMPI thread(void *) running",_IndClassName);

  PROV_LOG ("-- #1 Clone");
  clone = CMClone (_se, &rc_Clone);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));
  if (clone)
    CMRelease (clone);

  /*
     This functionality is not used in indication providers, but instead in
     ExecQuery provider API (instance providers). But for the sake
     of completness this functionality is also used here. */

  PROV_LOG ("-- #1.1 CMNewSelectExp");
  str = CMGetSelExpString (_se, &rc);
  clone =
    //CMPI_CQL_NewSelectExp (_broker, CMGetCharPtr (str), "CIMxCQL",
    CMNewSelectExp (_broker, CMGetCharPtr (str), "CIM:CQL",
                    &projection, &rc_Clone);

  PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));
  if (clone)
    {
      if (projection)
        {
          PROV_LOG ("--- #1.2 Projection list is: ");
          cnt = CMGetArrayCount (projection, &rc_Array);
          PROV_LOG ("---- %s", strCMPIStatus (rc_Array));
          PROV_LOG ("--- #1.3 CMGetArrayCount, %d", cnt);
          for (idx = 0; idx < cnt; idx++)
            {
              PROV_LOG ("--- #1.4 CMGetArrayElementAt");
              data = CMGetArrayElementAt (projection, idx, &rc_Array);
              PROV_LOG ("---- %s", strCMPIStatus (rc_Array));
              PROV_LOG ("---- type is : %d", data.type);
              if (data.type == CMPI_chars)
                {
                  PROV_LOG ("---- %s (chars)", data.value.chars);
                }
              if (data.type == CMPI_string)
                {
                  PROV_LOG ("---- %s (string)",
                            CMGetCharPtr (data.value.string));
                }
            }
        }
      else
        {
          PROV_LOG
            ("--- #1.2 No projection list, meaning it is SELECT * .... ");
        }
    }

  PROV_LOG ("-- #2 MakeObjectPath");
  // Create instance

  objPath = make_ObjectPath (_Namespace, _IndClassName);
  type = CDGetType (_broker, objPath, &rc_Inst);
  PROV_LOG ("---- %s (%s)", CMGetCharPtr (type), strCMPIStatus (rc_Inst));
  CMRelease (type);

  PROV_LOG ("-- #3 Instance");
  inst = CMNewInstance (_broker, objPath, &rc_Inst);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Inst));

  if (rc_Inst.rc == CMPI_RC_ERR_NOT_FOUND)
    {
      PROV_LOG (" --- Class %s is not found in the %s namespace!",
                _IndClassName, _Namespace);
      PROV_LOG (" --- Aborting!!! ");
      goto exit;
    }

  type = CDGetType (_broker, inst, &rc_Inst);
  if (type)
    {
      PROV_LOG ("---- %s (%s)", CMGetCharPtr (type), strCMPIStatus (rc_Inst));
      CMRelease (type);
    }

  PROV_LOG ("- CMGetPropertyCount: %d", CMGetPropertyCount (inst, &rc));
  // Set each property.
  for (idx = 0; idx < CMGetPropertyCount (inst, &rc); idx++)
    {
      prop_data = CMGetPropertyAt (inst, idx, &name, &rc_CMGetPropertyAt);
      // Really dumb way of doing it. Just set each property with its own property name.
      if (prop_data.type == CMPI_string)
        {
          CMSetProperty (inst, CMGetCharPtr (name), CMGetCharPtr (name),
                         CMPI_chars);
        }
      prop_data = CMGetPropertyAt (inst, idx, &name, &rc_CMGetPropertyAt);

      PROV_LOG ("-- %d: %s(%s: %s: %s) [%s]", idx,
                CMGetCharPtr (name),
                strCMPIType (prop_data.type),
                strCMPIValueState (prop_data.state),
                strCMPIValue (prop_data.value),
                strCMPIStatus (rc_CMGetPropertyAt));

    }

  PROV_LOG ("-- #4 Evaluate using instance");
  evalRes = CMEvaluateSelExp (_se, inst, &rc_Eval);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Eval));
  if (evalRes == CMPI_true)
    {
      PROV_LOG ("--- True, sending indication");
      rc_Eval = CBDeliverIndication (_broker, ctx, _Namespace, inst);
      PROV_LOG ("---- %s ", strCMPIStatus (rc_Eval));
    }
  else
    {
      PROV_LOG ("--- False");
    }

  PROV_LOG ("-- #4.1 Evalute using accessor");
  evalRes =
    CMEvaluateSelExpUsingAccessor (_se, instance_accessor, NULL, &rc_Eval);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Eval));
  if (evalRes == CMPI_true)
    {
      PROV_LOG ("--- True, but not sending indication");
    }
  else
    {
      PROV_LOG ("--- False");
    }
  PROV_LOG ("-- #5 CMGetSelExpString");
  str = CMGetSelExpString (_se, &rc);
  type = CDGetType (_broker, str, &rc_Inst);
  if (type != NULL)
    {
      PROV_LOG ("---- %s (%s)", CMGetCharPtr (type), strCMPIStatus (rc_Inst));
      CMRelease (type);
    }

  PROV_LOG ("---- %s", strCMPIStatus (rc));
  PROV_LOG ("-- #5.1 Query is [%s]", CMGetCharPtr (str));
  PROV_LOG ("-- #5.2 Query is [%s]", _ns);

  PROV_LOG ("-- #6 Continue evaluating using GetDOC");
  {
    cond = CMGetDoc (_se, &rc);
    PROV_LOG ("---- %s", strCMPIStatus (rc));
    if (cond != NULL)
      {

        PROV_LOG ("--- #6.1 CMGetSubCondCountAndType ");
        cnt = CMGetSubCondCountAndType (cond, &sub_type, &rc);
        PROV_LOG ("---- %s", strCMPIStatus (rc));


        PROV_LOG ("---- Number of disjunctives: %d, Type: %X", cnt, sub_type);

        /* Parsing the disjunctives */
        for (idx = 0; idx < cnt; idx++)
          {
            PROV_LOG ("--- #6.2 CMGetSubCondAt @ %d ", idx);
            subcnd = CMGetSubCondAt (cond, idx, &rc);
            PROV_LOG ("---- %s", strCMPIStatus (rc));

            PROV_LOG ("--- #6.3 CMGetPredicateCount");
            pred_cnt = CMGetPredicateCount (subcnd, &rc);
            PROV_LOG ("---- %s", strCMPIStatus (rc));
            PROV_LOG ("---- Number of predicates in the conjuctives: %d",
                      pred_cnt);

            /* Parsing throught conjuctives */
            for (pred_idx = 0; pred_idx < pred_cnt; pred_idx++)
              {
                PROV_LOG ("--- #6.4 CMGetPredicateAt, %d", pred_idx);
                pred = CMGetPredicateAt (subcnd, pred_idx, &rc);

                PROV_LOG ("---- %s", strCMPIStatus (rc));
                PROV_LOG ("--- #6.4 CMGetPredicateData");
                rc = CMGetPredicateData (pred,
                                         &pred_type,
                                         &pred_op, &left_side, &right_side);
                PROV_LOG ("---- %s", strCMPIStatus (rc));

                PROV_LOG ("----- Type: %s , CMPIPredOp: %s, LS: %s, RS: %s",
                          strCMPIType (pred_type), strCMPIPredOp (pred_op),
                          CMGetCharPtr (left_side),
                          CMGetCharPtr (right_side));

                PROV_LOG ("--- #6.5 Evaluate using predicate");
#ifdef CMPI_VER_100
// The CMEvaluatePredicate is gone in the CMPI 1.0 standard.
                evalRes =
                  CMEvaluatePredicateUsingAccessor (pred, instance_accessor,
                                                    NULL, &rc_Pred);
#else
                // One can also evaluate this specific predicate
                evalRes =
                  CMEvaluatePredicate (pred, "PredicateEvaluation",
                                       CMPI_chars, &rc_Pred);
#endif
                PROV_LOG ("---- %s", strCMPIStatus (rc_Pred));

                if (evalRes == CMPI_true)
                  {
                    PROV_LOG ("--- True");
                  }
                else
                  {
                    PROV_LOG ("--- False");
                  }
              }

          }
      }
  }
  PROV_LOG ("-- #7 GetCOD");
  {
    cond = CMGetCod (_se, &rc);
    PROV_LOG ("---- %s", strCMPIStatus (rc));
    /* Currently this is not supported in Pegasus. */
    if (cond != NULL)
      {
      }
  }
exit:
  PROV_LOG ("--- CBDetachThread called");
  rc = CBDetachThread (_broker, ctx);
  PROV_LOG ("---- %s", strCMPIStatus (rc));

  PROV_LOG ("--- %s CMPI thread(void *) exited", _IndClassName);

  _thread_active = 0;
  return (CMPI_THREAD_RETURN) 0;
}

/* ---------------------------------------------------------------------------*/
/*                       Indication Provider Interface                        */
/* ---------------------------------------------------------------------------*/

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIIndicationProviderIndicationCleanup (CMPIIndicationMI * mi,
                                             const CMPIContext * ctx,
                                             CMPIBoolean term)
#else
CMPIStatus
TestCMPIIndicationProviderIndicationCleanup (CMPIIndicationMI * mi,
                                             CMPIContext * ctx)
#endif
{
/*
   PROV_LOG ("--- %s CMPI IndicationCleanup() called", _IndClassName);
   PROV_LOG ("--- %s CMPI IndicationCleanup() exited", _IndClassName);
*/
   CMReturn (CMPI_RC_OK);
}

#ifdef CMPI_VER_100
/* Note: In the CMPI spec the CMPIResult parameter is not passed anymore. */
CMPIStatus
TestCMPIIndicationProviderAuthorizeFilter (CMPIIndicationMI * mi,
                                           const CMPIContext * ctx,
                                           const CMPISelectExp * se,
                                           const char *ns,
                                           const CMPIObjectPath * op,
                                           const char *user)
#else
CMPIStatus
TestCMPIIndicationProviderAuthorizeFilter (CMPIIndicationMI * mi,
                                           CMPIContext * ctx,
                                           CMPIResult * rslt,
                                           CMPISelectExp * se,
                                           const char *ns,
                                           CMPIObjectPath * op,
                                           const char *user)
#endif
{
  CMPIString *str = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG ("--- %s CMPI AuthorizeFilter() called", _IndClassName);
  /* we don't object */
  if (strcmp (ns, _IndClassName) == 0)
    {
      PROV_LOG ("--- %s Correct class", _IndClassName);
    }
  else
    {
      CMReturn (CMPI_RC_ERR_INVALID_CLASS);
    }
  PROV_LOG ("--- %s CMPI AuthorizeFilter() exited", _IndClassName);

  CMReturn (CMPI_RC_OK);
}

#ifdef CMPI_VER_100
/* Note: In the CMPI spec the CMPIResult parameter is not passed anymore. */
CMPIStatus
TestCMPIIndicationProviderMustPoll (CMPIIndicationMI * mi,
                                    const CMPIContext * ctx,
                                    const CMPISelectExp * se,
                                    const char *ns, const CMPIObjectPath * op)
#else
CMPIStatus
TestCMPIIndicationProviderMustPoll (CMPIIndicationMI * mi,
                                    CMPIContext * ctx,
                                    CMPIResult * rslt,
                                    CMPISelectExp * se,
                                    const char *ns, CMPIObjectPath * op)
#endif
{
  CMPIString *str = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };


  PROV_LOG ("--- %s CMPI MustPoll() called", _IndClassName);

  PROV_LOG ("--- %s CMPI MustPoll() exited", _IndClassName);

  /* no polling */
  CMReturn (CMPI_RC_ERR_NOT_SUPPORTED);
}

#ifdef CMPI_VER_100
/* Note: In the CMPI spec the CMPIResult parameter is not passed anymore. */
CMPIStatus
TestCMPIIndicationProviderActivateFilter (CMPIIndicationMI * mi,
                                          const CMPIContext * ctx,
                                          const CMPISelectExp * se,
                                          const char *ns,
                                          const CMPIObjectPath * op,
                                          CMPIBoolean firstActivation)
#else
CMPIStatus
TestCMPIIndicationProviderActivateFilter (CMPIIndicationMI * mi,
                                          CMPIContext * ctx,
                                          CMPIResult * rslt,
                                          CMPISelectExp * se,
                                          const char *ns,
                                          CMPIObjectPath * op,
                                          CMPIBoolean firstActivation)
#endif
{
  CMPIContext *context;
  CMPIStatus rc_Clone = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_IndClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI ActivateFilter() called", _IndClassName);
  // We have to pass in the parameters some way. One way would be
  // to do it via the void pointer to the thread, but for simplicity
  // we are just using global values.
  _se = (CMPISelectExp *) se;
  _ns = strdup (ns);
  // Initalize our mutex and condition. We use that to kick of thread
  // processing when 'enableIndication' has been called.
  _mutex = _broker->xft->newMutex (0);
  _cond = _broker->xft->newCondition (0);
  // Get a new CMPI_Context which the thread will use.
  context = CBPrepareAttachThread (_broker, ctx);
  _broker->xft->newThread (thread, context, 0);
  PROV_LOG ("--- %s CMPI ActivateFilter() exited", _IndClassName);
  CMReturn (CMPI_RC_OK);

}

#ifdef CMPI_VER_100
/* Note: In the CMPI spec the CMPIResult parameter is not passed anymore. */
CMPIStatus
TestCMPIIndicationProviderDeActivateFilter (CMPIIndicationMI * mi,
                                            const CMPIContext * ctx,
                                            const CMPISelectExp * se,
                                            const char *ns,
                                            const CMPIObjectPath * op,
                                            CMPIBoolean lastActivation)
#else
CMPIStatus
TestCMPIIndicationProviderDeActivateFilter (CMPIIndicationMI * mi,
                                            CMPIContext * ctx,
                                            CMPIResult * rslt,
                                            CMPISelectExp * se,
                                            const char *ns,
                                            CMPIObjectPath * op,
                                            CMPIBoolean lastActivation)
#endif
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIString *str = NULL;

  struct timespec wait = { 0, 0 };
  struct timeval t;

  // We want to delay until the thread is finished.
  //
  while (_thread_active)
    {
      gettimeofday (&t, NULL);
      // Set the time wait to 1 second.
      wait.tv_sec = t.tv_sec + 1;
      wait.tv_nsec = 0;

      _broker->xft->lockMutex (_mutex);
      // Wait 1 second has expired or the condition has changed.
      _broker->xft->timedCondWait (_cond, _mutex, &wait);
      _broker->xft->unlockMutex (_mutex);
    }
  // Release the internal data.
  free (_ns);
  _ns = NULL;
  // Release the mutex and condition
  _broker->xft->destroyMutex (_mutex);
  _broker->xft->destroyCondition (_cond);
  _mutex = NULL;
  _cond = NULL;
  CMReturn (CMPI_RC_OK);
}

#ifdef CMPI_VER_100
void
TestCMPIIndicationProviderEnableIndications (CMPIIndicationMI * mi,
                                             const CMPIContext * ctx)
#else
void
TestCMPIIndicationProviderEnableIndications (CMPIIndicationMI * mi)
#endif
{
  //PROV_LOG_OPEN (_IndClassName, _ProviderLocation);
  //PROV_LOG ("--- %s CMPI EnableIndication() exited", _IndClassName);
  _broker->xft->lockMutex (_mutex);
  _thread_runs = 1;
  _broker->xft->signalCondition (_cond);
  _broker->xft->unlockMutex (_mutex);
  //PROV_LOG ("--- %s CMPI EnableIndication() exited", _IndClassName);
}

#ifdef CMPI_VER_100
void
TestCMPIIndicationProviderDisableIndications (CMPIIndicationMI * mi,
                                              const CMPIContext * ctx)
#else
void
TestCMPIIndicationProviderDisableIndications (CMPIIndicationMI * mi)
#endif
{
  //PROV_LOG_OPEN (_IndClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI DisableIndication() exited", _IndClassName);

  //  Since DeActiveFilter is called before DisableIndications, and it
  // deletes the mutex and condition, we better check it first.
  if (_mutex)
    {
      _broker->xft->lockMutex (_mutex);
      _thread_runs = 0;
      _broker->xft->signalCondition (_cond);
      _broker->xft->unlockMutex (_mutex);
    }
  PROV_LOG ("--- %s CMPI DisableIndication() exited", _IndClassName);

  // In reality we should close in Cleanup function, but that function
  // is only called during unloading, which might take a long time
  // so we just do it here.
  PROV_LOG_CLOSE ();
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/
CMIndicationMIStub (TestCMPIIndicationProvider,
                    TestCMPIIndicationProvider, _broker, CMNoHook);
/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/
