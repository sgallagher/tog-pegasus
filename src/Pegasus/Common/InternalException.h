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
    static const char KEY[];     

    //NullPointer() : Exception(MSG) { }

    NullPointer() : Exception(MessageLoaderParms(KEY, MSG)) { }    
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE UndeclaredQualifier : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];   

    //UndeclaredQualifier(const String& qualifierName)
	//: Exception(MSG + qualifierName) { }

// NOTE - MSG needs to have a $0 arg for all the substitution cases	
  UndeclaredQualifier(const String& qualifierName) 
	: Exception(MessageLoaderParms(KEY, MSG, qualifierName)) { }  	
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE BadQualifierScope : public Exception
{
public:

    static const char MSG[]; 
    static const char KEY[];     

    //BadQualifierScope(const String& qualifierName, const String& scopeString)
	//: Exception(MSG + qualifierName + String(" scope=") + scopeString) { }

	
	BadQualifierScope(const String& qualifierName, const String& scopeString)
						 : Exception(MessageLoaderParms(KEY, 
														MSG,
														qualifierName,
														scopeString)) { }	
														
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE BadQualifierOverride : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //BadQualifierOverride(const String& qualifierName)
	//: Exception(MSG + qualifierName) { }
	
    BadQualifierOverride(const String& qualifierName)
    : Exception(MessageLoaderParms(KEY, MSG, qualifierName)) { }	
};

class PEGASUS_COMMON_LINKAGE BadQualifierType : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //BadQualifierType(const String& qualifierName)
	//: Exception(MSG + qualifierName) { }
	
    BadQualifierType(const String& qualifierName) 
	: Exception(MessageLoaderParms(KEY, MSG, qualifierName)) { }		
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE ClassAlreadyResolved : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //ClassAlreadyResolved(const String& className)
	//: Exception(MSG + className) { }
	
    ClassAlreadyResolved(const String& className) 
	: Exception(MessageLoaderParms(KEY, MSG, className)) { }		
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE ClassNotResolved : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //ClassNotResolved(const String& className)
	//: Exception(MSG + className) { }
	
    ClassNotResolved(const String& className) 
	: Exception(MessageLoaderParms(KEY, MSG, className)) { }		
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE InstanceAlreadyResolved : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //InstanceAlreadyResolved()
    // : Exception(MSG) { }
     
    InstanceAlreadyResolved() 
	: Exception(MessageLoaderParms(KEY, MSG)) { }	     
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE InstantiatedAbstractClass : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //InstantiatedAbstractClass(const String& className)
     //: Exception(MSG + className) { }
     
    InstantiatedAbstractClass(const String& className) 
	: Exception(MessageLoaderParms(KEY, MSG, className)) { }	     
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchProperty : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //NoSuchProperty(const String& propertyName)
	//: Exception(MSG + propertyName) { }
	
    NoSuchProperty(const String& propertyName) 
	: Exception(MessageLoaderParms(KEY, MSG, propertyName)) { }	
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchFile : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

   // NoSuchFile(const String& fileName) : Exception(MSG + fileName) { }
    
    NoSuchFile(const String& fileName) 
	: Exception(MessageLoaderParms(KEY, MSG, fileName)) { }    
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE FileNotReadable : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //FileNotReadable(const String& fileName) : Exception(MSG + fileName) { }
    
    FileNotReadable(const String& fileName) 
	: Exception(MessageLoaderParms(KEY, MSG, fileName)) { }     
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRemoveDirectory : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotRemoveDirectory(const String& path) : Exception(MSG + path) { }
    
    CannotRemoveDirectory(const String& path) 
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }     
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRemoveFile : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotRemoveFile(const String& path) : Exception(MSG + path) { }
    CannotRemoveFile(const String& path)  
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }         
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotRenameFile : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotRenameFile(const String& path) : Exception(MSG + path) { }
    CannotRenameFile(const String& path)  
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }      
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NoSuchDirectory : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //NoSuchDirectory(const String& directoryName)
	//: Exception(MSG + directoryName) { }
    NoSuchDirectory(const String& directoryName)  
	: Exception(MessageLoaderParms(KEY, MSG, directoryName)) { }  	
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotCreateDirectory : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotCreateDirectory(const String& path)
	//: Exception(MSG + path) { }
    CannotCreateDirectory(const String& path)  
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }  	
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE CannotOpenFile : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotOpenFile(const String& path)
	//: Exception(MSG + path) { }
    CannotOpenFile(const String& path)  
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }  	
};

// ATTN: P3  KS documentation Required
class PEGASUS_COMMON_LINKAGE NotImplemented : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

   // NotImplemented(const String& method) : Exception(MSG + method) { }
    NotImplemented(const String& method)  
	: Exception(MessageLoaderParms(KEY, MSG, method)) { }      
};

class PEGASUS_COMMON_LINKAGE StackUnderflow : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //StackUnderflow() : Exception(MSG) { }
    StackUnderflow()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }      
};

class PEGASUS_COMMON_LINKAGE StackOverflow : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //StackOverflow() : Exception(MSG) { }
    StackOverflow()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }       
};

