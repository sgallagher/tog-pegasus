#ifndef Pegasus_CQLChainedIdentifierRep_h
#define Pegasus_CQLChainedIdentifierRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLIdentifier.h>
PEGASUS_NAMESPACE_BEGIN 
class PEGASUS_CQL_LINKAGE CQLScope;
class PEGASUS_CQL_LINKAGE CQLFactory;
/**  
CQLChainedIdentifier class is used for parsing (not resolving)
the identifier portion of a CQL select statement.  
A couple examples of a CQLIdentifier:

  Class.EmbeddedObject.Property[3]
  Class.Property#'OK'


The CQLChainedIdentifier class can parse and return each
 section of a CQL identifier.  The possible sections are:

     class (optional)
     zero or more embedded objects
     property

Note:  this must handle wildcard("*"), embedded objects, arrays, symbolic
constants, etc.
   */
class PEGASUS_CQL_LINKAGE CQLChainedIdentifierRep
{
  public:

    CQLChainedIdentifierRep();
    /**  constructor 
          Parses the string into the various components of a CQL identifier.
          Throws parsing errors.
      */
    CQLChainedIdentifierRep(String inString);

    CQLChainedIdentifierRep(CQLIdentifier &id);
 
    CQLChainedIdentifierRep(const CQLChainedIdentifierRep* rep);

    ~CQLChainedIdentifierRep();

    /**  The getSubIdentifiers method
    returns the array of subIdentifiers from a CQL SELECT
    statement.  If there are no subidentifiers, an empty array is returned.
    
    Example:
        myClass.embeddedObject.myProperty would return an array of 2 strings:
    'myClass' and 'embeddedObject'.
        myProperty would return an empty array.
    
      */
    const Array<CQLIdentifier>& getSubIdentifiers()const;

    String toString()const;

    void append(CQLIdentifier & id);

    Uint32 size();

    Boolean prepend(CQLIdentifier & id);

    CQLIdentifier& operator[](Uint32 index);

    CQLChainedIdentifierRep& operator=(const CQLChainedIdentifierRep& rhs);

    Boolean isSubChain(CQLChainedIdentifier & chain);

    CQLIdentifier getLastIdentifier();
	
    Boolean applyScopes();

    friend class CQLFactory;

  private:

    void parse(String & string);

    /**  The _subIdentifiers member variable is an array of Strings that are the
    parts of the CQL identifier that were delimited by the dots (.) except
          for the final part which is the _property member variable.
      */
    Array<CQLIdentifier> _subIdentifiers;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CQLChainedIdentifierRep_h */
