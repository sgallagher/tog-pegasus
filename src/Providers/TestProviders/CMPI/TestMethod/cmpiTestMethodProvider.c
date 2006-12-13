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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Providers/TestProviders/CMPI/TestUtilLib/cmpiUtilLib.h>

#define _ClassName "TestCMPI_Method"
#define _ClassName_size strlen(_ClassName)
#define _Namespace    "test/TestProvider"

#define _ProviderLocation "/src/Providers/TestProviders/CMPI/TestMethod/tests/"

#ifdef CMPI_VER_100
static const CMPIBroker *_broker;
#else
static CMPIBroker *_broker;
#endif

/* ---------------------------------------------------------------------------*/
/*                       CMPI Helper function                        */
/* ---------------------------------------------------------------------------*/


/* 
 * Test routines 
 */

 int
_CDGetType (const void *o, char **result)
{

  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIString *type;

  PROV_LOG ("++++ #A CDGetType");
  type = CDGetType (_broker, o, &rc);
  if (type)
    {
      PROV_LOG ("++++ %s (%s)", CMGetCharPtr (type), strCMPIStatus (rc));
      // The result of the call is put in 'result' parameter.
      *result = (char *) strdup ((const char *) CMGetCharPtr (type));
      CMRelease (type);
    }
  else
    {
      PROV_LOG ("---- No type extracted.");
      *result = strdup ("No type extracted.");
    }
  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

 int
_CDToString (const void *o, char **result)
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIString *type;
  char *p, *q;

  PROV_LOG ("++++ #B CDToString");
  type = CDToString (_broker, o, &rc);
  if (type)
    {
      p = CMGetCharPtr (type);
      // The pointer to the null value.
      q = p + strlen (p);
      // The first couple of bytes are the address of the data. We don't want it here
      // so we are going to skip over them.
      while ((*p != ':') && (p < q))
        p++;
      // Skip over the space after the ':'
      p++;
      *result = strdup (p);
      PROV_LOG ("++++ %s (%s)", *result, strCMPIStatus (rc));
      CMRelease (type);
    }
  else
    {
      PROV_LOG ("---- No string extracted.");
      *result = strdup ("No string extracted.");
    }

  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

 int
_CDIsOfType (const void *o, char **result)
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIBoolean isOfType = 0;

  PROV_LOG ("++++ #D CDIsOfType ");

  isOfType = CDIsOfType (_broker, o, "CMPIInstance", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIInstance = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIObjectPath", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIObjectPath= Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIBroker", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIBroker = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIString", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIString = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIDateTime", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIDateTime  = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIArgs", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIArgs = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPISelectExp", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPISelectExp = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPISelectCond", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPISelectCond = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPISubCond", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPISubCond = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIPredicate", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIPredicate = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIArray", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIArray = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIEnumeration", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIEnumeration = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIContext", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIContext = Yes");
      goto end;
    }

  isOfType = CDIsOfType (_broker, o, "CMPIResult", &rc);
  if (isOfType)
    {
      *result = strdup ("++++ CMPIResult = Yes");
      goto end;
    }

end:

  PROV_LOG ("%s", *result);

  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

static int
_CMGetMessage (char **result)
{

  CMPIString *str = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };

  PROV_LOG ("++++ #A CMGetMessage");
  /* Gets the internationalized string */
  str =
    CMGetMessage (_broker, "Common.CIMStatusCode.CIM_ERR_SUCCESS",
                  "CIM_ERR_SUCCESS: Successful.", &rc, 0);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
  if (str)
    {
      PROV_LOG ("++++ [%s]", CMGetCharPtr (str));
      *result = strdup (CMGetCharPtr (str));
    }
  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

static int _CMGetMessage2 (char **result, char* msgFile, char* msgId, 
    char* insert1, CMPIUint32 insert2)
{
    CMPIString *str = NULL;
    CMPIStatus rc = { CMPI_RC_OK, NULL};
    CMPIMsgFileHandle msgFileHandle;

    PROV_LOG ("++++ #A CMOpenMessageFile");
    rc = CMOpenMessageFile(_broker, msgFile, &msgFileHandle);
    PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
    PROV_LOG ("++++ #B CMGetMessage2");
    /* Gets the internationalized string */
    str = CMGetMessage2 (_broker, msgId, msgFileHandle,
        "CIM_ERR_SUCCESS: Successful.", &rc,
        CMFmtArgs2(CMFmtChars(insert1),CMFmtUint(insert2)));
    PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
    if (str)
    {
        PROV_LOG ("++++ [%s]", CMGetCharPtr (str));
        *result = strdup (CMGetCharPtr (str));
    }

    PROV_LOG ("++++ #C CMCloseMessageFile");
    rc = CMCloseMessageFile(_broker, msgFileHandle);
    PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

    if (rc.rc == CMPI_RC_OK)
        return 0;
    return 1;
}

static int
_CMLogMessage (char **result)
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIString *str = CMNewString (_broker, "CMLogMessage", &rc);

  PROV_LOG ("++++ #B CMLogMessage");
  rc.rc = CMPI_RC_OK;
  rc = CMLogMessage (_broker, 64, _ClassName, "Log message", NULL);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  // Ths should return CMPI_RC_ERR_INVALID_PARAMETER
  rc.rc = CMPI_RC_OK;
  rc = CMLogMessage (_broker, 2, NULL, NULL, NULL);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  // This should return CMPI_RC_ERR_INVALID_PARAMETER too
  rc.rc = CMPI_RC_OK;
  rc = CMLogMessage (_broker, 1, NULL, "LogMessage with NULL", NULL);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  rc.rc = CMPI_RC_OK;
  rc = CMLogMessage (_broker, 1, _ClassName, NULL, str);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

static int
_CMTraceMessage (char **result)
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIString *str = CMNewString (_broker, "CMTraceMessage", &rc);

  PROV_LOG ("++++ #C CMLogTrace");

  // Ths should return CMPI_RC_ERR_INVALID_PARAMETER
  rc.rc = CMPI_RC_OK;
  rc = CMTraceMessage (_broker, 2, NULL, NULL, NULL);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  rc.rc = CMPI_RC_OK;
  rc = CMTraceMessage (_broker, 1, _ClassName, "CMTraceMessage", NULL);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  rc.rc = CMPI_RC_OK;
  rc = CMTraceMessage (_broker, 4, "Authorization", NULL, str);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  if (rc.rc == CMPI_RC_OK)
    return 0;
  return 1;
}