class PEGASUS_COMMON_LINKAGE QueueUnderflow : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //QueueUnderflow() : Exception(MSG) { }
    QueueUnderflow()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }       
};

class PEGASUS_COMMON_LINKAGE BadFormat : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //BadFormat() : Exception(MSG) { }
    BadFormat()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }   
};

class PEGASUS_COMMON_LINKAGE BadlyFormedCGIQueryString : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //BadlyFormedCGIQueryString() : Exception(MSG) { }
    BadlyFormedCGIQueryString()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }       
};

class PEGASUS_COMMON_LINKAGE DynamicLoadFailed : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //DynamicLoadFailed(const String& libraryName)
	//: Exception(MSG + libraryName) { }
	
    DynamicLoadFailed(const String& libraryName)  
	: Exception(MessageLoaderParms(KEY, MSG, libraryName)) { }  		
};

class PEGASUS_COMMON_LINKAGE DynamicLookupFailed : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //DynamicLookupFailed(const String& symbolName)
	//: Exception(MSG + symbolName) { }
    DynamicLookupFailed(const String& symbolName)  
	: Exception(MessageLoaderParms(KEY, MSG, symbolName)) { }  		
};

class PEGASUS_COMMON_LINKAGE CannotOpenDirectory : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //CannotOpenDirectory(const String& path) : Exception(MSG + path) { }
    CannotOpenDirectory(const String& path)  
	: Exception(MessageLoaderParms(KEY, MSG, path)) { }  	    
};

class PEGASUS_COMMON_LINKAGE ParseError : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //ParseError(const String& message) : Exception(MSG + message) { }
    ParseError(const String& message)  
	: Exception(MessageLoaderParms(KEY, MSG, message)) { }  	    
};

class PEGASUS_COMMON_LINKAGE MissingNullTerminator : public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //MissingNullTerminator() : Exception(MSG) { }
    MissingNullTerminator()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }  	    
};

//l10n start

class PEGASUS_COMMON_LINKAGE MalformedLanguageHeader: public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //MalformedLanguageHeader(const String& error) : Exception(MSG + error) { }
    MalformedLanguageHeader(const String& error)  
	: Exception(MessageLoaderParms(KEY, MSG, error)) { }      
};


class PEGASUS_COMMON_LINKAGE InvalidAcceptLanguageHeader: public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //InvalidAcceptLanguageHeader(const String& error) : Exception(MSG + error) { }
    InvalidAcceptLanguageHeader(const String& error)  
	: Exception(MessageLoaderParms(KEY, MSG, error)) { }          
};

class PEGASUS_COMMON_LINKAGE InvalidContentLanguageHeader: public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //InvalidContentLanguageHeader(const String& error) : Exception(MSG + error) { }
    InvalidContentLanguageHeader(const String& error)  
	: Exception(MessageLoaderParms(KEY, MSG, error)) { }       
};

//l10n end

class PEGASUS_COMMON_LINKAGE InvalidAuthHeader: public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //InvalidAuthHeader() : Exception(MSG) { }
    InvalidAuthHeader()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }       
};

class PEGASUS_COMMON_LINKAGE UnauthorizedAccess: public Exception
{
public:

    static const char MSG[];
    static const char KEY[];     

    //UnauthorizedAccess() : Exception(MSG) { }
    UnauthorizedAccess()  
	: Exception(MessageLoaderParms(KEY, MSG)) { }       
};

class PEGASUS_COMMON_LINKAGE IncompatibleTypesException : public Exception
{
public:
    IncompatibleTypesException();    
};

class PEGASUS_COMMON_LINKAGE InternalSystemError : public Exception
{
public:

    static const char MSG[];

    InternalSystemError()
        : Exception(MSG) { }
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
	const String& file,
	Uint32 line);
	
	//l10n
	TraceableCIMException(
	CIMStatusCode code,
	MessageLoaderParms parms,
	const String& file,
	Uint32 line);

// l10n
	TraceableCIMException(
	const ContentLanguages& langs,
    CIMStatusCode code,
    const String& message,
    const String& file,
    Uint32 line);

    TraceableCIMException(const CIMException & cimException);

    String getDescription() const;

    String getTraceDescription() const;

    String getCIMMessage() const;
    void setCIMMessage(const String& cimMessage);

    String getFile() const;
    Uint32 getLine() const;
    const ContentLanguages& getContentLanguages() const;
};

#define PEGASUS_CIM_EXCEPTION(CODE, EXTRA_MESSAGE) \
    TraceableCIMException(CODE, EXTRA_MESSAGE, String(__FILE__), __LINE__)

// l10n    
#define PEGASUS_CIM_EXCEPTION_LANG(LANGS, CODE, EXTRA_MESSAGE) \
    TraceableCIMException( \
        LANGS, CODE, EXTRA_MESSAGE, String(__FILE__), __LINE__)  
      
//l10n
#define PEGASUS_CIM_EXCEPTION_L(CODE, MSG_PARMS) \
    TraceableCIMException(CODE, MSG_PARMS, String(__FILE__), __LINE__)

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InternalException_h */
