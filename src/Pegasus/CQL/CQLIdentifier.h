#ifndef Pegasus_CQLIdentifier_h
#define Pegasus_CQLIdentifier_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/CQL/SubRange.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLIdentifierRep;

/** 
  The CQLIdentifier class encapsulates
  the different formats of the CQL property portion
  of a CQLChainedIdentifier. 

  For example, a CQLChainedIdentifier can have these parts:
 
    Class.EmbeddedObject.Property
    Class.Property


The "Property" portion of the CQLChainedIdentifier can be
in any of these 3 formats:
   (a)  property name
   (b)  property[3]     e.g. an array index
   (c)  property#'OK'    e.g. a symbolic constant
   (d)  *   (wildcard)

In the future, there may be support added for a set of indices (ranges).
  */
class PEGASUS_CQL_LINKAGE CQLIdentifier
{

/*
Exceptions:
        CQLIdentifierException - for example: "missing matching brackets"
				       - general case if logic falls through all previous cases
                                                                                                                           
*/


  public:
    CQLIdentifier();
    /**  The constructor for a CQLIdentifier object
          takes a string as input.  The string should contain the
          property portion of a CQLChainedIdentifier.
    
         The constructor parses the input string into the components of 
         the property identifier. 
    
         Throws parsing errors.
      */
    CQLIdentifier(String identifier);

    CQLIdentifier(const CQLIdentifier& id);

    ~CQLIdentifier();

    /**
      The getPropertyName method returns the property name portion of 
      a property.   For example, of the property string contained myProperty#'OK',
      the property name is the "myProperty" portion.  In that case, "myProperty"
    would
      be returned in the CIMName reference.
      */
    const CIMName& getName()const;

    /**
      The getSymbolicConstantName method returns the symbolic name portion of 
      a property.   For example, of the property string contained property#'OK',
      the symbolic constant is the "OK" portion.  In that case, "OK" would
      be returned in the String reference.
      */
    const String& getSymbolicConstantName()const;

    /**  The getSubRanges method returns an array of SubRanges 
           which contain all the array index ranges from the property
           portion of a CQLChainedIdentifier.
           If the property is not an array (e.g. the property is just
           a property name or a symbolic constant), an empty 
           array is returned.
      */
    const  Array<SubRange>& getSubRanges()const;

    /** The isArray method returns TRUE if the 
         CQL property identifier is an array (e.g. the
         property is of the format "property[index]).
         Otherwise FALSE is returned.
      */
    Boolean isArray()const;

    /** The isSymbolicConstant method returns TRUE if the 
         CQL property identifier is a symbolic constant (e.g. the
         property is of the format property#'CONSTANT'.
         Otherwise FALSE is returned.
      */
    Boolean isSymbolicConstant()const;

    /** The isWildcard method returns TRUE if the 
         CQL property identifier is a wildcard (e.g. the
         property is of the format "*").
         Otherwise FALSE is returned.
      */
    Boolean isWildcard()const;

    const String& getScope()const;

    Boolean isScoped()const;

    void applyScope(String scope);

    CQLIdentifier& operator=(const CQLIdentifier& rhs);

    Boolean operator==(const CIMName &rhs)const;
                                                                                                                                       
    Boolean operator!=(const CIMName &rhs)const;

    Boolean operator==(const CQLIdentifier &rhs)const;

    Boolean operator!=(const CQLIdentifier &rhs)const;

    String toString()const;

  private:
	CQLIdentifierRep* _rep;

};


#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLIdentifier
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END

#endif
#endif /* Pegasus_CQLIdentifier_h */
