//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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

    PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2, _rep->message);
}

const char NullPointer::MSG[] = "null pointer";
const char NullPointer::KEY[] = "Common.InternalException.NULL_POINTER";

//l10n 
const char UndeclaredQualifier::MSG[] = "undeclared qualifier: $0";
const char UndeclaredQualifier::KEY[] = "Common.InternalException.UNDECLARED_QUALIFIER";

//l10n 
const char BadQualifierScope::MSG[] = "qualifier invalid in this scope: $0 scope=$1";
const char BadQualifierScope::KEY[] = "Common.InternalException.BAD_QUALIFIER_SCOPE";

//l10n 
const char BadQualifierOverride::MSG[] = "qualifier not overridable: $0";
const char BadQualifierOverride::KEY[] = "Common.InternalException.BAD_QUALIFIER_OVERRIDE"; 

//l10n 
const char BadQualifierType::MSG[] = "CIMType of qualifier different than its declaration: $0";
const char BadQualifierType::KEY[] = "Common.InternalException.BAD_QUALIFIER_TYPE";

const char ClassAlreadyResolved::MSG[] = "attempt to resolve a class that is already resolved: $0";
const char ClassAlreadyResolved::KEY[] = "Common.InternalException.CLASS_ALREADY_RESOLVED";

//l10n 
const char ClassNotResolved::MSG[] = "class is not yet resolved: $0";
const char ClassNotResolved::KEY[] = "Common.InternalException.CLASS_NOT_RESOLVED";

const char InstanceAlreadyResolved::MSG[] = "attempted to resolve a instance that is already resolved";
const char InstanceAlreadyResolved::KEY[] = "Common.InternalException.INSTANCE_ALREADY_RESOLVED"; 

//l10n 
const char InstantiatedAbstractClass::MSG[] = "attempted to instantiate an abstract class $0";
const char InstantiatedAbstractClass::KEY[] = "Common.InternalException.INSTANTIATED_ABSTRACT_CLASS"; 

//l10n 
const char NoSuchProperty::MSG[] = "no such property: $0";
const char NoSuchProperty::KEY[] = "Common.InternalException.NO_SUCH_PROPERTY";

//l10n 
const char NoSuchFile::MSG[] = "no such file: $0";
const char NoSuchFile::KEY[] = "Common.InternalException.NO_SUCH_FILE";

//l10n 
const char FileNotReadable::MSG[] = "file not readable: $0";
const char FileNotReadable::KEY[] = "Common.InternalException.FILE_NOT_READABLE";

//l10n 
const char CannotRemoveDirectory::MSG[] = "cannot remove directory: $0";
const char CannotRemoveDirectory::KEY[] = "Common.InternalException.CANNOT_REMOVE_DIRECTORY";  

//l10n 
const char CannotRemoveFile::MSG[] = "cannot remove file: $0";
const char CannotRemoveFile::KEY[] = "Common.InternalException.CANNOT_REMOVE_FILE"; 

//l10n 
const char CannotRenameFile::MSG[] = "cannot rename file: $0";
const char CannotRenameFile::KEY[] = "Common.InternalException.CANNOT_RENAME_FILE";

//l10n 
const char NoSuchDirectory::MSG[] = "no such directory: $0";
const char NoSuchDirectory::KEY[] = "Common.InternalException.NO_SUCH_DIRECTORY";  

//l10n 
const char CannotCreateDirectory::MSG[] = "cannot create directory: $0";
const char CannotCreateDirectory::KEY[] = "Common.InternalException.CANNOT_CREATE_DIRECTORY";

//l10n 
const char CannotOpenFile::MSG[] = "cannot open file: $0";
const char CannotOpenFile::KEY[] = "Common.InternalException.CANNOT_OPEN_FILE";

//l10n 
const char NotImplemented::MSG[] = "not implemented: $0";
const char NotImplemented::KEY[] = "Common.InternalException.NOT_IMPLEMENTED"; 

