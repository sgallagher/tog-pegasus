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

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Providers/TestProviders/CMPI/TestUtilLib/cmpiUtilLib.h>

#define _ClassName "TestCMPI_Error"
#define _ClassName_size strlen(_ClassName)
#define _Namespace    "test/TestProvider"

#define _ProviderLocation "/src/Providers/TestProviders/CMPI/TestError/tests/"

static const CMPIBroker *_broker;

/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/
CMPIStatus
TestCMPIErrorProviderMethodCleanup (CMPIMethodMI * mi,
                                     const CMPIContext * ctx,
                                     CMPIBoolean  term)
{
  CMReturn (CMPI_RC_OK);
}

CMPIStatus
TestCMPIErrorProviderInvokeMethod (CMPIMethodMI * mi,
                                    const CMPIContext * ctx,
                                    const CMPIResult * rslt,
                                    const CMPIObjectPath * ref,
                                    const char *methodName,
                                    const CMPIArgs * in, CMPIArgs * out)
{
  CMPIString *class = NULL;
  CMPIStatus rc = { CMPI_RC_OK, NULL };
  CMPIError *cmpiError;
  CMPIError *cmpiErrorClone;
  CMPIUint32 oper_rc = 0;
  CMPICount i, arrSize;

  /* CMPIError data */
  const char* inOwningEntity = "ACME";
  CMPIString* outOwningEntity;
  const char* inMsgID = "ACME0023";
  CMPIString* outMsgID;
  const char* inMsg = "ACME0023 Need to restock Road Runner bait and traps.";
  CMPIString* outMsg;
  const CMPIErrorSeverity inSev = ErrorSevMedium;
  CMPIErrorSeverity outSev;
  const CMPIErrorProbableCause inPc = Equipment_Malfunction;
  CMPIErrorProbableCause outPc;
  const CMPIrc inCIMStatusCode = CMPI_RC_ERR_FAILED;
  CMPIrc outCIMStatusCode;
  const char * inCIMStatusCodeDesc = "another failed attempt at road runner elimination";
  CMPIString* outCIMStatusCodeDesc;
  const CMPIErrorType inErrType = OtherErrorType;
  CMPIErrorType outErrType;
  const char* inOtherErrType = "Pervasive Coyote Incompetance";
  CMPIString* outOtherErrType;
  const char* inPcd = "Incorrect use of equipment";
  CMPIString* outPcd;
  CMPIArray *inRecActions;
  CMPIArray *outRecActions;
  CMPIArray *inMsgArgs;
  CMPIArray *outMsgArgs;
  CMPIValue raOne;
  CMPIValue raTwo;
  const char* inErrSource = "Acme bomb";
  CMPIString* outErrSource;
  const CMPIErrorSrcFormat inErrSourceFormat = CMPIErrSrcOther;
  CMPIErrorSrcFormat outErrSourceFormat;
  const char* inOtherErrSourceFormat = "no idea";
  CMPIString* outOtherErrSourceFormat;

  PROV_LOG_OPEN (_ClassName, _ProviderLocation);

  PROV_LOG ("--- %s CMPI InvokeMethod() called", _ClassName);

  class = CMGetClassName (ref, &rc);

  PROV_LOG ("InvokeMethod: checking for correct classname [%s]",
            CMGetCharPtr (class));

  PROV_LOG ("Calling CMNewCMPIError");
  cmpiError = CMNewCMPIError(_broker, inOwningEntity, inMsgID, inMsg,
      inSev, inPc, inCIMStatusCode, &rc);
  PROV_LOG ("++++ (%s) CMNewCMPIError", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetErrorType");
  rc = CMSetErrorType(cmpiError, inErrType);
  PROV_LOG ("++++ (%s) CMSetErrorType", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetOtherErrorType");
  rc = CMSetOtherErrorType(cmpiError, inOtherErrType);
  PROV_LOG ("++++ (%s) CMSetOtherErrorType", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetProbableCauseDescription");
  rc = CMSetProbableCauseDescription(cmpiError, inPcd);
  PROV_LOG ("++++ (%s) CMSetProbableCauseDescription", strCMPIStatus (rc));

  inRecActions = CMNewArray(_broker, 2, CMPI_string, &rc);
  PROV_LOG ("++++ (%s) CMNewArray", strCMPIStatus (rc));
  raOne.string = CMNewString(_broker, "Fire coyote.", &rc);
  PROV_LOG ("++++ (%s) CMNewString", strCMPIStatus (rc));
  rc = CMSetArrayElementAt(inRecActions, 0, &raOne, CMPI_string);
  PROV_LOG ("++++ (%s) CMSetArrayElementAt [0]", strCMPIStatus (rc));
  raTwo.string = CMNewString(_broker, "Give peace a chance.", &rc);
  PROV_LOG ("++++ (%s) CMNewString", strCMPIStatus (rc));
  rc = CMSetArrayElementAt(inRecActions, 1, &raTwo, CMPI_string);
  PROV_LOG ("++++ (%s) CMSetArrayElementAt [1]", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetRecommendedActions");
  rc = CMSetRecommendedActions(cmpiError, inRecActions);
  PROV_LOG ("++++ (%s) CMSetRecommendedActions", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetErrorSource");
  rc = CMSetErrorSource(cmpiError, inErrSource);
  PROV_LOG ("++++ (%s) CMSetErrorSource", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetErrorSourceFormat");
  rc = CMSetErrorSourceFormat(cmpiError, inErrSourceFormat);
  PROV_LOG ("++++ (%s) CMSetErrorSourceFormat", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetOtherErrorSourceFormat");
  rc = CMSetOtherErrorSourceFormat(cmpiError, inOtherErrSourceFormat);
  PROV_LOG ("++++ (%s) CMSetOtherErrorSourceFormat", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetCIMStatusCodeDescription");
  rc = CMSetCIMStatusCodeDescription(cmpiError, inCIMStatusCodeDesc);
  PROV_LOG ("++++ (%s) CMSetCIMStatusCodeDescription", strCMPIStatus (rc));

  inMsgArgs = CMNewArray(_broker, 1, CMPI_string, &rc);
  PROV_LOG ("++++ (%s) CMNewArray", strCMPIStatus (rc));
  raOne.string = CMNewString(_broker, "no real inserts", &rc);
  PROV_LOG ("++++ (%s) CMNewString", strCMPIStatus (rc));
  rc = CMSetArrayElementAt(inMsgArgs, 0, &raOne, CMPI_string);
  PROV_LOG ("++++ (%s) CMSetArrayElementAt [0]", strCMPIStatus (rc));

  PROV_LOG ("Calling CMSetMessageArguments");
  rc = CMSetMessageArguments(cmpiError, inMsgArgs);
  PROV_LOG ("++++ (%s) CMSetMessageArguments", strCMPIStatus (rc));

  PROV_LOG ("First clone the CMPIError.");
  cmpiErrorClone = CMClone(cmpiError, &rc);
  PROV_LOG ("++++ (%s) CMClone", strCMPIStatus (rc));

  PROV_LOG ("Ok, reading back fields to verify");
  
  PROV_LOG ("Calling CMGetErrorType");
  outErrType = CMGetErrorType(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetErrorType (%d)", strCMPIStatus (rc), outErrType);

  PROV_LOG ("Calling CMGetOtherErrorType");
  outOtherErrType = CMGetOtherErrorType(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetOtherErrorType (%s)", strCMPIStatus (rc), 
      CMGetCharPtr(outOtherErrType));

  PROV_LOG ("Calling CMGetOwningEntity");
  outOwningEntity = CMGetOwningEntity(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetOwningEntity (%s)", strCMPIStatus (rc), 
      CMGetCharPtr(outOwningEntity));

  PROV_LOG ("Calling CMGetMessageID");
  outMsgID = CMGetMessageID(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetMessageID (%s)", strCMPIStatus (rc), 
      CMGetCharPtr(outMsgID));

  PROV_LOG ("Calling CMGetErrorMessage");
  outMsg = CMGetErrorMessage(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetErrorMessage (%s)", strCMPIStatus (rc), 
      CMGetCharPtr(outMsg));

  PROV_LOG ("Calling CMGetPerceivedSeverity");
  outSev = CMGetPerceivedSeverity(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetPerceivedSeverity (%d)", strCMPIStatus (rc),
      outSev);

  PROV_LOG ("Calling CMGetProbableCause");
  outPc = CMGetProbableCause(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetProbableCause (%d)", strCMPIStatus (rc),
      outPc);

  PROV_LOG ("Calling CMGetProbableCauseDescription");
  outPcd = CMGetProbableCauseDescription(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetProbableCauseDescription (%s)", strCMPIStatus (rc),
      CMGetCharPtr(outPcd));

  PROV_LOG ("Calling CMGetRecommendedActions");
  outRecActions = CMGetRecommendedActions(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetRecommendedActions", strCMPIStatus (rc));
  arrSize = CMGetArrayCount(outRecActions, &rc);
  PROV_LOG ("++++ (%s) CMGetArrayCount (%d)", strCMPIStatus (rc), arrSize);
  for (i = 0 ; i < arrSize ; i++)
  {
      CMPIData dta = CMGetArrayElementAt(outRecActions, i, &rc);
      PROV_LOG ("++++ (%s) CMGetArrayElementAt (%d:%s)", strCMPIStatus (rc),
          i, CMGetCharPtr(dta.value.string));
  }

  PROV_LOG ("Calling CMGetErrorSource");
  outErrSource = CMGetErrorSource(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetErrorSource (%s)", strCMPIStatus (rc),
      CMGetCharPtr(outErrSource));

  PROV_LOG ("Calling CMGetErrorSourceFormat");
  outErrSourceFormat = CMGetErrorSourceFormat(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetErrorSourceFormat (%d)", strCMPIStatus (rc),
      outErrSourceFormat);

  PROV_LOG ("Calling CMGetOtherErrorSourceFormat");
  outOtherErrSourceFormat = CMGetOtherErrorSourceFormat(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetOtherErrorSourceFormat (%s)", strCMPIStatus (rc),
      CMGetCharPtr(outOtherErrSourceFormat));

  PROV_LOG ("Calling CMGetCIMStatusCode");
  outCIMStatusCode = CMGetCIMStatusCode(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetCIMStatusCode (%d)", strCMPIStatus (rc),
      outCIMStatusCode);

  PROV_LOG ("Calling CMGetCIMStatusCodeDescription");
  outCIMStatusCodeDesc = CMGetCIMStatusCodeDescription(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetCIMStatusCodeDescription (%s)", strCMPIStatus (rc),
      CMGetCharPtr(outCIMStatusCodeDesc));

  PROV_LOG ("Calling CMGetMessageArguments");
  outMsgArgs = CMGetMessageArguments(cmpiErrorClone, &rc);
  PROV_LOG ("++++ (%s) CMGetMessageArguments", strCMPIStatus (rc));
  arrSize = CMGetArrayCount(outMsgArgs, &rc);
  PROV_LOG ("++++ (%s) CMGetArrayCount (%d)", strCMPIStatus (rc), arrSize);
  for (i = 0 ; i < arrSize ; i++)
  {
      CMPIData dta = CMGetArrayElementAt(outMsgArgs, i, &rc);
      PROV_LOG ("++++ (%s) CMGetArrayElementAt (%d:%s)", strCMPIStatus (rc),
          i, CMGetCharPtr(dta.value.string));
  }

  PROV_LOG ("Calling CMPIResultFT.returnData (should return not supported)");
  rc = (rslt)->ft->returnError (rslt, cmpiErrorClone);
  PROV_LOG ("++++ (%s) returnData", strCMPIStatus (rc));

  PROV_LOG ("Releasing cloned CMPIError");
  rc = CMRelease(cmpiErrorClone);
  PROV_LOG ("++++ (%s) CMClone", strCMPIStatus (rc));

  // Return the oper_rc value via Result
  PROV_LOG ("++++ Calling CMReturnData+Done");
  CMReturnData (rslt, (CMPIValue *) & oper_rc, CMPI_uint32);
  CMReturnDone (rslt);

  PROV_LOG ("--- %s CMPI InvokeMethod() exited", _ClassName);
  PROV_LOG_CLOSE();
  return rc;
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/


CMMethodMIStub (TestCMPIErrorProvider,
                TestCMPIErrorProvider, _broker, CMNoHook)

/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/
