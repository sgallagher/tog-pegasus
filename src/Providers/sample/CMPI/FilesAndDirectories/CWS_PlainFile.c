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
//%/////////////////////////////////////////////////////////////////////////////

#include "CWS_FileUtils.h"
#include "cwsutil.h"
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>

#define LOCALCLASSNAME "CWS_PlainFile"

#ifdef CMPI_VER_100
static const CMPIBroker * _broker;
#else
static CMPIBroker * _broker;
#endif
/* ------------------------------------------------------------------ *
 * Instance MI Cleanup
 * ------------------------------------------------------------------ */
#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileCleanup( CMPIInstanceMI * mi, 
				 const CMPIContext * ctx, CMPIBoolean term) 
#else
CMPIStatus CWS_PlainFileCleanup( CMPIInstanceMI * mi, 
				 CMPIContext * ctx) 
#endif
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Instance MI Functions
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileEnumInstanceNames( CMPIInstanceMI * mi, 
					   const CMPIContext * ctx, 
					   const CMPIResult * rslt, 
					   const CMPIObjectPath * ref) 
#else
CMPIStatus CWS_PlainFileEnumInstanceNames( CMPIInstanceMI * mi, 
					   CMPIContext * ctx, 
					   CMPIResult * rslt, 
					   CMPIObjectPath * ref) 
#endif
{
  CMPIObjectPath *op;
  CMPIStatus      st = {CMPI_RC_OK,NULL};
  void           *enumhdl;
  CWS_FILE        filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_PlainFileEnumInstanceNames() \n");

  enumhdl = CWS_Begin_Enum(CWS_FILEROOT,CWS_TYPE_PLAIN);

  if (enumhdl == NULL) {
    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			 "Could not begin file enumeration");
    return st;
  } else {
    while (CWS_Next_Enum(enumhdl,&filebuf)) {
      /* build object path from file buffer */
      op = makePath(_broker,
		    LOCALCLASSNAME,
		    CMGetCharPtr(CMGetNameSpace(ref,NULL)),
		    &filebuf);
      if (CMIsNullObject(op)) {
	CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			     "Could not construct object path");
	break;
      }
      CMReturnObjectPath(rslt,op);
    }
    CMReturnDone(rslt);
    CWS_End_Enum(enumhdl);
  }

  return st;
}

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileEnumInstances( CMPIInstanceMI * mi, 
				       const CMPIContext * ctx, 
				       const CMPIResult * rslt, 
				       const CMPIObjectPath * ref, 
				       const char ** properties) 
#else
CMPIStatus CWS_PlainFileEnumInstances( CMPIInstanceMI * mi, 
				       CMPIContext * ctx, 
				       CMPIResult * rslt, 
				       CMPIObjectPath * ref, 
				       char ** properties) 
#endif
{
  CMPIInstance   *in;
  CMPIStatus      st = {CMPI_RC_OK,NULL};
  void           *enumhdl;
  CWS_FILE        filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_PlainFileEnumInstances() \n");

  enumhdl = CWS_Begin_Enum(CWS_FILEROOT,CWS_TYPE_PLAIN);

  if (enumhdl == NULL) {
    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			 "Could not begin file enumeration");
    return st;
  } else {
    while (CWS_Next_Enum(enumhdl,&filebuf)) {
      /* build instance from file buffer */
      in = makeInstance(_broker,
			LOCALCLASSNAME,
			CMGetCharPtr(CMGetNameSpace(ref,NULL)),
			&filebuf);
      if (CMIsNullObject(in)) {
	CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			     "Could not construct instance");
	break;
      }
      CMReturnInstance(rslt,in);
    }
    CMReturnDone(rslt);
    CWS_End_Enum(enumhdl);
  }

  return st;
}

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileGetInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx, 
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop, 
				     const char ** properties) 
#else
CMPIStatus CWS_PlainFileGetInstance( CMPIInstanceMI * mi, 
				     CMPIContext * ctx, 
				     CMPIResult * rslt, 
				     CMPIObjectPath * cop, 
				     char ** properties) 
#endif
{
  CMPIInstance *in = NULL;
  CMPIStatus    st = {CMPI_RC_OK,NULL};
  CMPIData      nd = CMGetKey(cop,"Name",&st);
  CWS_FILE      filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_PlainFileGetInstance() \n");

  if (st.rc == CMPI_RC_OK &&
      nd.type == CMPI_string &&
      CWS_Get_File(CMGetCharPtr(nd.value.string),&filebuf))
    in = makeInstance(_broker,
		      LOCALCLASSNAME,
		      CMGetCharPtr(CMGetNameSpace(cop,NULL)),
		      &filebuf);

  if (CMIsNullObject(in)) {
    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			 "Could not find or construct instance");
  } else {
    CMReturnInstance(rslt,in);
    CMReturnDone(rslt);
  }

  return st;
}

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileCreateInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop, 
					const CMPIInstance * ci) 
