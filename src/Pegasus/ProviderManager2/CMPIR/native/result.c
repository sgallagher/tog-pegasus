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
  \file result.c
  \brief Native CMPIResult implementation.

  This is the native CMPIResult implementation as used for remote
  providers. It reflects the well-defined interface of a regular
  CMPIResult, however, it works independently from the management broker.
  
  It is part of a native broker implementation that simulates CMPI data
  types rather than interacting with the entities in a full-grown CIMOM.

  \author Frank Scheffler
*/

#include "mm.h"
#include "native.h"


struct native_result {
	CMPIResult result;
	int mem_state;

	CMPICount current;
	CMPIArray * data;
};


static struct native_result * __new_empty_result ( int, CMPIStatus * );


/*****************************************************************************/

static CMPIStatus __rft_release ( CMPIResult * result )
{
	struct native_result * r = (struct native_result *) result;

	if ( r->mem_state == TOOL_MM_NO_ADD ) {

		tool_mm_add ( result );
		return r->data->ft->release ( r->data );
	}

	CMReturn ( CMPI_RC_ERR_FAILED );
}


static CMPIResult * __rft_clone ( CONST CMPIResult * result,
				  CMPIStatus * rc )
{
	CMPIArray * a = ( (struct native_result *) result )->data;
	struct native_result * r = __new_empty_result ( TOOL_MM_NO_ADD, rc );

	if ( rc && rc->rc != CMPI_RC_OK )
		return NULL;
  
	r->data = a->ft->clone ( a, rc );

	return (CMPIResult *) r;
}


static CMPIStatus __rft_returnData ( CONST CMPIResult * result,
				     const CMPIValue * val,
				     CONST CMPIType type )
{
	struct native_result * r = (struct native_result *) result;

	if ( r->current == 0 ) {

		CMPIStatus rc;
		r->data = native_new_CMPIArray ( 1, type, &rc );
		if ( rc.rc != CMPI_RC_OK ) return rc;

	} else native_array_increase_size ( r->data, 1 );


	return CMSetArrayElementAt ( r->data,
				     r->current++,
				     val, 
				     type );
}


static CMPIStatus __rft_returnInstance ( CONST CMPIResult * result,
					 CONST CMPIInstance * instance )
{
	CMPIValue v;
	v.inst = (CMPIInstance *)instance;

	return __rft_returnData ( result, &v, CMPI_instance );
}


static CMPIStatus __rft_returnObjectPath ( CONST CMPIResult * result,
					   CONST CMPIObjectPath * cop )
{
	CMPIValue v;
	v.ref = (CMPIObjectPath *)cop;

	return __rft_returnData ( result, &v, CMPI_ref );
}


static CMPIStatus __rft_returnDone ( CONST CMPIResult * result )
{
	CMReturn ( CMPI_RC_OK );
}


static struct native_result * __new_empty_result ( int mm_add,
						   CMPIStatus * rc )
{
	static CMPIResultFT rft = {
		NATIVE_FT_VERSION,
		__rft_release,
		__rft_clone,
		__rft_returnData,
		__rft_returnInstance,
		__rft_returnObjectPath,
		__rft_returnDone
	};
	static CMPIResult r = {
		"CMPIResult",
		&rft
	};

	struct native_result * result =
		(struct native_result *)
		tool_mm_alloc ( mm_add, sizeof ( struct native_result ) );

	result->result    = r;
	result->mem_state = mm_add;

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
	return result;
}


CMPIResult * native_new_CMPIResult ( CMPIStatus * rc )
{
	return (CMPIResult *) __new_empty_result ( TOOL_MM_ADD, rc );
}


CMPIArray * native_result2array ( CMPIResult * result )
{
	struct native_result * r = (struct native_result *) result;

	return r->data;
}


/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
