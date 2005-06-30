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
#include <string.h>
#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#include <libgen.h>
#endif

#define LOCALCLASSNAME "CWS_DirectoryContainsFile"
#define DIRECTORYCLASS "CWS_Directory"
#define FILECLASS      "CWS_PlainFile"

static const CMPIBroker * _broker;

/* ------------------------------------------------------------------ *
 * Instance MI Cleanup
 * ------------------------------------------------------------------ */

CMPIStatus CWS_DirectoryContainsFileCleanup( CMPIInstanceMI * mi,
				 const CMPIContext * ctx, CMPIBoolean term)
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Instance MI Functions
 * ------------------------------------------------------------------ */

CMPIStatus CWS_DirectoryContainsFileEnumInstanceNames( CMPIInstanceMI * mi,
					   const CMPIContext * ctx,
					   const CMPIResult * rslt,
					   const CMPIObjectPath * ref)
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}
CMPIStatus CWS_DirectoryContainsFileEnumInstances( CMPIInstanceMI * mi,
				       const CMPIContext * ctx, 
				       const CMPIResult * rslt, 
				       const CMPIObjectPath * ref, 
				       const char ** properties) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileGetInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx,
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop, 
				     const char ** properties) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileCreateInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop, 
					const CMPIInstance * ci) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileModifyInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx, 
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop,
				     const CMPIInstance * ci, 
				     const char **properties) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileDeleteInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop) 
{ 
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileExecQuery( CMPIInstanceMI * mi, 
				   const CMPIContext * ctx, 
				   const CMPIResult * rslt, 
				   const CMPIObjectPath * cop, 
				   const char * lang, 
				   const char * query) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

/* ------------------------------------------------------------------ *
 * Association MI Cleanup
 * ------------------------------------------------------------------ */

CMPIStatus CWS_DirectoryContainsFileAssociationCleanup( CMPIAssociationMI * mi,
					      const CMPIContext * ctx, CMPIBoolean term) 
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Association MI Functions
 * ------------------------------------------------------------------ */

CMPIStatus CWS_DirectoryContainsFileAssociators( CMPIAssociationMI * mi,
				       const CMPIContext * ctx,
				       const CMPIResult * rslt,
				       const CMPIObjectPath * cop,
				       const char * assocClass,
				       const char * resultClass,
				       const char * role,
				       const char * resultRole,
				       const char ** propertyList ) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileAssociatorNames( CMPIAssociationMI * mi,
					   const CMPIContext * ctx,
					   const CMPIResult * rslt,
					   const CMPIObjectPath * cop,
					   const char * assocClass,
					   const char * resultClass,
					   const char * role,
					   const char * resultRole ) 
{
  CMPIStatus      st = {CMPI_RC_OK,NULL};
  CMPIString     *clsname;
  CMPIData        data;
  CMPIObjectPath *op;
  void           *enumhdl;
  CWS_FILE        filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_DirectoryContainsFileAssociatorNames()\n");
#ifdef SIMULATED
    CMSetHostname((CMPIObjectPath *)cop,CSName());
#endif

  /*
   * Check if the object path belongs to a supported class
   */
  clsname = CMGetClassName(cop,NULL);
  if (clsname) {
    if (strcasecmp(DIRECTORYCLASS,CMGetCharPtr(clsname))==0) {
      /* we have a directory and can return the children */
      data = CMGetKey(cop,"Name",NULL);

      enumhdl = CWS_Begin_Enum(CMGetCharPtr(data.value.string),CWS_TYPE_PLAIN);

      if (enumhdl == NULL) {
	CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			     "Could not begin file enumeration");
	return st;
      } else {
	while (CWS_Next_Enum(enumhdl,&filebuf)) {
	  /* build object path from file buffer */
	  op = makePath(_broker,
			FILECLASS,
			CMGetCharPtr(CMGetNameSpace(cop,NULL)),
			&filebuf);  CMSetHostname(op,CSName());
	  if (CMIsNullObject(op)) {
	    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
				 "Could not construct object path");
	    break;
	  }
	  CMReturnObjectPath(rslt,op);
	}
	CWS_End_Enum(enumhdl);
      }

    }

    if (strcasecmp(FILECLASS,CMGetCharPtr(clsname))==0 ||
	strcasecmp(DIRECTORYCLASS,CMGetCharPtr(clsname))==0) {
      /* we can always return the parent */
      data = CMGetKey(cop,"Name",NULL);
      if (CWS_Get_File(dirname(CMGetCharPtr(data.value.string)),&filebuf)) {
	op = makePath(_broker,
		      DIRECTORYCLASS,
		      CMGetCharPtr(CMGetNameSpace(cop,NULL)),
		      &filebuf);  CMSetHostname(op,CSName());
	if (CMIsNullObject(op)) {
	  CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			       "Could not construct object path");
	  return st;
	}
	CMReturnObjectPath(rslt,op);
      }

    }

    else {
      if (!silentMode()) fprintf(stderr,"--- CWS_DirectoryContainsFileAssociatorNames() unsupported class \n");
    }
    CMReturnDone(rslt);
  } /* if (clsname) */

  return st;
}
CMPIStatus CWS_DirectoryContainsFileReferences( CMPIAssociationMI * mi,
				      const CMPIContext * ctx,
				      const CMPIResult * rslt,
				      const CMPIObjectPath * cop,
				      const char * assocClass,
				      const char * role,
				      const char ** propertyList ) 
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