static CMPIInstance *
_createInstance()
{
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIObjectPath *objPath = NULL;
  CMPIObjectPath *fake_objPath  = NULL;
  CMPIObjectPath *temp_objPath = NULL;
  CMPIString *objName = NULL;
  CMPIInstance *inst = NULL;

  PROV_LOG("Calling CMNewObjectPath for %s", _ClassName );
  objPath  = CMNewObjectPath (_broker, _Namespace, "TestCMPI_Instance", &rc);
  CMAddKey (objPath, "ElementName", (CMPIValue *) "ObjPath", CMPI_chars);
  PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));

  PROV_LOG("Calling CMNewInstance ");
  inst = CMNewInstance(_broker, objPath, &rc);
  PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));

  // Get the object path
  PROV_LOG("Calling CMGetObjectPath");
  temp_objPath = CMGetObjectPath(inst, &rc);
  PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));

  objName = CMObjectPathToString(temp_objPath, &rc);
  PROV_LOG ("---- Object path is %s (rc:%s)", CMGetCharPtr(objName), strCMPIStatus (rc));

  // Create a new ObjectPath, in a different namespace.
  PROV_LOG("Calling CMNewObjectPath for %s", "TestCMPI_Instance");
  fake_objPath = CMNewObjectPath (_broker, "root/cimv2", "TestCMPI_Instance", &rc);  
  CMAddKey (fake_objPath, "ElementName", (CMPIValue *) "Fake_ObjPath", CMPI_chars);

  PROV_LOG ("---- (%s)", strCMPIStatus (rc));
  
  objName = CMObjectPathToString(fake_objPath, &rc);
  PROV_LOG ("---- Object path: %s (rc:%s)", CMGetCharPtr(objName), strCMPIStatus (rc));

  // Setting objPath to fake_ObjPath
  PROV_LOG("Calling CMSetObjectPath with object path: %s", CMGetCharPtr(objName));
  rc = CMSetObjectPath(inst, fake_objPath);
  PROV_LOG ("---- (%s)", strCMPIStatus (rc));

  // Please note that this instance now has this objectPath
  PROV_LOG("Calling CMGetObjectPath");
  temp_objPath = CMGetObjectPath(inst, &rc);
  PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));
  objName = CMObjectPathToString(temp_objPath, &rc);
  PROV_LOG ("---- Object path is %s (rc:%s)", CMGetCharPtr(objName), strCMPIStatus (rc));

  return inst;  
}
          /* and many more soon to come */

/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/
#ifdef CMPI_VER_100
CMPIStatus
TestCMPIMethodProviderMethodCleanup (CMPIMethodMI * mi,
                                     const CMPIContext * ctx,
                                     CMPIBoolean  term)
#else
CMPIStatus
TestCMPIMethodProviderMethodCleanup (CMPIMethodMI * mi, CMPIContext * ctx)
#endif
{
  CMReturn (CMPI_RC_OK);
}

