//%/////////////////////////////////////////////////////////////////////////////
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
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMPredicate.h"

PEGASUS_NAMESPACE_BEGIN
extern int _Compare(const String& s1_, const String& s2_);
static void _BubbleSort(Array<Predicate>& x)
{
    Uint32 n = x.size();

    if (n < 2)
	return;

    for (Uint32 i = 0; i < n - 1; i++)
    {
	for (Uint32 j = 0; j < n - 1; j++)
	{
	    if (_Compare(x[j].getName(), x[j+1].getName()) > 0)
	    {
		Predicate t = x[j];
		x[j] = x[j+1];
		x[j+1] = t;
	    }
	}
    }
}

// evaluation 
//-----------------------------------------
// p_name p_op p_val, r_name r_val 
//-- --- translates to --------------------
// r_val p_op p_val
//----------------------------------------
//

static Boolean evaluate_string(String ref, ExpressionOperator op, String pred)
{

   switch(op) 
   {
      case EQUAL:
	 return(ref == pred);
      case NE:
	 return(ref != pred);
      case GT:
	 return(ref > pred);
      case GTE:
	 return(ref >= pred);
      case LT:
	 return(ref < pred);
      case LTE:
	 return(ref <= pred);
      case PRESENT:
	 if(ref.size()) 
	    return true;
      default:
	 break;
   }
   return false;
}

static Boolean evaluate_int(String ref, ExpressionOperator op, String pred)
{
   
   char *ref_string = ref.allocateCString();
   if(ref_string == NULL)
      return false;
   
   char *pred_string = pred.allocateCString();
   if (pred_string == NULL)
      return false;
   
   if(*ref_string == '+' || *ref_string == '-' || 
      *pred_string == '+' || *pred_string == '-')
   {
      Sint32 ref_val = strtol(ref_string, (char **)0, 0);
      Sint32 pred_val = strtol(pred_string, (char **)0, 0);
      switch(op) 
      {
	 case EQUAL:
	    return(ref == pred);
	 case NE:
	    return(ref != pred);
	 case GT:
	    return(ref > pred);
	 case GTE:
	    return(ref >= pred);
	 case LT:
	    return(ref < pred);
	 case LTE:
	    return(ref <= pred);
	 case PRESENT:
	    if(ref != 0)
	       return true;
	 default:
	    break;
      }
   }
   else
   {
      Uint32 ref_val = strtoul(ref_string, (char **)0, 0);
      Uint32 pred_val = strtoul(pred_string, (char **)0, 0);
      switch(op) 
      {
	 case EQUAL:
	    return(ref == pred);
	 case NE:
	    return(ref != pred);
	 case GT:
	    return(ref > pred);
	 case GTE:
	    return(ref >= pred);
	 case LT:
	    return(ref < pred);
	 case LTE:
	    return(ref <= pred);
	 case PRESENT:
	    if(ref != 0)
	       return true;
	 default:
	    break;
      }
   }
   return false;
}

static Boolean evaluate_bool(String ref, ExpressionOperator op, String pred)
{

   
   switch(op) 
   {
      case EQUAL:
	 return(String::equalNoCase(ref, pred));
      case NE:
	 if(true == String::equalNoCase(ref, pred))
	    return(false);
	 return(true);
      case GT:
      case GTE:
      case LT:
      case LTE:
      case PRESENT:
      default:
	 break;
   }
   return false;
}

Predicate::Predicate(void) : KeyBinding() { }

Predicate::Predicate(const Predicate& x) 
   : KeyBinding(x), _op(x._op), _truth_value(x._truth_value) { }

Predicate::Predicate(const KeyBinding& x, ExpressionOperator op = EQUAL, Boolean truth = true)
{
   if(this != &x)
   {
      KeyBinding::operator=(x);
      _op = op;
      _truth_value = truth;
   }
   
}

