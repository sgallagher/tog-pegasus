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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_ProviderException_h
#define Pegasus_ProviderException_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_NAMESPACE_BEGIN

class OperationFailure : public CIMException
{
public:
    OperationFailure(const Uint32 code, const String & message) : CIMException(CIM_ERR_FAILED, "", 0, message)
    {
    }
};

class NotSupported : public OperationFailure
{
public:
    NotSupported(const String & message) : OperationFailure(CIM_ERR_NOT_SUPPORTED, message)
    {
    }
};

class AccessDenied : public OperationFailure
{
public:
    AccessDenied(const String & message) : OperationFailure(CIM_ERR_ACCESS_DENIED, message)
    {
    }
};

class InvalidParameter : public OperationFailure
{
public:
    InvalidParameter(const String & message) : OperationFailure(CIM_ERR_INVALID_PARAMETER,  message)
    {
    }
};

class InvalidClass : public OperationFailure
{
public:
    InvalidClass(const String & message) : OperationFailure(CIM_ERR_INVALID_CLASS,  message)
    {
    }
};

class InvalidQuery : public OperationFailure
{
public:
    InvalidQuery(const String & message) : OperationFailure(CIM_ERR_INVALID_QUERY, message)
    {
    }
};

class ObjectAlreadyExists : public OperationFailure
{
public:
    ObjectAlreadyExists(const String & message) : OperationFailure(CIM_ERR_ALREADY_EXISTS, message)
    {
    }
};

class ObjectNotFound : public OperationFailure
{
public:
    ObjectNotFound(const String & message) : OperationFailure(CIM_ERR_NOT_FOUND, message)
    {
    }
};

class MethodNotFound : public OperationFailure
{
public:
    MethodNotFound(const String & message) : OperationFailure(CIM_ERR_METHOD_NOT_FOUND, message)
    {
    }
};

class PropertyNotFound : public OperationFailure
{
public:
    PropertyNotFound(const String & message) : OperationFailure(CIM_ERR_NO_SUCH_PROPERTY, message)
    {
    }
};

PEGASUS_NAMESPACE_END

#endif
