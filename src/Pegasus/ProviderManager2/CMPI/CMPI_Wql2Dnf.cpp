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
// Author:      Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By: Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////


#define CMPI_VER_86 1

#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Stack.h>
#include <Pegasus/WQL/WQLOperation.h>
#include <Pegasus/WQL/WQLOperand.h>
#include <Pegasus/WQL/WQLParser.h>

#include "CMPI_Wql2Dnf.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

#define PEGASUS_ARRAY_T term_el
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T eval_el
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T stack_el
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T TableauRow
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


//
// Terminal element methods 
//
void term_el::negate(void)
{
    switch (op)
    {
        case WQL_EQ: op = WQL_NE; break;
        case WQL_NE: op = WQL_EQ; break;
        case WQL_LT: op = WQL_GE; break;
        case WQL_LE: op = WQL_GT; break;
        case WQL_GT: op = WQL_LE; break;
        case WQL_GE: op = WQL_LT; break;
        default: break;
    }
};

String opnd2string(const WQLOperand &o) {
    switch (o.getType()) {
    case WQLOperand::PROPERTY_NAME:
       return o.getPropertyName();
    case WQLOperand::STRING_VALUE:
       return o.getStringValue();
    case WQLOperand::INTEGER_VALUE:
       return Formatter::format("$0",o.getIntegerValue());
    case WQLOperand::DOUBLE_VALUE:
       return Formatter::format("$0",o.getDoubleValue());
    case WQLOperand::BOOLEAN_VALUE:
       return Formatter::format("$0",o.getBooleanValue());
    default: ;
   }
   return "NULL_VALUE";
}


CMPIPredOp mapOperation(WQLOperation op) {
   static CMPIPredOp ops[]={(CMPIPredOp)0,(CMPIPredOp)0,(CMPIPredOp)0,
      CMPI_PredOp_Equals,
      CMPI_PredOp_NotEquals,
      CMPI_PredOp_LessThan,
      CMPI_PredOp_LessThanOrEquals,
      CMPI_PredOp_GreaterThan,
      CMPI_PredOp_GreaterThanOrEquals,
      (CMPIPredOp)0,(CMPIPredOp)0,(CMPIPredOp)0,(CMPIPredOp)0,(CMPIPredOp)0,(CMPIPredOp)0};
   return ops[(int)op];
}

CMPIType mapType(WQLOperand::Type typ) {
   switch (typ) {
    case WQLOperand::PROPERTY_NAME:
       return CMPI_nameString;
    case WQLOperand::STRING_VALUE:
       return CMPI_charString;
    case WQLOperand::INTEGER_VALUE:
       return CMPI_integerString;
    case WQLOperand::DOUBLE_VALUE:
       return CMPI_realString;
    case WQLOperand::BOOLEAN_VALUE:
       return CMPI_booleanString;
    case WQLOperand::NULL_VALUE:
       return CMPI_null;
  }
  return CMPI_null;
}

int term_el::toStrings(CMPIType &typ, CMPIPredOp &opr, String &o1, String &o2) const {
   opr=mapOperation(op);
   o1=opnd2string(opn1);
   o2=opnd2string(opn2);
   if (opn1.getType()==WQLOperand::PROPERTY_NAME) typ=mapType(opn2.getType());
   else typ=mapType(opn1.getType());
   return 0;
}

//
// Evaluation heap element methods
//
stack_el eval_el::getFirst() 
{ 
   return stack_el(opn1, is_terminal1);
}

stack_el eval_el::getSecond()
{
   return stack_el(opn2, is_terminal2);
}

void eval_el::setFirst(const stack_el s)
{
     opn1 = s.opn;
     is_terminal1 = s.is_terminal;
}

void eval_el::setSecond(const stack_el s)
{
    opn2 = s.opn;
    is_terminal2 = s.is_terminal;
}

void eval_el::assign_unary_to_first(const eval_el & assignee)
{
    opn1 = assignee.opn1;
    is_terminal1 = assignee.is_terminal1;
}

void eval_el::assign_unary_to_second(const eval_el & assignee)
{
    opn2 = assignee.opn1;
    is_terminal2 = assignee.is_terminal1;
}

// Ordering operators, so that op1 > op2 for all non-terminals
// and terminals appear in the second operand first
void eval_el::order(void)
{
    int k;
    if ((!is_terminal1) && (!is_terminal2))
        if ((k = opn2) > opn1)
        {
            opn2 = opn1;
            opn1 =  k;
        }
    else if ((is_terminal1) && (!is_terminal2))
        if ((k = opn2) > opn1)
        {
            opn2 = opn1;
            opn1 =  k;
            is_terminal1 = false;
            is_terminal2 = true;
        }
}