#ifdef CMPI_VER_100
CMPIStatus
TestCMPIMethodProviderInvokeMethod (CMPIMethodMI * mi,
                                    const CMPIContext * ctx,
                                    const CMPIResult * rslt,
                                    const CMPIObjectPath * ref,
                                    const char *methodName,
                                    const CMPIArgs * in, CMPIArgs * out)
#else
CMPIStatus
TestCMPIMethodProviderInvokeMethod (CMPIMethodMI * mi,
                                    CMPIContext * ctx,
                                    CMPIResult * rslt,
                                    CMPIObjectPath * ref,
                                    char *methodName,
                                    CMPIArgs * in, CMPIArgs * out)
#endif
{
  CMPIString *class = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIData data;
  CMPIString *argName = NULL;
  CMPIInstance *instance = NULL;
  CMPIInstance *paramInst = NULL;
  unsigned int arg_cnt = 0, index = 0;
  CMPIUint32 oper_rc = 1;
  char *result = NULL;


  PROV_LOG_OPEN (_ClassName, _ProviderLocation);

  PROV_LOG ("--- %s CMPI InvokeMethod() called", _ClassName);

  class = CMGetClassName (ref, &rc);

  PROV_LOG ("InvokeMethod: checking for correct classname [%s]",
            CMGetCharPtr (class));

  PROV_LOG ("Calling CMGetArgCount");
  arg_cnt = CMGetArgCount (in, &rc);
  PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

  PROV_LOG ("InvokeMethod: We have %d arguments for operation [%s]: ",
            arg_cnt, methodName);
  if (arg_cnt > 0)
    {
      PROV_LOG ("Calling CMGetArgAt");
      for (index = 0; index < arg_cnt; index++)
        {
          data = CMGetArgAt (in, index, &argName, &rc);
          if (data.type == CMPI_uint32)
            {
              PROV_LOG ("#%d: %s (uint32), value: %d", index,
                        CMGetCharPtr (argName), data.value.uint32);
            }
          else if (data.type == CMPI_string)
            {
              PROV_LOG ("#%d: %s (string) value: %s", index,
                        CMGetCharPtr (argName),
                        CMGetCharPtr (data.value.string));
            }
          else
            {
              PROV_LOG ("#%d: %s (type: %x)", index, CMGetCharPtr (argName),
                        data.type);
            }
          CMRelease (argName);
        }
    }
  if (strncmp (CMGetCharPtr (class), _ClassName, strlen (_ClassName)) == 0)
    {
      if (strncmp ("TestCMPIBroker", methodName, strlen ("TestCMPIBroker")) ==
          0)
        {
          // Parse the CMPIArgs in to figure out which operation it is.
          // There are six of them:
          //   ValueMap { "1", "2", "3", "4", "5", "6", "7"},
          //        Values {"CDGetType", "CDToString", "CDIsOfType", "CMGetMessage",  "CMLogMessage","CDTraceMessage","CMGetMessage2"}]
          //    uint32 Operation,
          //    [OUT]string Result);
          PROV_LOG ("++++ Calling CMGetArg");
          data = CMGetArg (in, "Operation", &rc);
          PROV_LOG ("++++ (%s)", strCMPIStatus (rc));

          if (data.type == CMPI_uint32)
            {
              switch (data.value.uint32)
                {
                case 1:
                  oper_rc = _CDGetType (in, &result);
                  break;
                case 2:
                  oper_rc = _CDToString (in, &result);
                  break;
                case 3:
                  oper_rc = _CDIsOfType (in, &result);
                  break;
                case 4:
                  oper_rc = _CMGetMessage (&result);
                  break;
                case 5:
                  oper_rc = _CMLogMessage (&result);
                  break;
                case 6:
                  oper_rc = _CMTraceMessage (&result);
                  break;
                case 7:
                  {
                      CMPIData msgFileData;
                      CMPIData msgIdData;
                      CMPIData insert1Data;
                      CMPIData insert2Data;
                      PROV_LOG ("++++ Calling CMGetArg for msgFile");
                      msgFileData = CMGetArg (in, "msgFile", &rc);
                      PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
                      PROV_LOG ("++++ Calling CMGetArg for msgId");
                      msgIdData = CMGetArg (in, "msgId", &rc);
                      PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
                      PROV_LOG ("++++ Calling CMGetArg for insert1");
                      insert1Data = CMGetArg (in, "insert1", &rc);
                      PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
                      PROV_LOG ("++++ Calling CMGetArg for insert2");
                      insert2Data = CMGetArg (in, "insert2", &rc);
                      PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
                      oper_rc = _CMGetMessage2 (&result,
                          CMGetCharPtr(msgFileData.value.string),
                          CMGetCharPtr(msgIdData.value.string),
                          CMGetCharPtr(insert1Data.value.string),
                          insert2Data.value.uint32
                          );
                      break;
                  }
                default:
                  break;
                }
              // Return the oper_rc value via Result
              PROV_LOG ("++++ Calling CMReturnData+Done");
              CMReturnData (rslt, (CMPIValue *) & oper_rc, CMPI_uint32);
              CMReturnDone (rslt);

              // Return the string value value via putting it on the out parameter.
              PROV_LOG ("++++ Calling CMAddArg");
              rc = CMAddArg (out, "Result", (CMPIValue *) result, CMPI_chars);
              PROV_LOG ("++++ (%s)", strCMPIStatus (rc));
              free (result);
            }
          else                  // type of data
            {
              PROV_LOG
                ("Unknown type of data [%X] for the Operation parameter.",
                 data.type);
              oper_rc = 1;
              PROV_LOG ("++++ Calling CMReturnData+Done");
              CMReturnData (rslt, (CMPIValue *) & oper_rc, CMPI_uint32);
              CMReturnDone (rslt);
            }
        }

      else if (strncmp ("returnString", methodName, strlen ("returnString"))
               == 0)
        {
          result = strdup ("Returning string");
          PROV_LOG
            ("++++ Calling CMReturnData+Done on returnString operation");
          CMReturnData (rslt, (CMPIValue *) result, CMPI_chars);
          CMReturnDone (rslt);
	  free(result);
        }
      else if (strncmp ("returnUint32", methodName, strlen ("returnUint32"))
               == 0)
        {
          oper_rc = 42;
          PROV_LOG
            ("++++ Calling CMReturnData+Done on returnUint32 operation");
          CMReturnData (rslt, (CMPIValue *) & oper_rc, CMPI_uint32);
          CMReturnDone (rslt);
        }
      else if (strncmp ("returnInstance", methodName, strlen ("returnInstance"))
               == 0)
		{
		  instance = _createInstance();
          PROV_LOG
            ("++++ Calling CMReturnData+Done on returnInstance operation");
          CMReturnData (rslt, (CMPIValue *) & instance, CMPI_instance);
          CMReturnDone (rslt);
		}
	else if (strncmp("returnDateTime", methodName, strlen("returnDateTime")) == 0)
	{
		CMPIUint64 ret_val = 0;
		CMPIStatus rc={CMPI_RC_OK, NULL};

		CMPIDateTime *dateTime = CMNewDateTime(_broker, &rc); 
  		PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));
		// Checking the date.
		ret_val = CMGetBinaryFormat (dateTime, &rc);
  		PROV_LOG ("---- (rc:%s)", strCMPIStatus (rc));
		if (ret_val == 0)
			PROV_LOG("Invalid conversion of date to CMPIDateTime");

         PROV_LOG
            ("++++ Calling CMReturnData+Done on returnDateTime operation");

        CMReturnData (rslt, (CMPIValue *) & dateTime, CMPI_dateTime);
        CMReturnDone (rslt);
	}
    else if(strncmp("processEmbeddedInstance", methodName,
      strlen ("processEmbeddedInstance"))== 0)
    {
        PROV_LOG("++++ Creating instance for processEmbeddedInstance");
        instance = _createInstance();
        PROV_LOG("++++ Getting inputInstance arg");
        data = CMGetArg(in, "inputInstance", &rc);
        PROV_LOG("++++ (%s)", strCMPIStatus (rc));

        PROV_LOG("++++ Cloning inputInstance arg");
        paramInst = data.value.inst->ft->clone(
          data.value.inst, &rc);
        PROV_LOG("++++ (%s)", strCMPIStatus (rc));

        PROV_LOG("++++ Setting outputInstance arg");
        rc = CMAddArg (out, "outputInstance",
          (CMPIValue *) &paramInst, CMPI_instance);
        PROV_LOG("++++ (%s)", strCMPIStatus (rc));

        PROV_LOG(
            "++++ Calling CMReturnData on processEmbeddedInstance operation");
        CMReturnData (rslt, (CMPIValue *) &instance, CMPI_instance);

        PROV_LOG(
            "++++ Calling CMReturnDone on processEmbeddedInstance operation");
        CMReturnDone (rslt);
        paramInst->ft->release(paramInst);
    }

      else
        {
          PROV_LOG ("++++ Could not find the %s operation", methodName);
          CMSetStatusWithChars (_broker, &rc,
                                CMPI_RC_ERR_NOT_FOUND, methodName);
        }
    }
  PROV_LOG ("--- %s CMPI InvokeMethod() exited", _ClassName);
  PROV_LOG_CLOSE();
  return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/


CMMethodMIStub (TestCMPIMethodProvider,
                TestCMPIMethodProvider, _broker, CMNoHook)

/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/
