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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "Exception.h"

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
    sprintf(lineStr, "%d", line);

    _message = file;
    _message.append("(");
    _message.append(lineStr);
    _message.append("): ");
    _message.append(message);
}

const char OutOfBounds::MSG[] = "out of bounds";

const char AlreadyExists::MSG[] = "already exists: ";

const char NullPointer::MSG[] = "null pointer";

const char UnitializedHandle::MSG[] = "unitialized handle";

const char IllegalName::MSG[] = "illegal CIM name";

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
    "attempted to instantiated an abstract class";

const char NoSuchProperty::MSG[] = "no such property: ";

const char TruncatedCharacter::MSG[] = 
    "truncated character during conversion from Char16 to char";

const char ExpectedReferenceValue::MSG[] = 
    "Expected CIMValue object to be CIMType::REFERENCE or CIMType::REFERENCE_ARRAY "
    "in this context";

const char MissingReferenceClassName::MSG[] = "missing reference class name";

const char IllegalTypeTag::MSG[] = "illegal type tag";

const char TypeMismatch::MSG[] = "type mismatch";

const char NoSuchFile::MSG[] = "no such file: ";

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

const char StackUnderflow::MSG[] = "stack overflow";

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

////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

static const char* _cimMessages[] =
{
    "SUCCESS: successful",

    "FAILED: A general error occurred that is not covered by a more specific "
    "error code",

    "ACCESS_DENIED: Access to a CIM resource was not available to the client",

    "INVALID_NAMESPACE: The target namespace does not exist",

    "INVALID_PARAMETER: One or more parameter values passed to the method "
    "were invalid",

    "INVALID_CLASS: The specified class does not exist",

    "NOT_FOUND: The requested object could not be found",

    "NOT_SUPPORTED: The requested operation is not supported",

    "CLASS_HAS_CHILDREN: Operation cannot be carried out on this class since "
    "it has subclasses",

    "CLASS_HAS_INSTANCES: Operation cannot be carried out on this class since "
    "it has instances",

    "INVALID_SUPERCLASS: Operation cannot be carried out since the specified "
    "superclass does not exist",

    "ALREADY_EXISTS: Operation cannot be carried out because an object already "
    "exists",

    "NO_SUCH_PROPERTY: The specified property does not exist",

    "TYPE_MISMATCH: The value supplied is incompatible with the type",

    "QUERY_LANGUAGE_NOT_SUPPORTED: The query language is not recognized or "
    "supported",

    "INVALID_QUERY: The query is not valid for the specified query language",

    "METHOD_NOT_AVAILABLE: The extrinsic method could not be executed",

    "METHOD_NOT_FOUND: The specified extrinsic method does not exist"
};

static String _makeCIMExceptionMessage(
    CIMException::Code code, 
    const char* file,
    Uint32 line,
    const String& extraMessage)
{
    String tmp = file;
    tmp.append("(");
    char buffer[32];
    sprintf(buffer, "%d", line);
    tmp.append(buffer);
    tmp.append("): ");

    tmp.append(_cimMessages[Uint32(code)]);
    tmp.append(": \"");
    tmp.append(extraMessage);
    tmp.append("\"");
    return tmp;
}

CIMException::CIMException(
    CIMException::Code code, 
    const char* file,
    Uint32 line,
    const String& extraMessage)
    : Exception(_makeCIMExceptionMessage(code, file, line, extraMessage)),
    _code(code)
{

}

const char* CIMException::codeToString(CIMException::Code code)
{
    return _cimMessages[Uint32(code)];
}

void ThrowUnitializedHandle()
{
    throw UnitializedHandle();
}

PEGASUS_NAMESPACE_END
