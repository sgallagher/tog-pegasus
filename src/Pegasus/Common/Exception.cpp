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
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Jenny Yu, Hewlett-Packard Company (jenny_yu@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Sushma Fernandes , Hewlett-Packard Company
//                (sushma_fernandes@hp.com)
//              Amit K Arora, IBM (amita@in.ibm.com) for bug# 1593
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "Exception.h"
#include <Pegasus/Common/ExceptionRep.h>
#include <Pegasus/Common/CIMExceptionRep.h>
#include "Tracer.h"

PEGASUS_NAMESPACE_BEGIN

Exception::Exception(const String& message)
{
    _rep = new ExceptionRep();
    _rep->message = message;
    _rep->contentLanguages = ContentLanguages::EMPTY;  // l10n
}

Exception::Exception(const Exception& exception)
{
    _rep = new ExceptionRep();
    _rep->message = exception._rep->message;
    _rep->contentLanguages = exception._rep->contentLanguages;    // l10n
}

// l10n
Exception::Exception(const MessageLoaderParms& msgParms)
{
    _rep = new ExceptionRep();
    _rep->message = MessageLoader::getMessage(
    	const_cast<MessageLoaderParms &>(msgParms));
    // Must be after MessageLoader::getMessage call    
    _rep->contentLanguages = msgParms.contentlanguages;
}

Exception::Exception()
{
    _rep = NULL;
}

Exception::~Exception()
{
    delete _rep;
}

const String& Exception::getMessage() const
{
    return _rep->message;
}

// l10n
const ContentLanguages& Exception::getContentLanguages() const
{
	return _rep->contentLanguages;
} 

// l10n
void Exception::setContentLanguages(const ContentLanguages& langs)
{
	_rep->contentLanguages = langs;
} 

/*
IndexOutOfBoundsException::IndexOutOfBoundsException()
    : Exception("index out of bounds")
{
}
*/

IndexOutOfBoundsException::IndexOutOfBoundsException()
    : Exception(MessageLoaderParms("Common.Exception.INDEX_OUT_OF_BOUNDS_EXCEPTION","index out of bounds"))
{
}

/*
AlreadyExistsException::AlreadyExistsException(const String& message)
    : Exception("already exists: " + message)
{
}
*/

AlreadyExistsException::AlreadyExistsException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.ALREADY_EXISTS_EXCEPTION",
    								"already exists: $0",    								
    								 message))
{
}

AlreadyExistsException::AlreadyExistsException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.ALREADY_EXISTS_EXCEPTION",
    								"already exists: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
InvalidNameException::InvalidNameException(const String& name)
    : Exception("invalid CIM name: " + name)
{
}
*/

InvalidNameException::InvalidNameException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_NAME_EXCEPTION",
    								"invalid CIM name: $0",
    								 message))
{
}

InvalidNameException::InvalidNameException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_NAME_EXCEPTION",
    								"invalid CIM name: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
InvalidNamespaceNameException::InvalidNamespaceNameException(const String& name)
    : Exception("invalid CIM namespace name: " + name)
{
}
*/


InvalidNamespaceNameException::InvalidNamespaceNameException(const String& name)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_NAMESACE_NAME_EXCEPTION",
    								"invalid CIM namespace name: $0",
    								 name))
{
}

InvalidNamespaceNameException::InvalidNamespaceNameException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_NAMESPACE_NAME_EXCEPTION",
    								"invalid CIM namespace name: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
UninitializedObjectException::UninitializedObjectException()
    : Exception("uninitialized object")
{
}
*/

UninitializedObjectException::UninitializedObjectException()
    : Exception(MessageLoaderParms("Common.Exception.UNINITIALIZED_OBJECT_EXCEPTION",
    								"uninitialized object"))
{
}

/*
TypeMismatchException::TypeMismatchException()
    : Exception("type mismatch")
{
}
*/