Predicate::Predicate(const String& name, 
		     const String& value, 
		     Type type,
		     ExpressionOperator op = EQUAL,
		     Boolean truth = true) 
   : KeyBinding(name, value, type), _op(op), _truth_value(truth) { }

Predicate::~Predicate() { }

Predicate& Predicate::operator=(const Predicate& x)
{
   if(this != &x)
   {
      KeyBinding::operator=(x);
      _op = x._op;
      _truth_value = x._truth_value;
   }
   return *this;
}

Boolean  Predicate::evaluate(const KeyBinding& key)
{
   _truth_value = false;
   if(true == CIMName::equal(this->getName() , key.getName()))
   { 
      Type type;
      if((type = this->getType()) == key.getType())
      {
	 switch(type)
	 {
	    case BOOLEAN:
	       _truth_value = evaluate_bool(key.getValue(), _op, this->getValue());
	       break;
	    case STRING:
	       _truth_value = evaluate_string(key.getValue(), _op, this->getValue());
	       break;
	    case NUMERIC:
	       _truth_value = evaluate_int(key.getValue(), _op, this->getValue());
	       break;
	    default:
	       break;
	 }
      }
   }
   return _truth_value;
}


//-----------------------------------------------------------------
// PredicateReference class implementation
//-----------------------------------------------------------------

PredicateReference::PredicateReference() 
   : CIMReference(), _truth_value(false), 
     _logical_op(AND), _predicates() { }

PredicateReference::PredicateReference(const CIMReference& x)
   : CIMReference(x), _truth_value(false), 
     _logical_op(AND), _predicates() { }

PredicateReference::PredicateReference(const PredicateReference& x)
   : CIMReference(x), _truth_value(x._truth_value),
     _logical_op(x._logical_op), _predicates(x._predicates)
{
   _BubbleSort(_predicates);
}

PredicateReference::PredicateReference(const String& objectName)
   :CIMReference(objectName), _truth_value(false), 
    _logical_op(AND), _predicates() { }

PredicateReference::PredicateReference(const char *objectName)
   :CIMReference(objectName), _truth_value(false), 
    _logical_op(AND), _predicates() { }

PredicateArray PredicateReference::getPredicateArray()
{
   return PredicateArray();
}

PredicateReference::PredicateReference(
	 const String& host,
	 const String& nameSpace,
	 const String& className,
	 const KeyBindingArray& keyBindings,
	 const PredicateArray& predicates,
	 Boolean truth ,
	 LogicalOperator lop )
   : CIMReference(host, nameSpace, className, keyBindings),
    _truth_value(true), _logical_op(lop)
{
   setPredicates(predicates);
}

PredicateReference::~PredicateReference() 
{

}
      
PredicateReference& PredicateReference::operator=(const PredicateReference& x)
{
   if(&x != this) 
   {
      CIMReference::operator=(x);
      _truth_value = x._truth_value;
      _logical_op = x._logical_op;
      _predicates = x._predicates;
   }
   return *this;
}

PredicateReference& PredicateReference::operator=(const CIMReference& x)
{
   if(&x != this)
   {
      CIMReference::operator=(x);
      _truth_value = true;
      _logical_op = AND;
      _predicates =  PredicateArray();
   }
   return *this;
}

void PredicateReference::clear()
{
   CIMReference::clear();
   _truth_value = true;
   _logical_op = AND;
   _predicates.clear();
}
      
void PredicateReference::set(
   const String& host,
   const String& nameSpace,
   const String& className,
   const KeyBindingArray& keyBindings,
   const PredicateArray& predicates,
   Boolean truth ,
   LogicalOperator lop )
{
   CIMReference::set(host, nameSpace, className, keyBindings);
   _truth_value = truth;
   _logical_op = lop;
   setPredicates(predicates);
}

void PredicateReference::setPredicates(const Array<Predicate>& predicates)
{
   _predicates = predicates;
   _BubbleSort(_predicates);
}

