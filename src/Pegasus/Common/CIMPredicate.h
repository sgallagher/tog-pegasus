//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Linkage.h>

//#include <iostream>

PEGASUS_NAMESPACE_BEGIN

// REVIEW: Where are these classes used?
   
extern void PEGASUS_COMMON_LINKAGE _BubbleSort(Array<KeyBinding>& x);
enum LogicalOperator { AND, NOT, OR };
enum ExpressionOperator { EQUAL, NE, GT, GTE, LT, LTE, PRESENT };


/** 
    Identity operator for Predicate objects. 
*/
inline Boolean operator==(const Predicate& x, const Predicate& y)
{
   return
      x.getType() == y.getType() &&
      x._op == y._op &&
      x._truth_value == y._truth_value &&
      CIMName::equal(x.getName(), y.getName()) &&
      String::equal(x.getValue(), y.getValue());
}

/**
   Determines if a Predicate object and a KeyBinding object both
   refer to the same property name. This is as close as we can get
   to identity for a Predicate with the context of a KeyBinding. 
*/
inline Boolean operator==(const Predicate& p, const KeyBinding& k)
{
   return
      CIMName::equal(p.getName(), k.getName());
}


/** 
A Predicate is derived from the keybinding class, with the addition
of a logical value (true or false) and an operator (greater-than, 
less-than, equal, etc.). 

With a predicate and a keybinding, you can do logical evaluations 
of expressions that are constructed from CIM instance values. For 
example, with a predicate:

&lt;predicate&gt; operator &lt;value&gt;

and a keybinding:

&lt;property&gt; &lt;value&gt;

you can do the following comparison:

&lt;property.value&gt; predicate.operator &lt;predicate.value&gt;

In other words, the predicate allows you evaluate the truth of 
expressions such as &quot;disk_space > 12MB&quot;

The predicate class may have a multi-valued  array of predicates 
and values. In every case, however, there is only one operator. 
When there are many predicates and values, the predicate object 
evaluates to true if and only if each predicate and value pair 
evaluates to true. 

A predicate can only be evaluated in conjunction with  a keybinding. 
Both the predicate and the keybinding are assumed to have properties
of the same name. If not, the evaluation is automatically false. 

Predicates and keybindings by themselves do not contain enough 
information to resolve their properties to specific CIM instances.
The necessary context for resolving a predicate or keybinding to 
a specific instance is provided through a CIMObjectPath or 
PredicateReference object. Hence, Predicates are usually contained
by a PredicateReference object. (Just as keybindings are usually 
contained by a CIMObjectPath object.)

*/

class  PEGASUS_COMMON_LINKAGE Predicate: public KeyBinding
{
   public:
   
      Predicate( ) ;
      /** 
	  Copy constructor
      */
      Predicate(const Predicate& x) ;
      /** 
	  Create a predicate by copying an existing KeyBinding, then adding
	  an expression operator and a truth value. 
      */
      Predicate(const KeyBinding& x, 
		ExpressionOperator op, 
		Boolean truth );

      /**
	 Create a predicate from its constituent parts. 
      */

      Predicate(const String& name, 
		const String& value, 
		Type type, 
		ExpressionOperator op = EQUAL, 
		Boolean truth_value = true );

      ~Predicate();
      
      /**
	 Evaluate a predicate. Construct an expression by combining the 
	 predicate's property name and operator with the keybinding's property value. 
	 Then deterine the truthfulness of the expression and set the predicate's truth
	 value accordingly. 
	 For example:

	 <pre>
	 <code>
	 KeyBinding kb = KeyBinding("bytes_per_second", "4501", NUMERIC);
	 Predicate pr = Predicate("bytes_per_second", "3000", NUMERIC, GT );

	 pr.evaluate(kb);
	 if(pr.getTruth() == true)
	    cout &lt;&lt; "expression is true"
	 </code>
	 </pre>

	 The code above will construct and evaluate the following expression:

	 <pre>
	 <code>
	 (KeyBinding.value Predicate.Operator Predicate.Value)
	 
	 or, 

	 (4501 &gt; 3000) 
	 </code>
	 </pre>

	 @param key provides a complete KeyBinding object, which contains the actual 
	 CIM property value that will be used by the Predicate to construct and 
	 evaluate the expression. 

      */
      Boolean evaluate(const KeyBinding& key);