CMPIStatus CWS_DirectoryContainsFileReferenceNames( CMPIAssociationMI * mi,
					  const CMPIContext * ctx,
					  const CMPIResult * rslt,
					  const CMPIObjectPath * cop,
					  const char * assocClass,
					  const char * role) 
{
  CMPIStatus      st = {CMPI_RC_OK,NULL};
  CMPIString     *clsname;
  CMPIData        data;
  CMPIObjectPath *op;
  CMPIObjectPath *opRef;
  void           *enumhdl;
  CWS_FILE        filebuf;

  if (!silentMode()) fprintf(stderr,"--- CWS_DirectoryContainsFileReferenceNames()\n");
#ifdef SIMULATED
    CMSetHostname((CMPIObjectPath*)cop,CSName());
#endif

  /*
   * Check if the object path belongs to a supported class
   */
  clsname = CMGetClassName(cop,NULL);
  if (clsname) {
    if (strcasecmp(DIRECTORYCLASS,CMGetCharPtr(clsname))==0) {
      /* we have a directory and can return the children */
      data = CMGetKey(cop,"Name",NULL);

      enumhdl = CWS_Begin_Enum(CMGetCharPtr(data.value.string),CWS_TYPE_PLAIN);

      if (enumhdl == NULL) {
	CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			     "Could not begin file enumeration");
	return st;
      } else {
	while (CWS_Next_Enum(enumhdl,&filebuf)) {
	  /* build object path from file buffer */
	  op = makePath(_broker,
			FILECLASS,
			CMGetCharPtr(CMGetNameSpace(cop,NULL)),
			&filebuf);
	  if (CMIsNullObject(op)) {
	    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
				 "Could not construct object path");
	    break;
	  }
	  /* make reference object path */
	  opRef = CMNewObjectPath(_broker,
				  CMGetCharPtr(CMGetNameSpace(cop,NULL)),
				  LOCALCLASSNAME,
				  NULL);  CMSetHostname(opRef,CSName());
	  if (CMIsNullObject(op)) {
	    CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
				 "Could not construct object path");
	    break;
	  }
	  CMAddKey(opRef,"GroupComponent",&cop,CMPI_ref);
	  CMAddKey(opRef,"PathComponent",&op,CMPI_ref);
	  CMReturnObjectPath(rslt,opRef);
	}
	CWS_End_Enum(enumhdl);
      }

    }

    if (strcasecmp(FILECLASS,CMGetCharPtr(clsname))==0 ||
	strcasecmp(DIRECTORYCLASS,CMGetCharPtr(clsname))==0) {
      /* we can always return the parent */
      data = CMGetKey(cop,"Name",NULL);
      if (CWS_Get_File(dirname(CMGetCharPtr(data.value.string)),&filebuf)) {
	op = makePath(_broker,
		      DIRECTORYCLASS,
		      CMGetCharPtr(CMGetNameSpace(cop,NULL)),
		      &filebuf);
	if (CMIsNullObject(op)) {
	  CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			       "Could not construct object path");
	  return st;
	}
	/* make reference object path */
	opRef = CMNewObjectPath(_broker,
				CMGetCharPtr(CMGetNameSpace(cop,NULL)),
				LOCALCLASSNAME,
				NULL);  CMSetHostname(opRef,CSName());
	if (CMIsNullObject(op)) {
	  CMSetStatusWithChars(_broker, &st, CMPI_RC_ERR_FAILED,
			       "Could not construct object path");
	  return st;
	}
	CMAddKey(opRef,"GroupComponent",&op,CMPI_ref);
	CMAddKey(opRef,"PartComponent",&cop,CMPI_ref);
	CMReturnObjectPath(rslt,opRef);
      }

    }

    else {
      if (!silentMode()) fprintf(stderr,"--- CWS_DirectoryContainsFileReferenceNames() unsupported class \n");
    }
    CMReturnDone(rslt);
  } /* if (clsname) */

  return st;
}

/* ------------------------------------------------------------------ *
 * Instance MI Factory
 *
 * NOTE: This is an example using the convenience macros. This is OK
 *       as long as the MI has no special requirements, i.e. to store
 *       data between calls.
 * ------------------------------------------------------------------ */

CMInstanceMIStub( CWS_DirectoryContainsFile,
		  CWS_DirectoryContainsFileProvider,
		  _broker,
		  CMNoHook);

CMAssociationMIStub( CWS_DirectoryContainsFile,
		  CWS_DirectoryContainsFileProvider,
		  _broker,
		  CMNoHook);

