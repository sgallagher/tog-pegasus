//%/////////-*-c++-*-////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Day (mdday@us.ibm.com)
//
// Modified By:
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_Predicate_h
#define Pegasus_Predicate_h



#include <Pegasus/Common/IPC.h>
#include <Pegasus/Common/DQueue.h>
#include <iostream>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>

#include <Pegasus/Common/CIMReference.h>

PEGASUS_NAMESPACE_BEGIN


/** The Predicate class adds a logical expression, truth value, 
    and one or more operators to the KeyBinding class. Using the 
    Predicate class, it is possible to construct evaluate a compiled
    expression using actual cim data values.
*/

typedef enum LogicalOperator { AND, NOT, OR };
typedef enum ExpressionOperator { EQUAL, NE, GT, GTE, LT, LTE, PRESENT };

class PEGASUS_COMMON_LINKAGE Predicate: public KeyBinding
{
   public:


      Predicate( ) ;
      Predicate(const Predicate& x) ;
      Predicate(const KeyBinding& x, 
		ExpressionOperator op, 
		Boolean truth) ;
      Predicate(const String& name, 
		const String& value, 
		Type type, 
		ExpressionOperator op = EQUAL, 
		Boolean truth_value = true );

      ~Predicate();

      Predicate& operator=(const Predicate& x);
  
      ExpressionOperator getOperator(void) const
      {  
	 return _op;
      }

      void setOperator(ExpressionOperator op)
      {
	 _op = op;
      }
      
      Boolean getTruth(void) const
      {
	 return _truth_value;
      }

      Boolean evaluate(KeyBinding& key);

   private:
      void setTruth(Boolean truth)
      {
	 _truth_value = truth;
      }
      ExpressionOperator _op;
      Boolean _truth_value;
} ;


// PredicateTree represents a compiled wql expression or LDAP filter
// it contains nested predicates that correspond to CIMReferences.
// e.g.,    AND
//           |
//        foo == 1, fee != 2, OR
//                             |
//                 namespace "nothing" PRESENT, namespace "another" PRESENT

// which would be TRUE when:
// foo == 1 AND fee != 2 AND ((nothing is present) OR (another is present))


class PEGASUS_COMMON_LINKAGE PredicateReference : public CIMReference
{
   public:
      PredicateReference();
      PredicateReference(const CIMReference& x);
      PredicateReference(const PredicateReference& x);
      PredicateReference(const String& objectName);
      PredicateReference(const char *objectName);
      static PredicateArray getPredicateArray();
      PredicateReference(
	 const String& host,
	 const String& nameSpace,
	 const String& className,
	 const KeyBindingArray& keyBindings = getKeyBindingArray(),
	 const PredicateArray& predicates = getPredicateArray(),
	 Boolean truth = true,
	 LogicalOperator lop = AND );
      
      
      ~PredicateReference();
      
      PredicateReference& operator=(const PredicateReference& x);
      PredicateReference& operator=(const CIMReference& x);
      
      void clear();
      
      void set(
	 const String& host,
	 const String& nameSpace,
	 const STring& className,
	 const KeyBindingArray& keyBindings = getKeyBindingArray(),
	 const PredicateArray& predicates = getPredicateArray(),
	 Boolean truth = true,
	 LogicalOperator lop = AND);

      void set(const String& objectName);
      
      PredicateReference& operator=(String& objectName)
      {
	 set(objectName);
	 return *this;
      }
      
      PredicateReference& operator=(const char *objectName)
      {
	 set(objectName);
	 return *this;
      }
      
      

      const String& getHost() const
      {
	 return _host;
      }


      void setHost(const String& host)
      {
	 _host = host;
      }

      const String& getNameSpace() const
      {
	 return _nameSpace;
      }


      void setNameSpace(const String& nameSpace);


      const String& getClassName() const
      {
	 return _className;
      }


      void setClassName(const String& className);


      const Array<KeyBinding>& getKeyBindings() const
      {
	 return _keyBindings;
      }

      */
      void setKeyBindings(const Array<KeyBinding>& keyBindings);

      String toString() const;

      String toStringCanonical() const;


      Boolean identical(const CIMReference& x) const;


      void toXml(Array<Sint8>& out, Boolean putValueWrapper = true) const;

      void toMof(Array<Sint8>& out, Boolean putValueWrapper = true) const;

      void print(PEGASUS_STD(ostream)& os = PEGASUS_STD(cout)) const;


      Uint32 makeHashCode() const;


      Boolean isInstanceName() const
      {
	 return _keyBindings.size() != 0;
      }

      Boolean isClassName() const 
      {
	 return !isInstanceName();
      }

   private:
      Boolean _truth_value;
      LogicalOperator _logical_op;
      Array<Predicate> _predicates;
}


// class PEGASUS_COMMON_LINKAGE PredicateTree
// {
//    public:

//       PredicateTree();
//       PredicateTree(String& expression);
//       ~PredicateTree();
//       PredicateTree(const PredicateTree& x);
//       PredicateTree& operator=(const Predicate& x);
//       Boolean evaluate(void);

//    private:
//       Boolean _truth_value;
//       LogicalOperator _logical_op;
//       DQueue<PredicateTree> _peers;
//       DQueue<PredicateTree> _children;
//       DQueue<PredicateReference> _predicates;
  
//       friend Boolean operator==(const PredicateTree& x, const CIMReference& y);

// } ;


// // To evaluate a predicate tree, go recursive:

// Boolean PredicateTree::evaluate(PredicateTree *tree, CIMReference& cim_reference)
// {
//    evaluate(_children, cim_reference);
//    evaluate(_peers, cim_reference);
//    _truth_value = evaluate(_logical_op, _predicates, cim_reference);
//    return(_truth_value);
// }


// // here is what the indication provider interface looks like using predicates

// class PEGASUS_PROVIDER_LINKAGE CIMIndicationProvider : public CIMBaseProvider
// {
//    public:
//       CIMIndicationProvider(void);
//       virtual ~CIMIndicationProvider(void);

   
//       //  OperationContext contains security credentials and other 
//       //  data that is unique to the provider/cimom

//       // IndicationName is the name given to this subscription 
   


//       // The entire predicate tree, given an array of CIM References, 
//       // can be evaluated to TRUE or FALSE. If TRUE, the Indication Provider 
//       // will FIRE the indication. 

//       // The Provider should take the Predicate Tree and store it in a way
//       // that optimizes monitoring and evaluation to determine if the predicate
//       // is true or not. 

//       // propertyList is the set of property values that will be built into 
//       // the indication itself when it fires. 
//       // 

//       virtual void enableIndication(
// 	 const OperationContext & context,
// 	 const String & indicationName,
// 	 const PredicateTree & ref, 
// 	 const Array<String> & propertyList) = 0;
   
//       virtual void disableIndication(
//          const OperationContext & context,
// 	 const String & indicationName) = 0;
// };


PEGASUS_NAMESPACE_END

#endif /* Predicate_h */
