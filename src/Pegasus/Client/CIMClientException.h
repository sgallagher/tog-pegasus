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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ClientExceptions_h
#define Pegasus_ClientExceptions_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This exception indicates that a CIM request was not processed successfully.
    All exceptions intentionally thrown by the CIMClient interface are of this
    type.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientException
{
public:

    CIMClientException(const String& message);

    CIMClientException(const char* message);

    ~CIMClientException();

    const String& getMessage() const;

protected:

    String _message;
};

/**
    An exception of this type indicates a problem with a connection to a CIM
    Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientConnectionException
    : public CIMClientException
{
public:
    CIMClientConnectionException(const String& message);
};

/**
    An exception of this type indicates a failure to connect to a CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientCannotConnectException
    : public CIMClientConnectionException
{
public:
    CIMClientCannotConnectException(const String& message);
};

/**
    An exception of this type indicates a failure to create a CIM Client
    socket.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientCannotCreateSocketException
    : public CIMClientConnectionException
{
public:
    CIMClientCannotCreateSocketException(const String& message);
};

/**
    An exception of this type indicates that the CIM Client could not bind the
    connection to a CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientBindFailedException
    : public CIMClientConnectionException
{
public:
    CIMClientBindFailedException(const String& message);
};

/**
    An exception of this type indicates that the CIM Server locator provided
    is invalid.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientInvalidLocatorException
    : public CIMClientConnectionException
{
public:
    CIMClientInvalidLocatorException(const String& message);
};

/**
    An exception of this type indicates that a connection to a CIM Server was
    attempted while the CIM Client was already connected to a CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientAlreadyConnectedException
    : public CIMClientConnectionException
{
public:
    CIMClientAlreadyConnectedException();

private:
    static const char MSG[];
};

/**
    An exception of this type indicates a CIM operation was attempted when
    the CIM Client was not connected to a CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientNotConnectedException
    : public CIMClientConnectionException
{
public:
    CIMClientNotConnectedException();

private:
    static const char MSG[];
};

/**
    An exception of this type indicates a failure to create a SSLContext
    by the CIM Client.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientCannotCreateSSLContextException
    : public CIMClientException
{
public:
    CIMClientCannotCreateSSLContextException(const String& message);
};

/**
    An exception of this type indicates that the configured timeout interval
    has been reached while waiting for a response from the CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientTimeoutException
    : public CIMClientException
{
public:
    CIMClientTimeoutException();

private:
    static const char MSG[];
};

/**
    An exception of this type indicates that the HTTP response from the CIM
    Server was improperly formed.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientMalformedHTTPException
    : public CIMClientException
{
public:
    CIMClientMalformedHTTPException(const String& message);
};

/**
    An exception of this type indicates that an HTTP error response was sent
    by the CIM Server.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientHTTPError : public CIMClientException
{
public:
    CIMClientHTTPError(
        Uint32 httpStatusCode,
        const String& cimError = String::EMPTY,
        const String& pegasusError = String::EMPTY);
    CIMClientHTTPError(const CIMClientHTTPError& httpError);

    Uint32 getCode() const;
    String getCIMError() const;
    String getPegasusError() const;

private:
    Uint32 _httpStatusCode;
    String _cimError;
    String _pegasusError;
};

/**
    An exception of this type indicates that the CIM response sent by the
    CIM Server could not be decoded from XML.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientXmlException : public CIMClientException
{
public:
    CIMClientXmlException(const String& message);
};

/**
    An exception of this type indicates that the CIM response sent by the
    CIM Server contained unexpected data.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientResponseException
    : public CIMClientException
{
public:
    CIMClientResponseException(const String& message);
};

/**
    An exception of this type indicates that the CIM Server processed the
    CIM request successfully, but the operation resulted in an error.
*/
class PEGASUS_CLIENT_LINKAGE CIMClientCIMException : public CIMClientException
{
public:
    CIMClientCIMException(
        CIMStatusCode code,
        const String& message);

    CIMClientCIMException(
        const CIMClientCIMException& cimException);

    CIMStatusCode getCode() const;

private:
    CIMStatusCode  _code;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClientExceptions_h */
