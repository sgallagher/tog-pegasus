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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_InternalException_h
#define Pegasus_InternalException_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

/** Class AssertionFailureException
This is an Exception class tied to the definiton of an assert named
PEGASUS_ASSERT.  This assertion can be included at any point in Pegasus
code
*/
class PEGASUS_COMMON_LINKAGE AssertionFailureException : public Exception
{
public:

    AssertionFailureException(
	const char* file,
	size_t line,
	const String& message);
};

/** define PEGASUS_ASSERT assertion statement.  This statement tests the
    condition defined by the parameters and if not True executes an

    <pre>
    throw AssertionFailureException
    </pre>

    defining the file, line and condition that was tested.
*/
#ifdef NDEBUG
#define PEGASUS_ASSERT(COND)
#else
#define PEGASUS_ASSERT(COND) \
    do \
    { \
        if (!(COND)) \
        { \
            throw AssertionFailureException(__FILE__, __LINE__, #COND); \
        } \
    } while (0)
#endif

/* Macro to Create the equivalent of an assert but without the
   termination.  This can be used as a temporary marker for asserts
   that are not working.  Prints out the error but continues.
   NOTE: This is useful in test programs to keep us aware that we
   have problems without halting the test sequence.
   This was created primarily to put temporary asserts into tests that
   are not yet working correctly but will not stop the test sequence.
*/
#define ASSERTTEMP(COND) \
    do \
    { \
	if (!(COND)) \
	{ \
	    cerr << "TEMP Assert Error TEMP **********"	\
		<<__FILE__ << " " << __LINE__ \
		<< " " << #COND << endl; \
	} \
    } while (0)


// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NullPointer : public Exception
{
public:

    static const char MSG[];

    NullPointer() : Exception(MSG) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE UndeclaredQualifier : public Exception
{
public:

    static const char MSG[];

    UndeclaredQualifier(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE BadQualifierScope : public Exception
{
public:

    static const char MSG[];

    BadQualifierScope(const String& qualifierName, const String& scopeString)
	: Exception(MSG + qualifierName + String(" scope=") + scopeString) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE BadQualifierOverride : public Exception
{
public:

    static const char MSG[];

    BadQualifierOverride(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

class PEGASUS_COMMON_LINKAGE BadQualifierType : public Exception
{
public:

    static const char MSG[];

    BadQualifierType(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NullType : public Exception
{
public:

    static const char MSG[];

    NullType() : Exception(MSG) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE ClassAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    ClassAlreadyResolved(const String& className)
	: Exception(MSG + className) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE ClassNotResolved : public Exception
{
public:

    static const char MSG[];

    ClassNotResolved(const String& className)
	: Exception(MSG + className) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE InstanceAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    InstanceAlreadyResolved()
     : Exception(MSG) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE InstantiatedAbstractClass : public Exception
{
public:

    static const char MSG[];

    InstantiatedAbstractClass(const String& className)
     : Exception(MSG + className) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchProperty : public Exception
{
public:

    static const char MSG[];

    NoSuchProperty(const String& propertyName)
	: Exception(MSG + propertyName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchFile : public Exception
{
public:

    static const char MSG[];

    NoSuchFile(const String& fileName) : Exception(MSG + fileName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE FileNotReadable : public Exception
{
public:

    static const char MSG[];

    FileNotReadable(const String& fileName) : Exception(MSG + fileName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRemoveDirectory : public Exception
{
public:

    static const char MSG[];

    CannotRemoveDirectory(const String& path) : Exception(MSG + path) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRemoveFile : public Exception
{
public:

    static const char MSG[];

    CannotRemoveFile(const String& path) : Exception(MSG + path) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRenameFile : public Exception
{
public:

    static const char MSG[];

    CannotRenameFile(const String& path) : Exception(MSG + path) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchDirectory : public Exception
{
public:

    static const char MSG[];

    NoSuchDirectory(const String& directoryName)
	: Exception(MSG + directoryName) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotCreateDirectory : public Exception
{
public:

    static const char MSG[];

    CannotCreateDirectory(const String& path)
	: Exception(MSG + path) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotOpenFile : public Exception
{
public:

    static const char MSG[];

    CannotOpenFile(const String& path)
	: Exception(MSG + path) { }
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NotImplemented : public Exception
{
public:

    static const char MSG[];

    NotImplemented(const String& method) : Exception(MSG + method) { }
};

class PEGASUS_COMMON_LINKAGE StackUnderflow : public Exception
{
public:

    static const char MSG[];

    StackUnderflow() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE StackOverflow : public Exception
{
public:

    static const char MSG[];

    StackOverflow() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE QueueUnderflow : public Exception
{
public:

    static const char MSG[];

    QueueUnderflow() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE BadFormat : public Exception
{
public:

    static const char MSG[];

    BadFormat() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE BadlyFormedCGIQueryString : public Exception
{
public:

    static const char MSG[];

    BadlyFormedCGIQueryString() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE DynamicLoadFailed : public Exception
{
public:

    static const char MSG[];

    DynamicLoadFailed(const String& libraryName)
	: Exception(MSG + libraryName) { }
};

class PEGASUS_COMMON_LINKAGE DynamicLookupFailed : public Exception
{
public:

    static const char MSG[];

    DynamicLookupFailed(const String& symbolName)
	: Exception(MSG + symbolName) { }
};

class PEGASUS_COMMON_LINKAGE CannotOpenDirectory : public Exception
{
public:

    static const char MSG[];

    CannotOpenDirectory(const String& path) : Exception(MSG + path) { }
};

class PEGASUS_COMMON_LINKAGE ParseError : public Exception
{
public:

    static const char MSG[];

    ParseError(const String& message) : Exception(MSG + message) { }
};

class PEGASUS_COMMON_LINKAGE MissingNullTerminator : public Exception
{
public:

    static const char MSG[];

    MissingNullTerminator() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE InvalidAuthHeader: public Exception
{
public:

    static const char MSG[];

    InvalidAuthHeader() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE UnauthorizedAccess: public Exception
{
public:

    static const char MSG[];

    UnauthorizedAccess() : Exception(MSG) { }
};

/** The CIMException defines the CIM exceptions that are formally defined in
    the CIM Operations over HTTP specification.  TraceableCIMException allows
    file name and line number information to be added for diagnostic purposes.
*/
class PEGASUS_COMMON_LINKAGE TraceableCIMException : public CIMException
{
public:

    TraceableCIMException(
	CIMStatusCode code,
	const String& message,
	const char* file,
	Uint32 line);

    TraceableCIMException(const CIMException & cimException);

    String getDescription() const;

    String getTraceDescription() const;
};

#define PEGASUS_CIM_EXCEPTION(CODE, EXTRA_MESSAGE) \
    TraceableCIMException(CODE, EXTRA_MESSAGE, __FILE__, __LINE__)

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InternalException_h */