      /**
	 Assignment constructor
      */
      Predicate& operator=(const Predicate& x);
  
      /**
	 operator Accessor 
      */
      ExpressionOperator getOperator(void) const
      {  
	 return _op;
      }
	
      void setOperator(ExpressionOperator op)
      {
	 _op = op;
      }
      
      /** 
	  Truth value Accessor 
      */
      Boolean getTruth(void) const
      {
	 return _truth_value;
      }


   private:
      void setTruth(Boolean truth)
      {
	 _truth_value = truth;
      }
      ExpressionOperator _op;
      Boolean _truth_value;
      friend Boolean operator==(const Predicate& x, const Predicate& y);
      friend Boolean operator==(const Predicate& p, const KeyBinding& k);
      friend class PredicateReference;
      
} ;



#define PEGASUS_ARRAY_T Predicate
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

typedef Array<Predicate> PredicateArray;

/**
   The PredicateReference class encapsulates a CIMObjectPath and combines it with 
   a Predicate object. This allows predicate evaluations of specific CIM 
   objects. One way to think of the PredicateReference class is that it is a 
   container for CIM instances and expressions that refer to CIM instances.

   Or, put differently, a PredicateReference provides a context within which
   Predicates and KeyBindings (see documentation for those classes) may be
   evaluated as logical expressions. 

   The PredicateReference class also has properties to enable timing and hysterises
   when evaluating expressions that may cause an indication to be generated. 
   These include the check interval and the persistence interval. 

   The check interval controls how frequently a PredicateReference should be 
   evaluated. Each evaluation causes at least one property value to be retrieved
   and compared. 

   The persistence interval control how long an expression must remain constant
   before it can be used in an evaluation. For example, a temperature reading must
   remain above a threshold for some interval, such as 30 seconds, before the 
   temperature can be deemed too hot.
 
*/
class PEGASUS_COMMON_LINKAGE PredicateReference : public CIMObjectPath
{
   public:
      
      /** ATTN;
      */
      PredicateReference();
      PredicateReference(const CIMObjectPath& x);
      PredicateReference(const PredicateReference& x);
      PredicateReference(const String& objectName);
      PredicateReference(const char *objectName);
      static PredicateArray getPredicateArray();
      PredicateReference(
	 const String& host,
	 const String& nameSpace,
	 const String& className,
        //
        //  NOTE: Due to a bug in MSVC 5, the following will not work on MSVC 5
        //
	 const KeyBindingArray& keyBindings = KeyBindingArray (),
	 const PredicateArray& predicates = getPredicateArray(),
	 Boolean truth = true,
	 LogicalOperator lop = AND );
      
      
      ~PredicateReference();
      
      PredicateReference& operator=(const PredicateReference& x);
      PredicateReference& operator=(const CIMObjectPath& x);
      
      /** 
	  Clear the underlying reference object and all the predicates. 
      */
      void clear();
      
      /** 
	  Initialize the object using parameters. 
	  See the documentation of the CIMObjectPath object for the
	  host, nameSpace, className, and keyBindings parameters. 

	  @param predicates Array of Predicate objects
	  @param truth Explicitly set the truth value to this parameter
	  @param lop Logical operator for this object - must be AND, OR, NOT
      **/
      void set(
	 const String& host,
	 const String& nameSpace,
	 const String& className,
        //
        //  NOTE: Due to a bug in MSVC 5, the following will not work on MSVC 5
        //
	 const KeyBindingArray& keyBindings = KeyBindingArray (),
	 const PredicateArray& predicates = getPredicateArray(),
	 Boolean truth = true,
	 LogicalOperator lop = AND);