//
// Helper function copied from WQLSelectStatement
// 

template<class T>
inline static Boolean _Compare(const T& x, const T& y, WQLOperation op)
{
    switch (op)
    {
        case WQL_EQ: 
            return x == y;

        case WQL_NE: 
            return x != y;

        case WQL_LT: 
            return x < y;
        case WQL_LE: 
            return x <= y;

        case WQL_GT: 
            return x > y;

        case WQL_GE: 
            return x >= y;

        default:
            PEGASUS_ASSERT(0);
    }

    return false;
}

static bool operator==(const WQLOperand& x, const WQLOperand& y)
{
   if (x.getType()==y.getType()) switch (x.getType()) {
   case WQLOperand::PROPERTY_NAME:
      return x.getPropertyName()==y.getPropertyName();
   case WQLOperand::INTEGER_VALUE:
      return x.getIntegerValue()==y.getIntegerValue();
   case WQLOperand::DOUBLE_VALUE:
      return x.getDoubleValue()==y.getDoubleValue();
   case WQLOperand::BOOLEAN_VALUE:
      return x.getBooleanValue()==y.getBooleanValue();
   case WQLOperand::STRING_VALUE:
      return x.getStringValue()==y.getStringValue();
   case WQLOperand::NULL_VALUE: 
      return true;
   }
   return false;
}

static bool operator==(const term_el& x, const term_el& y)
{
	return x.op == y.op && 
	   x.opn1 == y.opn1 && 
	   x.opn2 == y.opn2;
}

static void addIfNotExists(TableauRow &tr, const term_el& el)
{
   for (int i=0,m=tr.size(); i<m; i++) {
      if (tr[i]==el) return;
   }
   tr.append(el);
}


static Boolean _Evaluate(
    const WQLOperand& lhs, 
    const WQLOperand& rhs, 
    WQLOperation op)
{
    switch (lhs.getType())
    {
        case WQLOperand::NULL_VALUE:
        {
            // This cannot happen since expressions of the form
            // OPERAND OPERATOR NULL are converted to unary form.
            // For example: "count IS NULL" is treated as a unary
            // operation in which IS_NULL is the unary operation
            // and count is the the unary operand.

            PEGASUS_ASSERT(0);
            break;
        }

        case WQLOperand::INTEGER_VALUE:
        {
            return _Compare(
                lhs.getIntegerValue(),
                rhs.getIntegerValue(),
                op);
        }

        case WQLOperand::DOUBLE_VALUE:
        {
            return _Compare(
                lhs.getDoubleValue(),
                rhs.getDoubleValue(),
                op);
        }

        case WQLOperand::BOOLEAN_VALUE:
        {
            return _Compare(
                lhs.getBooleanValue(),
                rhs.getBooleanValue(),
                op);
        }

        case WQLOperand::STRING_VALUE:
        {
            return _Compare(
                lhs.getStringValue(),
                rhs.getStringValue(),
                op);
        }

        default:
            PEGASUS_ASSERT(0);
    }

    return false;
}


//
// WQL Compiler methods
//
    
CMPI_Wql2Dnf::CMPI_Wql2Dnf(const String condition, const String pref) 
{
    WQLSelectStatement wqs;
    WQLParser::parse(pref+condition,wqs);
    eval_heap.reserveCapacity(16);
    terminal_heap.reserveCapacity(16);
    _tableau.clear();
    compile(&wqs);
}

CMPI_Wql2Dnf::CMPI_Wql2Dnf() 
{
    eval_heap.reserveCapacity(16);
    terminal_heap.reserveCapacity(16);
    _tableau.clear();
}

CMPI_Wql2Dnf::CMPI_Wql2Dnf(const WQLSelectStatement & wqs)
{
    eval_heap.reserveCapacity(16);
    terminal_heap.reserveCapacity(16);
    _tableau.clear();
    compile(&wqs);
}

CMPI_Wql2Dnf::CMPI_Wql2Dnf(const WQLSelectStatement * wqs)
{
    eval_heap.reserveCapacity(16);
    terminal_heap.reserveCapacity(16);
    _tableau.clear();
    compile(wqs);
}

