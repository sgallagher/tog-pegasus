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
// Author: Dave Rosckes (rosckes@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PEGASUS_CQLVALUE_H
#define PEGASUS_CQLVALUE_H


#include <Pegasus/Common/Config.h>

#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/CQL/CQLChainedIdentifier.h>
#include <Pegasus/CQL/CQLScope.h>

#include <Pegasus/CQL/Linkage.h>

PEGASUS_NAMESPACE_BEGIN
class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLValueRep;

/** The CQLValue class encapulates a value
     that is a CQL value.  The possible CQLValue
     types are the following:

         Sint64
         Uint64
         String
         CIMDateTime
         CIMReference
         CQLIdentifier


     This class can resolve an identifier to a primitive 
     value such as Sint64, Uint64 or String, CIMDateTime,
     and CIMReference.  

    This class overloads and performs type checking
     on the following operators:
         <, >, =, >=. <=. <>

    This class overloads, performs type checking and
     uint64 handling on the following operators:
           +. -. *, /

    NOTE:  the CQLValue class assumes a symbolic constant
                is fully qualified.

  */

class PEGASUS_CQL_LINKAGE CQLValue
{
  public:
    /**  Initializes object as a string.
       */
  enum NumericType  { Hex, Binary,  Decimal, Real};
  enum CQLValueType { Null_type, Sint64_type, Uint64_type, Real_type, String_type,
		      CIMDateTime_type,  CIMReference_type, CQLIdentifier_type,
		      CIMInstance_type, Boolean_type, CIMClass_type};

    CQLValue();
    ~CQLValue();
    CQLValue(const CQLValue& val);
    CQLValue(String inString, NumericType inValueType,
        Boolean inSign = true);

    /**  Initializes object as a CQLIdentifier.
       */
    CQLValue(CQLChainedIdentifier inCQLIdent);

   /**  Initializes object as a CIMObjectPath.
       */
    CQLValue(CIMObjectPath inObjPath);

   /**  Initializes object as a CIMDateTime.
       */
    CQLValue(CIMDateTime inDateTime);

    /**  Initializes object as a literal string (non-numeric).
       */
    CQLValue(String inString);

   CQLValue(Sint64 inSint);
   CQLValue(Uint64 inUint);
   CQLValue(Real64 inReal);
   CQLValue(CIMInstance inInstance);
   CQLValue(Boolean inBool);
   CQLValue(CIMClass inClass);
   CQLValue(CQLValueRep rhs);

    /**  This method is used to ask an identifier to resolve itself to 
           a number or string (primitive value).
    
          Resolves:
           - symbolic constants
           - indexing an array
           - decoding identifier
                * URI reference
                * embedded object (e.g. myclass.embeddedobject.prop1)
                    and this is recursive
           - class aliasing
     */

    void resolve(const CIMInstance& CI,const  QueryContext& inQueryCtx);

    CQLValue& operator=(const CQLValue& rhs);

    /** operator == compares two CQLValue objects for equality and
                            performs type checking.
        @param x - First CQLValue to compare
        @param y - Second CQLValue to compare
        @return true if they are identical in type, attribute and value,
        false otherwise.
    */

    Boolean operator==(const CQLValue& x);

    /** operator == compares two CQLValue objects for inequality and
                            performs type checking.
        @param x - First CQLValue to compare
        @param y - Second CQLValue to compare
        @return true if they are NOT identical in type, attribute and value,
        false otherwise.
    */

    Boolean operator!=(const CQLValue& x);

    /** operator <= compares two CQLValue objects to see if x is less than or
                            equal to y and performs type checking.
        @param x - First CQLValue to compare
        @param y - Second CQLValue to compare
        @return true if x is less than or equal to y,
        false otherwise.
    */

    Boolean operator<=(const CQLValue& x);

    /** operator >= compares two CQLValue objects to see if x is greater than or
                            equal to y and performs type checking.
        @param x - First CQLValue to compare
        @param y - Second CQLValue to compare
        @return true if x is greater than or equal to y,
        false otherwise.
    */