TypeMismatchException::TypeMismatchException()
    : Exception(MessageLoaderParms("Common.Exception.TYPE_MISMATCH_EXCEPTION",
    								"type mismatch"))
{
}

TypeMismatchException::TypeMismatchException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.TYPE_MISMATCH_EXCEPTION",
    								"type mismatch: $0",message))
{

}

TypeMismatchException::TypeMismatchException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.TYPE_MISMATCH_EXCEPTION",
    								"type mismatch: "))
{
    _rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
DynamicCastFailedException::DynamicCastFailedException()
    : Exception("dynamic cast failed")
{
}
*/

DynamicCastFailedException::DynamicCastFailedException()
    : Exception(MessageLoaderParms("Common.Exception.DYNAMIC_CAST_FAILED_EXCEPTION",
    								"dynamic cast failed"))
{
}

/*
InvalidDateTimeFormatException::InvalidDateTimeFormatException()
    : Exception("invalid datetime format")
{
}
*/

InvalidDateTimeFormatException::InvalidDateTimeFormatException()
    : Exception(MessageLoaderParms("Common.Exception.INVALID_DATETIME_FORMAT_EXCEPTION",
    								"invalid datetime format"))
{
}

/*
MalformedObjectNameException::MalformedObjectNameException(
    const String& objectName)
    : Exception("malformed object name: " + objectName)
{
}
*/

MalformedObjectNameException::MalformedObjectNameException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.MALFORMED_OBJECT_NAME_EXCEPTION",
    								"malformed object name: $0",
    								 message))
{
}

MalformedObjectNameException::MalformedObjectNameException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.MALFORMED_OBJECT_NAME_EXCEPTION",
    								"malformed object name: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
BindFailedException::BindFailedException(const String& message)
    : Exception("Bind failed: " + message)
{
}
*/

BindFailedException::BindFailedException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.BIND_FAILED_EXCEPTION",
    								"Bind failed: $0",
    								 message))
{
}

BindFailedException::BindFailedException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.BIND_FAILED_EXCEPTION",
    								"Bind failed: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
InvalidLocatorException::InvalidLocatorException(const String& locator)
    : Exception("Invalid locator: " + locator)
{
}
*/

InvalidLocatorException::InvalidLocatorException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_LOCATOR_EXCEPTION",
    								"Invalid locator: $0",
    								 message))
{
}

InvalidLocatorException::InvalidLocatorException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.INVALID_LOCATOR_EXCEPTION",
    								"Invalid locator: "))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
CannotCreateSocketException::CannotCreateSocketException()
    : Exception("Cannot create socket")
{
}
*/

CannotCreateSocketException::CannotCreateSocketException()
    : Exception(MessageLoaderParms("Common.Exception.CANNOT_CREATE_SOCKET_EXCEPTION",
    								"Cannot create socket"))
{
}


CannotConnectException::CannotConnectException(const String& message)//???
    : Exception(message)
{
}


CannotConnectException::CannotConnectException(MessageLoaderParms& msgParms)
    : Exception(msgParms)
{
}

/*
AlreadyConnectedException::AlreadyConnectedException()
    : Exception("already connected")
{
}
*/

AlreadyConnectedException::AlreadyConnectedException()
    : Exception(MessageLoaderParms("Common.Exception.ALREADY_CONNECTED_EXCEPTION",
    								"already connected"))
{
}

/*
NotConnectedException::NotConnectedException()
    : Exception("not connected")
{
}
*/

NotConnectedException::NotConnectedException()
    : Exception(MessageLoaderParms("Common.Exception.NOT_CONNECTED_EXCEPTION",
    								"not connected"))
{
}

/*
ConnectionTimeoutException::ConnectionTimeoutException()
    : Exception("connection timed out")
{
}
*/

ConnectionTimeoutException::ConnectionTimeoutException()
    : Exception(MessageLoaderParms("Common.Exception.CONNECTION_TIMEOUT_EXCEPTION",
    								"connection timed out"))
{
}

