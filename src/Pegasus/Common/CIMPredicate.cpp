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

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/CIMReference.h>

PEGASUS_NAMESPACE_BEGIN


#define PEGASUS_ARRAY_T Predicate
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


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

Predicate(const KeyBinding& x, ExpressionOperator op = Equal, Boolean truth = true)
{
   if(this != &x)
   {
      KeyBinding::operator=(x);
      _op = op;
      _truth = truth;
   }
   
}

Predicate::Predicate(const String& name, 
		     const String& value, 
		     Type type,
		     ExpressionOperator op = Equal,
		     Boolean truth = true) 
   : KeyBinding(name, value, type), _op(op), _truth_value(truth) { }

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

void Predicate::evaluate(KeyBinding& key)
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
}






PEGASUS_NAMESPACE_END
