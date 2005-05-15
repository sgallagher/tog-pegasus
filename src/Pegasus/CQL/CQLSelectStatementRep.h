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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLSelectStatementRep_h
#define Pegasus_CQLSelectStatementRep_h

#include <Pegasus/Query/QueryCommon/SelectStatementRep.h>
#include <Pegasus/Query/QueryCommon/QueryChainedIdentifier.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

struct PropertyNode;

class PEGASUS_CQL_LINKAGE CQLSelectStatementRep : public SelectStatementRep
{
  public:

    CQLSelectStatementRep();

    CQLSelectStatementRep(String& inQlang,
                          String& inQuery,
                          QueryContext& inCtx);

    CQLSelectStatementRep(String& inQlang,
                          String& inQuery);

    CQLSelectStatementRep(const CQLSelectStatementRep& rep);

    ~CQLSelectStatementRep();

    CQLSelectStatementRep& operator=(const CQLSelectStatementRep& cqlss);

    Boolean evaluate(const CIMInstance& inCI);

    void applyProjection(CIMInstance& inCI,
        Boolean allowMissing);

    void validate();

    Array<CIMObjectPath> getClassPathList();

    CIMPropertyList getPropertyList(const CIMObjectPath& inClassName);

    CIMPropertyList getSelectPropertyList(const CIMObjectPath& inClassName);

    CIMPropertyList getWherePropertyList(const CIMObjectPath& inClassName);

    Array<CQLChainedIdentifier> getSelectChainedIdentifiers();

    Array<CQLChainedIdentifier> getWhereChainedIdentifiers();

    void appendClassPath(const CQLIdentifier& inIdentifier);

    void setPredicate(const CQLPredicate& inPredicate);

    CQLPredicate getPredicate() const;

    void insertClassPathAlias(const CQLIdentifier& inIdentifier,
                              String inAlias);

    void appendSelectIdentifier(const CQLChainedIdentifier& x);

    void applyContext();

    void normalizeToDOC();

    String toString();

    void setHasWhereClause();

    Boolean hasWhereClause();

    void clear();

  protected:

    Array<CQLChainedIdentifier> _selectIdentifiers;

    Boolean _hasWhereClause;

  private:

    Boolean applyProjection(PropertyNode* node,
                            CIMProperty& nodeProp,
                            Boolean& preservePropsForParent,
                            Boolean allowMissing);

    void validateProperty(QueryChainedIdentifier& chainId);

    CIMName lookupFromClass(const String&  lookup);

    CIMPropertyList getPropertyListInternal(const CIMObjectPath& inClassName,
                                            Boolean includeSelect,
                                            Boolean includeWhere);

    Boolean addRequiredProperty(Array<CIMName>& reqProps,
                                CIMName& className,
                                QueryChainedIdentifier& chainId,
                                Array<CIMName>& matchedScopes,
                                Array<CIMName>& unmatchedScopes);

    Boolean isFilterable(const CIMInstance& inst,
                         PropertyNode* node);

    void filterInstance(CIMInstance& inst,
                        Boolean& allPropsRequired,
                        const CIMName& allPropsClass,
                        Array<CIMName>& requiredProps,
                        Boolean& preserveProps,
                        Boolean allowMissing);

    Boolean containsProperty(const CIMName& name,
                             const Array<CIMName>& props);

    Boolean isFromChild(const CIMName& className);

    void checkWellFormedIdentifier(const QueryChainedIdentifier& chainId,
                                   Boolean isSelectListId);

    CQLPredicate _predicate;

    Boolean _contextApplied;
};

PEGASUS_NAMESPACE_END

#endif
