#ifndef Pegasus_SelectStatement_h 
#define Pegasus_SelectStatement_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/CQL/CQLPredicate.h>


#include <Pegasus/CQL/Linkage.h>


PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE SelectStatementRep;

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
class PEGASUS_CQL_LINKAGE SelectStatement
{
  public:
    SelectStatement(){};
    /**  This is the constructor for the SelectStatement object.  
           The ctor requires 3 parameters:   
                 query language (qlang) which is either WQL or CQL,
    the query itself, 
                 and the name of the CIM namespace.
       */
    SelectStatement(
        /** input parameter containing the type of
             query language to use in processing
             the query string.
        
            For example, if the query language is CQL,
            this string would contain "CQL".
        
            If the query language is WQL, this string
            would contain "WQL".
           */
        String inQlang, 
        /**  input parameter containing the query string.
           */
        String inQuery, 
        /** Input parameter containing a pointer to the
             QueryContext object to use for the accessing
             data in the repository.
           */
        QueryContext& inCtx);

    ~SelectStatement();

    /** 
       Returns the query language (WQL or CQL).
     */
    String getQueryLanguage();

    /**  Returns the query string.
     */
    String getQuery();

    /** This method operates on a single CIMInstance.
    
         Returns a boolean value indicating the evaluation result:
            TRUE means the CIMInstance passed conforms to the
                       criteria on the WHERE clause
            FALSE means the CIMInstance passed does not
                       conform to the criteria on the WHERE clause
     */
    Boolean evaluate(
        /**  The CIM instance that will be evaluated.
               The CIMInstance object is not modified by this method.
           */
        const CIMInstance inCI);

    /** Takes a set of CIMInstance objects and executes the query on each
         object.  This method processes each CIMInstance by calling:
    
            A.  evaluate() method to see if the CIMInstance conforms to the
                 criteria on the WHERE clause
    
            B.  applyProjection() method to see what properties to include
    
          Returns a subset of the inputted CIMInstances - only the CIMInstance
          objects that conform to the query.
    
          TODO:  document the exceptions.
     */
    Array<CIMInstance> executeQuery(
        /** Input parameter that is an array of CIM Instance objects on which
             to execute the query.
           */
        Array<CIMInstance> inCIMInstanceArray) throw(Exception);

    /** applyProjection() method operates on a single CIMInstance to
    determine what properties to include.
         On that CIMInstance it will remove all propoerties that are not
         included in the projection.
    
        TODO:  document the exceptions!
     */
    CIMInstance applyProjection(
        /**  Input the CIMInstance object in which to apply the
              projection.
           */
        CIMInstance inCI) throw(Exception);

    /** This method validates that the class passed in exists 
          both in the FROM list (in the query string)
          and in the namespace.
         This method uses the default namespace passed in the CTOR,
         and the FROM list from the query string passed in the CTOR.
    
          An exception is thrown if a class does not exist in the namespace
          or the FROM list.
          TODO: document the exceptions.
             repository errors, namespace doesn't exist, etc.
     */
    void validateClass(const CIMObjectPath& inClassName) throw(Exception);

    /** Validates that all the property name identifiers actually exist on a
    class from the FROM list of the query string.  It checks the class
         in the default namespace (passed on the CTOR) in the repository.
    
          An exception is thrown if a property is not valid.
          TODO: document the exceptions.
             repository errors, namespace not found, etc.
     */
    void validateProperties() throw(Exception);

    /** Returns an array of CIMObjectPath objects that are the 
          class paths from the select statement in the FROM list.
     */
    Array<CIMObjectPath> const getClassPathList();

    /** Returns the required properties from the combined SELECT and WHERE
         clauses for the classname passed in.
         This is a pure virtual function that must be implemented in one or more
         subclasses.
    
         If all the properties for the input classname are required, a null
         CIMPropertyList is returned.
       */
    CIMPropertyList getPropertyList(
        /**  The input parameter className is one of the
              classes from the FROM list.
           */
        const CIMObjectPath& inClassName);

  protected:

  	SelectStatementRep* _rep;

};

PEGASUS_NAMESPACE_END

#endif 
