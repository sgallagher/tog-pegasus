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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "DeclContext.h"
#include "CIMName.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN

DeclContext::~DeclContext()
{

}

SimpleDeclContext::~SimpleDeclContext()
{

}

void SimpleDeclContext::addQualifierDecl(
    const CIMNamespaceName& nameSpace,
    const CIMQualifierDecl& x)
{
    if (!lookupQualifierDecl(nameSpace, x.getName()).isUninitialized()){
    	//l10n
		//throw AlreadyExistsException("declaration of qualifier \"" +
                       //x.getName().getString () + "\"");
        MessageLoaderParms parms("Common.DeclContext.DECLARATION_OF_QUALIFIER",
        						 "declaration of qualifier \"$0\"",
        						 x.getName().getString());
        throw AlreadyExistsException(parms);                
    }

    _qualifierDeclarations.append(QPair(nameSpace, x));
}

void SimpleDeclContext::addClass(
    const CIMNamespaceName& nameSpace,
    const CIMClass& x)
{
    if (!lookupClass(nameSpace, x.getClassName()).isUninitialized()){
    	//l10n
		//throw AlreadyExistsException
            //("class \"" + x.getClassName().getString () + "\"");
        MessageLoaderParms parms("Common.DeclContext.CLASS",
        						 "class \"$0\"",
        						 x.getClassName().getString());
        throw AlreadyExistsException(parms);
    }

    _classDeclarations.append(CPair(nameSpace, x));
}

CIMQualifierDecl SimpleDeclContext::lookupQualifierDecl(
    const CIMNamespaceName& nameSpace,
    const CIMName& name) const
{
    for (Uint32 i = 0, n = _qualifierDeclarations.size(); i < n; i++)
    {
	const CIMNamespaceName& first = _qualifierDeclarations[i].first;
	const CIMQualifierDecl& second = _qualifierDeclarations[i].second;

	if (first.equal(nameSpace) && 
	    second.getName().equal(name))
	{
	    return second;
	}
    }

    // Not found:
    return CIMQualifierDecl();
}

CIMClass SimpleDeclContext::lookupClass(
    const CIMNamespaceName& nameSpace,
    const CIMName& name) const
{
    for (Uint32 i = 0, n = _classDeclarations.size(); i < n; i++)
    {
	const CIMNamespaceName& first = _classDeclarations[i].first;
	const CIMClass& second = _classDeclarations[i].second;

	if (first.equal(nameSpace) && 
	    second.getClassName().equal(name))
	{
	    return second;
	}
    }

    // Not found:
    return CIMClass();
}

PEGASUS_NAMESPACE_END