    Boolean operator>=(const CQLValue& x);

    /** The overload operator (<) compares CQLValue obects and
          performs type checking.
        <pre>
            CQLValue   val1 = "def";
            CQLValue   val2 = "a";
            assert (val2 < val1);
        </pre>
    */

    Boolean operator<(const CQLValue& val1);

    /** The overload operator (>) compares CQLValue obects and
          performs type checking.
        <pre>
            CQLValue   val1 = "def";
            CQLValue   val2 = "a";
            assert (val2 < val1);
        </pre>
    */

    Boolean operator>(const CQLValue& val1);

    /** This overload operator (+) concatenates or adds the value of
    CQLValue objects and performs type checking and Uint64 handling.
          For example, 
        <pre>
            CQLValue  t1 = "abc";  // it's a string type
            CQLValue  t2 = "def";   // it's another string type
            CQLValue   t3 = t1 + t2;
            assert(t3 == "abcdef");
        </pre>
    */
 
    CQLValue operator+(const CQLValue& x);

    /** This overload operator (-) subtracts the value of CQLValue
          objects and performs type checking and Uint64 handling.
          For example, 
        <pre>
            CQLValue  t1 = 5;  
            CQLValue  t2 = 2;   
            CQLValue   t3 = t1 - t2;
            assert(t3 == 3);
        </pre>
    */
 
    //CQLValue operator-(const CQLValue& x);

    /** This overload operator (*) multiplies the values of CQLValue
          objects and performs type checking and Uint64 handling.
          For example, 
        <pre>
            CQLValue  t1 = 3;  
            CQLValue  t2 = 2;   
            CQLValue   t3 = t1 *  t2;
            assert(t3 == 6);
        </pre>
    */

    //CQLValue operator*(const CQLValue& x);

    /** This overload operator (*) divides the values of CQLValue
          objects and performs type checking and Uint64 handling.
          For example, 
        <pre>
            CQLValue  t1 = 6;  
            CQLValue  t2 = 2;   
            CQLValue   t3 = t1 /  t2;
            assert(t3 == 2);
        </pre>
    */

    //CQLValue operator/(const CQLValue& x);

    /** Accessor for getting the type of the CQLValue.
    
    TODO:  do we need this method?
      */

    CQLValueType getValueType();

    /** Clears this object and sets its type to NULL_VALUE
      */

    void setNull();

    /** Returns whether the value has been resolved to a primitive.
      */

    Boolean isResolved();

    /** Returns whether the CQLValue is null.
      */

    Boolean isNull();

    /** Tests to see if this "isa" the input string.
    */

    Boolean isa(const CQLChainedIdentifier& cid, QueryContext& QueryCtx);

    /** Tests to see if this "like" the input string.
          Both sides of the LIKE comparison must have a String type:
          the result is a Boolean type.
    
         The LIKE comparison allows a string to be tested by pattern-matching,
          using special characters n the right-hand-side string.
          See the DMTF CQL Specification for details.
    
          For Basic Query, the left-hand expression (this) may be restricted to
    chain, and 
          the right-hand expression may be restricted to be a literal-string.
    */

    Boolean like(const CQLValue& inVal);
    /*
   void invert();
    */
   CQLChainedIdentifier getChainedIdentifier()const;
   Uint64 getUint()const;
   Sint64 getSint()const;
   Real64 getReal()const;
   String getString()const;
   Boolean getBool()const;
   CIMDateTime getDateTime()const;
   CIMObjectPath getReference()const;
   CIMInstance getInstance()const;
   CIMClass getClass()const;
   String toString()const;
   void applyContext(QueryContext& _ctx,
                     CQLChainedIdentifier& inCid);
   friend class CQLFactory; 

  private:


   CQLValueRep *_rep;
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLValue
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END

#endif /* CQLVALUE_H */
