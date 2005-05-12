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
  \file native.h
  \brief Header file for the native encapsulated CMPI data type implementation.

  This file defines all the data types and functions necessary to use
  native encapsulated CMPI data objects. These are clones of the regular
  CMPI data types like CMPIObjectPath, CMPIInstance etc., however, they
  can be instantiated and manipulated without a full blown CIMOM. Instead,
  they use an autononmous CIMOM clone that provides all the functions
  to create these objects as defined by the CMPIBrokerEncFT.

  \author Frank Scheffler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"
#include "native.h"


struct native_cop {
	CMPIObjectPath cop;
	int mem_state;

	char * namespace;
	char * classname;
	struct native_property * keys;
};


static struct native_cop * __new_empty_cop ( int, 
					     const char *,
					     const char *,
					     CMPIStatus * );

/****************************************************************************/


static CMPIStatus __oft_release ( CMPIObjectPath * cop )
{
	struct native_cop * o = (struct native_cop *) cop;

	if ( o->mem_state == TOOL_MM_NO_ADD ) {

		o->mem_state = TOOL_MM_ADD;

		tool_mm_add ( o );
		tool_mm_add ( o->classname );
		tool_mm_add ( o->namespace );

		propertyFT.release ( o->keys );

		CMReturn ( CMPI_RC_OK );
	}

	CMReturn ( CMPI_RC_ERR_FAILED );
}


static CMPIObjectPath * __oft_clone ( CONST CMPIObjectPath * cop, CMPIStatus * rc )
{
	CMPIStatus tmp;
	struct native_cop * o   = (struct native_cop *) cop;
	struct native_cop * new = __new_empty_cop ( TOOL_MM_NO_ADD,
						    o->namespace,
						    o->classname,
						    &tmp );

	if ( tmp.rc == CMPI_RC_OK ) {
		new->keys = propertyFT.clone ( o->keys, rc );

	} else if ( rc ) CMSetStatus ( rc, tmp.rc );

	return (CMPIObjectPath *) new;
}



static CMPIStatus __oft_setNameSpace ( CMPIObjectPath * cop,
				       const char * namespace )
{
	struct native_cop * o = (struct native_cop *) cop;

	char * ns = ( namespace )? strdup ( namespace ): NULL;
  
	if ( o->mem_state == TOOL_MM_NO_ADD ) {
		free ( o->namespace );
	} else {
		tool_mm_add ( ns );
	}

	o->namespace = ns;
	CMReturn ( CMPI_RC_OK );
}


static CMPIString * __oft_getNameSpace ( CONST CMPIObjectPath * cop,
					 CMPIStatus * rc )
{
	struct native_cop * o = (struct native_cop *) cop;

	return native_new_CMPIString ( o->namespace, rc );
}


static CMPIStatus __oft_setHostName ( CMPIObjectPath * cop, const char * hn )
{
	CMReturn ( CMPI_RC_ERR_NOT_SUPPORTED );
}


static CMPIString * __oft_getHostName ( CONST CMPIObjectPath * cop,
					CMPIStatus * rc )
{
	if ( rc ) CMSetStatus ( rc, CMPI_RC_ERR_NOT_SUPPORTED );
	return NULL;
}


static CMPIStatus __oft_setClassName ( CMPIObjectPath * cop,
				       const char * classname )
{
	struct native_cop * o = (struct native_cop *) cop;

	char * cn = ( classname )? strdup ( classname ): NULL;
  
	if ( o->mem_state == TOOL_MM_NO_ADD ) {
		free ( o->classname );
	} else {
		tool_mm_add ( cn );
	}

	o->classname = cn;
	CMReturn ( CMPI_RC_OK );
}


CMPIString * __oft_getClassName ( CONST CMPIObjectPath * cop,
					 CMPIStatus * rc )
{
	struct native_cop * o = (struct native_cop *) cop;

	return native_new_CMPIString ( o->classname, rc );
}


static CMPIStatus __oft_addKey ( CMPIObjectPath * cop,
				 const char * name,
				 CONST CMPIValue * value,
				 CONST CMPIType type )
{
	struct native_cop * o = (struct native_cop *) cop;

	CMReturn ( ( propertyFT.addProperty ( &o->keys,
					      o->mem_state,
					      name,
					      type,
					      CMPI_keyValue,
					      value ) )?
		   CMPI_RC_ERR_ALREADY_EXISTS:
		   CMPI_RC_OK );
}


