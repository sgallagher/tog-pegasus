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
  \file broker.c
  \brief Native CMPI broker encapsulated functionality.

  This module implements a complete CMPI broker encapsulated function table
  (CMPIBrokerEncFT) natively. Thus, CMPI data types may be created remotely
  without the need to connect to the CIMOM.

  \author Frank Scheffler
*/

#include "tool.h"
#include "native.h"
#include "debug.h"


static CMPIInstance * __beft_newInstance ( CONST CMPIBroker * broker,
					   CONST CMPIObjectPath * cop,
					   CMPIStatus * rc )

{
	TRACE_NORMAL(("Creating new native CMPIInstance."));
	return native_new_CMPIInstance ( cop, rc );
}


static CMPIObjectPath * __beft_newObjectPath ( CONST CMPIBroker * broker,
					       const char * namespace,
					       const char * classname,
					       CMPIStatus * rc )

{
	TRACE_NORMAL(("Creating new native CMPIObjectPath."));
	return native_new_CMPIObjectPath ( namespace, classname, rc );
}



static CMPIArgs * __beft_newArgs ( CONST CMPIBroker * broker, CMPIStatus * rc )
{
	TRACE_NORMAL(("Creating new native CMPIArgs."));
	return native_new_CMPIArgs ( rc );
}


static CMPIString * __beft_newString ( CONST CMPIBroker * broker,
				       const char * str,
				       CMPIStatus * rc )

{
	TRACE_NORMAL(("Creating new native CMPIString."));
	TRACE_INFO(("String: %s", str ));
	return native_new_CMPIString ( str, rc );
}


static CMPIArray * __beft_newArray ( CONST CMPIBroker * broker,
				     CMPICount size,
				     CMPIType type,
				     CMPIStatus * rc )
{
	TRACE_NORMAL(("Creating new native CMPIArray."));
	TRACE_INFO(("type: 0x%x\nsize: %d", type, size ));
	return native_new_CMPIArray ( size, type, rc );
}



static CMPIDateTime * __beft_newDateTime ( CONST CMPIBroker * broker,
					   CMPIStatus * rc )
{
	TRACE_NORMAL(("Creating new native CMPIDateTime."));
	return native_new_CMPIDateTime ( rc );
}


static CMPIDateTime * __beft_newDateTimeFromBinary ( CONST CMPIBroker * broker,
						     CMPIUint64 time,
						     CMPIBoolean interval,
						     CMPIStatus * rc )

{
	TRACE_NORMAL(("Creating new native CMPIDateTime."));
#if defined CMPI_PLATFORM_WIN32_IX86_MSVC
	TRACE_INFO(("time: %I64\ninterval: %d", time, interval ));
#else
	TRACE_INFO(("time: %lld\ninterval: %d", time, interval ));
#endif
	return native_new_CMPIDateTime_fromBinary ( time, interval, rc );
}



static CMPIDateTime * __beft_newDateTimeFromChars ( CONST CMPIBroker * broker,
						    CONST char * string,
						    CMPIStatus * rc )
{
	TRACE_NORMAL(("Creating new native CMPIDateTime."));
	TRACE_INFO(("time: %s", string ));
	return native_new_CMPIDateTime_fromChars ( string, rc );
}



static CMPISelectExp * __beft_newSelectExp ( CONST CMPIBroker * broker,
					     const char * queryString,
					     const char * language,
					     CMPIArray ** projection,
					     CMPIStatus * rc )

{
	TRACE_NORMAL(("Creating new native CMPISelectExp."));
	return native_new_CMPISelectExp ( queryString,
					  language,
					  projection,
					  rc );
}

static CMPIBoolean __beft_classPathIsA ( CONST CMPIBroker * broker,
					 CONST CMPIObjectPath * cop,
					 const char * type,
					 CMPIStatus * rc )

{

     CMPIString *clsn;

     if (rc) CMSetStatus(rc,CMPI_RC_OK);

     clsn=CMGetClassName(cop,NULL);

     if (clsn && clsn->hdl) {
        if (strcasecmp(type,(char*)clsn->hdl)==0) return 1;
     }
     else return 0;

     return ((NativeCMPIBrokerFT*)(broker->bft))->classPathIsA(broker,cop,type,rc);
}

