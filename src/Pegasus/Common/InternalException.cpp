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
#include <Pegasus/Common/ContentLanguages.h>  // l10n
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

AssertionFailureException::AssertionFailureException(
    const char* file,
    size_t line,
    const String& message) : Exception(String::EMPTY)
{
    char lineStr[32];
    sprintf(lineStr, "%u", Uint32(line));

    _rep->message = file;
    _rep->message.append("(");
    _rep->message.append(lineStr);
    _rep->message.append("): ");
    _rep->message.append(message);

    // ATTN-RK-P3-20020408: Should define a "test" trace component
    PEG_TRACE_STRING(TRC_SERVER, Tracer::LEVEL2, _rep->message);
}

const char NullPointer::MSG[] = "null pointer";
const char NullPointer::KEY[] = "Common.InternalException.NullPointer";

const char UndeclaredQualifier::MSG[] = "undeclared qualifier: $0";
const char UndeclaredQualifier::KEY[] = "Common.InternalException.UndeclaredQualifier";

const char BadQualifierScope::MSG[] = "qualifier invalid in this scope: ";
const char BadQualifierScope::KEY[] = "Common.InternalException.BadQualifierScope";

const char BadQualifierOverride::MSG[] = "qualifier not overridable: ";
const char BadQualifierOverride::KEY[] = "Common.InternalException.BadQualifierOverride"; 

const char BadQualifierType::MSG[] = "CIMType of qualifier different than its declaration: ";
const char BadQualifierType::KEY[] = "Common.InternalException.BadQualifierType";

const char ClassAlreadyResolved::MSG[] = "attempt to resolve a class that is already resolved: ";
const char ClassAlreadyResolved::KEY[] = "Common.InternalException.ClassAlreadyResolved";

const char ClassNotResolved::MSG[] = "class is not yet resolved: ";
const char ClassNotResolved::KEY[] = "Common.InternalException.ClassNotResolved";

const char InstanceAlreadyResolved::MSG[] = "attempted to resolve a instance that is already resolved";
const char InstanceAlreadyResolved::KEY[] = "Common.InternalException.InstanceAlreadyResolved"; 

const char InstantiatedAbstractClass::MSG[] = "attempted to instantiate an abstract class ";
const char InstantiatedAbstractClass::KEY[] = "Common.InternalException.InstantiatedAbstractClass"; 

const char NoSuchProperty::MSG[] = "no such property: ";
const char NoSuchProperty::KEY[] = "Common.InternalException.NoSuchProperty";

const char NoSuchFile::MSG[] = "no such file: ";
const char NoSuchFile::KEY[] = "Common.InternalException.NoSuchFile";

const char FileNotReadable::MSG[] = "file not readable: ";
const char FileNotReadable::KEY[] = "Common.InternalException.FileNotReadable";

const char CannotRemoveDirectory::MSG[] = "cannot remove directory: ";
const char CannotRemoveDirectory::KEY[] = "Common.InternalException.CannotRemoveDirectory";  

const char CannotRemoveFile::MSG[] = "cannot remove file: ";
const char CannotRemoveFile::KEY[] = "Common.InternalException.CannotRemoveFile"; 

const char CannotRenameFile::MSG[] = "cannot rename file: ";
const char CannotRenameFile::KEY[] = "Common.InternalException.CannotRenameFile";

const char NoSuchDirectory::MSG[] = "no such directory: ";
const char NoSuchDirectory::KEY[] = "Common.InternalException.NoSuchDirectory";  

const char CannotCreateDirectory::MSG[] = "cannot create directory: ";
const char CannotCreateDirectory::KEY[] = "Common.InternalException.CannotCreateDirectory";

const char CannotOpenFile::MSG[] = "cannot open file: ";
const char CannotOpenFile::KEY[] = "Common.InternalException.CannotOpenFile";

const char NotImplemented::MSG[] = "not implemented: ";
const char NotImplemented::KEY[] = "Common.InternalException.NotImplemented"; 

const char StackUnderflow::MSG[] = "stack underflow";
const char StackUnderflow::KEY[] = "Common.InternalException.StackUnderflow";

