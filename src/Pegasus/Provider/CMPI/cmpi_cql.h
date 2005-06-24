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

#ifndef _CMPICQL_H
#define _CMPICQL_H

#include "cmpift.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef PEGASUS_USE_EXPERIMENTAL
/*
This function shall return a new CMPISelectExp object.

The mb argument points to a CMPIBroker object.

The query argument is a pointer to a string
containing the select expression.

The lang argument is a pointer to a string
containing the query language.

The projection output argument is a pointer to a
CMPIArray structure of CMPIString entries containing
projection specification. It shall be set to NULL if no
projection is defined. The projection specification is query
language specific. Hence the entries format of the
projection output array CMPIString might be different
depending on the query language. Be sure to check the lang
argument for the query language your provider will support. 


The rc output argument, if not NULL, is used to
return a CMPIStatus structure containing the service return
status.


RETURN VALUE

This  function shall return a pointer to a new CMPISelectExp
object. NULL shall be returned in case an error is detected.
The following CMPIrc codes shall be recognized: 

      CMPI_RC_OK Operation successful.


      CMPI_RC_ERR_QUERY_LANGUAGE_NOT_SUPPORTED lang is not
      supported.


      CMPI_RC_ERR_INVALID_QUERY The query expression is not
      valid.


      CMPI_IRC_INVALID_HANDLE The mb handle is invalid.
   */
   CMPISelectExp *CMPI_CQL_NewSelectExp
    (const CMPIBroker * mb, const char *query, const char *lang,
     const CMPIArray ** projection, CMPIStatus * rc)
  {
    return ((mb)->eft->newSelectExp ((CMPIBroker *)(mb),  (char *)(query),"CIMxCQL", (CMPIArray **)(projection), rc));
  }

#endif


#ifdef __cplusplus
};
#endif

#endif // _CMPICQL_H
