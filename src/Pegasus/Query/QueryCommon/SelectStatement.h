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

#ifndef Pegasus_SelectStatement_h
#define Pegasus_SelectStatement_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Query/QueryCommon/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Query/QueryCommon/QueryContext.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_QUERYCOMMON_LINKAGE SelectStatementRep;

/** This class is an abstract base class for the query language (e,g. WQL or
CQL) select statement.

      A example of a simple query language SELECT statement may take the
following form although
     CQL architecture includes much more extensive forms of the SELECT
statement:

    <pre>
        SELECT <select-list>
        FROM <class list>
        WHERE <selection expression>
    </pre>

    There are methods for obtaining the various elements of the select
statement.

    See the concrete subclasses for specific information on each type of query
language
    select statement.
*/
class PEGASUS_QUERYCOMMON_LINKAGE SelectStatement
{
public:

    virtual ~SelectStatement();

    /**
       Returns the query language (WQL or CQL).
     */
    String getQueryLanguage() const;

    /**  Returns the query string.
     */
    String getQuery() const;

    virtual void setQueryContext(QueryContext& inCtx);

    /** This method operates on a single CIMInstance.

         Returns a boolean value indicating the evaluation result:
            TRUE means the CIMInstance passed conforms to the
                       criteria on the WHERE clause
            FALSE means the CIMInstance passed does not
                       conform to the criteria on the WHERE clause
     */
    virtual Boolean evaluate(

        /**  The CIM instance that will be evaluated.
               The CIMInstance object is not modified by this method.
           */
        const CIMInstance& inCI) = 0;

    /** applyProjection() method operates on a single CIMInstance to
        determine what properties to include.
        On that CIMInstance it will remove all properties that are not
        included in the projection.

        @param  allowMissing  Boolean specifying whether missing project
                              properties are allowed

        TODO:  document the exceptions!
     */
    virtual void applyProjection(

    /**  Input the CIMInstance object in which to apply the
         projection.

        @param  allowMissing  Boolean specifying whether missing project
                              properties are allowed
     */
        CIMInstance& inCI,
        Boolean allowMissing) = 0;

    /** Validates that all the property name identifiers actually exist on a
        class from the FROM list of the query string.  It checks the class
        in the default namespace (passed on the CTOR) in the repository.

        An exception is thrown if a property is not valid.
        TODO: document the exceptions.
              repository errors, namespace not found, etc.
     */
    virtual void validate() = 0;

    /** Returns an array of CIMObjectPath objects that are the
        class paths from the select statement in the FROM list.
     */
    virtual Array<CIMObjectPath> getClassPathList() = 0;

    /** Returns the required properties from the combined SELECT and WHERE
        clauses for the classname passed in.
        This is a pure virtual function that must be implemented in one or more
        subclasses.

        If all the properties for the input classname are required, a null
        CIMPropertyList is returned.
    */
    virtual CIMPropertyList getPropertyList(

    /**  The input parameter className is one of the classes from the FROM list.
    */
    const CIMObjectPath& inClassName) = 0;

    /**
        Returns the required properties from the SELECT clause for the specified
        class.

        This is a pure virtual function that must be implemented in one or more
        subclasses.

        @param  inClassName  name of the class; must be one of the classes from
                             the FROM clause

        @return  CIMPropertyList containing the required properties from the
                 SELECT clause for the specified class;
                 or a null CIMPropertyList if all properties of the specified
                 class are required
    */
    virtual CIMPropertyList getSelectPropertyList
        (const CIMObjectPath& inClassName) = 0;

    /**
        Returns the required properties from the WHERE clause for the specified
        class.

        This is a pure virtual function that must be implemented in one or more
        subclasses.

        @param  inClassName  name of the class; must be one of the classes from
                             the FROM clause

        @return  CIMPropertyList containing the required properties from the
                 WHERE clause for the specified class;
                 or a null CIMPropertyList if all properties of the specified
                 class are required
     */
    virtual CIMPropertyList getWherePropertyList
        (const CIMObjectPath& inClassName) = 0;

protected:
    SelectStatement();

    SelectStatement(const SelectStatement& ss);

    SelectStatementRep* _rep;

private:
    SelectStatement& operator=(const SelectStatement& rhs);
};

PEGASUS_NAMESPACE_END
#endif
#endif
