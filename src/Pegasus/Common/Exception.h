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
// Modified By: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Exception_h
#define Pegasus_Exception_h

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMStatusCode.h>

PEGASUS_NAMESPACE_BEGIN

/** Class Exception  - This 
*/
class PEGASUS_COMMON_LINKAGE Exception
{
public:

    Exception(const String& message);

    Exception(const char* message);

    ~Exception();

    const String& getMessage() const { return _message; }

protected:

    String _message;
};

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
#define PEGASUS_ASSERT(COND) \
    do \
    { \
	if (!(COND)) \
	{ \
	    throw AssertionFailureException(__FILE__, __LINE__, #COND); \
	} \
    } while (0)

/// ATTN:
class PEGASUS_COMMON_LINKAGE BadReference : public Exception
{
public:

    BadReference(const String& message) : Exception(message) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE OutOfBounds : public Exception
{
public:

    static const char MSG[];

    OutOfBounds() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE AlreadyExists : public Exception
{
public:

    static const char MSG[];

    AlreadyExists(const String& x = String()) : Exception(MSG + x) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NullPointer : public Exception
{
public:

    static const char MSG[];

    NullPointer() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE IllegalName : public Exception
{
public:

    static const char MSG[];

    IllegalName() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE UnitializedHandle : public Exception
{
public:

    static const char MSG[];

    UnitializedHandle() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE InvalidPropertyOverride : public Exception
{
public:

    static const char MSG[];

    InvalidPropertyOverride(const String& message)
	: Exception(MSG + message) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE InvalidMethodOverride : public Exception
{
public:

    static const char MSG[];

    InvalidMethodOverride(const String& message)
	: Exception(MSG + message) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE UndeclaredQualifier : public Exception
{
public:

    static const char MSG[];

    UndeclaredQualifier(const String& qualifierName)
	: Exception(MSG + qualifierName) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE BadQualifierScope : public Exception
{
public:

    static const char MSG[];

    BadQualifierScope(const String& qualifierName, const String& scopeString)
	: Exception(MSG + qualifierName + String(" scope=") + scopeString) { }
};

/// ATTN:
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

/// ATTN:
class PEGASUS_COMMON_LINKAGE NullType : public Exception
{
public:

    static const char MSG[];

    NullType() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE AddedReferenceToClass : public Exception
{
public:

    static const char MSG[];

    AddedReferenceToClass(const String& className)
	: Exception(MSG + className) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE ClassAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    ClassAlreadyResolved(const String& className)
	: Exception(MSG + className) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE ClassNotResolved : public Exception
{
public:

    static const char MSG[];

    ClassNotResolved(const String& className)
	: Exception(MSG + className) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE InstanceAlreadyResolved : public Exception
{
public:

    static const char MSG[];

    InstanceAlreadyResolved() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE InstantiatedAbstractClass : public Exception
{
public:

    static const char MSG[];

    InstantiatedAbstractClass() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NoSuchProperty : public Exception
{
public:

    static const char MSG[];

    NoSuchProperty(const String& propertyName)
	: Exception(MSG + propertyName) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE TruncatedCharacter : public Exception
{
public:

    static const char MSG[];

    TruncatedCharacter() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE ExpectedReferenceValue : public Exception
{
public:

    static const char MSG[];

    ExpectedReferenceValue() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE MissingReferenceClassName : public Exception
{
public:

    static const char MSG[];

    MissingReferenceClassName() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE IllegalTypeTag : public Exception
{
public:

    static const char MSG[];

    IllegalTypeTag() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE TypeMismatch : public Exception
{
public:

    static const char MSG[];

    TypeMismatch() : Exception(MSG) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NoSuchFile : public Exception
{
public:

    static const char MSG[];

    NoSuchFile(const String& fileName) : Exception(MSG + fileName) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE FileNotReadable : public Exception
{
public:

    static const char MSG[];

    FileNotReadable(const String& fileName) : Exception(MSG + fileName) { }
};

class PEGASUS_COMMON_LINKAGE CannotBindToAddress : public Exception
{
public:

    static const char MSG[];

    CannotBindToAddress(const String& address) : Exception(MSG + address) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE CannotRemoveDirectory : public Exception
{
public:

    static const char MSG[];

    CannotRemoveDirectory(const String& path) : Exception(MSG + path) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE CannotRemoveFile : public Exception
{
public:

    static const char MSG[];

    CannotRemoveFile(const String& path) : Exception(MSG + path) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE CannotRenameFile : public Exception
{
public:

    static const char MSG[];

    CannotRenameFile(const String& path) : Exception(MSG + path) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NoSuchDirectory : public Exception
{
public:

    static const char MSG[];

    NoSuchDirectory(const String& directoryName)
	: Exception(MSG + directoryName) { }
};

class PEGASUS_COMMON_LINKAGE ChangeDirectoryFailed : public Exception
{
public:

    static const char MSG[];

    ChangeDirectoryFailed(const String& directoryName)
	: Exception(MSG + directoryName) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE CannotCreateDirectory : public Exception
{
public:

    static const char MSG[];

    CannotCreateDirectory(const String& path)
	: Exception(MSG + path) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NoSuchNameSpace : public Exception
{
public:

    static const char MSG[];

    NoSuchNameSpace(const String& directoryName)
	: Exception(MSG + directoryName) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE CannotOpenFile : public Exception
{
public:

    static const char MSG[];

    CannotOpenFile(const String& path)
	: Exception(MSG + path) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NotImplemented : public Exception
{
public:

    static const char MSG[];

    NotImplemented(const String& method) : Exception(MSG + method) { }
};

#define PEGASUS_CIM_EXCEPTION(CODE, EXTRA_MESSAGE) \
    CIMException(CODE, __FILE__, __LINE__, EXTRA_MESSAGE)

/** The CIMException defines the CIM exceptions that are formally defined in 
    the CIM Operations over HTTP specification.
    @example
    <PRE>
	throw CIMException(CIM_ERR_NOT_SUPPORTED);
    </PRE>
*/
class PEGASUS_COMMON_LINKAGE CIMException : public Exception
{
public:

    CIMException(
	CIMStatusCode code, 
	const char* file = "",
	Uint32 line = 0,
	const String& extraMessage = String());

    CIMStatusCode getCode() const { return _code; }

private:

    CIMStatusCode _code;
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

class PEGASUS_COMMON_LINKAGE BadDateTimeFormat : public Exception
{
public:

    static const char MSG[];

    BadDateTimeFormat() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE IncompatibleTypes : public Exception
{
public:

    static const char MSG[];

    IncompatibleTypes() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE BadlyFormedCGIQueryString : public Exception
{
public:

    static const char MSG[];

    BadlyFormedCGIQueryString() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE IllformedObjectName : public Exception
{
public:

    static const char MSG[];

    IllformedObjectName(const String& instanceName)
	: Exception(MSG + instanceName) { }
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

class PEGASUS_COMMON_LINKAGE CorruptFile : public Exception
{
public:

    static const char MSG[];

    CorruptFile(const String& path) : Exception(MSG + path) { }
};

class PEGASUS_COMMON_LINKAGE BindFailed : public Exception
{
public:

    static const char MSG[];

    BindFailed(const String& message) : Exception(MSG + message) { }
};

class PEGASUS_COMMON_LINKAGE InvalidLocator : public Exception
{
public:

    static const char MSG[];

    InvalidLocator(const String& locator) : Exception(MSG + locator) { }
};

class PEGASUS_COMMON_LINKAGE CannotCreateSocket : public Exception
{
public:

    static const char MSG[];

    CannotCreateSocket() : Exception(MSG) { }
};

class PEGASUS_COMMON_LINKAGE CannotConnect : public Exception
{
public:

    static const char MSG[];

    CannotConnect(const String& locator) : Exception(MSG + locator) { }
};

class PEGASUS_COMMON_LINKAGE UnexpectedFailure : public Exception
{
public:

    static const char MSG[];

    UnexpectedFailure() : Exception(MSG) { }
};

PEGASUS_COMMON_LINKAGE void ThrowUnitializedHandle();

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Exception_h */
