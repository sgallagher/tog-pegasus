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
// $Log: Exception.h,v $
// Revision 1.10  2001/04/25 20:43:31  mike
// better
//
// Revision 1.9  2001/03/05 19:54:49  mike
// Fixed earlier boo boo (renamed CimException to CIMException).
//
// Revision 1.8  2001/03/05 04:29:02  mike
// renamed CimException to CIMException
//
// Revision 1.7  2001/02/21 01:50:30  karl
// comments
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
// Revision 1.1.1.1  2001/01/14 19:51:34  mike
// Pegasus import
//
//
//END_HISTORY

////////////////////////////////////////////////////////////////////////////////
//
// Exception.h
//
//	This file defines all exceptions used by the Pegasus library.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Exception_h
#define Pegasus_Exception_h

#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN
/** Class Exception
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
/** define PEGASUS_ASSERT assertion statement.  This statement
tests the condition defined by the parameters and if not True
executes a

<PRE>
	throw AssertionFailureException
</PRE>
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
class PEGASUS_COMMON_LINKAGE NoSuchSuperClass : public Exception
{
public:

    static const char MSG[];

    NoSuchSuperClass(const String& className) : Exception(MSG + className) { }
};

/// ATTN:
class PEGASUS_COMMON_LINKAGE NoSuchClass : public Exception
{
public:

    static const char MSG[];

    NoSuchClass(const String& className) : Exception(MSG + className) { }
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
class PEGASUS_COMMON_LINKAGE FailedToRemoveDirectory : public Exception
{
public:

    static const char MSG[];

    FailedToRemoveDirectory(const String& path) : Exception(MSG + path) { }
};
/// ATTN:
class PEGASUS_COMMON_LINKAGE FailedToRemoveFile : public Exception
{
public:

    static const char MSG[];

    FailedToRemoveFile(const String& path) : Exception(MSG + path) { }
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

/*  Class CIMException - Defines the CIM exceptions that are formally
    defined in the CIM Operations over HTTP specification.
    @example
    <PRE>
	throw CIMException(CIMException::NOT_SUPPORTED);
    </PRE>
*/
class PEGASUS_COMMON_LINKAGE CIMException : public Exception
{
public:

    enum Code
    {
	SUCCESS = 0,

	// A general error occurred that is not covered by a more
	// specific error code.

	FAILED = 1,

	// Access to a CIM resource was not available to the client.

	ACCESS_DENIED = 2,

	// The target namespace does not exist.

	INVALID_NAMESPACE = 3,

	// One or more parameter values passed to the method were invalid.

	INVALID_PARAMETER = 4,

	// The specified class does not exist.

	INVALID_CLASS = 5,

	// The requested object could not be found.

	NOT_FOUND = 6,

	// The requested operation is not supported.

	NOT_SUPPORTED = 7,

	// Operation cannot be carried out on this class since it has
	// subclasses.

	CLASS_HAS_CHILDREN = 8,

	// Operation cannot be carried out on this class since it has
	// instances.

	CLASS_HAS_INSTANCES = 9,

	// Operation cannot be carried out since the specified superClass
	// does not exist.

	INVALID_SUPERCLASS = 10,

	// Operation cannot be carried out because an object already exists.

	ALREADY_EXISTS = 11,

	// The specified property does not exist:

	NO_SUCH_PROPERTY = 12,

	// The value supplied is incompatible with the type.

	TYPE_MISMATCH = 13,

	// The query language is not recognized or supported.

	QUERY_LANGUAGE_NOT_SUPPORTED = 14,

	// The query is not valid for the specified query language.

	INVALID_QUERY = 15,

	// The extrinsic method could not be executed.

	METHOD_NOT_AVAILABLE = 16,

	// The specified extrinsic method does not exist.

	METHOD_NOT_FOUND = 17
    };

    CIMException(Code code, const String& extraMessage = String());

    CIMException::Code getCode() const { return _code; }

    static const char* codeToString(Code code);

private:

    Code _code;
};

class PEGASUS_COMMON_LINKAGE StackUnderflow : public Exception
{
public:

    static const char MSG[];

    StackUnderflow() : Exception(MSG) { }
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

class PEGASUS_COMMON_LINKAGE IllformedObjectPath : public Exception
{
public:

    static const char MSG[];

    IllformedObjectPath(const String& instanceName)
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

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Exception_h */
