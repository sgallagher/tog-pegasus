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
#include <Pegasus/Common/ExceptionRep.h>
#include <Pegasus/Common/CIMExceptionRep.h>
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

Exception::Exception(const String& message)
{
    _rep = new ExceptionRep();
    _rep->message = message;
}

Exception::Exception(const Exception& exception)
{
    _rep = new ExceptionRep();
    _rep->message = exception._rep->message;
}

Exception::Exception()
{
}

Exception::~Exception()
{
    delete _rep;
}

const String& Exception::getMessage() const
{
    return _rep->message;
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

UninitializedObjectException::UninitializedObjectException()
    : Exception("uninitialized object")
{
}

TypeMismatchException::TypeMismatchException()
    : Exception("type mismatch")
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
{
    CIMExceptionRep * tmp = new CIMExceptionRep ();
    tmp->message = message;
    tmp->code = code;
    tmp->file = "";
    tmp->line = 0;
    _rep = tmp;
}

CIMException::CIMException(const CIMException & cimException)
{
    CIMExceptionRep * tmp = new CIMExceptionRep ();
    CIMExceptionRep * rep;
    rep = reinterpret_cast<CIMExceptionRep*>(cimException._rep);
    tmp->message = rep->message;
    tmp->code = rep->code;
    tmp->file = rep->file;
    tmp->line = rep->line;
    _rep = tmp;
}

CIMException& CIMException::operator=(const CIMException & cimException)
{
    CIMExceptionRep* left;
    CIMExceptionRep* right;
    left = reinterpret_cast<CIMExceptionRep*>(this->_rep);
    right = reinterpret_cast<CIMExceptionRep*>(cimException._rep);
    left->message = right->message;
    left->code = right->code;
    left->file = right->file;
    left->line = right->line;
    return *this;
}

CIMException::~CIMException()
{
}

CIMStatusCode CIMException::getCode() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->code;
}

PEGASUS_NAMESPACE_END
