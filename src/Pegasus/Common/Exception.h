//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Exception_h
#define Pegasus_Exception_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMStatusCode.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/ContentLanguageList.h>

#ifdef PEGASUS_INTERNALONLY
# include <Pegasus/Common/MessageLoader.h>
#endif

PEGASUS_NAMESPACE_BEGIN
class ExceptionRep;

/** 
<p>The <tt>Exception</tt> class is the parent class for all
exceptions that can be generated by any component of the
Pegasus infrastructure. It includes not only the CIM exceptions
that are defined by the DMTF, but also various exceptions that
may occur during the processing of functions called by clients
and providers.</p>
*/
class PEGASUS_COMMON_LINKAGE Exception
{
public:

    ///
    Exception(const String& message);

    ///
    Exception(const Exception& exception);

#ifdef PEGASUS_INTERNALONLY
    Exception(const MessageLoaderParms& msgParms);
#endif

    ///
    virtual ~Exception();

    ///
    virtual const String& getMessage() const;
    
#ifdef PEGASUS_INTERNALONLY
    // Note: Not virtual to preserve binary compatibility.
    const ContentLanguageList& getContentLanguages() const;
    
    // Not virtual to preserve binary compatibility.
    void setContentLanguages(const ContentLanguageList& langs); 
#endif   

protected:

    Exception();

    ExceptionRep * _rep;
};

///
class PEGASUS_COMMON_LINKAGE IndexOutOfBoundsException : public Exception
{
public:
    ///
    IndexOutOfBoundsException();
};

///
class PEGASUS_COMMON_LINKAGE AlreadyExistsException : public Exception
{
public:
    ///
    AlreadyExistsException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    AlreadyExistsException(MessageLoaderParms& msgParms);
#endif
};

///
class PEGASUS_COMMON_LINKAGE InvalidNameException : public Exception
{
public:
    ///
    InvalidNameException(const String& name);

#ifdef PEGASUS_INTERNALONLY
    InvalidNameException(MessageLoaderParms& msgParms); 
#endif   
};

///
class PEGASUS_COMMON_LINKAGE InvalidNamespaceNameException : public Exception
{
public:
    ///
    InvalidNamespaceNameException(const String& name);

#ifdef PEGASUS_INTERNALONLY
    InvalidNamespaceNameException(MessageLoaderParms& msgParms);
#endif    
};

///
class PEGASUS_COMMON_LINKAGE UninitializedObjectException : public Exception
{
public:
    ///
    UninitializedObjectException();
};

///
class PEGASUS_COMMON_LINKAGE TypeMismatchException : public Exception
{
public:
    ///
    TypeMismatchException();
    TypeMismatchException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    TypeMismatchException(MessageLoaderParms& msgParms);
#endif    

};

///
class PEGASUS_COMMON_LINKAGE DynamicCastFailedException : public Exception
{
public:
    ///
    DynamicCastFailedException();
};

///
class PEGASUS_COMMON_LINKAGE InvalidDateTimeFormatException : public Exception
{
public:
    ///
    InvalidDateTimeFormatException();
};

///
class PEGASUS_COMMON_LINKAGE MalformedObjectNameException : public Exception
{
public:
    ///
    MalformedObjectNameException(const String& objectName);

#ifdef PEGASUS_INTERNALONLY
    MalformedObjectNameException(MessageLoaderParms& msgParms); 
#endif   
};

///
class PEGASUS_COMMON_LINKAGE BindFailedException : public Exception
{
public:
    ///
    BindFailedException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    BindFailedException(MessageLoaderParms& msgParms);
#endif    
};

///
class PEGASUS_COMMON_LINKAGE InvalidLocatorException : public Exception
{
public:
    ///
    InvalidLocatorException(const String& locator);

#ifdef PEGASUS_INTERNALONLY
    InvalidLocatorException(MessageLoaderParms& msgParms); 
#endif  
};

///
class PEGASUS_COMMON_LINKAGE CannotCreateSocketException : public Exception
{
public:
    ///
    CannotCreateSocketException();
};

///
class PEGASUS_COMMON_LINKAGE CannotConnectException : public Exception
{
public:
    ///
    CannotConnectException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    CannotConnectException(MessageLoaderParms& msgParms); 
#endif  
};

///
class PEGASUS_COMMON_LINKAGE AlreadyConnectedException: public Exception
{
public:
    ///
    AlreadyConnectedException();
};

///
class PEGASUS_COMMON_LINKAGE NotConnectedException: public Exception
{
public:
    ///
    NotConnectedException();
};

///
class PEGASUS_COMMON_LINKAGE ConnectionTimeoutException: public Exception
{
public:
    ///
    ConnectionTimeoutException();
};

///
class PEGASUS_COMMON_LINKAGE SSLException: public Exception
{
public:
    ///
    SSLException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    SSLException(MessageLoaderParms& msgParms); 
#endif   
};

///
class PEGASUS_COMMON_LINKAGE DateTimeOutOfRangeException : public Exception
{
public:
    ///
    DateTimeOutOfRangeException(const String& message);

#ifdef PEGASUS_INTERNALONLY
    DateTimeOutOfRangeException(MessageLoaderParms& msgParms);
#endif     
}; 

/** The CIMException defines the CIM exceptions that are formally defined in
    the CIM Operations over HTTP specification.
*/
class PEGASUS_COMMON_LINKAGE CIMException : public Exception
{
public:

    ///
    CIMException(
	CIMStatusCode code = CIM_ERR_SUCCESS,
	const String& message = String::EMPTY);
	
#ifdef PEGASUS_INTERNALONLY
    CIMException(
	CIMStatusCode code,
	const MessageLoaderParms& msgParms);
#endif	

    ///
    CIMException(const CIMException & cimException);

    ///
    CIMException& operator=(const CIMException & cimException);

    ///
    virtual ~CIMException();

    ///
    CIMStatusCode getCode() const;
};


PEGASUS_NAMESPACE_END

#endif /* Pegasus_Exception_h */
