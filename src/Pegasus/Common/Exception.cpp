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
// Modified By: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Jenny Yu (jenny_yu@am.exch.hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "Exception.h"
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

AssertionFailureException::AssertionFailureException(
    const char* file,
    size_t line,
    const String& message) : Exception(String())
{
    char lineStr[32];
    sprintf(lineStr, "%u", line);

    _message = file;
    _message.append("(");
    _message.append(lineStr);
    _message.append("): ");
    _message.append(message);

    // ATTN-RK-P3-20020408: Should define a "test" trace component
    PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2, _message);
}

const char OutOfBounds::MSG[] = "out of bounds";

const char AlreadyExists::MSG[] = "already exists: ";

const char NullPointer::MSG[] = "null pointer";

const char UninitializedHandle::MSG[] = "uninitialized handle";

const char IllegalName::MSG[] = "illegal CIM name";

const char IllegalNamespaceName::MSG[] = "illegal CIM namespace name";

const char InvalidPropertyOverride::MSG[] = "invalid property override: ";

const char InvalidMethodOverride::MSG[] = "invalid method override: ";

const char UndeclaredQualifier::MSG[] = "undeclared qualifier: ";

const char BadQualifierScope::MSG[] = "qualifier invalid in this scope: ";

const char BadQualifierOverride::MSG[] = "qualifier not overridable: ";

const char BadQualifierType::MSG[] =
    "CIMType of qualifier different than its declaration: ";

const char NullType::MSG[] = "type is null";

const char AddedReferenceToClass::MSG[] =
    "attempted to add reference to a non-association class: ";

const char ClassAlreadyResolved::MSG[] =
    "attempt to resolve a class that is already resolved: ";

const char ClassNotResolved::MSG[] =
    "class is not yet resolved: ";

const char InstanceAlreadyResolved::MSG[] =
    "attempted to resolve a instance that is already resolved";

const char InstantiatedAbstractClass::MSG[] =
    "attempted to instantiate an abstract class ";

const char NoSuchProperty::MSG[] = "no such property: ";

const char TruncatedCharacter::MSG[] =
    "truncated character during conversion from Char16 to char";

const char ExpectedReferenceValue::MSG[] =
    "Expected CIMValue object to be of type reference "
    "in this context";

const char MissingReferenceClassName::MSG[] = "missing reference class name";

const char IllegalTypeTag::MSG[] = "illegal type tag";

const char TypeMismatch::MSG[] = "type mismatch";

const char CIMValueIsNull::MSG[] = "null CIMValue accessed";

const char CIMValueInvalidType::MSG[] = "invalid CIMValue type";

const char DynamicCastFailed::MSG[] = "dynamic cast failed";

const char NoSuchFile::MSG[] = "no such file: ";

const char FileNotReadable::MSG[] = "file not readable: ";

const char CannotBindToAddress::MSG[] = "cannot bind to address: ";

const char NoSuchDirectory::MSG[] = "no such directory: ";

const char ChangeDirectoryFailed::MSG[] = "cannot change directory: ";

const char CannotCreateDirectory::MSG[] = "cannot create directory: ";

const char NoSuchNameSpace::MSG[] = "no such namespace: ";

const char CannotOpenFile::MSG[] = "cannot open file: ";

const char NotImplemented::MSG[] = "not implemented: ";

const char CannotRemoveDirectory::MSG[] = "cannot remove directory: ";

const char CannotRemoveFile::MSG[] = "cannot remove file: ";

const char CannotRenameFile::MSG[] = "cannot rename file: ";

const char StackUnderflow::MSG[] = "stack underflow";

const char StackOverflow::MSG[] = "stack overflow";

const char QueueUnderflow::MSG[] = "queue Underflow";

const char BadFormat::MSG[] = "bad format passed to Formatter::format()";

const char BadDateTimeFormat::MSG[] = "bad datetime format";

const char IncompatibleTypes::MSG[] = "incompatible types";

const char BadlyFormedCGIQueryString::MSG[] = "badly formed CGI query string";

const char IllformedObjectName::MSG[] = "illformed object name: ";

const char DynamicLoadFailed::MSG[] = "load of dynamic library failed: ";

const char DynamicLookupFailed::MSG[] =
    "lookup of symbol in dynamic library failed: ";

const char CannotOpenDirectory::MSG[] = "cannot open directory: ";

const char CorruptFile::MSG[] = "corrupt file: ";

const char BindFailed::MSG[] = "Bind failed: ";

const char InvalidLocator::MSG[] = "Invalid locator: ";

const char CannotCreateSocket::MSG[] = "Cannot create socket";

const char CannotConnect::MSG[] = "Cannot connect to: ";

const char UnexpectedFailure::MSG[] = "Unexpected failure";

const char AlreadyConnected::MSG[] = "already connected";

const char NotConnected::MSG[] = "not connected";

const char TimedOut::MSG[] = "timed out";

const char ParseError::MSG[] = "parse error: ";

const char MissingNullTerminator::MSG[] = "missing null terminator: ";

const char SSL_Exception::MSG[] = "SSL Exception ";

const char InvalidAuthHeader::MSG[] = "Invalid Authorization header";

const char UnauthorizedAccess::MSG[] = "Unauthorized access";


////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

//
// Creates a description without source file name and line number.
//
static String _makeCIMExceptionDescription(
    CIMStatusCode code,
    const String& message)
{
    String tmp;
    tmp.append(CIMStatusCodeToString(code));
    if (message != String::EMPTY)
    {
        tmp.append(": \"");
        tmp.append(message);
        tmp.append("\"");
    }
    return tmp;
}

//
// Creates a description with source file name and line number.
//
static String _makeCIMExceptionDescription(
    CIMStatusCode code,
    const String& message,
    const char* file,
    Uint32 line)
{
    String tmp = file;
    tmp.append("(");
    char buffer[32];
    sprintf(buffer, "%d", line);
    tmp.append(buffer);
    tmp.append("): ");
    tmp.append(_makeCIMExceptionDescription(code, message));
    return tmp;
}

CIMException::CIMException(
    CIMStatusCode code,
    const String& message,
    const char* file,
    Uint32 line)
    :
    Exception(message),
    _code(code),
    _file(file),
    _line(line)
{

}

//
// Returns a description string fit for human consumption
//
String CIMException::getDescription() const
{
#ifdef DEBUG_CIMEXCEPTION
    return getTraceDescription();
#else
    return _makeCIMExceptionDescription(_code, getMessage());
#endif
}

//
// Returns a description string with filename and line number information
// specifically for tracing.
//
String CIMException::getTraceDescription() const
{
    String traceDescription =
        _makeCIMExceptionDescription(_code, getMessage(), _file, _line);

    return traceDescription;
}

void ThrowUninitializedHandle()
{
    throw UninitializedHandle();
}

PEGASUS_NAMESPACE_END