const char StackOverflow::MSG[] = "stack overflow";
const char StackOverflow::KEY[] = "Common.InternalException.StackOverflow"; 

const char QueueUnderflow::MSG[] = "queue Underflow";
const char QueueUnderflow::KEY[] = "Common.InternalException.QueueUnderflow"; 

const char BadFormat::MSG[] = "bad format passed to Formatter::format()";
const char BadFormat::KEY[] = "Common.InternalException.BadFormat";

const char BadlyFormedCGIQueryString::MSG[] = "badly formed CGI query string";
const char BadlyFormedCGIQueryString::KEY[] = "Common.InternalException.BadlyFormedCGIQueryString"; 

const char DynamicLoadFailed::MSG[] = "load of dynamic library failed: ";
const char DynamicLoadFailed::KEY[] = "Common.InternalException.DynamicLoadFailed";

const char DynamicLookupFailed::MSG[] = "lookup of symbol in dynamic library failed: ";
const char DynamicLookupFailed::KEY[] = "Common.InternalException.DynamicLookupFailed";

const char CannotOpenDirectory::MSG[] = "cannot open directory: ";
const char CannotOpenDirectory::KEY[] = "Common.InternalException.CannotOpenDirectory"; 

const char ParseError::MSG[] = "parse error: ";
const char ParseError::KEY[] = "Common.InternalException.ParseError";

const char MissingNullTerminator::MSG[] = "missing null terminator: ";
const char MissingNullTerminator::KEY[] = "Common.InternalException.MissingNullTerminator";

//l10n
const char MalformedLanguageHeader::MSG[] = "malformed language header: ";
const char MalformedLanguageHeader::KEY[] = "Common.InternalException.MalformedLanguageHeader";

const char InvalidAcceptLanguageHeader::MSG[] = "invalid acceptlanguage header: ";
const char InvalidAcceptLanguageHeader::KEY[] = "Common.InternalException.InvalidAcceptLanguageHeader";

const char InvalidContentLanguageHeader::MSG[] = "invalid contentlanguage header: ";
const char InvalidContentLanguageHeader::KEY[] = "Common.InternalException.InvalidContentLanguageHeader";
//l10n

const char InvalidAuthHeader::MSG[] = "Invalid Authorization header";
const char InvalidAuthHeader::KEY[] = "Common.InternalException.InvalidAuthHeader"; 

const char UnauthorizedAccess::MSG[] = "Unauthorized access";
const char UnauthorizedAccess::KEY[] = "Common.InternalException.UnauthorizedAccess";  

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

// l10n
//
// Creates a description without source file name and line number.
//
static String _makeCIMExceptionDescription(
    CIMStatusCode code,
    const String& message,
    ContentLanguages &contentLanguages)
{
    String tmp;
    tmp = cimStatusCodeToString(code, contentLanguages);
    if (message != String::EMPTY)
    {
        tmp.append(": \"");
        tmp.append(message);
        tmp.append("\"");
    }
    return tmp;
}

// l10n
//
// Creates a description without source file name and line number.
//
static String _makeCIMExceptionDescription(
    const String& cimMessage,
    const String& extraMessage)
{
    String tmp;
    tmp = cimMessage;
    if (extraMessage != String::EMPTY)
    {
        tmp.append(": \"");
        tmp.append(extraMessage);
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

// l10n
	// Localize the cim message from the code.  Use the language of
	// the current thread.
	rep->contentLanguages = cimStatusCodeToString_Thread(rep->cimMessage, code);
}

// l10n
TraceableCIMException::TraceableCIMException(
	const ContentLanguages& langs,
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

// l10n
	rep->contentLanguages = langs;
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
// l10n    
    left->contentLanguages = right->contentLanguages;    
    left->cimMessage = right->cimMessage;
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
// l10n  - TODO uncomment when CIMStatus code is done
	//return _makeCIMExceptionDescription(rep->code, getMessage());  
	
	if (rep->cimMessage == String::EMPTY)
	{
	    return _makeCIMExceptionDescription(rep->code, 
    										getMessage(),
    										rep->contentLanguages);    		
	}
	else
	{
		return _makeCIMExceptionDescription(rep->cimMessage,
											getMessage());
	}

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