Boolean PredicateReference::identical(const CIMReference& x) const
{
   return
      String::equal(getHost(), x.getHost()) &&
      String::equal(getNameSpace(), x.getNameSpace()) &&
      CIMName::equal(getClassName(), x.getClassName());
   
}

Boolean PredicateReference::identical(const PredicateReference& x) const
{
   return
      _truth_value == x._truth_value &&
      _logical_op == x._logical_op &&
      CIMReference::identical(x) &&
      _predicates == x._predicates;
}

Boolean PredicateReference::evaluate(void)
{
   _truth_value = false;
   
   const Array<KeyBinding>& keys = CIMReference::getKeyBindings();
   int x = _predicates.size();
   int y = keys.size();
   int i, j;
   
   for( i = 0; i < x; i++)
   {
      Predicate& pred = _predicates[i];
      for ( j = 0; j < y; j++ ) 
      {
	 const KeyBinding& key = keys[i];
	 if(pred == key)
	 {
	    if(true == pred.evaluate(key))
	    {
	       switch(_logical_op)
	       {
		  case AND:
		     _truth_value = true;
		     j = y; // force an exit from the loop
		     break;
		  case OR:
		     _truth_value = true;
		     return true;
		  case NOT:
		  default:
		     _truth_value = false;
		     return false;
	       }
	    }
	    else 
	    {
	       switch(_logical_op)
	       {
		  case AND:
		     _truth_value = false;
		     return false;
		  case OR:
		     break;
		  case NOT:
		     _truth_value = true;
		     break;
		  default:
		     _truth_value = false;
		     return false;
	       }
	    }
	 }
      }
   }
   return _truth_value;
}


PredicateTree::PredicateTree(void)
   : _mut(), _truth_value(true), _logical_op(AND),
     _peers(true), _children(true), _pred(NULL) { }

PredicateTree::~PredicateTree(void)
{
   _children.empty_list();
   _peers.empty_list();
   
   delete _pred;
}

PredicateTree::PredicateTree(const PredicateReference& pred)
   : _mut(), _truth_value(true), _logical_op(AND),
     _peers(true), _children(true)
{
   _pred = new PredicateReference(pred);
}

Boolean PredicateTree::evaluate(void)
{
   if(_pred != NULL)
   {
      Boolean truth =  _pred->evaluate();
      switch(_logical_op)
      {
	 case AND:
	    if(truth == false)
	    {
	       _truth_value = false;
	       return _truth_value;
	    }
	    else
	       _truth_value = true;
	    break;
	 case OR:
	    if(truth == true)
	    {
	       _truth_value = true;
	       return _truth_value;
	    }
	    else
	       _truth_value = false;
	    break;
	 case NOT:
	    if(truth == true)
	    {
	       _truth_value = false;
	       return _truth_value;
	    }
	    else
	       _truth_value = true;
	    break;
	 default:
	    _truth_value = true;
	    break;
      }
   }
   else 
      _truth_value = true;

   if(_children.count())
   {
      PredicateTree *children = NULL;

// rw lock may be more appropriate
// try ... catch 
      try 
      {
	 _children.lock();
      }
      catch ( IPCException& e)
      {
	 throw;
      }
      
      while( NULL != ( children = _children.next(children)))
      {
	 Boolean truth = children->evaluate();
	 switch(_logical_op)
	 {
	    case AND:
	       if(truth == false )
	       {
		  _truth_value = false;
		  _children.unlock();
		  return _truth_value;
	       }
	       break;
	    case OR:
	       if(truth == true)
	       {
		  _truth_value = true;
		  _children.unlock();
		  return _truth_value;
	       }
	       else
		  _truth_value = false;
	       break;
	    case NOT:
	       if(truth == true)
	       {
		  _truth_value = false
		  _children.unlock();
		  return _truth_value;
	       }
	       break;
	    default:
	       break;
	 }
      }
      _children.unlock();
   }
   return _truth_value;
}


PEGASUS_NAMESPACE_END