      /** 
	  Construct an empty Predicate reference and initialize it with 
	  the object name passed in the only parameter. 
      */
      PredicateReference& operator=(String& objectName)
      {
	 CIMObjectPath::set(objectName);
	 return *this;
      }
      
      /** 
	  Construct an empty Predicate reference and initialize it with 
	  the object name passed in the only parameter. 
      */
      PredicateReference& operator=(const char *objectName)
      {
	 CIMObjectPath::set(objectName);
	 return *this;
      }

      /**
	 Accessor for Predicate array. See the documentation for the 
	 Predicate object for further information. 
      */
      const Array<Predicate>& getPredicates() const
      {
	 return _predicates;
      }

      /** 
	  Initialize this Predicate Reference with an array 
	  of Predicate objects. 
	  
	  @param predicates Array of initialized Predicate objects that 
	  will be added to this PredicateReference object. 
      */
      void setPredicates(const Array<Predicate>& predicates);


      /**
	 Provide the CIMOM or any intelligent providers with a hint
	 concerning how frequently this PredicateReference should be 
	 evaluated for a truth value. 
	 
	 @param tv Recommended interval for evaluating this PredicateReference
      */
      void setCheckInterval(const struct timeval *tv)
      {
	 if(tv == NULL)
	 {
	    _check_interval.tv_sec = _check_interval.tv_usec = 0;
	 }
	 else
	 {
	    
	    _check_interval.tv_sec = tv->tv_sec;
	    _check_interval.tv_usec = tv->tv_usec;
	 }
      }
      
      /**
	 Accessor for check interval property.

	 @param buffer Pointer to a buffer into which the 
	 check interval for this object should be placed. 
      */
      void getCheckInterval(struct timeval *buffer) const 
      {
	 if(buffer != NULL)
	 {
	    buffer->tv_sec = _check_interval.tv_sec;
	    buffer->tv_usec = _check_interval.tv_usec;
	 }
      }
      
      /**
	 The persistence interval is a hint to the CIMOM and to intelligent providers
	 that this PredicateReference should retain a constant evaluated value for at 
	 least the specified time period before changing the truth value of the object. 

	 This is useful for hysterisis; i.e., to ensure that indications are not triggered
	 to frequently. 

	 @param tv Pointer to a <code>timeval</code> struct containing the new persistence 
	 interval for this object. 
      */
      void setPersistInterval(const struct timeval *tv)
      {
	 if(tv == NULL)
	 {
	    _persist_interval.tv_sec = _persist_interval.tv_usec = 0;
	 }
	 else
	 {
	    
	    _persist_interval.tv_sec = tv->tv_sec;
	    _persist_interval.tv_usec = tv->tv_usec;
	 }
      }
      
      /**
	 Accessor for object's persistence interval. 

	 @param buffer Pointer into which the object will place a copy of its 
	 persistence interval. 
      */ 
      void getPersistInterval(struct timeval *buffer) const 
      {
	 
	 if(buffer != NULL)
	 {
	    buffer->tv_sec = _persist_interval.tv_sec;
	    buffer->tv_usec = _persist_interval.tv_usec;
	 }
      }

      /** Returns true if a CIMObjectPath and the PredicateReference both describe
	  the same CIM object. 
	  
	  @param x is a reference to CIMObjectPath that will be resolved to determine if it
	  describes the same object as the PredicateReference. 
      */
      Boolean identical(const CIMObjectPath& x) const;

      /** 
	  Identity operator for PredicateReference.
      */
      Boolean identical(const PredicateReference& x) const;

      /**
	 Evaluates a PredicateReference, setting and returning the object's truth value.
	 
	 For each Predicate object contained by this PredicateReference, this method
	 evaluates that Predicate against each KeyBinding contained by this PredicateReference. 
	 The result is either true or false. 
      */
      Boolean evaluate(void);

