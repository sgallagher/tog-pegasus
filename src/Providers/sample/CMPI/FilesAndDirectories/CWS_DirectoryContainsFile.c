//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author:      Viktor Mihajlovski <mihajlov@de.ibm.com>
//
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
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

#ifdef CMPI_VER_100
static const CMPIBroker * _broker;
#else
static CMPIBroker * _broker;
#endif

/* ------------------------------------------------------------------ *
 * Instance MI Cleanup
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileCleanup( CMPIInstanceMI * mi,
				 const CMPIContext * ctx, CMPIBoolean *term)
#else
CMPIStatus CWS_DirectoryContainsFileCleanup( CMPIInstanceMI * mi,
				 CMPIContext * ctx)
#endif
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Instance MI Functions
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileEnumInstanceNames( CMPIInstanceMI * mi,
					   const CMPIContext * ctx,
					   const CMPIResult * rslt,
					   const CMPIObjectPath * ref)
#else
CMPIStatus CWS_DirectoryContainsFileEnumInstanceNames( CMPIInstanceMI * mi,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * ref)
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}
#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileEnumInstances( CMPIInstanceMI * mi,
				       const CMPIContext * ctx, 
				       const CMPIResult * rslt, 
				       const CMPIObjectPath * ref, 
				       const char ** properties) 
#else
CMPIStatus CWS_DirectoryContainsFileEnumInstances( CMPIInstanceMI * mi,
				       CMPIContext * ctx, 
				       CMPIResult * rslt, 
				       CMPIObjectPath * ref, 
				       char ** properties) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileGetInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx,
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop, 
				     const char ** properties) 
#else
CMPIStatus CWS_DirectoryContainsFileGetInstance( CMPIInstanceMI * mi,
				     CMPIContext * ctx,
				     CMPIResult * rslt, 
				     CMPIObjectPath * cop, 
				     char ** properties) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileCreateInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop, 
					const CMPIInstance * ci) 
#else
CMPIStatus CWS_DirectoryContainsFileCreateInstance( CMPIInstanceMI * mi, 
					CMPIContext * ctx, 
					CMPIResult * rslt, 
					CMPIObjectPath * cop, 
					CMPIInstance * ci) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileModifyInstance( CMPIInstanceMI * mi, 
				     const CMPIContext * ctx, 
				     const CMPIResult * rslt, 
				     const CMPIObjectPath * cop,
				     const CMPIInstance * ci, 
				     const char **properties) 
#else
CMPIStatus CWS_DirectoryContainsFileSetInstance( CMPIInstanceMI * mi, 
				     CMPIContext * ctx, 
				     CMPIResult * rslt, 
				     CMPIObjectPath * cop,
				     CMPIInstance * ci, 
				     char **properties) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileDeleteInstance( CMPIInstanceMI * mi, 
					const CMPIContext * ctx, 
					const CMPIResult * rslt, 
					const CMPIObjectPath * cop) 
#else
CMPIStatus CWS_DirectoryContainsFileDeleteInstance( CMPIInstanceMI * mi, 
					CMPIContext * ctx, 
					CMPIResult * rslt, 
					CMPIObjectPath * cop) 
#endif
{ 
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileExecQuery( CMPIInstanceMI * mi, 
				   const CMPIContext * ctx, 
				   const CMPIResult * rslt, 
				   const CMPIObjectPath * cop, 
				   const char * lang, 
				   const char * query) 
#else
CMPIStatus CWS_DirectoryContainsFileExecQuery( CMPIInstanceMI * mi, 
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
 * Association MI Cleanup
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileAssociationCleanup( CMPIAssociationMI * mi,
					      const CMPIContext * ctx, CMPIBoolean *term) 
#else
CMPIStatus CWS_DirectoryContainsFileAssociationCleanup( CMPIAssociationMI * mi,
					      CMPIContext * ctx) 
#endif
{
  CMReturn(CMPI_RC_OK);
}

/* ------------------------------------------------------------------ *
 * Association MI Functions
 * ------------------------------------------------------------------ */

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileAssociators( CMPIAssociationMI * mi,
				       const CMPIContext * ctx,
				       const CMPIResult * rslt,
				       const CMPIObjectPath * cop,
				       const char * assocClass,
				       const char * resultClass,
				       const char * role,
				       const char * resultRole,
				       const char ** propertyList ) 
#else
CMPIStatus CWS_DirectoryContainsFileAssociators( CMPIAssociationMI * mi,
				       CMPIContext * ctx,
				       CMPIResult * rslt,
				       CMPIObjectPath * cop,
				       const char * assocClass,
				       const char * resultClass,
				       const char * role,
				       const char * resultRole,
				       char ** propertyList ) 

#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileAssociatorNames( CMPIAssociationMI * mi,
					   const CMPIContext * ctx,
					   const CMPIResult * rslt,
					   const CMPIObjectPath * cop,
					   const char * assocClass,
					   const char * resultClass,
					   const char * role,
					   const char * resultRole ) 
#else
CMPIStatus CWS_DirectoryContainsFileAssociatorNames( CMPIAssociationMI * mi,
					   CMPIContext * ctx,
					   CMPIResult * rslt,
					   CMPIObjectPath * cop,
					   const char * assocClass,
					   const char * resultClass,
					   const char * role,
					   const char * resultRole ) 
#endif
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
#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileReferences( CMPIAssociationMI * mi,
				      const CMPIContext * ctx,
				      const CMPIResult * rslt,
				      const CMPIObjectPath * cop,
				      const char * assocClass,
				      const char * role,
				      const char ** propertyList ) 
#else
CMPIStatus CWS_DirectoryContainsFileReferences( CMPIAssociationMI * mi,
				      CMPIContext * ctx,
				      CMPIResult * rslt,
				      CMPIObjectPath * cop,
				      const char * assocClass,
				      const char * role,
				      char ** propertyList ) 
#endif
{
  CMReturn( CMPI_RC_ERR_NOT_SUPPORTED );
}

#ifdef CMPI_VER_100
CMPIStatus CWS_DirectoryContainsFileReferenceNames( CMPIAssociationMI * mi,
					  const CMPIContext * ctx,
					  const CMPIResult * rslt,
					  const CMPIObjectPath * cop,
					  const char * assocClass,
					  const char * role) 
#else
CMPIStatus CWS_DirectoryContainsFileReferenceNames( CMPIAssociationMI * mi,
					  CMPIContext * ctx,
					  CMPIResult * rslt,
					  CMPIObjectPath * cop,
					  const char * assocClass,
					  const char * role) 
#endif
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

