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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "Exception.h"
#include <Pegasus/Common/CIMExceptionRep.h>
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

Exception::Exception(const String& message) : _message(message)
{
}

Exception::~Exception()
{
}

const String& Exception::getMessage() const
{
    return _message;
}


IndexOutOfBoundsException::IndexOutOfBoundsException()
    : Exception("index out of bounds")
{
}

AlreadyExistsException::AlreadyExistsException(const String& message)
    : Exception("already exists: " + message)
{
}

InvalidNameException::InvalidNameException(const String& name)
    : Exception("invalid CIM name: " + name)
{
}

InvalidNamespaceNameException::InvalidNamespaceNameException(const String& name)
    : Exception("invalid CIM namespace name: " + name)
{
}

UninitializedHandleException::UninitializedHandleException()
    : Exception("uninitialized handle")
{
}

UninitializedObjectException::UninitializedObjectException()
    : Exception("uninitialized object")
{
}

ExpectedReferenceValueException::ExpectedReferenceValueException()
    : Exception("Expected CIMValue object to be of type reference "
                    "in this context")
{
}

MissingReferenceClassNameException::MissingReferenceClassNameException()
    : Exception("missing reference class name")
{
}

TypeMismatchException::TypeMismatchException()
    : Exception("type mismatch")
{
}

CIMValueInvalidTypeException::CIMValueInvalidTypeException()
    : Exception("invalid CIMValue type")
{
}

DynamicCastFailedException::DynamicCastFailedException()
    : Exception("dynamic cast failed")
{
}

InvalidDateTimeFormatException::InvalidDateTimeFormatException()
    : Exception("invalid datetime format")
{
}

IncompatibleTypesException::IncompatibleTypesException()
    : Exception("incompatible types")
{
}

MalformedObjectNameException::MalformedObjectNameException(
    const String& objectName)
    : Exception("malformed object name: " + objectName)
{
}

BindFailedException::BindFailedException(const String& message)
    : Exception("Bind failed: " + message)
{
}

InvalidLocatorException::InvalidLocatorException(const String& locator)
    : Exception("Invalid locator: " + locator)
{
}

CannotCreateSocketException::CannotCreateSocketException()
    : Exception("Cannot create socket")
{
}

CannotConnectException::CannotConnectException(const String& locator)
    : Exception("Cannot connect to: " + locator)
{
}

UnexpectedFailureException::UnexpectedFailureException()
    : Exception("Unexpected failure")
{
}

AlreadyConnectedException::AlreadyConnectedException()
    : Exception("already connected")
{
}

NotConnectedException::NotConnectedException()
    : Exception("not connected")
{
}

ConnectionTimeoutException::ConnectionTimeoutException()
    : Exception("connection timed out")
{
}

SSLException::SSLException(const String& message)
    : Exception("SSL Exception: " + message)
{
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

CIMException::CIMException(
    CIMStatusCode code,
    const String& message)
    :
    Exception(message)
{
    _rep = new CIMExceptionRep();
    _rep->code = code;
    _rep->file = "";
    _rep->line = 0;
}

CIMException::CIMException(const CIMException & cimException)
    : Exception(cimException.getMessage())
{
    _rep = new CIMExceptionRep();
    _rep->code = cimException._rep->code;
    _rep->file = cimException._rep->file;
    _rep->line = cimException._rep->line;
}

CIMException& CIMException::operator=(const CIMException & cimException)
{
    _message = cimException._message;
    _rep->code = cimException._rep->code;
    _rep->file = cimException._rep->file;
    _rep->line = cimException._rep->line;
    return *this;
}

CIMException::~CIMException()
{
    delete _rep;
}

CIMStatusCode CIMException::getCode() const
{
    return _rep->code;
}

PEGASUS_NAMESPACE_END
