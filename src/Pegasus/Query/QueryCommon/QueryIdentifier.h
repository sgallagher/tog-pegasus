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

#ifndef Pegasus_QueryIdentifier_h
#define Pegasus_QueryIdentifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Query/QueryCommon/Linkage.h>
#include <Pegasus/Common/String.h>
//#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Query/QueryCommon/SubRange.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_QUERYCOMMON_LINKAGE QueryIdentifierRep;

class PEGASUS_QUERYCOMMON_LINKAGE QueryIdentifier
{
  public:
    QueryIdentifier();

    QueryIdentifier(const QueryIdentifier& id);

    ~QueryIdentifier();

    QueryIdentifier& operator=(const QueryIdentifier& rhs);

    const CIMName& getName()const;
    
    void setName(const CIMName& name);

    const String& getSymbolicConstantName()const;

    const  Array<SubRange>& getSubRanges()const;

    Boolean isArray()const;

    Boolean isSymbolicConstant()const;

    Boolean isWildcard()const;

    const String& getScope()const;

    Boolean isScoped()const;

    void applyScope(String scope);

    Boolean operator==(const CIMName &rhs)const;

    Boolean operator!=(const CIMName &rhs)const;

    Boolean operator==(const QueryIdentifier &rhs)const;

    Boolean operator!=(const QueryIdentifier &rhs)const;

    String toString()const;

  protected:
	QueryIdentifierRep* _rep;

};

/*
#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T QueryIdentifier
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
*/

PEGASUS_NAMESPACE_END
//#endif
#endif
#endif /* Pegasus_QueryIdentifier_h */
