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

Exception::Exception(const char* message) : _message(message)
{
}

Exception::~Exception()
{
}


const char OutOfBounds::MSG[] = "out of bounds";

const char AlreadyExists::MSG[] = "already exists: ";

const char IllegalName::MSG[] = "illegal CIM name";

const char IllegalNamespaceName::MSG[] = "illegal CIM namespace name";

const char UninitializedHandle::MSG[] = "uninitialized handle";

const char UninitializedObject::MSG[] = "uninitialized object";

const char AddedReferenceToClass::MSG[] =
    "attempted to add reference to a non-association class: ";

const char TruncatedCharacter::MSG[] =
    "truncated character during conversion from Char16 to char";

const char ExpectedReferenceValue::MSG[] =
    "Expected CIMValue object to be of type reference "
    "in this context";

const char MissingReferenceClassName::MSG[] = "missing reference class name";

const char TypeMismatch::MSG[] = "type mismatch";

const char CIMValueInvalidType::MSG[] = "invalid CIMValue type";

const char DynamicCastFailed::MSG[] = "dynamic cast failed";

const char BadDateTimeFormat::MSG[] = "bad datetime format";

const char IncompatibleTypes::MSG[] = "incompatible types";

const char IllformedObjectName::MSG[] = "illformed object name: ";

const char BindFailedException::MSG[] = "Bind failed: ";

const char InvalidLocatorException::MSG[] = "Invalid locator: ";

const char CannotCreateSocketException::MSG[] = "Cannot create socket";

const char CannotConnectException::MSG[] = "Cannot connect to: ";

const char UnexpectedFailureException::MSG[] = "Unexpected failure";

const char AlreadyConnectedException::MSG[] = "already connected";

const char NotConnectedException::MSG[] = "not connected";

const char ConnectionTimeoutException::MSG[] = "connection timed out";

const char SSLException::MSG[] = "SSL Exception ";


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