CMPI_Wql2Dnf::~CMPI_Wql2Dnf() {}

void CMPI_Wql2Dnf::compile(const WQLSelectStatement * wqs)
{
    if (!wqs->hasWhereClause()) return;
    _tableau.clear();

    _buildEvalHeap(wqs);
    _pushNOTDown();
    _factoring();
    
    Array<stack_el> disj;
    _gatherDisj(disj);
    if (disj.size() == 0)
        if (terminal_heap.size() > 0)
           // point to the remaining terminal element
            disj.append(stack_el(0,true));

    for (Uint32 i=0, n =disj.size(); i< n; i++)
    {
        TableauRow tr;
        Array<stack_el> conj;

        if (!disj[i].is_terminal)
        {
           _gatherConj(conj, disj[i]);
            for( Uint32 j=0, m = conj.size(); j < m; j++)
	        addIfNotExists(tr,terminal_heap[conj[j].opn]);
//                tr.append(terminal_heap[conj[j].opn]);
        }
        else
	   addIfNotExists(tr,terminal_heap[disj[i].opn]);
//	   tr.append(terminal_heap[disj[i].opn]);
        _tableau.append(tr);
    }

    eval_heap.clear();
       
    //print();
    printTableau();
    //_sortTableau();
}

Boolean CMPI_Wql2Dnf::evaluate(WQLPropertySource * source) const
{
   Boolean b = false;
   WQLOperand lhs, rhs;

   for(Uint32 i=0,n = _tableau.size(); i < n; i++)
   {
       TableauRow tr = _tableau[i];
       for(Uint32 j=0,m = tr.size(); j < m; j++)
       {
           lhs = tr[j].opn1;
           CMPI_Wql2Dnf::_ResolveProperty(lhs,source);
           rhs = tr[j].opn2;
           CMPI_Wql2Dnf::_ResolveProperty(rhs,source);

           if (rhs.getType() != lhs.getType())
               throw TypeMismatchException();

           if (!_Evaluate(lhs, rhs, tr[j].op))
           {
               b = false;
               break;
           }
           else
               b = true;
       }
       if (b) return true;
   }
   return false;
}

void CMPI_Wql2Dnf::print(void)
{
for (Uint32 i=0, n=eval_heap.size();i < n;i++) {
    WQLOperation wop = eval_heap[i].op;
    if (wop == WQL_IS_TRUE) continue;
    cout << "Eval element " << i << ": ";
    if (eval_heap[i].is_terminal1) cout << "T(";
    else cout << "E(";
    cout << eval_heap[i].opn1 << ") ";
    cout << WQLOperationToString(eval_heap[i].op);
    if (eval_heap[i].is_terminal2) cout << " T(";
    else cout << " E(";
    cout << eval_heap[i].opn2 << ")" << endl;
}
for (Uint32 i=0, n=terminal_heap.size();i < n;i++) {
    cout << "Terminal expression " << i << ": ";
    cout << terminal_heap[i].opn1.toString() << " ";
    cout << WQLOperationToString(terminal_heap[i].op) << " "
         << terminal_heap[i].opn2.toString() << endl;
}
}

void CMPI_Wql2Dnf::printTableau(void)
{
   for(Uint32 i=0,n = _tableau.size(); i < n; i++)
   {
       cout << "Tableau " << i << endl;
       TableauRow tr = _tableau[i];
       for(Uint32 j=0,m = tr.size(); j < m; j++)
       {
           cout << tr[j].opn1.toString() << " ";
           cout << WQLOperationToString(tr[j].op) << " "
                << tr[j].opn2.toString() << endl;
       }

   }

}

