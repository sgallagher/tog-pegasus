//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

#include "QuerySupportRouter.h"

#include <Pegasus/Server/WQLOperationRequestDispatcher.h>
#include <Pegasus/Server/CQLOperationRequestDispatcher.h>

PEGASUS_NAMESPACE_BEGIN

// Call the appropriate handleQueryRequest handler for the defined
// query language type or return false if the language type not supported.
Boolean QuerySupportRouter::routeHandleExecQueryRequest(
    CIMOperationRequestDispatcher* opThis,
    CIMExecQueryRequestMessage* msg)
{
    if (msg->queryLanguage=="WQL")
        ((WQLOperationRequestDispatcher*)opThis)->handleQueryRequest(msg);
#ifdef PEGASUS_ENABLE_CQL
    else if(msg->queryLanguage == "DMTF:CQL")
        ((CQLOperationRequestDispatcher*)opThis)->handleQueryRequest(msg);
#endif
    else
    {
        return false;
    }

    return true;
}

// Get pointer to the correct function.  Note that since
// the queryLanguage has already been tested in
// routeHandleExecQueryRequest the assert should be unreachable.

applyQueryFunctionPtr QuerySupportRouter::getFunctPtr(
   CIMOperationRequestDispatcher* opThis,
   QueryExpressionRep* query)
{
    if (query->getQueryLanguage() == "WQL")
    {
        return &((WQLOperationRequestDispatcher*)opThis)->
            applyQueryToEnumeration;
    }
#ifdef PEGASUS_ENABLE_CQL
    else if(query->getQueryLanguage() == "DMTF:CQL")
    {
        return &((CQLOperationRequestDispatcher*)opThis)->
            applyQueryToEnumeration;
    }
#endif
    else
    {
        PEGASUS_UNREACHABLE(PEGASUS_ASSERT(false);)
    }
}

PEGASUS_NAMESPACE_END
