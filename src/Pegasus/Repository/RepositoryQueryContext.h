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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_RepositoryQueryContext_h
#define Pegasus_RepositoryQueryContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Repository/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>
#include <Pegasus/Repository/CIMRepository.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_REPOSITORY_LINKAGE RepositoryQueryContext: public QueryContext
{
   public:

   RepositoryQueryContext(const CIMNamespaceName& inNS, CIMRepository* inCIMRep);

	RepositoryQueryContext(const RepositoryQueryContext& ctx);
	
	RepositoryQueryContext& operator=(const RepositoryQueryContext& rhs);
	
	~RepositoryQueryContext();

	QueryContext* clone();

        CIMClass getClass (const CIMName& inClassName)const;

	Array<CIMName> enumerateClassNames(const CIMName& inClassName)const;

        // Returns true if the derived class is a subclass of the base class.
        // Note: this will return false if the classes are the same.
        // Note: the default namespace of the query is used.
        Boolean isSubClass(const CIMName& baseClass,
                           const CIMName& derivedClass)const;

        // Returns the relationship between the anchor class and the related
        // class in the class schema of the query's default name space.
        ClassRelation getClassRelation(const CIMName& anchorClass,
                                       const CIMName& relatedClass)const;
        
   private: 
	RepositoryQueryContext();

        CIMRepository* _CIMRep;
};

PEGASUS_NAMESPACE_END
#endif
#endif
