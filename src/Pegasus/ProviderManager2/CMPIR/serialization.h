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
  \file serialization.h
  \brief Header file for the binary serializer component for Remote CMPI.

  This file defines a function table of methods to be used for serializing
  and deserializing CMPI data respectively. It covers most of the encapsulated
  data types, however, those which cannot be fully serialized or
  deserialized due to API restrictions are not included, e.g. CMPIResult,
  CMPIContext or CMPIEnumeration.

  \author Frank Scheffler
*/

#ifndef _SERIALIZATION_H
#define _SERIALIZATION_H

extern const struct BinarySerializerFT binarySerializerFT;

#ifndef CMPI_VER_100
#define CMPI_VER_100
#endif

#ifndef CONST 
  #ifdef CMPI_VER_100 
  #define CONST const
  #else
  #define CONST 
  #endif
#endif


#include <Pegasus/Provider/CMPI/cmpidt.h>

struct BinarySerializerFT {
	ssize_t (* serialize_UINT8) ( int, CMPIUint8 );
	CMPIUint8 (* deserialize_UINT8) ( int );
	
	ssize_t (* serialize_UINT16) ( int, CMPIUint16 );
	CMPIUint16 (* deserialize_UINT16) ( int );

	ssize_t (* serialize_UINT32) ( int, CMPIUint32 );
	CMPIUint32 (* deserialize_UINT32) ( int );

	ssize_t (* serialize_UINT64) ( int, CMPIUint64 );
	CMPIUint64 (* deserialize_UINT64) ( int );

	ssize_t (* serialize_CMPIValue) ( int, CONST CMPIType, CONST CMPIValue * );
	CMPIValue (* deserialize_CMPIValue) ( int, CMPIType, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIType) ( int, CMPIType );
	CMPIType (* deserialize_CMPIType) ( int );

	ssize_t (* serialize_CMPIData) ( int, CMPIData );
	CMPIData (* deserialize_CMPIData) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIStatus) ( int, CMPIStatus * );
	CMPIStatus (* deserialize_CMPIStatus) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_string) ( int, const char * );
	char * (* deserialize_string) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIString) ( int, CONST CMPIString * );
	CMPIString * (* deserialize_CMPIString) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIArgs) ( int, CONST CMPIArgs * );
	CMPIArgs * (* deserialize_CMPIArgs) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIObjectPath) ( int, CONST CMPIObjectPath * );
	CMPIObjectPath * (* deserialize_CMPIObjectPath) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIArray) ( int, CONST CMPIArray * );
	CMPIArray * (* deserialize_CMPIArray) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIInstance) ( int, CONST CMPIInstance * );
	CMPIInstance * (* deserialize_CMPIInstance) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPISelectExp) ( int, CONST CMPISelectExp * );
	CMPISelectExp * (* deserialize_CMPISelectExp) ( int, CONST CMPIBroker * );

	ssize_t (* serialize_CMPIDateTime) ( int, CMPIDateTime * );
	CMPIDateTime * (* deserialize_CMPIDateTime) ( int, CONST CMPIBroker * );
};

#endif

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