void CMPI_Wql2Dnf::_buildEvalHeap(const WQLSelectStatement * wqs)
{

    //WQLSelectStatement* that = (WQLSelectStatement*)wqs;

    WQLOperand dummy;
    dummy.clear();
    Stack<stack_el> stack;

    // Counter for Operands

    Uint32 j = 0;

    //cerr << "Build eval heap\n";

    for (Uint32 i = 0, n = wqs->_operations.size(); i < n; i++)
    {
        WQLOperation op = wqs->_operations[i];

        switch (op)
        {
            case WQL_OR:
            case WQL_AND:
            {
                PEGASUS_ASSERT(stack.size() >= 2);

                stack_el op1 = stack.top();
                stack.pop();

                stack_el op2 = stack.top();

                // generate Eval expression
                eval_heap.append(eval_el(0, op , op1.opn, op1.is_terminal,
                                 op2.opn , op2.is_terminal));

                stack.top() = stack_el(eval_heap.size()-1, false);

                break;
            }

            case WQL_NOT:
            case WQL_IS_FALSE:
            case WQL_IS_NOT_TRUE:
            {
                PEGASUS_ASSERT(stack.size() >= 1);

                stack_el op1 = stack.top();

                // generate Eval expression
                eval_heap.append(eval_el(0, op , op1.opn, op1.is_terminal,
                                 -1, true));

                stack.top() = stack_el(eval_heap.size()-1, false);

                break;
            }

            case WQL_EQ:
            case WQL_NE:
            case WQL_LT:
            case WQL_LE:
            case WQL_GT:
            case WQL_GE:
            {
                PEGASUS_ASSERT(wqs->_operands.size() >= 2);

                WQLOperand lhs = wqs->_operands[j++];

                WQLOperand rhs = wqs->_operands[j++];

                terminal_heap.push(term_el(false, op, lhs, rhs));

                stack.push(stack_el(terminal_heap.size()-1, true));

                break;
            }

            case WQL_IS_TRUE:
            case WQL_IS_NOT_FALSE:
            {
                PEGASUS_ASSERT(stack.size() >= 1);
                break;
            }

            case WQL_IS_NULL:
            {
                PEGASUS_ASSERT(wqs->_operands.size() >= 1);
                WQLOperand op = wqs->_operands[j++];

                terminal_heap.push(term_el(false, WQL_EQ, op, dummy));

                stack.push(stack_el(terminal_heap.size()-1, true));

                break;
            }

            case WQL_IS_NOT_NULL:
            {
                PEGASUS_ASSERT(wqs->_operands.size() >= 1);
                WQLOperand op = wqs->_operands[j++];

                terminal_heap.push(term_el(false, WQL_NE, op, dummy));

                stack.push(stack_el(terminal_heap.size()-1, true));

                break;
            }
        }
    }

    PEGASUS_ASSERT(stack.size() == 1);
}

void CMPI_Wql2Dnf::_pushNOTDown()
{
    for (int i=eval_heap.size()-1; i >= 0; i--)
    {
        Boolean _found = false;
        int k;

        // Order all operators, so that op1 > op2 for non-terminals
        // and terminals appear as second operand

        eval_heap[i].order();

        // First solve the unary NOT operator

        if (eval_heap[i].op == WQL_NOT ||
            eval_heap[i].op == WQL_IS_FALSE ||
            eval_heap[i].op == WQL_IS_NOT_TRUE)
       {
            // This serves as the equivalent of an empty operator
            eval_heap[i].op = WQL_IS_TRUE;

            // Substitute this expression in all higher order eval statements
            // so that this node becomes disconnected from the tree

            for (int j=eval_heap.size()-1; j > i;j--)
            {
               // Test first operand
               if ((!eval_heap[j].is_terminal1) && (eval_heap[j].opn1 == i))

                   eval_heap[j].assign_unary_to_first(eval_heap[i]);

               // Test second operand
               if ((!eval_heap[j].is_terminal2) && (eval_heap[j].opn2 == i))

                   eval_heap[j].assign_unary_to_second(eval_heap[i]);
            }

            // Test: Double NOT created by moving down

            if (eval_heap[i].mark)
               eval_heap[i].mark = false;
            else
               _found = true;
            // else indicate a pending NOT to be pushed down further
        }

        // Simple NOT created by moving down

        if (eval_heap[i].mark)
        {
            // Remove the mark, indicate a pending NOT to be pushed down
            // further and switch operators (AND / OR)

            eval_heap[i].mark=false;
            if (eval_heap[i].op == WQL_OR) eval_heap[i].op = WQL_AND;
            else if (eval_heap[i].op == WQL_AND) eval_heap[i].op = WQL_OR;

            // NOT operator is already ruled out
            _found = true;
        }

        // Push a pending NOT further down
        if (_found)
        {
             // First operand

             int j = eval_heap[i].opn1;
             if (eval_heap[i].is_terminal1)
                 // Flip NOT mark
                 terminal_heap[j].negate();
             else
                 eval_heap[j].mark = !(eval_heap[j].mark);

             //Second operand (if it exists)

             if ((j = eval_heap[i].opn2) >= 0)
             {
                 if (eval_heap[i].is_terminal2)
                     // Flip NOT mark
                     terminal_heap[j].negate();
                 else
                     eval_heap[j].mark = !(eval_heap[j].mark);
             }
        }
    }
}

