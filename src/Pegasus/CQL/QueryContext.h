//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_QueryContext_h
#define Pegasus_QueryContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/HashTable.h>
#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/Common/String.h>

PEGASUS_NAMESPACE_BEGIN

typedef HashTable<String, CQLIdentifier, EqualNoCaseFunc, HashLowerCaseFunc> HT_Alias_Class;

class PEGASUS_CQL_LINKAGE QueryContext
{
   public:

	virtual ~QueryContext();

        virtual CIMClass getClass(const CIMName& inClassName) = 0;

	virtual Array<CIMName> enumerateClassNames(const CIMName& inClassName) = 0;

	virtual QueryContext* clone() = 0;

        String getHost(Boolean fullyQualified = true);

        CIMNamespaceName getNamespace() const;

        void insertClassPath(const CQLIdentifier& inIdentifier, String inAlias = String::EMPTY);

        CQLIdentifier findClass(const String& inAlias);

        Array<CQLIdentifier> getFromList() const;
	
	String getFromString() const;

        void clear();

   protected:

	QueryContext(const QueryContext& ctx);

        QueryContext(CIMNamespaceName& inNS);

	QueryContext& operator=(const QueryContext& rhs);
          
   private: 

	QueryContext();

        // members
        CIMNamespaceName _NS;
	HT_Alias_Class _AliasClassTable;
	Array<CQLIdentifier> _fromList;
};

PEGASUS_NAMESPACE_END
#endif
