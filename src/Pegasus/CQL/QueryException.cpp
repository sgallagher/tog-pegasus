//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "QueryException.h"

PEGASUS_NAMESPACE_BEGIN

const char CQLChainedIdParseException::MSG[] = "Default Message: CQLChainedIdException";
const char CQLChainedIdParseException::KEY[] = "CQL.QueryException.CQL_CHAINED_ID_EXCEPTION";

const char CQLIdentifierParseException::MSG[] = "Default Message: CQLIdentifierException";
const char CQLIdentifierParseException::KEY[] = "CQL.QueryException.CQL_IDENTIFIER_EXCEPTION";

const char CQLValidationException::MSG[] = "Default Message: CQLValidationException";
const char CQLValidationException::KEY[] = "CQL.QueryException.CQL_VALIDATION_EXCEPTION";

const char CQLRuntimeException::MSG[] = "Default Message: CQLRuntimeException";
const char CQLRuntimeException::KEY[] = "CQL.QueryException.CQL_RUNTIME_EXCEPTION";

const char CQLSyntaxErrorException::MSG[] = "Default Message: CQLSyntaxErrorException: $0 in position $1";
const char CQLSyntaxErrorException::KEY[] = "CQL.QueryException.CQL_SYNTAX_ERROR_EXCEPTION";

const char CQLParseException::MSG[] = "Default Message: CQLParseException";
const char CQLParseException::KEY[] = "CQL.QueryException.CQL_PARSE_EXCEPTION";

const char QueryException::MSG[] = "Default Message: QueryException";
const char QueryException::KEY[] = "CQL.QueryException.QUERY_EXCEPTION";

PEGASUS_NAMESPACE_END
