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
//==============================================================================//
//
// Author:       Humberto Rivero (hurivero@us.ibm.com)
//
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Cql2Dnf_h
#define Cql2Dnf_h

#include <Pegasus/Provider/CMPI/cmpidt.h>

#include <Pegasus/Common/Stack.h>
//#include <Pegasus/WQL/WQLOperation.h>
//#include <Pegasus/WQL/WQLOperand.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLExpression.h>

PEGASUS_NAMESPACE_BEGIN

#undef  PEGASUS_COMMON_LINKAGE
#define PEGASUS_COMMON_LINKAGE

#include <Pegasus/Common/Linkage.h> 

enum OperationType { CQL_LT, CQL_GT, CQL_EQ, CQL_LE, CQL_GE, CQL_NE, CQL_IS_NULL, CQL_IS_NOT_NULL, CQL_AND, CQL_OR, CQL_NOT, CQL_NOOP};

class term_el
{
public:
    term_el() {}
    term_el(Boolean m, CQLSimplePredicate simplePredicate) :
       mark(m), _simplePredicate(simplePredicate) {}
    Boolean mark;
    CQLSimplePredicate _simplePredicate;

    void negate();
    //int toStrings(CMPIType &typ, CMPIPredOp &opr, String &o1, String &o2) const;
};

class stack_el
{
public:
   stack_el() {}
   stack_el(int o, Boolean i) : opn(o), is_terminal(i) {}
   int   opn;     // either to terminals or eval_heap
   Boolean is_terminal;
};


class eval_el
{
public:
    eval_el() {}
    eval_el(Boolean m, OperationType o, int op1, Boolean i1, int op2, Boolean i2) :
       mark(m), op(o), opn1(op1), is_terminal1(i1), opn2(op2), is_terminal2(i2) {}
    Boolean mark;
    OperationType op;
    int opn1;
    Boolean is_terminal1; // if yes, look in terminal Array
    int opn2;
    Boolean is_terminal2; // if no, look in eval heap

    stack_el getFirst();

    stack_el getSecond();

    void setFirst(const stack_el s);

    void setSecond(const stack_el s);

    void assign_unary_to_first(const eval_el & assignee);

    void assign_unary_to_second(const eval_el & assignee);

    // Ordering operators, so that op1 > op2 for all non-terminals
    // and terminals appear in the second operand first
    void order(void);
};

#define PEGASUS_ARRAY_T term_el
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T eval_el
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T stack_el
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#undef PEGASUS_COMMON_LINKAGE


class Cql2Dnf
{
public:
    Cql2Dnf();

    Cql2Dnf(CQLSelectStatement& cqs);

    Cql2Dnf(CQLSelectStatement * cqs);

    Cql2Dnf(CQLPredicate& topLevel);

    ~Cql2Dnf();

    void compile (CQLSelectStatement * cqs);
    void compile (CQLPredicate& topLevel);
    void print();
    CQLPredicate getDnfPredicate();

protected:
    void _buildEvalHeap();

    void _pushNOTDown(void);

    void _factoring(void);

	//
	// _strip_ops_operands(CQLSelectStatement *cqs)
	//
	// This function takes a CQLSelectStatement and does a depth first search looking for the operations and operands.
	// The operations are appended to the _operations array and the operands appended to the _operands array
	// When finished, we will have two arrays, representing the statement tree, from which we can start the process
	// to put the statement into DNF.
	//
	// Example:  a=b^(!c=d v e=f)
	// _operations array will look like:
	//	[=][=][!][=][v][^]
	// _operands array will look like:
	// 	[a][b][c][d][e][f]
	//

    void _strip_ops_operands(CQLPredicate& topLevel);

	//
	// _destruct(const CQLPredicate& _p
	//
	// Recursively does a depth first search of the statement tree and extracts the operations
	// and operands.
	// 

    void _destruct(CQLPredicate& _p);

	//
	// _construct()
    	//
        // Each eval_el on the eval heap contains all the information needed to make a CQLPredicate.
        // We will build a CQLPredicate for every element in the eval heap. So there is a 1 to 1 correspondence
        // between elements in the eval heap and elements in the CQLPredicate array used below.
        // The first eval_el on the eval heap will always contain at least one terminal if the operation is a NOT
        // or two terminals if the operation is AND or OR.  We are guaranteed to build a CQLPredicate from the first
        // position in the eval_heap array.
        //
        // The key to the algorithm is the isterminalX flag.  When set to true, we go to the
        // term_heap and get the CQLSimplePredicate.  When set to false, we go to the _preds array below
        // and get the CQLPredicate.  Since there is a 1 - 1 correspondence, as explained above, the index
        // referred to by eval.opn1 or eval.opn2 is valid into the _preds array.
        //
        // For ANDs and ORs, we need two operands, as explained above, we get those operands
        // from either the term_heap or the _preds array.  For NOTs, we need only 1 operand, and that
        // comes from either the term_heap or the _preds array.
        //
        // When finished, the last element in the _preds array contains the top level CQLPredicate (the rebuilt tree)
        //
        // Example:  a=b^(!c=d v e=f)
        // If the current eval_heap looks like:
        //      0,NOT,1,True,-1,True [index = 0]
        //      0,OR,2,True,0,False  [index = 1]
        //      0,AND,1,False,0,True [index = 2]
        //
        // And the current term_heap looks like:
        //      CQLSimplePredicate(a=b) [index = 0]
        //      CQLSimplePredicate(c=d) [index = 1]
        //      CQLSimplePredicate(e=f) [index = 0]
        //
        // The _preds array at the end would look like:
        //      CQLPredicate(!c==d)        [index = 0]
        //      CQLPredicate(e==f v !c==d) [index = 1]
        //      CQLPredicate((e==f v !c==d) ^ a==b) [index = 2]  (the rebuilt tree)
        //
    void _construct();

    OperationType _convertOpType(ExpressionOpType op);

    void _gatherDisj(Array<stack_el>& stk);

    void _gatherConj(Array<stack_el>& stk, stack_el sel);

    void _gather(Array<stack_el>& stk, stack_el sel, Boolean or_flag);
    
private:

    //
    // The eval_heap structure contains an ordered tree of non-terminal
    // expressions, the term_heap structure the corresponding terminal
    // expressions
    //

    Stack<term_el> terminal_heap;

    Array<eval_el> eval_heap;

    Array<CQLExpression> _operands;
    Array<OperationType> _operations;
    CQLPredicate _dnfPredicate;
    //friend WQLSelectStatement;
};


PEGASUS_NAMESPACE_END

#endif /* Cql2Dnf_h */
