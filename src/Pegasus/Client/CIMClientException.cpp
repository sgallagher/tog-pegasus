//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// CIMClientException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientException::CIMClientException(const String& message)
    : _message(message)
{ 
}

CIMClientException::CIMClientException(const char* message)
    : _message(message)
{
}

CIMClientException::~CIMClientException()
{
}

const String& CIMClientException::getMessage() const
{
    return _message;
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientConnectionException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientConnectionException::CIMClientConnectionException(
    const String& message)
    : CIMClientException(message)
{ 
}

CIMClientCannotConnectException::CIMClientCannotConnectException(
    const String& message)
    : CIMClientConnectionException(message)
{ 
}

CIMClientCannotCreateSocketException::CIMClientCannotCreateSocketException(
    const String& message)
    : CIMClientConnectionException(message)
{ 
}

CIMClientBindFailedException::CIMClientBindFailedException(
    const String& message)
    : CIMClientConnectionException(message)
{ 
}

CIMClientInvalidLocatorException::CIMClientInvalidLocatorException(
    const String& message)
    : CIMClientConnectionException(message)
{ 
}

const char CIMClientAlreadyConnectedException::MSG[] =
    "CIMClient is already connected.";

CIMClientAlreadyConnectedException::CIMClientAlreadyConnectedException()
    : CIMClientConnectionException(MSG)
{ 
}

const char CIMClientNotConnectedException::MSG[] =
    "CIMClient is not connected.";

CIMClientNotConnectedException::CIMClientNotConnectedException()
    : CIMClientConnectionException(MSG)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientTimeoutException
//
////////////////////////////////////////////////////////////////////////////////

const char CIMClientTimeoutException::MSG[] =
    "Timed out waiting for CIM response.";

CIMClientTimeoutException::CIMClientTimeoutException()
    : CIMClientException(MSG)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientMalformedHTTPException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientMalformedHTTPException::CIMClientMalformedHTTPException(
    const String& message)
    : CIMClientException(message)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientHTTPError
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

CIMClientHTTPError::CIMClientHTTPError(
    Uint32 httpStatusCode, 
    const String& cimError,
    const String& pegasusError)
    : 
    CIMClientException(
        _makeHTTPErrorMessage(httpStatusCode, cimError, pegasusError)),
    _httpStatusCode(httpStatusCode),
    _cimError(cimError),
    _pegasusError(pegasusError)
{
}

CIMClientHTTPError::CIMClientHTTPError(const CIMClientHTTPError& httpError)
    :
    CIMClientException(httpError.getMessage()),
    _httpStatusCode(httpError._httpStatusCode),
    _cimError(httpError._cimError),
    _pegasusError(httpError._pegasusError)
{
}

Uint32 CIMClientHTTPError::getCode() const
{
    return _httpStatusCode;
}

String CIMClientHTTPError::getCIMError() const
{
    return _cimError;
}

String CIMClientHTTPError::getPegasusError() const
{
    return _pegasusError;
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientXmlException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientXmlException::CIMClientXmlException(const String& message)
    : CIMClientException(message)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientResponseException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientResponseException::CIMClientResponseException(const String& message)
    : CIMClientException(message)
{ 
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMClientCIMException
//
////////////////////////////////////////////////////////////////////////////////

CIMClientCIMException::CIMClientCIMException(
    CIMStatusCode code, 
    const String& message)
    : 
    CIMClientException(message),
    _code(code)
{
}

CIMClientCIMException::CIMClientCIMException(
    const CIMClientCIMException& cimException)
    :
    CIMClientException(cimException.getMessage()),
    _code(cimException._code)
{
}

CIMStatusCode CIMClientCIMException::getCode() const
{
    return _code;
}

PEGASUS_NAMESPACE_END
