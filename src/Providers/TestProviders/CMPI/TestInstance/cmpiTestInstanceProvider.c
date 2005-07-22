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

#define _ClassName "TestCMPI_Instance"
#define _ClassName_size strlen(_ClassName)
#define _Namespace    "test/TestProvider"
#define _ProviderLocation "/src/Providers/TestProviders/CMPI/TestInstance/tests/"
#define _LogExtension ".log"

#ifdef CMPI_VER_100
static const CMPIBroker *_broker;
#else
static CMPIBroker *_broker;
#endif

unsigned char CMPI_true = 1;
unsigned char CMPI_false = 0;
static FILE *fd = NULL;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
/* ---------------------------------------------------------------------------*/


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

  env = PEGASUS_ROOT;
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
make_ObjectPath (const CMPIBroker *broker, const char *ns, const char *class)
{
  CMPIObjectPath *objPath = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG ("--- make_ObjectPath: CMNewObjectPath");
  objPath = CMNewObjectPath (broker, ns, class, &rc);
  //assert ( rc.rc == CMPI_RC_OK);
  PROV_LOG ("----- %s", strCMPIStatus (rc));
  CMAddKey (objPath, "ElementName", (CMPIValue *) class, CMPI_chars);

  return objPath;
}

CMPIInstance *
make_Instance (CMPIObjectPath * op)
{

  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIInstance *ci = NULL;


  PROV_LOG ("--- make_Instance: CMNewInstance");
  ci = CMNewInstance (_broker, op, &rc);
  PROV_LOG ("----- %s", strCMPIStatus (rc));
  if (rc.rc == CMPI_RC_ERR_NOT_FOUND)
    {
      PROV_LOG (" --- Class %s is not found in the %s namespace!",
                _ClassName, _Namespace);
      PROV_LOG (" --- Aborting!!! ");
      return NULL;
    }
  return ci;
}


int
_setProperty (CMPIInstance * ci, const char *p)
{
  CMPIValue val;
  const char *property;
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  /*
     WQL just gives the property names, such as :"ElementName".
     CQL is more complex and gives the class from which the
     property belongs, such as "TestCMPIInstance.ElementName".
     So we find the classname in the property name we just
     ignore it.
   */

  if (strncmp (p, _ClassName, _ClassName_size) == 0)
    {
      // The first couple of bytes are the ClassName, then
      // move the pointer (+1 for the dot)
      property = p + _ClassName_size + 1;
    }
  else
    property = p;

  PROV_LOG ("--- _setProperty: %s -> %s", p, property);
  if ((strncmp (property, "ElementName", 11) == 0)
      && (strlen (property) == 11))
    {
      rc =
        CMSetProperty (ci, "ElementName", (CMPIValue *) _ClassName,
                       CMPI_chars);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "s", 1) == 0) && (strlen (property) == 1))
    {
      rc = CMSetProperty (ci, "s", (CMPIValue *) "s", CMPI_chars);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "n32", 3) == 0) && (strlen (property) == 3))
    {
      val.uint32 = 42;
      rc = CMSetProperty (ci, "n32", &val, CMPI_uint32);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "n64", 3) == 0) && (strlen (property) == 3))
    {
      val.uint64 = 64;
      rc = CMSetProperty (ci, "n64", &val, CMPI_uint64);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "n16", 3) == 0) && (strlen (property) == 3))
    {
      val.uint16 = 16;
      rc = CMSetProperty (ci, "n16", &val, CMPI_uint16);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "f", 1) == 0) && (strlen (property) == 1))
    {
      val.real64 = 1.232;
      rc = CMSetProperty (ci, "f", &val, CMPI_real64);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  else if ((strncmp (property, "d", 1) == 0) && (strlen (property) == 1))
    {
      PROV_LOG ("---- CMNewDateTime");
      val.dateTime = CMNewDateTime (_broker, &rc);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
      rc = CMSetProperty (ci, "d", &val, CMPI_dateTime);
      PROV_LOG ("---- %s", strCMPIStatus (rc));
    }
  // This means include _all_ of the properties
  // This means include _all_ of the properties
  else if ((strncmp (property, "*", 1) == 0) && (strlen (property) == 1))
    {
      PROV_LOG ("---- All properties");
      _setProperty (ci, "ElementName");
      _setProperty (ci, "s");
      _setProperty (ci, "n16");
      _setProperty (ci, "n32");
      _setProperty (ci, "n64");
      _setProperty (ci, "f");
      _setProperty (ci, "d");
    }
  else
    {
      PROV_LOG ("---- No implementation for property: %s", property);
      return 1;
    }
  return 0;
}

          /* and many more .. */

