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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderException_h
#define Pegasus_ProviderException_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/**
Parent class for exceptions thrown by providers.

<p>The <tt>OperationFailure</tt> class is an exception class,
and the parent class from which exceptions that can be thrown
by providers are derived. It may also be thrown directly by
providers to signal a generic operation failure.</p>

<p>Providers do not throw every possible exception that clients
may received from the CIMOM. The exceptions which may be thrown
by providers are a subset of the possible exceptions, and are
described in their respective sections.</p>

<p>All of the provider exceptions accept a <tt>message</tt>
argument that allows the provider to send additional text
in the string that will be returned to the client. While
localization of text is not currently supported, it is
recommended that text strings be structured in message
catalogs to facilitate future localization.</p>
*/
class OperationFailure : public CIMException
{
public:
    /**
    Generic operation failure.
    
    <p>This exception will cause a <tt>CIM_ERR_FAILED</tt>
    status code to be returned to the client.</p>
    */
    OperationFailure(const String & message) : CIMException(CIM_ERR_FAILED, message)
    {
    }

protected:
    OperationFailure(const CIMStatusCode code, const String & message) : CIMException(code, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_NOT_SUPPORTED</tt> status code to be
returned to the client.
*/
class NotSupported : public OperationFailure
{
public:
    NotSupported(const String & message) : OperationFailure(CIM_ERR_NOT_SUPPORTED, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_ACCESS_DENIED</tt> status code to be
returned to the client.
*/
class AccessDenied : public OperationFailure
{
public:
    AccessDenied(const String & message) : OperationFailure(CIM_ERR_ACCESS_DENIED, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_INVALID_PARAMETER</tt> status code to be
returned to the client.
*/
class InvalidParameter : public OperationFailure
{
public:
    InvalidParameter(const String & message) : OperationFailure(CIM_ERR_INVALID_PARAMETER,  message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_INVALID_CLASS</tt> status code to be
returned to the client.
*/
class InvalidClass : public OperationFailure
{
public:
    InvalidClass(const String & message) : OperationFailure(CIM_ERR_INVALID_CLASS,  message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_INVALID_QUERY</tt> status code to be
returned to the client.
*/
class InvalidQuery : public OperationFailure
{
public:
    InvalidQuery(const String & message) : OperationFailure(CIM_ERR_INVALID_QUERY, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_ALREADY_EXISTS</tt> status code to be
returned to the client.
*/
class ObjectAlreadyExists : public OperationFailure
{
public:
    ObjectAlreadyExists(const String & message) : OperationFailure(CIM_ERR_ALREADY_EXISTS, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_NOT_FOUND</tt> status code to be
returned to the client.
*/
class ObjectNotFound : public OperationFailure
{
public:
    ObjectNotFound(const String & message) : OperationFailure(CIM_ERR_NOT_FOUND, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_METHOD_NOT_FOUND</tt> status code to be
returned to the client.
*/
class MethodNotFound : public OperationFailure
{
public:
    MethodNotFound(const String & message) : OperationFailure(CIM_ERR_METHOD_NOT_FOUND, message)
    {
    }
};

/**
Cause a <tt>CIM_ERR_NO_SUCH_PROPERTY</tt> status code to be
returned to the client.
*/
class PropertyNotFound : public OperationFailure
{
public:
    PropertyNotFound(const String & message) : OperationFailure(CIM_ERR_NO_SUCH_PROPERTY, message)
    {
    }
};

PEGASUS_NAMESPACE_END

#endif
