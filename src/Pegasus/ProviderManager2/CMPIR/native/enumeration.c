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
  \file enumeration.c
  \brief Native CMPIEnumeration implementation.

  This is the native CMPIEnumeration implementation as used for remote
  providers. It reflects the well-defined interface of a regular
  CMPIEnumeration, however, it works independently from the management broker.
  
  It is part of a native broker implementation that simulates CMPI data
  types rather than interacting with the entities in a full-grown CIMOM.

  \author Frank Scheffler
*/

#include "mm.h"
#include "native.h"


struct native_enum {
	CMPIEnumeration enumeration;
	int mem_state;

	CMPICount current;
	CMPIArray * data;
};


static struct native_enum * __new_enumeration ( int,
						CMPIArray *,
						CMPIStatus * );


/*****************************************************************************/

static CMPIStatus __eft_release ( CMPIEnumeration * enumeration )
{
	struct native_enum * e = (struct native_enum *) enumeration;

	if ( e->mem_state == TOOL_MM_NO_ADD ) {

		tool_mm_add ( enumeration );
		return e->data->ft->release ( e->data );
	}

	CMReturn ( CMPI_RC_ERR_FAILED );
}


static CMPIEnumeration * __eft_clone ( CONST CMPIEnumeration * enumeration,
				       CMPIStatus * rc )
{
	CMPIStatus tmp;
	struct native_enum * e = (struct native_enum *) enumeration;
	CMPIArray * data       = CMClone ( e->data, &tmp );

	if ( tmp.rc != CMPI_RC_OK ) {

		if ( rc ) CMSetStatus ( rc, CMPI_RC_ERR_FAILED );
		return NULL;
	}

	return 
		(CMPIEnumeration *) __new_enumeration ( TOOL_MM_NO_ADD,
							data,
							rc );
}


static CMPIData __eft_getNext ( CONST CMPIEnumeration * enumeration,
				CMPIStatus * rc )
{
	struct native_enum * e = (struct native_enum *) enumeration;
	return CMGetArrayElementAt ( e->data, e->current++, rc );
}


static CMPIBoolean __eft_hasNext ( CONST CMPIEnumeration * enumeration,
				   CMPIStatus * rc )
{
	struct native_enum * e = (struct native_enum *) enumeration;
	return ( e->current < CMGetArrayCount ( e->data, rc ) );
}


static CMPIArray * __eft_toArray ( CONST CMPIEnumeration * enumeration,
				   CMPIStatus * rc )
{
	struct native_enum * e = (struct native_enum *) enumeration;
	rc->rc = CMPI_RC_OK;
	return e->data;
}


static struct native_enum * __new_enumeration ( int mm_add,
						CMPIArray * array,
						CMPIStatus * rc )
{
	static CMPIEnumerationFT eft = {
		NATIVE_FT_VERSION,
		__eft_release,
		__eft_clone,
		__eft_getNext,
		__eft_hasNext,
		__eft_toArray
	};
	static CMPIEnumeration e = {
		"CMPIEnumeration",
		&eft
	};

	struct native_enum * enumeration =
		(struct native_enum *)
		tool_mm_alloc ( mm_add, sizeof ( struct native_enum ) );

	enumeration->enumeration = e;
	enumeration->mem_state   = mm_add;
	enumeration->data =
		( mm_add == TOOL_MM_NO_ADD )?
		CMClone ( array, rc ): array;

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
	return enumeration;
}


CMPIEnumeration * native_new_CMPIEnumeration ( CMPIArray * array,
					       CMPIStatus * rc )
{
	return (CMPIEnumeration *) __new_enumeration ( TOOL_MM_ADD,
						       array,
						       rc );
}


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
