//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: Exception.cpp,v $
// Revision 1.9  2001/04/25 20:43:31  mike
// better
//
// Revision 1.8  2001/03/05 04:29:01  mike
// renamed CimException to CIMException
//
// Revision 1.7  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.6  2001/02/11 05:42:33  mike
// new
//
// Revision 1.5  2001/01/29 02:23:44  mike
// Added support for GetInstance operation
//
// Revision 1.4  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.3  2001/01/23 01:25:35  mike
// Reworked resolve scheme.
//
// Revision 1.2  2001/01/22 00:45:47  mike
// more work on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:51:33  mike
// Pegasus import
//
//
//END_HISTORY

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

const char UnitializedHandle::MSG[] = "unitialized reference";

const char IllegalName::MSG[] = "illegal CIM name";

const char NoSuchSuperClass::MSG[] = "no such super class: ";

const char NoSuchClass::MSG[] = "no such class: ";

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

const char NoSuchDirectory::MSG[] = "no such directory: ";

const char ChangeDirectoryFailed::MSG[] = "failed to change directory: ";

const char CannotCreateDirectory::MSG[] = "cannot create directory: ";

const char NoSuchNameSpace::MSG[] = "no such namespace: ";

const char CannotOpenFile::MSG[] = "cannot open file: ";

const char NotImplemented::MSG[] = "not implemented: ";

const char FailedToRemoveDirectory::MSG[] = "failed to remove directory: ";

const char FailedToRemoveFile::MSG[] = "failed to remove file: ";

const char StackUnderflow::MSG[] = "stack overflow";

const char BadFormat::MSG[] = "bad format passed to Formatter::format()";

const char BadDateTimeFormat::MSG[] = "bad datetime format";

const char IncompatibleTypes::MSG[] = "incompatible types";

const char BadlyFormedCGIQueryString::MSG[] = "badly formed CGI query string";

const char IllformedObjectPath::MSG[] = "illformed object path: ";

const char DynamicLoadFailed::MSG[] = "load of dynamic library failed: ";

const char DynamicLookupFailed::MSG[] = 
    "lookup of symbol in dynamic library failed: ";

const char CannotOpenDirectory::MSG[] = "cannot open directory: ";

////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

static char* _cimMessages[] =
{
    "successful",

    "A general error occurred that is not covered by a more specific "
    "error code.",

    "Access to a CIM resource was not available to the client.",

    "The target namespace does not exist.",

    "One or more parameter values passed to the method were invalid.",

    "The specified class does not exist.",

    "The requested object could not be found.",

    "The requested operation is not supported.",

    "Operation cannot be carried out on this class since it has subclasses.",

    "Operation cannot be carried out on this class since it has instances.",

    "Operation cannot be carried out since the specified "
    "superClass does not exist.",

    "Operation cannot be carried out because an object already exists.",

    "The specified property does not exist.",

    "The value supplied is incompatible with the type.",

    "The query language is not recognized or supported.",

    "The query is not valid for the specified query language.",

    "The extrinsic method could not be executed.",

    "The specified extrinsic method does not exist."
};

static String _makeCIMExceptionMessage(
    CIMException::Code code, 
    const String& extraMessage) 
{
    String tmp = _cimMessages[Uint32(code)];
    tmp.append(": ");
    tmp.append(extraMessage);
    return tmp;
}

CIMException::CIMException(
    CIMException::Code code, 
    const String& extraMessage) 
    : Exception(_makeCIMExceptionMessage(code, extraMessage)), _code(code)
{

}

const char* CIMException::codeToString(CIMException::Code code)
{
    return _cimMessages[Uint32(code)];
}

PEGASUS_NAMESPACE_END