      /**
	 Returns true if the PredicateReference contains KeyBindings, false otherwise. 

	 A PredicateReference always evaluates to false if it is not bound (i.e., if it 
	 does not contain any KeyBindings. Note that a KeyBinding represents an actual 
	 CIM property value. 
      */
      Boolean isBound(void) 
      {
	 if(getKeyBindings().size())
	    return true;
	 return false;
      }
      
   private:
      struct timeval _check_interval;
      struct timeval _persist_interval;
      Boolean _truth_value;
      // for histeresis in indication evaluations
      Boolean _recent_truth_value;
      LogicalOperator _logical_op;
      Array<Predicate> _predicates;
};

#define PEGASUS_ARRAY_T PredicateReference
#include "ArrayInter.h"
#undef PEGASUS_ARRAY_T


/**
   The PredicateTree object provides a way to organize PredicateReference objects
   into a tree structure, meaning they can have parents, children, and siblings. 

   A group of PredicateReference objects can be linked into a tree that represents part 
   of a a parsed WQL expression or all of an LDAP filter. 

   For example: 
   <pre> 
            AND 
             | 
          foo == 1, fee != 2, OR 
                               |
                   namespace "nothing" PRESENT, namespace "another" PRESENT 

   would be TRUE when: 
   foo == 1 AND fee != 2 AND ((nothing is present) OR (another is present)) 
   </pre> 

   As the example above shows, a PredicateTree may be evaluated for a truth value.
   This allows the evaluation of nested or complex expressions. 

   Every PredicateTree object is capable of containing one PredicateReference object. 
   Further, each PredicateTree may contain a list of children - other PredicateTree
   objects that are lower in the tree than the subject object. 
*/
class PEGASUS_COMMON_LINKAGE PredicateTree
{
   public:

      PredicateTree();
      ~PredicateTree(); 

      /** 
	  Construct a PredicateTree and initialize to contain a copy of existing PredicateReference.
	 	  
	  @param pred Reference to an existing PredicateReference.
      */
      PredicateTree(const PredicateReference &pred);

      /**
	 Construct a PredicateTree and initialize it with a pointer to an existing PredicateReference. 
	 
	 @param pred Pointer to an existing PredicateReference
      */
      PredicateTree(PredicateReference *pred);

      /**
	 Evaluates a PredicateTree by doing a depth-first traversal and evaluating each
	 PredicateReference. The entire tree is evaluated to be either true or false. The
	 truth value of the tree is returned by this method as well as set within the 
	 PredicateTree object itself. 

	 Because evaluating a PredicateTree may involve locking the list of children, this 
	 method may throw an IPC exception. 

	 @exception IPCException
	 @return true if PredicateTree evaluates to true, false otherwise.
      */
      Boolean evaluate(void) throw(IPCException);
      /**
	 Locks the PredicateTree object. 
	 @exception IPCException
      */
      void lock(void) throw(IPCException) { _children.lock(); }
      /** Unlocks the PredicateTree object. 
	  @exception IPCException
      */
      void unlock(void) throw(IPCException) { _children.unlock(); }
      void _addChild(PredicateTree *cd) throw(IPCException)
      {
	 _children.insert_last(cd);
      }
      // special identity operator for DQueue access
      Boolean operator==(const void *key) const
      { 
	 if(key == (void *)this) 
	    return true; 
	 return false; 
      }
      // special identity operator for DQueue access

      Boolean operator ==(const PredicateTree & b) const bo
      {
	 return(operator==((const void *)&b));
      }
   private:
      PredicateTree(const PredicateTree& x);
      PredicateTree& operator=(const PredicateTree& x);
      Boolean _truth_value;
      LogicalOperator _logical_op;
      DQueue<PredicateTree> _children;
      PredicateReference *_pred;
      friend class CIMQuery;
} ;




PEGASUS_NAMESPACE_END

#endif // Predicate_h
