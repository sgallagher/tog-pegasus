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
// Author: Frank Scheffler
//
// Modified By:  Adrian Schuur (schuur@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

/*!
  \file selectexp.c
  \brief Native CMPISelectExp implementation.

  This is the native CMPISelectExp implementation as used for remote
  providers. It reflects the well-defined interface of a regular
  CMPISelectExp, however, it works independently from the management broker.
  
  It is part of a native broker implementation that simulates CMPI data
  types rather than interacting with the entities in a full-grown CIMOM.

  \author Frank Scheffler

  \todo THIS IS NOT FULLY IMPLEMENTED YET!!!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"
#include "native.h"


struct native_selectexp {
	CMPISelectExp exp;
	int mem_state;

	char * queryString;
	char * language;
};


static struct native_selectexp * __new_exp ( int, 
					     const char *,
					     const char *,
					     CMPIArray **,
					     CMPIStatus * );


/*****************************************************************************/

static CMPIStatus __eft_release ( CMPISelectExp * exp )
{
	struct native_selectexp * e = (struct native_selectexp *) exp;

	if ( e->mem_state == TOOL_MM_NO_ADD ) {

		tool_mm_add ( e );
		tool_mm_add ( e->queryString );
		tool_mm_add ( e->language );

		CMReturn ( CMPI_RC_OK );
	}

	CMReturn ( CMPI_RC_ERR_FAILED );
}


static CMPISelectExp * __eft_clone ( CONST CMPISelectExp * exp, CMPIStatus * rc )
{
	struct native_selectexp * e   = (struct native_selectexp *) exp;

	return (CMPISelectExp *) __new_exp ( TOOL_MM_NO_ADD,
					     e->queryString,
					     e->language,
					     NULL,
					     rc );
}



CMPIBoolean __eft_evaluate ( CONST CMPISelectExp * exp,
			     CONST CMPIInstance * inst,
			     CMPIStatus * rc )
{
	if (rc) rc->rc = CMPI_RC_ERR_NOT_SUPPORTED;
	return 0;
}


CMPIString * __eft_getString ( CONST CMPISelectExp * exp, CMPIStatus * rc )
{
	if (rc) rc->rc = CMPI_RC_ERR_NOT_SUPPORTED;
	return NULL;
}


CMPISelectCond * __eft_getDOC ( CONST CMPISelectExp * exp, CMPIStatus * rc )
{
	if (rc) rc->rc = CMPI_RC_ERR_NOT_SUPPORTED;
	return NULL;
}


CMPISelectCond * __eft_getCOD ( CONST CMPISelectExp * exp, CMPIStatus * rc )
{
	if (rc) rc->rc = CMPI_RC_ERR_NOT_SUPPORTED;
	return NULL;
}

CMPIBoolean __eft_evaluateUsingAccessor (CONST CMPISelectExp* se,
        CMPIAccessor *accessor, void *parm, CMPIStatus* rc)
{
	if (rc) rc->rc = CMPI_RC_ERR_NOT_SUPPORTED;
	return 0;
}

static struct native_selectexp * __new_exp ( int mm_add,
					     const char * queryString,
					     const char * language,
					     CMPIArray ** projection,
					     CMPIStatus * rc )
{
	static CMPISelectExpFT eft = {
		NATIVE_FT_VERSION,
		__eft_release,
		__eft_clone,
		__eft_evaluate,
		__eft_getString,
		__eft_getDOC,
		__eft_getCOD,
		__eft_evaluateUsingAccessor
	};
	static CMPISelectExp e = {
		"CMPISelectExp",
		&eft
	};


	struct native_selectexp * exp =
		(struct native_selectexp *) 
		tool_mm_alloc ( mm_add, sizeof ( struct native_selectexp ) );

	exp->exp         = e;
	exp->mem_state   = mm_add;
	exp->queryString = strdup ( queryString );
	exp->language    = strdup ( language );

	if ( mm_add == TOOL_MM_ADD ) {
		tool_mm_add ( exp->queryString );
		tool_mm_add ( exp->language );
	}

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
	return exp;
}


CMPISelectExp * native_new_CMPISelectExp ( const char * queryString, 
					   const char * language, 
					   CMPIArray ** projection,
					   CMPIStatus * rc )
{
	return (CMPISelectExp *) __new_exp ( TOOL_MM_ADD, 
					     queryString,
					     language,
					     projection,
					     rc );
}


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
