//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "CIMClientException.h"
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
//
// CIMClientMalformedHTTPException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientMalformedHTTPException::CIMClientMalformedHTTPException(
    const String& message)
    : Exception(message)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientHTTPErrorException
//
////////////////////////////////////////////////////////////////////////////////

static String _makeHTTPErrorMessage(
    Uint32 httpStatusCode, 
    const String& cimError,
    const String& pegasusError)
{
    String tmp = "HTTP Error (status code ";
    char buffer[32];
    sprintf(buffer, "%u", httpStatusCode);
    tmp.append(buffer);
    tmp.append(")");

    if ((cimError != String::EMPTY) || (pegasusError != String::EMPTY))
    {
        tmp.append(":");
        if (cimError != String::EMPTY)
        {
            tmp.append(" CIMError = \"");
            tmp.append(cimError);
            tmp.append("\"");
        }
        if (pegasusError != String::EMPTY)
        {
            tmp.append(" Detail = \"");
            tmp.append(pegasusError);
            tmp.append("\"");
        }
    }
    tmp.append(".");
    return tmp;
}

CIMClientHTTPErrorException::CIMClientHTTPErrorException(
    Uint32 httpStatusCode, 
    const String& cimError,
    const String& pegasusError)
    : 
    Exception(
        _makeHTTPErrorMessage(httpStatusCode, cimError, pegasusError)),
    _httpStatusCode(httpStatusCode),
    _cimError(cimError),
    _pegasusError(pegasusError)
{
}

CIMClientHTTPErrorException::CIMClientHTTPErrorException(
    const CIMClientHTTPErrorException& httpError)
    :
    Exception(httpError.getMessage()),
    _httpStatusCode(httpError._httpStatusCode),
    _cimError(httpError._cimError),
    _pegasusError(httpError._pegasusError)
{
}

Uint32 CIMClientHTTPErrorException::getCode() const
{
    return _httpStatusCode;
}

String CIMClientHTTPErrorException::getCIMError() const
{
    return _cimError;
}

String CIMClientHTTPErrorException::getPegasusError() const
{
    return _pegasusError;
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientXmlException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientXmlException::CIMClientXmlException(const String& message)
    : Exception(message)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientResponseException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientResponseException::CIMClientResponseException(const String& message)
    : Exception(message)
{ 
}

PEGASUS_NAMESPACE_END
