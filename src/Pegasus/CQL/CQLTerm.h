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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CQLTerm_h
#define Pegasus_CQLTerm_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/Linkage.h>
#include <Pegasus/CQL/CQLFactor.h>

#ifdef PEGASUS_USE_EXPERIMENTAL_INTERFACES

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_CQL_LINKAGE CQLFactory;
class PEGASUS_CQL_LINKAGE CQLTermRep;
class PEGASUS_CQL_LINKAGE QueryContext;

/** 
    enum of multiply, divide and concatenation operators.
*/
enum FactorOpType { mult, divide, concat };

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T FactorOpType
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

/**
    The CQLTerm class encapsulates a generic CQL term to make it 
    easier to break into pieces (factors) and process the term.  
    A CQL term is made up of factors and operators. 
    There must be exactly one more factor than there are operators.
*/

class PEGASUS_CQL_LINKAGE CQLTerm
{
 public:

  /** 
      Contructs CQLTermRep default object.
      
      @param  - None.
      @return - None.
      @throw  - None.
      @experimental
  */
  CQLTerm();

  /** 
      Contructs CQLTermRep from a CQLFactor object.
      
      @param  - theFactor is a CQLFactor object.
      @return - None.
      @throw  - None.
      @experimental
  */
  CQLTerm(const CQLFactor& theFactor);

  /** 
      Contructs CQLTermRep from a CQLTermRep object. (copy-constructor)
      
      @param  - rep is a CQLTermRep object.
      @return - None.
      @throw  - None.
      @experimental
  */
  CQLTerm(const CQLTerm& inTerm);

  /** 
      Destroyes CQLTermRep object.
     
      @param  - None.
      @return - None.
      @throw  - None.
      @experimental
  */  
  ~CQLTerm();
  
  /** the resolveValue method evaluates the CQL Term and returns the value.
      Any property that need to be resolved into a value is taken from the
      CIMInstance.
      
      @param  - CI - The CIMInstance used for the evaluate.
      @param  - QueryCtx - Reference to the Querycontext object.
      @return - CQLValue - The value of the object being resolved.
      @throw  - CQLRuntimeException 
      @experimental
  */
  CQLValue resolveValue(const CIMInstance& CI, const QueryContext& QueryCtx);
  
  /** The function is used by Bison.
      It is invoked 0 or more times for the CQLTerm, and
      when invoked will always pass in an integer that is the Factor operation
      type and a CQLFactor object.
     
      @param  - inFactorOpType - next operation to be performed.
      @param  - inFactor - next CQLFactor to be operated on.
      @return - None.
      @throw  - None.
      @experimental
  */
  void appendOperation(FactorOpType inFactorOpType, CQLFactor inFactor);

  /** 
      Converts the contents of the object to a String.
      
      @param  - None.
      @return - A string version of the object.
      @throw  - None.
      @experimental
  */  
  String toString()const;

  /** 
      Returns true if the object is simple.  Simple is defined as only having
      a one CQLFactor object. The CQLFactor object could be complex.
      
      @param  - None.
      @return - true or false
      @throw  - None.
      @experimental
  */
  Boolean isSimple()const;

  /** 
      Returns true if the value within the object is Simple.  In this case
      the object only has one CQLFactor object.  This CQLFactor object in 
      not a complex object.
      
      @param  - None.
      @return - true or false
      @throw  - None.
      @experimental
  */
  Boolean isSimpleValue()const;

  /** 
      Will return an array of CQLFactors.
      
      @param  - None.
      @return - An array of CQLFactor
      @throw  - None.
      @experimental
  */
  Array<CQLFactor> getFactors()const;

  /** 
      Will return an array of Operations.
      
      @param  - None.
      @return - an array of FactorOpType
      @throw  - None.
      @experimental
  */  
  Array<FactorOpType> getOperators()const;

  /** 
      Calling applyContext function for every internal object.  This
      will fully qualify the Chained Identifiers within all the CQLValue objects.
      
      @param  - inContext - Query Context used to access the repository.
      @param  - inCid - Chained Identifier used for standalone symbolic constants.
      @return - None.
      @throw  - None.
      @experimental
  */  
  void applyContext(QueryContext& inContext,
		    CQLChainedIdentifier& inCid); 

  /** 
      Assignment operator.
      
      @param  - rhs - a CQLTerm to be assined.
      @return - a reference of a CQLTerm.
      @throw  - None.
      @experimental
  */ 
  CQLTerm& operator=(const CQLTerm& rhs);

  
  friend class CQLFactory;
  
 private:
  
  CQLTermRep *_rep;
};

#ifndef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CQLTerm
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T
#endif

PEGASUS_NAMESPACE_END
#endif
#endif 