#else
CMPIStatus CWS_PlainFileCreateInstance( CMPIInstanceMI * mi, 
					CMPIContext * ctx, 
					CMPIResult * rslt, 
					CMPIObjectPath * cop, 
					CMPIInstance * ci) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileModifyInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx, 
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop,
				     const CMPIInstance * ci, 
				     const char **properties) 
#else
CMPIStatus CWS_PlainFileSetInstance( CMPIInstanceMI * mi, 
				     CMPIContext * ctx, 
				     CMPIResult * rslt, 
				     CMPIObjectPath * cop,
				     CMPIInstance * ci, 
				     char **properties) 
#endif
{
  CMPIStatus st = {CMPI_RC_OK,NULL};
  CWS_FILE   filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_PlainFileSetInstance() \n");

  if (!makeFileBuf(ci,&filebuf) || !CWS_Update_File(&filebuf))
    CMSetStatusWithChars(_broker,&st,CMPI_RC_ERR_FAILED,
			 "Could not update instance");

  return st;
}
#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileDeleteInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop) 
#else
CMPIStatus CWS_PlainFileDeleteInstance( CMPIInstanceMI * mi, 
					CMPIContext * ctx, 
					CMPIResult * rslt, 
					CMPIObjectPath * cop) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileExecQuery( CMPIInstanceMI * mi, 
				   const CMPIContext * ctx, 
				   const CMPIResult * rslt, 
				   const CMPIObjectPath * cop, 
				   const char * lang, 
				   const char * query) 
#else
CMPIStatus CWS_PlainFileExecQuery( CMPIInstanceMI * mi, 
				   CMPIContext * ctx, 
				   CMPIResult * rslt, 
				   CMPIObjectPath * cop, 
				   char * lang, 
				   char * query) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

/* ------------------------------------------------------------------ *
 * Method MI Cleanup
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileMethodCleanup( CMPIMethodMI * mi,
				       const CMPIContext * ctx, CMPIBoolean term)
#else
CMPIStatus CWS_PlainFileMethodCleanup( CMPIMethodMI * mi,
				       CMPIContext * ctx)
#endif
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Method MI Functions
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_PlainFileInvokeMethod( CMPIMethodMI * mi, 
				      const CMPIContext * ctx, 
				      const CMPIResult * rslt,
				      const CMPIObjectPath * cop,
				      const char * method,
				      const CMPIArgs * in,
				      CMPIArgs * out)
#else
CMPIStatus CWS_PlainFileInvokeMethod( CMPIMethodMI * mi,
				      CMPIContext * ctx,
				      CMPIResult * rslt,
				      CMPIObjectPath * cop,
				      const char * method,
				      CMPIArgs * in,
				      CMPIArgs * out)
#endif
{
  CMPIStatus st = {CMPI_RC_OK,NULL};
  CMPIData   dt;
  char typebuf[1000];

  if (!silentMode()) fprintf(stderr,"--- CWS_PlainFileInvokeMethod()\n");

  dt=CMGetKey(cop,"Name",&st);
  if (st.rc != CMPI_RC_OK) {
    CMSetStatusWithChars(_broker,&st,CMPI_RC_ERR_FAILED,
			 "Could not get instance name");
  } else if (CWS_Get_FileType(CMGetCharPtr(dt.value.string),typebuf,
			      sizeof(typebuf))) {
    CMSetStatusWithChars(_broker,&st,CMPI_RC_ERR_FAILED,
			 "Could not get type");
  } else {
    CMReturnData(rslt,typebuf,CMPI_chars);
    CMReturnDone(rslt);
  }

  return st;
}

/* ------------------------------------------------------------------ *
 * Instance MI Factory
 *
 * NOTE: This is an example using the convenience macros. This is OK
 *       as long as the MI has no special requirements, i.e. to store
 *       data between calls.
 * ------------------------------------------------------------------ */

CMInstanceMIStub( CWS_PlainFile,
		  CWS_PlainFileProvider,
		  _broker,
		  CMNoHook);

/* ------------------------------------------------------------------ *
 * Method MI Factory
 *
 * ------------------------------------------------------------------ */

CMMethodMIStub( CWS_PlainFile,
		CWS_PlainFileProvider,
		_broker,
		CMNoHook);


