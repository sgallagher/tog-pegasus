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
#include "InternalException.h"
#include <Pegasus/Common/CIMExceptionRep.h>
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

AssertionFailureException::AssertionFailureException(
    const char* file,
    size_t line,
    const String& message) : Exception(String::EMPTY)
{
    char lineStr[32];
    sprintf(lineStr, "%u", line);

    _rep->message = file;
    _rep->message.append("(");
    _rep->message.append(lineStr);
    _rep->message.append("): ");
    _rep->message.append(message);

    // ATTN-RK-P3-20020408: Should define a "test" trace component
    PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2, _rep->message);
}

const char NullPointer::MSG[] = "null pointer";

const char UndeclaredQualifier::MSG[] = "undeclared qualifier: ";

const char BadQualifierScope::MSG[] = "qualifier invalid in this scope: ";

const char BadQualifierOverride::MSG[] = "qualifier not overridable: ";

const char BadQualifierType::MSG[] =
    "CIMType of qualifier different than its declaration: ";

const char ClassAlreadyResolved::MSG[] =
    "attempt to resolve a class that is already resolved: ";

const char ClassNotResolved::MSG[] =
    "class is not yet resolved: ";

const char InstanceAlreadyResolved::MSG[] =
    "attempted to resolve a instance that is already resolved";

const char InstantiatedAbstractClass::MSG[] =
    "attempted to instantiate an abstract class ";

const char NoSuchProperty::MSG[] = "no such property: ";

const char NoSuchFile::MSG[] = "no such file: ";

const char FileNotReadable::MSG[] = "file not readable: ";

const char CannotRemoveDirectory::MSG[] = "cannot remove directory: ";

const char CannotRemoveFile::MSG[] = "cannot remove file: ";

const char CannotRenameFile::MSG[] = "cannot rename file: ";

const char NoSuchDirectory::MSG[] = "no such directory: ";

const char CannotCreateDirectory::MSG[] = "cannot create directory: ";

const char CannotOpenFile::MSG[] = "cannot open file: ";

const char NotImplemented::MSG[] = "not implemented: ";

const char StackUnderflow::MSG[] = "stack underflow";

const char StackOverflow::MSG[] = "stack overflow";

const char QueueUnderflow::MSG[] = "queue Underflow";

const char BadFormat::MSG[] = "bad format passed to Formatter::format()";

const char BadlyFormedCGIQueryString::MSG[] = "badly formed CGI query string";

const char DynamicLoadFailed::MSG[] = "load of dynamic library failed: ";

const char DynamicLookupFailed::MSG[] =
    "lookup of symbol in dynamic library failed: ";

const char CannotOpenDirectory::MSG[] = "cannot open directory: ";

const char ParseError::MSG[] = "parse error: ";

const char MissingNullTerminator::MSG[] = "missing null terminator: ";

const char InvalidAuthHeader::MSG[] = "Invalid Authorization header";

const char UnauthorizedAccess::MSG[] = "Unauthorized access";

IncompatibleTypesException::IncompatibleTypesException()
    : Exception("incompatible types")
{
}


////////////////////////////////////////////////////////////////////////////////
//
// TraceableCIMException
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
    tmp.append(cimStatusCodeToString(code));
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

TraceableCIMException::TraceableCIMException(
    CIMStatusCode code,
    const String& message,
    const char* file,
    Uint32 line)
    :
    CIMException(code, message)
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    rep->file = file;
    rep->line = line;
}

TraceableCIMException::TraceableCIMException(const CIMException & cimException)
    : CIMException(cimException.getCode(), cimException.getMessage())
{
    TraceableCIMException * t = (TraceableCIMException *)&cimException;
    CIMExceptionRep* left;
    CIMExceptionRep* right;
    left = reinterpret_cast<CIMExceptionRep*>(_rep);
    right = reinterpret_cast<CIMExceptionRep*>(t->_rep);
    left->file = right->file;
    left->line = right->line;
}

//
// Returns a description string fit for human consumption
//
String TraceableCIMException::getDescription() const
{
#ifdef PEGASUS_DEBUG_CIMEXCEPTION
    return getTraceDescription();
#else
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return _makeCIMExceptionDescription(rep->code, getMessage());
#endif
}

//
// Returns a description string with filename and line number information
// specifically for tracing.
//
String TraceableCIMException::getTraceDescription() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    String traceDescription =
        _makeCIMExceptionDescription(
            rep->code, getMessage(), rep->file, rep->line);

    return traceDescription;
}

PEGASUS_NAMESPACE_END