/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderCleanup (CMPIInstanceMI * mi, const CMPIContext * ctx,
                                 CMPIBoolean  term)
#else
CMPIStatus
TestCMPIInstanceProviderCleanup (CMPIInstanceMI * mi, CMPIContext * ctx)
#endif
{

  //PROV_LOG("--- %s CMPI Cleanup() called",_ClassName);
  //PROV_LOG("--- %s CMPI Cleanup() exited",_ClassName);
  CMReturn (CMPI_RC_OK);
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderEnumInstanceNames (CMPIInstanceMI * mi,
                                           const CMPIContext * ctx,
                                           const CMPIResult * rslt,
                                           const CMPIObjectPath * ref)
#else
CMPIStatus
TestCMPIInstanceProviderEnumInstanceNames (CMPIInstanceMI * mi,
                                           CMPIContext * ctx,
                                           CMPIResult * rslt,
                                           CMPIObjectPath * ref)
#endif
{
  CMPIObjectPath *op = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);

  PROV_LOG ("--- %s CMPI EnumInstanceNames() called", _ClassName);

  op = make_ObjectPath (_broker, CMGetCharPtr (CMGetNameSpace (ref, &rc)), _ClassName);

  /* Just one key */
  CMAddKey (op, "ElementName", (CMPIValue *) _ClassName, CMPI_chars);

  CMReturnObjectPath (rslt, op);
  CMReturnDone (rslt);

  PROV_LOG ("--- %s CMPI EnumInstanceNames() exited", _ClassName);
  PROV_LOG_CLOSE ();
  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderEnumInstances (CMPIInstanceMI * mi,
                                       const CMPIContext * ctx,
                                       const CMPIResult * rslt,
                                       const CMPIObjectPath * ref,
                                       const char **properties)
#else
CMPIStatus
TestCMPIInstanceProviderEnumInstances (CMPIInstanceMI * mi,
                                       CMPIContext * ctx,
                                       CMPIResult * rslt,
                                       CMPIObjectPath * ref,
                                       char **properties)
#endif
{

  CMPIObjectPath *op = NULL;
  CMPIInstance *ci = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI EnumInstances() called", _ClassName);

  op = make_ObjectPath (_broker, CMGetCharPtr (CMGetNameSpace (ref, &rc)), _ClassName);

  PROV_LOG (" New Object Path [%s]",
            CMGetCharPtr (CMGetNameSpace (ref, &rc)));

  ci = make_Instance (op);
  if (ci != NULL)
    {
      CMReturnInstance (rslt, ci);

      CMReturnDone (rslt);
    }
  else
    {
      rc.rc = CMPI_RC_ERR_NOT_FOUND;
    }
  PROV_LOG ("--- %s CMPI EnumInstances() exited", _ClassName);
  PROV_LOG_CLOSE ();
  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderGetInstance (CMPIInstanceMI * mi,
                                     const CMPIContext * ctx,
                                     const CMPIResult * rslt,
                                     const CMPIObjectPath * cop,
                                     const char **properties)
#else
CMPIStatus
TestCMPIInstanceProviderGetInstance (CMPIInstanceMI * mi,
                                     CMPIContext * ctx,
                                     CMPIResult * rslt,
                                     CMPIObjectPath * cop, char **properties)
#endif
{
  /*CMPIInstance * ci = NULL; */
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI GetInstance() called", _ClassName);

  CMSetStatusWithChars (_broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED");

  PROV_LOG ("--- %s CMPI GetInstance() exited", _ClassName);
  PROV_LOG_CLOSE ();
  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderCreateInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop,
                                        const CMPIInstance * ci)
#else
CMPIStatus
TestCMPIInstanceProviderCreateInstance (CMPIInstanceMI * mi,
                                        CMPIContext * ctx,
                                        CMPIResult * rslt,
                                        CMPIObjectPath * cop,
                                        CMPIInstance * ci)
#endif
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI CreateInstance() called", _ClassName);

  CMSetStatusWithChars (_broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED");

  PROV_LOG ("--- %s CMPI CreateInstance() exited", _ClassName);
  PROV_LOG_CLOSE ();
  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderModifyInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop,
                                        const CMPIInstance * ci,
                                        const char **properties)
#else
CMPIStatus
TestCMPIInstanceProviderSetInstance (CMPIInstanceMI * mi,
                                     CMPIContext * ctx,
                                     CMPIResult * rslt,
                                     CMPIObjectPath * cop,
                                     CMPIInstance * ci, char **properties)
#endif
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI SetInstance() called", _ClassName);

  CMSetStatusWithChars (_broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED");

  PROV_LOG ("--- %s CMPI SetInstance() exited", _ClassName);
  PROV_LOG_CLOSE ();

  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderDeleteInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop)
#else
CMPIStatus
TestCMPIInstanceProviderDeleteInstance (CMPIInstanceMI * mi,
                                        CMPIContext * ctx,
                                        CMPIResult * rslt,
                                        CMPIObjectPath * cop)
#endif
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  PROV_LOG ("--- %s CMPI DeleteInstance() called", _ClassName);

  CMSetStatusWithChars (_broker, &rc,
                        CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED");

  PROV_LOG ("--- %s CMPI DeleteInstance() exited", _ClassName);
  PROV_LOG_CLOSE ();

  return rc;
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIInstanceProviderExecQuery (CMPIInstanceMI * mi,
                                   const CMPIContext * ctx,
                                   const CMPIResult * rslt,
                                   const CMPIObjectPath * ref,
                                   const char *lang, const char *query)
#else
CMPIStatus
TestCMPIInstanceProviderExecQuery (CMPIInstanceMI * mi,
                                   CMPIContext * ctx,
                                   CMPIResult * rslt,
                                   CMPIObjectPath * ref,
                                   char *lang, char *query)
#endif
{

  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Eval = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Clone = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Inst = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Pred = { CMPI_RC_OK, NULL };
  CMPIStatus rc_Array = { CMPI_RC_OK, NULL };
  CMPIString *type = NULL;
  CMPISelectExp *se_def = NULL;
  CMPISelectExp *se_CQL = NULL;
  CMPISelectExp *clone = NULL;
  CMPIBoolean evalRes;
  CMPIInstance *inst = NULL;
  CMPIObjectPath *objPath = NULL;
  unsigned int idx;
  CMPIString *name = NULL;
  CMPIStatus rc_CMGetPropertyAt = { CMPI_RC_OK, NULL };
  CMPIData data;
  CMPIArray *projection = NULL;
  CMPICount cnt = 0;
  int rc_setProperty = 0;

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);
  
  PROV_LOG ("--- %s CMPI ExecQuery() called", _ClassName);
  PROV_LOG ("--- Query: [%s], language: [%s]", query, lang);

  PROV_LOG ("-- #1 MakeObjectPath");
  // Create instance

  objPath = make_ObjectPath (_broker, _Namespace, _ClassName);


  inst = make_Instance (objPath);
  if (!inst)
    {
      CMReturn (CMPI_RC_ERR_NOT_FOUND);
    }
  PROV_LOG("--- #2 CDGetType");
  type = CDGetType (_broker, inst, &rc_Inst);
  if (type)
    {
      PROV_LOG ("---- %s (%s)", CMGetCharPtr (type), strCMPIStatus (rc_Inst));
      CMRelease (type);
    }

  // Here the fun starts;
  PROV_LOG ("-- #3 CMNewSelectExp");
  se_def = CMNewSelectExp (_broker, query, lang, &projection, &rc_Clone);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));
  /* This is used to figure what properties to construct against */
  if (se_def)
    {

      PROV_LOG ("--- Projection list is: ");
      if (projection)
        {
          PROV_LOG ("---- #3.1 CMGetArrayCount");
          cnt = CMGetArrayCount (projection, &rc_Array);
          PROV_LOG ("---- %s", strCMPIStatus (rc_Array));
          PROV_LOG ("---- CMGetArrayCount, %d", cnt);
          for (idx = 0; idx < cnt; idx++)
            {
              PROV_LOG ("--- #3.2 CMGetArrayElementAt");
              data = CMGetArrayElementAt (projection, idx, &rc_Array);
              PROV_LOG ("---- %s", strCMPIStatus (rc_Array));
              PROV_LOG ("---- type is : %d", data.type);
              if (data.type == CMPI_chars)
                {
                  PROV_LOG ("---- %s (chars)", data.value.chars);
                  rc_setProperty = _setProperty (inst, data.value.chars);
                  if (rc_setProperty)
                    PROV_LOG ("--- Error finding the property");
                  // At which point we just leace the function - as we cannot satisfy the
                  // request. But this is a test-case provider so we are continuing on and we
                  // just won't send the instance.      Wait you say, won't CMEvaluteSelExp figure
                  // this too - yes, but only the CQL one. The WQL is not smart enough
                }
              if (data.type == CMPI_string)
                {
                  PROV_LOG ("---- %s (string)",
                            CMGetCharPtr (data.value.string));
                  // The _setProperty is a simple function to set _only_ properties
                  // that are needed.
                  rc_setProperty =
                    _setProperty (inst, CMGetCharPtr (data.value.string));
                  if (rc_setProperty)
                    PROV_LOG ("--- Error finding the property");
                }
            }

        }
      else
        {
          // Just add all of them then.
          PROV_LOG ("-- #4.2 Projection list is NULL. Meaning *");
          _setProperty (inst, "*");
        }
    }
  if (se_def)
    {
      PROV_LOG ("-- #5 Clone");
      clone = CMClone (se_def, &rc_Clone);
      PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));

      if (clone)
        CMRelease (clone);
    }



  PROV_LOG ("-- #4 CMPI_CQL_NewSelectExp");
  //se_CQL = CMPI_CQL_NewSelectExp (_broker, query, lang, &projection, &rc_Clone);
  se_CQL = CMNewSelectExp (_broker, query, "CIMxCQL", &projection, &rc_Clone);
  PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));
  /* This can be used to figure what properties to construct against when using CQL */
  if (se_CQL)
    {
      if (projection)
        {
          PROV_LOG ("--- Projection list is: ");
          cnt = CMGetArrayCount (projection, &rc_Array);
          PROV_LOG ("---- %s", strCMPIStatus (rc_Array));
          PROV_LOG ("---- #4.1 CMGetArrayCount, %d", cnt);
          for (idx = 0; idx < cnt; idx++)
            {
              PROV_LOG ("--- #4.2 CMGetArrayElementAt");
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
                  // This is just done for test-purpose. The previous if (se) loop would
                  // have take care of this.
                  _setProperty (inst, CMGetCharPtr (data.value.string));
                }
            }
        }
      else
        {
          PROV_LOG ("-- #4.2 Projection list is NULL. Meaning *");
        }
    }

  if (se_CQL)
    {
      PROV_LOG ("-- #5 Clone");
      clone = CMClone (se_CQL, &rc_Clone);
      PROV_LOG ("---- %s", strCMPIStatus (rc_Clone));

      if (clone)
        CMRelease (clone);
    }

  /* See if our instance matches the query expression */
  PROV_LOG ("-- #6 Evaluate using instance (default)");

  if (se_def)
    {
      evalRes = CMEvaluateSelExp (se_def, inst, &rc_Eval);
      PROV_LOG ("---- %s", strCMPIStatus (rc_Eval));
      if (evalRes == CMPI_true)
        {
          PROV_LOG ("--- True");
          /* If it evaluated to true, return the instance */
          if (rc_setProperty)
            {
              PROV_LOG
                ("--- Can't send the instances as the SELECT <...> part has incorrect properties");
            }
          else
            {
              PROV_LOG ("--- Sending instance ");
              CMReturnInstance (rslt, inst);
              CMReturnDone (rslt);
            }
        }
      else
        {
          PROV_LOG ("--- False");
        }
      // Either way, no need for the SelectExp structure
      CMRelease (se_def);
    }
  PROV_LOG ("-- #6.1 Evaluate using instance (CQL)");
  if (se_CQL)
    {
      evalRes = CMEvaluateSelExp (se_CQL, inst, &rc_Eval);
      PROV_LOG ("---- %s", strCMPIStatus (rc_Eval));
      if (evalRes == CMPI_true)
        {
          PROV_LOG ("--- True, but not sending anything..");
          /* We are not sending anything back b/c the previous
             block of code would have sent the instance already */
          //CMReturnInstance (rslt, inst);
          //CMReturnDone (rslt);
        }
      else
        {
          PROV_LOG ("--- False");
        }
      CMRelease (se_CQL);
    }

  PROV_LOG ("--- %s CMPI ExecQuery() exited", _ClassName);
  PROV_LOG_CLOSE ();
  CMReturn (CMPI_RC_OK);
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub (TestCMPIInstanceProvider,
                  TestCMPIInstanceProvider, _broker, CMNoHook);


/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/
