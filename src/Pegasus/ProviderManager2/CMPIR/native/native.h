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

#ifndef _REMOTE_CMPI_NATIVE_DATA_H
#define _REMOTE_CMPI_NATIVE_DATA_H

#define NATIVE_FT_VERSION 1

#ifndef CMPI_VERSION
#define CMPI_VERSION 100
#endif

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>


//! Forward declaration for anonymous struct.
struct native_property;


//! Function table for native_property handling functions.
/*!
  This structure holds all the function pointers necessary to handle
  linked lists of native_property structs.

  \sa propertyFT in native.h
*/
#ifdef CMPI_VER_100
#define CONST const
struct native_propertyFT {

	//! Adds a new native_property to a list.
	int (* addProperty) ( struct native_property **,
			      int,
			      const char *,
			      const CMPIType,
			      CMPIValueState,
			      const CMPIValue * );

	//! Resets the values of an existing native_property, if existant.
	int (* setProperty) ( struct native_property *,
			      int,
			      const char *,
			      CMPIType,
			      const CMPIValue * );

	//! Looks up a specifix native_property in CMPIData format.
	CMPIData (* getDataProperty) ( struct native_property *,
				       const char *,
				       CMPIStatus * );

	//! Extract an indexed native_property in CMPIData format.
	CMPIData (* getDataPropertyAt) ( struct native_property *,
					 unsigned int,
					 CMPIString **,
					 CMPIStatus * );

	//! Yields the number of native_property items in a list.
	CMPICount (* getPropertyCount) ( struct native_property *,
					 CMPIStatus * );

	//! Releases a complete list of native_property items.
	void (* release) ( struct native_property * );

	//! Clones a complete list of native_property items.
	struct native_property * (* clone) ( struct native_property *,
					     CMPIStatus * );
};
struct _NativeCMPIBrokerFT {
   CMPIBrokerFT brokerFt;
   CMPIString* (*getMessage) (const CMPIBroker* mb,
        const char *msgId, const char *defMsg, CMPIStatus* rc,
	unsigned int count, va_list);
   CMPIStatus (*logMessage)
       (const CMPIBroker*,int severity ,const char *id,const char *text,
    const CMPIString *string);

     CMPIStatus (*trace)
       (const CMPIBroker*,int level,const char *component,const char *text,
    const CMPIString *string);

   CMPIBoolean (*classPathIsA) ( const CMPIBroker * broker,
        const CMPIObjectPath * cop, const char * type, CMPIStatus * rc );
};
#else
#define CONST
struct native_propertyFT {

	//! Adds a new native_property to a list.
	int (* addProperty) ( struct native_property **,
			      int,
			      const char *,
			      CMPIType,
			      CMPIValueState,
			      CMPIValue * );

	//! Resets the values of an existing native_property, if existant.
	int (* setProperty) ( struct native_property *,
			      int,
			      const char *,
			      CMPIType,
			      CMPIValue * );

	//! Looks up a specifix native_property in CMPIData format.
	CMPIData (* getDataProperty) ( struct native_property *,
				       const char *,
				       CMPIStatus * );

	//! Extract an indexed native_property in CMPIData format.
	CMPIData (* getDataPropertyAt) ( struct native_property *,
					 unsigned int,
					 CMPIString **,
					 CMPIStatus * );

	//! Yields the number of native_property items in a list.
	CMPICount (* getPropertyCount) ( struct native_property *,
					 CMPIStatus * );

	//! Releases a complete list of native_property items.
	void (* release) ( struct native_property * );

	//! Clones a complete list of native_property items.
	struct native_property * (* clone) ( struct native_property *,
					     CMPIStatus * );
};

struct _NativeCMPIBrokerFT {
   CMPIBrokerFT brokerFt;
   CMPIArray *(*getKeyNames)(CMPIBroker * broker,
        CMPIContext * context, CMPIObjectPath * cop, CMPIStatus * rc);
   CMPIString* (*getMessage) (CMPIBroker* mb,
        const char *msgId, const char *defMsg, CMPIStatus* rc,
	unsigned int count, va_list);
   CMPIBoolean (*classPathIsA) ( CMPIBroker * broker,
        CMPIObjectPath * cop, const char * type, CMPIStatus * rc );
};
#endif
typedef struct _NativeCMPIBrokerFT NativeCMPIBrokerFT;


/****************************************************************************/

void native_release_CMPIValue ( CMPIType, CMPIValue * val );
CMPIValue native_clone_CMPIValue ( CMPIType, CONST CMPIValue * val, CMPIStatus * );
CMPIString * native_new_CMPIString ( const char *, CMPIStatus * );
CMPIArray * native_new_CMPIArray ( CMPICount size,
				   CMPIType type,
				   CMPIStatus * );
void native_array_increase_size ( CMPIArray *, CMPICount );
CMPIInstance * native_new_CMPIInstance ( CONST CMPIObjectPath *, CMPIStatus * );
CMPIResult * native_new_CMPIResult ( CMPIStatus * );
CMPIArray * native_result2array ( CMPIResult * );
CMPIEnumeration * native_new_CMPIEnumeration ( CMPIArray *, CMPIStatus * );
CMPIObjectPath * native_new_CMPIObjectPath ( const char *,
					     const char *,
					     CMPIStatus * );
CMPIArgs * native_new_CMPIArgs ( CMPIStatus * );
CMPIDateTime * native_new_CMPIDateTime ( CMPIStatus * );
CMPIDateTime * native_new_CMPIDateTime_fromBinary ( CMPIUint64,
						    CMPIBoolean,
						    CMPIStatus * );
CMPIDateTime * native_new_CMPIDateTime_fromChars ( const char *,
						   CMPIStatus * );
CMPISelectExp * native_new_CMPISelectExp ( const char *,
					   const char *,
					   CMPIArray **,
					   CMPIStatus * );
CMPIContext * native_new_CMPIContext ( int mem_state );
void native_release_CMPIContext ( CONST CMPIContext * );

extern CMPIBrokerExtFT *CMPI_BrokerExt_Ftab;

/****************************************************************************/

extern CMPIBrokerEncFT  native_brokerEncFT;
extern struct native_propertyFT propertyFT;

#endif

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
