//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L. P.;
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLSelectStatementRep_h
#define Pegasus_CQLSelectStatementRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/SelectStatementRep.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/CQLIdentifier.h>
#include <Pegasus/CQL/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

struct EmbeddedPropertyNode;

/**  
This class is derived from the SelectStatement base class.  
The purpose of this class is to perform the select statement operations for
CIM Query Language (CQL). 

Notes on CQLSelectStatement class:

(A) Contains a CQLPredicate for the WHERE clause
(B) Contains an array of CQLIdentifiers for the SELECT projection
(C) Contains an array of classpaths from the FROM clause


   */
class PEGASUS_CQL_LINKAGE CQLSelectStatementRep : public SelectStatementRep
{
  public:

  CQLSelectStatementRep();

    /**  This is the constructor for the CQLSelectStatement object.  
           The ctor requires 3 parameters:   
                 query language (qlang) which is CQL,  
                 the query itself, 
                 and the name of the CIM namespace.
       */
    CQLSelectStatementRep(
        /**  The query language is needed so the
              base class can retrieve the query language.
              The class member variable where this data
              is stored is located in the base SelectStatement
              class.
           */
        String& inQlang, 
        /**  input parameter containing the query string.
           */
        String& inQuery, 
        
        QueryContext* inCtx);

    CQLSelectStatementRep(const CQLSelectStatementRep& rep);

    ~CQLSelectStatementRep();

    CQLSelectStatementRep& operator=(const CQLSelectStatementRep& cqlss);
    
    /**  Implements the evaluate method from the
          base SelectStatement class.
    
       */
    Boolean evaluate(
        /**  The CIM instance that will be evaluated.
               The CIMInstance object is not modified by this method.
           */
        const CIMInstance& inCI);


    /**  Implements the applyProjection method from the
          base SelectStatement class.
       */
    void applyProjection(
        /**  Input the CIMInstance object in which to apply the
              projection.
           */
        CIMInstance& inCI) throw(Exception);

    /**  Implements the validatedClass method from the
          base SelectStatement class.
       */
    void validateClass(const CIMObjectPath& inClassName) throw(Exception);

    /**  Implements the validatedProperties method from the
          base SelectStatement class.
       */
    void validateProperties() throw(Exception);

    /** Returns an array of CIMObjectPath objects that are the 
          class paths from the select statement in the FROM list.
     */
    Array<CIMObjectPath> getClassPathList();

    /** Returns the required properties from the combined SELECT and WHERE
         clauses for the classname passed in.
    
         If all the properties for the input classname are required, a null
         CIMPropertyList is returned.
       */
    CIMPropertyList getPropertyList(
        /**  The input parameter className is one of the
              classes from the FROM list.
           */
        const CIMObjectPath& inClassName);

    /** Modifier. This method should not be called by the user (only by the
            parser).
         Appends a CQLIdentifier to an array of CIMObjectPaths from the FROM
    statement.
        */
    void appendClassPath(
        /**  
           */
        const CQLIdentifier& inIdentifier);

    /** Sets a predicate into this object. This method should only
            be called by Bison.
        */
    void setPredicate(
        // CQLOperation is an enum similar to the WQLOperation enum in CVS:
        // enum CQLOperation
        // {
        //     <,
        //     >,
        //     =,
        //    >=,
        //    <=,
        //     +,
        //     -,
        //     *,
        //     /,
        //     CQL_IS_NULL,
        //     CQL_IS_NOT_NULL,
        //     CQL_IS_A,
        //     CQL_LIKE
        // };
        CQLPredicate inPredicate);

    /**  This method calls QueryContext::insertClassPathAlias()  to insert a
    classpath-alias pair
          into the hash table.  The record is keyed by the class alias.
    
         This method is used by Bison.
    
        TODO:  think about exceptions such as duplicate key.
     */
    void insertClassPathAlias(
        /**  The CQLIdentifier object that contains the class path.
           */
        const CQLIdentifier& inIdentifier, 
        /**  The alias for the class.
           */
        String inAlias);

    /** Appends a CQL chained identifier to the CQL identifier list. The user
    should
            not call this method; it should only be called by the Bison.
        */
    void appendSelectIdentifier(const CQLChainedIdentifier& x);

    /** Appends a CQL Identifier to the where CQL identifier list. The user
    should not call this method; it should only be called by Bison.
    
            @param x  the CQL identifier.
            @return false if that identifier already exists.
    
    
        Since the identifiers in the WHERE clause must be distinct,
        a boolean is returned indicating if the identifier is not distinct.
    
     TODO:  THIS MAY BE NEEDED IN A FUTURE RELEASE.
       NOT IMPLEMENTED IN PEGASUS V2.5
        */
    Boolean appendWhereIdentifier(
        /**  Input the chained CQL identifiers to append. 
           */
        const CQLChainedIdentifier& x);

    /**
       Applies the class contexts from the FROM list to the identifiers
       in the statement.
     */
    void applyContext();

    /**
       Normalizes the WHERE clause to disjunction of conjunctions.
     */
    void normalizeToDOC();

    String toString();

    void setHasWhereClause();

    Boolean hasWhereClause();

    void clear();

  protected:
    /** 
        // The list of CQL identifiers being selected. For example, see
    "firstName",
        // and "lastName" below.
        //
        //     SELECT firstName, lastName 
        //     FROM TargetClass
        //     WHERE ...
        //
        // NOTE: duplicate identifiers are not removed from the select list 
        // (e.g. SELECT firstName, firstName FROM...) results in a list of 
        // two identifiers
        //
    
        */
    Array<CQLChainedIdentifier> _selectIdentifiers;

    /** 
        // The unique list of CQL query identifiers appearing in the WHERE clause.
        // Although a property may occur many times in the WHERE clause, it will
        // only appear once in this list.
        //
    
       TODO:  THIS MAY BE NEEDED IN A FUTURE RELEASE.
       NOT IMPLEMENTED IN PEGASUS V2.5
       */
    Array<CQLChainedIdentifier> _whereIdentifiers;

    Boolean _hasWhereClause;

  private:

    void applyProjection(EmbeddedPropertyNode* node, CIMProperty& nodeProp);

    void removeUnneededProperties(CIMInstance& inst, 
				  Array<CIMName>& requiredProps);

    void validateProperty(CQLChainedIdentifier& chainId);

    CIMName lookupFromClass(const String&  lookup);

    Boolean addRequiredProperty(Array<CIMName>& reqProps,
				CIMClass& theClass,
				CQLChainedIdentifier& chainId);

    Boolean containsProperty(const CIMName& name,
			     const Array<CIMName>& props);

    Boolean isSubClass(const CIMName& derived,
		       const CIMName& base); 

    CQLPredicate _predicate;
};

PEGASUS_NAMESPACE_END

#endif 