extern CMPIObjectPathFT *CMPI_ObjectPath_FT;
extern CMPIInstanceFT *CMPI_Instance_FT;


extern CMPIString *__oft_toString( CONST CMPIObjectPath * cop, CMPIStatus *rc);
extern CMPIString *instance2String( CONST CMPIInstance *inst, CMPIStatus *rc);

static CMPIString * __beft_toString ( CONST CMPIBroker * broker,
				      CONST void * object,
				      CMPIStatus * rc )


{
        if (object) {
	   if (((CMPIInstance*)object)->ft) {
	      if (((CMPIObjectPath*)object)->ft==CMPI_ObjectPath_FT)
	         return __oft_toString((CMPIObjectPath*)object,rc);
	      if (((CMPIInstance*)object)->ft==CMPI_Instance_FT)
	         return instance2String((CMPIInstance*)object,rc);
	   }
	}
	TRACE_CRITICAL(("This operation is not yet supported."));
	if ( rc ) CMSetStatus ( rc, CMPI_RC_ERR_NOT_SUPPORTED );
	return NULL;
}


static CMPIBoolean __beft_isOfType ( CONST CMPIBroker * broker,
				     CONST void * object,
				     const char * type,
				     CMPIStatus * rc )

{
	char * t = * ( (char **) object );

	TRACE_NORMAL(("Verifying encapsulated object type."));

	if ( rc ) CMSetStatus ( rc, CMPI_RC_OK );
	return ( strcmp ( t, type ) == 0 );
}


static CMPIString * __beft_getType ( CONST CMPIBroker * broker,
				     CONST void * object,
				     CMPIStatus * rc )

{
	TRACE_NORMAL(("Returning encapsulated object type."));
	return __beft_newString ( broker, *( (char **) object ), rc );
}




static CMPIString*  __beft_getMessage (CONST CMPIBroker* broker,
        const char *msgId, const char *defMsg, CMPIStatus* rc, unsigned int count, ...)

{
        CMPIStatus nrc;
	CMPIString *msg;
        va_list argptr;
        va_start(argptr,count);

        msg=((NativeCMPIBrokerFT*)(broker->bft))->getMessage(broker,msgId, defMsg,
	         &nrc, count,argptr);
	if (rc) *rc=nrc;
	return msg;
}

#if defined CMPI_VER_100

static CMPIStatus __beft_logMessage(const CMPIBroker*broker,
				    int severity ,
				    const char *id,
				    const char *text,
				    const CMPIString *string)
{

        return ((NativeCMPIBrokerFT*)(broker->bft))->logMessage(broker,severity, id, text, string);

}


static CMPIStatus __beft_traceMessage(const CMPIBroker* broker,
				      int level,
				      const char *component,
				      const char *text,
				      const CMPIString *string)
{
	return ((NativeCMPIBrokerFT*)(broker->bft))->trace(broker,level, component, text, string);

}
#else

static CMPIArray *__beft_getKeyNames(CMPIBroker * broker,
				     CMPIContext * context,
				     CMPIObjectPath * cop, CMPIStatus * rc)
{
     return ((NativeCMPIBrokerFT*)(broker->bft))->getKeyNames(broker,context,cop,rc);
}
#endif

/****************************************************************************/


CMPIBrokerEncFT native_brokerEncFT = {
	NATIVE_FT_VERSION,
	__beft_newInstance,
	__beft_newObjectPath,
	__beft_newArgs,
	__beft_newString,
	__beft_newArray,
	__beft_newDateTime,
	__beft_newDateTimeFromBinary,
	__beft_newDateTimeFromChars,
	__beft_newSelectExp,
	__beft_classPathIsA,
	__beft_toString,
	__beft_isOfType,
	__beft_getType,
	__beft_getMessage,
#ifdef CMPI_VER_100
	__beft_logMessage,
	__beft_traceMessage,
#else
	__beft_getKeyNames,
#endif
};

/****************************************************************************/

/*** Local Variables:  ***/
/*** mode: C           ***/
/*** c-basic-offset: 8 ***/
/*** End:              ***/