void CMPI_Wql2Dnf::_factoring(void)
{
    int i = 0,n = eval_heap.size();
    //for (int i=eval_heap.size()-1; i >= 0; i--)
    while (i < n)
    {
        int _found = 0;
        int index = 0;

        // look for expressions (A | B) & C  ---> A & C | A & B
        if (eval_heap[i].op == WQL_AND)
        {
            if (!eval_heap[i].is_terminal1)
            {
                index = eval_heap[i].opn1; // remember the index
                if (eval_heap[index].op == WQL_OR) _found = 1;
            }

            if ((_found == 0) && (!eval_heap[i].is_terminal2))
            {
                index = eval_heap[i].opn2; // remember the index
                if (eval_heap[index].op == WQL_OR) _found = 2;
            }

            if (_found != 0)
            {
                 //int u1,u1_t,u2,u2_t,u3,u3_t;
                 stack_el s;

                 if (_found == 1)
                     s = eval_heap[i].getSecond();
                 else
                     s = eval_heap[i].getFirst();

                 // insert two new expression before entry i
                 eval_el evl;

                 evl = eval_el(false, WQL_OR, i+1, false, i, false);
                 if ((Uint32 )i < eval_heap.size()-1)
                     eval_heap.insert(i+1, evl);
                 else
                     eval_heap.append(evl);
                 eval_heap.insert(i+1, evl);

                 for (int j=eval_heap.size()-1; j > i + 2; j--)
                 {
                     //eval_heap[j] = eval_heap[j-2];

                     // adjust pointers

                     if ((!eval_heap[j].is_terminal1)&&
                         (eval_heap[j].opn1 >= i))
                         eval_heap[j].opn1 += 2;
                     if ((!eval_heap[j].is_terminal2)&&
                         (eval_heap[j].opn2 >= i))
                         eval_heap[j].opn2 += 2;
                 }

                 n+=2; // increase size of array

                 // generate the new expressions : new OR expression


                 // first new AND expression
                 eval_heap[i+1].mark = false;
                 eval_heap[i+1].op = WQL_AND;
                 eval_heap[i+1].setFirst(s);
                 eval_heap[i+1].setSecond( eval_heap[index].getFirst());
                 eval_heap[i+1].order();


                 // second new AND expression
                 eval_heap[i].mark = false;
                 eval_heap[i].op = WQL_AND;
                 eval_heap[i].setFirst(s);
                 eval_heap[i].setSecond( eval_heap[index].getSecond());
                 eval_heap[i].order();

                 // mark the indexed expression as inactive
                 //eval_heap[index].op = WQL_IS_TRUE; possible disconnects
                 i--;

            } /* endif _found > 0 */

        } /* endif found AND operator */

        i++; // increase pointer
    }
}

void CMPI_Wql2Dnf::_gatherDisj(Array<stack_el>& stk)
{
    _gather(stk, stack_el(0,true), true);
}

void CMPI_Wql2Dnf::_gatherConj(Array<stack_el>& stk, stack_el sel)
{
    _gather(stk, sel, false);
}

void CMPI_Wql2Dnf::_gather(Array<stack_el>& stk, stack_el sel, Boolean or_flag)
{
    Uint32 i = 0;

    stk.clear();
    stk.reserveCapacity(16);

    if ((i = eval_heap.size()) == 0) return;

    while (eval_heap[i-1].op == WQL_IS_TRUE)
    {
        eval_heap.remove(i-1);
        i--;
        if (i == 0) return;
    }
    //if (i == 0) return;

    if (or_flag)
        stk.append(stack_el(i-1,false));
    else
    {
       if (sel.is_terminal) return;
       stk.append(sel);
    }

    i = 0;

    while (i<stk.size())
    {
        int k = stk[i].opn;

        if ((k < 0) || (stk[i].is_terminal))
           i++;
        else
        {
            if ( ((eval_heap[k].op != WQL_OR) && (or_flag)) ||
                 ((eval_heap[k].op != WQL_AND) && (!or_flag))  )
                i++;
            else
            {
                // replace the element with disjunction
                stk[i] = eval_heap[k].getSecond();
                stk.insert(i, eval_heap[k].getFirst());
                if (or_flag)
                    eval_heap[k].op = WQL_IS_TRUE;
            }
        }
    }
}

PEGASUS_NAMESPACE_END