/*
SSLException::SSLException(const String& message)
    : Exception("SSL Exception: " + message)
{
}
*/

SSLException::SSLException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.SSL_EXCEPTION",
    								"SSL Exception: $0" ,
    								 message))
{
}

SSLException::SSLException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.SSL_EXCEPTION",
    								"SSL Exception: " ))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}

/*
DateTimeOutOfRangeException::DateTimeOutOfRangeException(const String& message)
    : Exception("DateTime is out of range : " + message)
{
}
*/

DateTimeOutOfRangeException::DateTimeOutOfRangeException(const String& message)
    : Exception(MessageLoaderParms("Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
    								"DateTime is out of range : $0" ,
    								 message))
{
}

DateTimeOutOfRangeException::DateTimeOutOfRangeException(MessageLoaderParms& msgParms)
    : Exception(MessageLoaderParms("Common.Exception.DATETIME_OUT_OF_RANGE_EXCEPTION",
    								"DateTime is out of range : " ))
{
	_rep->message.append(MessageLoader::getMessage(msgParms));
}


////////////////////////////////////////////////////////////////////////////////
//
// CIMException
//
////////////////////////////////////////////////////////////////////////////////

// l10n - note - use this when you have an exception with no
// detail message, or one with an untranslated detail message
// The pegasus message associated with code will be translated.
CIMException::CIMException(
    CIMStatusCode code,
    const String& message)
    : Exception()
{
    CIMExceptionRep * tmp = new CIMExceptionRep ();
    tmp->message = message;
    tmp->code = code;
    tmp->file = "";
    tmp->line = 0;
    tmp->contentLanguages = ContentLanguages::EMPTY;     
    tmp->cimMessage = String::EMPTY;    
    _rep = tmp;
}

// l10n - note use this when you have an exception with a translated
// detail message
// l10n
CIMException::CIMException(
    CIMStatusCode code,
    const MessageLoaderParms& msgParms)
    : Exception()
{
    CIMExceptionRep * tmp = new CIMExceptionRep ();
    tmp->message = MessageLoader::getMessage(
    	const_cast<MessageLoaderParms &>(msgParms));
    // Must be after MessageLoader::getMessage call
    tmp->contentLanguages = msgParms.contentlanguages;     
    tmp->cimMessage = String::EMPTY;
    tmp->code = code;
    tmp->file = "";
    tmp->line = 0;
    _rep = tmp;
}

CIMException::CIMException(const CIMException & cimException)
    : Exception()
{
    CIMExceptionRep * tmp = new CIMExceptionRep ();
    CIMExceptionRep * rep;
    rep = reinterpret_cast<CIMExceptionRep*>(cimException._rep);
    tmp->message = rep->message;
    tmp->contentLanguages = rep->contentLanguages;  // l10n
    tmp->cimMessage = rep->cimMessage;  // l10n    
    tmp->code = rep->code;
    tmp->file = rep->file;
    tmp->line = rep->line;
    _rep = tmp;
}

CIMException& CIMException::operator=(const CIMException & cimException)
{
    if (&cimException != this)
    {
        CIMExceptionRep* left;
        CIMExceptionRep* right;
        left = reinterpret_cast<CIMExceptionRep*>(this->_rep);
        right = reinterpret_cast<CIMExceptionRep*>(cimException._rep);
        left->message = right->message;
        left->contentLanguages = right->contentLanguages;  // l10n    
        left->cimMessage = right->cimMessage;  // l10n
        left->code = right->code;
        left->file = right->file;
        left->line = right->line;
    }
    return *this;
}

CIMException::~CIMException()
{
}

CIMStatusCode CIMException::getCode() const
{
    CIMExceptionRep* rep;
    rep = reinterpret_cast<CIMExceptionRep*>(_rep);
    return rep->code;
}

PEGASUS_NAMESPACE_END