static CMPIData __oft_getKey ( CONST CMPIObjectPath * cop,
			       const char * name,
			       CMPIStatus * rc )
{
	struct native_cop * o = (struct native_cop *) cop;

	return propertyFT.getDataProperty ( o->keys, name, rc );
}


static CMPIData __oft_getKeyAt ( CONST CMPIObjectPath * cop,
				 unsigned int index,
				 CMPIString ** name,
				 CMPIStatus * rc )
{
	struct native_cop * o = (struct native_cop *) cop;

	return propertyFT.getDataPropertyAt ( o->keys, index, name, rc );
}


static unsigned int __oft_getKeyCount ( CONST CMPIObjectPath * cop, CMPIStatus * rc )
{
	struct native_cop * o = (struct native_cop *) cop;

	return propertyFT.getPropertyCount ( o->keys, rc );
}


static CMPIStatus __oft_setNameSpaceFromObjectPath ( CMPIObjectPath * cop,
						     CONST CMPIObjectPath * src )
{
	struct native_cop * s = (struct native_cop *) src;
	return __oft_setNameSpace ( cop, s->namespace );
}

extern char * value2Chars ( CMPIType type, CMPIValue * value );

CMPIString *__oft_toString( CONST CMPIObjectPath * cop,
                                  CMPIStatus *rc)
{
//            "//atp:9999/root/cimv25:TennisPlayer.first="Patrick",last="Rafter";

   char str[2048]={0};
   CMPIString *ns;
   CMPIString *cn;
   CMPIString *hn;
   CMPIString *name;
   CMPIData data;
   unsigned int i,m;
   char *v;

   hn=__oft_getHostName(cop,rc);
   ns=__oft_getNameSpace(cop,rc);
   cn=__oft_getClassName(cop,rc);
   strcat(str,(char*)cn->hdl);
   for (i=0,m=__oft_getKeyCount(cop,rc); i<m; i++) {
        data=__oft_getKeyAt(cop,i,&name,rc);
	if (i) strcat(str,",");
        else strcat(str,".");
	strcat(str,(char*)name->hdl);
        strcat(str,"=");
	v=value2Chars(data.type,&data.value);
	strcat(str,v);
	free(v);
   };
//   printf("--- toString(): >%s<\n",str);
   return native_new_CMPIString ( str, rc );
}

static CMPIObjectPathFT oft = {
	NATIVE_FT_VERSION,
	__oft_release,
	__oft_clone,
	__oft_setNameSpace,
	__oft_getNameSpace,
	__oft_setHostName,
	__oft_getHostName,
	__oft_setClassName,
	__oft_getClassName,
	__oft_addKey,
	__oft_getKey,
	__oft_getKeyAt,
	__oft_getKeyCount,
	__oft_setNameSpaceFromObjectPath,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	__oft_toString
};

CMPIObjectPathFT *CMPI_ObjectPath_FT=&oft;

static struct native_cop * __new_empty_cop ( int mm_add,
					     const char * namespace,
					     const char * classname,
					     CMPIStatus * rc )
{
	static CMPIObjectPath o = {
		"CMPIObjectPath",
		&oft
	};

	struct native_cop * cop =
		(struct native_cop *) 
		tool_mm_alloc ( mm_add, sizeof ( struct native_cop ) );

	cop->cop       = o;
	cop->mem_state = mm_add;
	cop->classname = ( classname )? strdup ( classname ): NULL;
	cop->namespace = ( namespace )? strdup ( namespace ): NULL;

	if ( mm_add == TOOL_MM_ADD ) {
		tool_mm_add ( cop->classname );
		tool_mm_add ( cop->namespace );
	}

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
	return cop;
}



CMPIObjectPath * native_new_CMPIObjectPath ( const char * namespace, 
					     const char * classname,
					     CMPIStatus * rc )
{
	return (CMPIObjectPath *) __new_empty_cop ( TOOL_MM_ADD,
						    namespace,
						    classname,
						    rc );
}


/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
