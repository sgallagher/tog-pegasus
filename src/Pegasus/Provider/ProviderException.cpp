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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "ProviderException.h"

// l10n - added MessagesParms

PEGASUS_NAMESPACE_BEGIN

CIMOperationFailedException::CIMOperationFailedException(const String & message)
    : CIMException(CIM_ERR_FAILED, message)
{
}

CIMOperationFailedException::CIMOperationFailedException(const MessageLoaderParms & parms)
    : CIMException(CIM_ERR_FAILED, parms)
{
}

CIMOperationFailedException::CIMOperationFailedException(
    const CIMStatusCode code,
    const String & message)
    : CIMException(code, message)
{
}

CIMOperationFailedException::CIMOperationFailedException(
    const CIMStatusCode code,
    const MessageLoaderParms & parms)
    : CIMException(code, parms)
{
}

CIMAccessDeniedException::CIMAccessDeniedException(const String & message)
    : CIMOperationFailedException(CIM_ERR_ACCESS_DENIED, message)
{
}

CIMAccessDeniedException::CIMAccessDeniedException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_ACCESS_DENIED, parms)
{
}

CIMInvalidParameterException::CIMInvalidParameterException(
    const String & message)
    : CIMOperationFailedException(CIM_ERR_INVALID_PARAMETER,  message)
{
}

CIMInvalidParameterException::CIMInvalidParameterException(
    const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_INVALID_PARAMETER,  parms)
{
}

#if 0
CIMInvalidClassException::CIMInvalidClassException(const String & message)
    : CIMOperationFailedException(CIM_ERR_INVALID_CLASS,  message)
{
}

CIMInvalidClassException::CIMInvalidClassException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_INVALID_CLASS,  parms)
{
}
#endif

CIMObjectNotFoundException::CIMObjectNotFoundException(const String & message)
    : CIMOperationFailedException(CIM_ERR_NOT_FOUND, message)
{
}

CIMObjectNotFoundException::CIMObjectNotFoundException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_NOT_FOUND, parms)
{
}

CIMNotSupportedException::CIMNotSupportedException(const String & message)
    : CIMOperationFailedException(CIM_ERR_NOT_SUPPORTED, message)
{
}

CIMNotSupportedException::CIMNotSupportedException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_NOT_SUPPORTED, parms)
{
}

CIMObjectAlreadyExistsException::CIMObjectAlreadyExistsException(
    const String & message)
    : CIMOperationFailedException(CIM_ERR_ALREADY_EXISTS, message)
{
}

CIMObjectAlreadyExistsException::CIMObjectAlreadyExistsException(
    const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_ALREADY_EXISTS, parms)
{
}

CIMPropertyNotFoundException::CIMPropertyNotFoundException(const String & message)
    : CIMOperationFailedException(CIM_ERR_NO_SUCH_PROPERTY, message)
{
}

CIMPropertyNotFoundException::CIMPropertyNotFoundException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_NO_SUCH_PROPERTY, parms)
{
}

#if 0
// Query operations are not yet supported in Pegasus
CIMInvalidQueryException::CIMInvalidQueryException(const String & message)
    : CIMOperationFailedException(CIM_ERR_INVALID_QUERY, message)
{
}

CIMInvalidQueryException::CIMInvalidQueryException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_INVALID_QUERY, parms)
{
}
#endif

CIMMethodNotFoundException::CIMMethodNotFoundException(const String & message)
    : CIMOperationFailedException(CIM_ERR_METHOD_NOT_FOUND, message)
{
}

CIMMethodNotFoundException::CIMMethodNotFoundException(const MessageLoaderParms & parms)
    : CIMOperationFailedException(CIM_ERR_METHOD_NOT_FOUND, parms)
{
}

PEGASUS_NAMESPACE_END
