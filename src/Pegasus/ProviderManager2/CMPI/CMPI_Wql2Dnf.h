//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author:       Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:  Adrian Schuur, schuur@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef CMPI_Wql2Dnf_h
#define CMPI_Wql2Dnf_h

#include <Pegasus/Provider/CMPI/cmpidt.h>

#include <Pegasus/Common/Stack.h>
#include <Pegasus/WQL/WQLOperation.h>
#include <Pegasus/WQL/WQLOperand.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include "CMPI_Query2Dnf.h"

PEGASUS_NAMESPACE_BEGIN

#undef  PEGASUS_COMMON_LINKAGE
#define PEGASUS_COMMON_LINKAGE

#include <Pegasus/Common/Linkage.h> 

class term_el_WQL
{
public:
    term_el_WQL() {}
    term_el_WQL(Boolean m, WQLOperation o, WQLOperand op1, WQLOperand op2) :
       mark(m), op(o), opn1(op1), opn2(op2) {}
    Boolean mark;
    WQLOperation op;
    WQLOperand opn1;
    WQLOperand opn2;

    void negate(void);
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
    eval_el(Boolean m, WQLOperation o, int op1, Boolean i1, int op2, Boolean i2) :
       mark(m), op(o), opn1(op1), is_terminal1(i1), opn2(op2), is_terminal2(i2) {}
    Boolean mark;
    WQLOperation op;
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

#define PEGASUS_ARRAY_T term_el_WQL
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T eval_el
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#define PEGASUS_ARRAY_T stack_el
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

typedef Array<term_el_WQL> TableauRow_WQL;

#define PEGASUS_ARRAY_T TableauRow_WQL
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

#undef PEGASUS_COMMON_LINKAGE

typedef Array<TableauRow_WQL> Tableau_WQL;


class CMPI_Wql2Dnf
{
public:
    CMPI_Wql2Dnf();

    CMPI_Wql2Dnf(const WQLSelectStatement& wqs);

    CMPI_Wql2Dnf(const WQLSelectStatement * wqs);

    CMPI_Wql2Dnf(const String &condition, const String &pref); 
    
    ~CMPI_Wql2Dnf();

    void compile (const WQLSelectStatement * wqs);

    CMPI_Tableau *getTableau() {return &_CMPI_tableau;}

    Boolean evaluate(WQLPropertySource * source) const;

    void print();

    void printTableau();


protected:
	void _populateTableau(void);
	
    void _buildEvalHeap(const WQLSelectStatement * wqs);

    void _pushNOTDown(void);

    void _factoring(void);

    void _gatherDisj(Array<stack_el>& stk);

    void _gatherConj(Array<stack_el>& stk, stack_el sel);

    void _gather(Array<stack_el>& stk, stack_el sel, Boolean or_flag);
    
    static inline void _ResolveProperty(
        WQLOperand& op,
        const WQLPropertySource* source)
    {
        //
        // Resolve the operand: if it's a property name, look up its value:
        //

        if (op.getType() == WQLOperand::PROPERTY_NAME)
        {
            const String& propertyName = op.getPropertyName();

            if (!source->getValue(propertyName, op))
                throw NoSuchProperty(propertyName);
        }
    }


    // Structure to contain the compiled DNF form

    Tableau_WQL _tableau;
	CMPI_Tableau _CMPI_tableau;

    //
    // The eval_heap structure contains an ordered tree of non-terminal
    // expressions, the term_heap structure the corresponding terminal
    // expressions
    //

    Stack<term_el_WQL> terminal_heap;

    Array<eval_el> eval_heap;

    //friend WQLSelectStatement;
};

PEGASUS_NAMESPACE_END

#endif /* CMPI_Wql2Dnf_h */
