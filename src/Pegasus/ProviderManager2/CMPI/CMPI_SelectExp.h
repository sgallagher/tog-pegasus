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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _CMPI_SelectExp_H_
#define _CMPI_SelectExp_H_

#include "CMPI_SelectCond.h"
#include "CMPI_Query2Dnf.h"
#include "CMPI_Wql2Dnf.h"
#include "CMPI_Cql2Dnf.h"
#include "CMPI_Object.h"
#include "CMPI_Ftabs.h"

#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>

#include <Pegasus/ProviderManager2/CMPI/Linkage.h>
PEGASUS_NAMESPACE_BEGIN

struct PEGASUS_CMPIPM_LINKAGE CMPI_SelectExp : CMPISelectExp {
   CMPI_SelectExp(const OperationContext& ct, QueryContext *context, String cond_, String lang_);
   CMPI_SelectExp(WQLSelectStatement*);
   CMPI_SelectExp(CQLSelectStatement*);

   CMPI_Object *next,*prev;
   const char **props;
   Array<CIMObjectPath> classNames;
   const OperationContext ctx;
   SubscriptionFilterConditionContainer* fcc;
   String cond,lang;
   CMPI_Wql2Dnf *wql_dnf;
   CMPI_Cql2Dnf *cql_dnf;
   CMPI_Tableau *tableau;
   WQLSelectStatement *wql_stmt;
   CQLSelectStatement *cql_stmt;
   QueryContext *_context;
};

PEGASUS_NAMESPACE_END

#endif