const char StackUnderflow::MSG[] = "stack underflow";
const char StackUnderflow::KEY[] = "Common.InternalException.STACK_UNDERFLOW";

const char StackOverflow::MSG[] = "stack overflow";
const char StackOverflow::KEY[] = "Common.InternalException.STACK_OVERFLOW"; 

const char QueueUnderflow::MSG[] = "queue Underflow";
const char QueueUnderflow::KEY[] = "Common.InternalException.QUEUE UNDERFLOW"; 

const char BadFormat::MSG[] = "bad format passed to Formatter::format()";
const char BadFormat::KEY[] = "Common.InternalException.BAD_FORMAT";

const char BadlyFormedCGIQueryString::MSG[] = "badly formed CGI query string";
const char BadlyFormedCGIQueryString::KEY[] = "Common.InternalException.BADLY_FORMED_CGI_QUERY_STRING"; 

//l10n 
const char DynamicLoadFailed::MSG[] = "load of dynamic library failed: $0";
const char DynamicLoadFailed::KEY[] = "Common.InternalException.DYNAMIC_LOAD_FAILED";

//l10n 
const char DynamicLookupFailed::MSG[] = "lookup of symbol in dynamic library failed: $0";
const char DynamicLookupFailed::KEY[] = "Common.InternalException.DYNAMIC_LOOKUP_FAILED";

//l10n 
const char CannotOpenDirectory::MSG[] = "cannot open directory: $0";
const char CannotOpenDirectory::KEY[] = "Common.InternalException.CANNOT_OPEN_DIRECTORY"; 

//l10n 
const char ParseError::MSG[] = "parse error: $0";
const char ParseError::KEY[] = "Common.InternalException.PARSE_ERROR";

//l10n 
const char MissingNullTerminator::MSG[] = "missing null terminator: $0";
const char MissingNullTerminator::KEY[] = "Common.InternalException.MISSING_NULL_TERMINATOR";

//l10n
const char MalformedLanguageHeader::MSG[] = "malformed language header: $0";
const char MalformedLanguageHeader::KEY[] = "Common.InternalException.MALFORMED_LANGUAGE_HEADER";

//l10n 
const char InvalidAcceptLanguageHeader::MSG[] = "invalid acceptlanguage header: $0";
const char InvalidAcceptLanguageHeader::KEY[] = "Common.InternalException.INVALID_ACCEPTLANGUAGE_HEADER";

//l10n 
const char InvalidContentLanguageHeader::MSG[] = "invalid contentlanguage header: $0";
const char InvalidContentLanguageHeader::KEY[] = "Common.InternalException.INVALID_CONTENTLANGUAGE_HEADER";

const char InvalidAuthHeader::MSG[] = "Invalid Authorization header";
const char InvalidAuthHeader::KEY[] = "Common.InternalException.INVALID_AUTH_HEADER"; 

const char UnauthorizedAccess::MSG[] = "Unauthorized access";
const char UnauthorizedAccess::KEY[] = "Common.InternalException.UNAUTHORIZED_ACCESS";  

const char InternalSystemError::MSG[] = "Unable to authenticate user";

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
    const String& file,
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

//l10n 
TraceableCIMException::TraceableCIMException(
    CIMStatusCode code,
    MessageLoaderParms parms,
    const String& file,
    Uint32 line)
    :
    CIMException(code, parms)
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

TraceableCIMException::TraceableCIMException(
    CIMStatusCode code,
    const String& message,
    const String& file,
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
    const String& file,
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

String TraceableCIMException::getCIMMessage() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->cimMessage;
}

void TraceableCIMException::setCIMMessage(const String& cimMessage)
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    rep->cimMessage = cimMessage;
}

String TraceableCIMException::getFile() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->file;
}

Uint32 TraceableCIMException::getLine() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->line;
}

const ContentLanguages& TraceableCIMException::getContentLanguages() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->contentLanguages;
}

PEGASUS_NAMESPACE_END
