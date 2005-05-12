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
  \file property.c
  \brief Native property implementation.

  This module implements a native property, which is not public to any
  provider programmer. It is used to implement various other data types
  natively, such as instances, object-paths and args.

  It provides means to maintain linked lists of named properties including
  functionality to add, remove, clone and release them.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <string.h>

#include "mm.h"
#include "native.h"

//! Storage container for commonly needed data within native CMPI data types.
/*!
  This structure is used to build linked lists of data containers as needed
  for various native data types.
*/
struct native_property {
	char * name;		        //!< Property identifier.
	CMPIType type;		        //!< Associated CMPIType.
	CMPIValueState state; 	        //!< Current value state.
	CMPIValue value;	        //!< Current value.
	struct native_property * next;	//!< Pointer to next property.
};


/****************************************************************************/

static CMPIData __convert2CMPIData ( struct native_property * prop,
				     CMPIString ** propname )
{
	CMPIData result;

	if ( prop != NULL ) {
		result.type  = prop->type;
		result.state = prop->state;
		result.value = prop->value;

		if ( propname ) {
			*propname  = native_new_CMPIString ( prop->name,
							     NULL );
		}

	} else {
		result.state = CMPI_nullValue;
	}

	return result;
}


/**
 * returns non-zero if already existant
 */
static int __addProperty ( struct native_property ** prop,
			   int mm_add, 
			   const char * name,
			   CONST CMPIType type,  
			   CMPIValueState state, 
			   CONST CMPIValue * value )
{
	CMPIValue v;
	
	if ( *prop == NULL ) {
		struct native_property * tmp = *prop =
			(struct native_property *) 
			tool_mm_alloc ( mm_add,
					sizeof ( struct native_property ) );
  
		tmp->name = strdup ( name );

		if ( mm_add == TOOL_MM_ADD ) tool_mm_add ( tmp->name );

		tmp->type  = type;
		if ( type == CMPI_chars ) {

			tmp->type = CMPI_string;
			v.string = native_new_CMPIString ( (char *) value,
							   NULL );
			value = &v;
		}



		if ( type != CMPI_null ) {
			tmp->state = state;

			if ( mm_add == TOOL_MM_ADD ) {

				tmp->value = *value;
			} else {
			
				CMPIStatus rc;
				tmp->value = native_clone_CMPIValue ( type,
								      value,
								      &rc );
				// what if clone() fails???
			}
		} else tmp->state = CMPI_nullValue;

		return 0;
	}
	return ( strcmp ( (*prop)->name, name ) == 0 ||
		 __addProperty ( &( (*prop)->next ), 
				 mm_add, 
				 name, 
				 type, 
				 state, 
				 value ) );
}


/**
 * returns -1 if non-existant
 */
static int __setProperty ( struct native_property * prop, 
			   int mm_add,
			   const char * name, 
			   CMPIType type,
			   CONST CMPIValue * value )
{
	CMPIValue v;
	if ( prop == NULL ) {
		return -1;
	}

	if ( strcmp ( prop->name, name ) == 0 ) {

		CMPIStatus rc;

		if ( ! ( prop->state & CMPI_nullValue ) )
			native_release_CMPIValue ( prop->type, &prop->value );

		if ( type == CMPI_chars ) {

			type = CMPI_string;
			v.string = native_new_CMPIString ( (char *) value,
							   NULL );
			value = &v;
		}

		prop->type  = type;

		if ( type != CMPI_null ) {
			prop->value =
				( mm_add == TOOL_MM_ADD )?
				*value:
				native_clone_CMPIValue ( type, value, &rc );

			// what if clone() fails ???

		} else prop->state = CMPI_nullValue;

		return 0;
	}
	return __setProperty ( prop->next, mm_add, name, type, value);
}


static struct native_property * __getProperty ( struct native_property * prop, 
						const char * name )
{
	if ( ! prop || ! name ) {
		return NULL;
	}
	return ( strcmp ( prop->name, name ) == 0 )?
		prop: __getProperty ( prop->next, name );
}


static CMPIData __getDataProperty ( struct native_property * prop, 
				    const char * name,
				    CMPIStatus * rc )
{
	struct native_property * p = __getProperty ( prop, name );

	if ( rc ) CMSetStatus ( rc,
				( p )?
				CMPI_RC_OK:
				CMPI_RC_ERR_NO_SUCH_PROPERTY );

	return __convert2CMPIData ( p, NULL );
}


static struct native_property * __getPropertyAt
( struct native_property * prop, unsigned int pos )
{
	if ( ! prop ) {
		return NULL;
	} 

	return ( pos == 0 )?
		prop: __getPropertyAt ( prop->next, --pos );
}


static CMPIData __getDataPropertyAt ( struct native_property * prop, 
				      unsigned int pos,
				      CMPIString ** propname,
				      CMPIStatus * rc )
{
	struct native_property * p = __getPropertyAt ( prop, pos );

	if ( rc ) CMSetStatus ( rc,
				( p )?
				CMPI_RC_OK:
				CMPI_RC_ERR_NO_SUCH_PROPERTY );

	return __convert2CMPIData ( p, propname );
}


static CMPICount __getPropertyCount ( struct native_property * prop,
				      CMPIStatus * rc )
{
	CMPICount c = 0;

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );

	while ( prop != NULL ) {
		c++;
		prop = prop->next;
	}

	return c;
}


static void __release ( struct native_property * prop )
{
	for ( ; prop; prop = prop->next ) {
		tool_mm_add ( prop );
		tool_mm_add ( prop->name );
		native_release_CMPIValue ( prop->type, &prop->value );
	}
}


static struct native_property * __clone ( struct native_property * prop,
					  CMPIStatus * rc )
{
	struct native_property * result;
	CMPIStatus tmp;

	if ( prop == NULL ) {

		if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
		return NULL;
	}

	result = 
		(struct native_property * ) 
		tool_mm_alloc ( TOOL_MM_NO_ADD,
				sizeof ( struct native_property ) );

	result->name  = strdup ( prop->name );
	result->type  = prop->type;
	result->state = prop->state;
	result->value = native_clone_CMPIValue ( prop->type,
						 &prop->value,
						 &tmp );

	if ( tmp.rc != CMPI_RC_OK ) {

		result->state = CMPI_nullValue;
	}
  
	result->next  = __clone ( prop->next, rc );
	return result;
}


/**
 * Global function table to access native_property helper functions.
 */
struct native_propertyFT propertyFT = {
	__addProperty,
	__setProperty,
	__getDataProperty,
	__getDataPropertyAt,
	__getPropertyCount,
	__release,
	__clone
};

/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
