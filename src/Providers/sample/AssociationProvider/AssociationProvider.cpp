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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "AssociationProvider.h"

PEGASUS_NAMESPACE_BEGIN

AssociationProvider::AssociationProvider(void)
{
}

AssociationProvider::~AssociationProvider(void)
{
}

void AssociationProvider::initialize(CIMOMHandle & cimom)
{
}

void AssociationProvider::terminate(void)
{
}

void AssociationProvider::associators(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMObject> & handler)
{
	throw NotSupported("AssociationProvider::associators");
}

void AssociationProvider::associatorNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const String & associationClass,
	const String & resultClass,
	const String & role,
	const String & resultRole,
	ResponseHandler<CIMObjectPath> & handler)
{
	throw NotSupported("AssociationProvider::associatorNames");
}

void AssociationProvider::references(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const String & resultClass,
	const String & role,
	const Uint32 flags,
	const CIMPropertyList & propertyList,
	ResponseHandler<CIMObject> & handler)
{
	throw NotSupported("AssociationProvider::references");
}

void AssociationProvider::referenceNames(
	const OperationContext & context,
	const CIMObjectPath & objectName,
	const String & resultClass,
	const String & role,
	ResponseHandler<CIMObjectPath> & handler)
{
	throw NotSupported("AssociationProvider::referenceNames");
}

PEGASUS_NAMESPACE_END
