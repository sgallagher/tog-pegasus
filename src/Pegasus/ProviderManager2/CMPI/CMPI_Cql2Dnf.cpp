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
// Author:      Konrad Rzeszutek <konradr@us.ibm.com>
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#include "CMPI_Version.h"

#include <Pegasus/Common/Stack.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLSelectStatementRep.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLValue.h>

//#include "CMPI_Query2Dnf.h"
#include "CMPI_Cql2Dnf.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
#define PEGASUS_ARRAY_T CMPI_term_el
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CMPI_TableauRow
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
//
// Routine to map the CQL data to String
//
  String
CQL2String (const CQLExpression & o)
{
  CQLValue val = o.getTerms ()[0].getFactors ()[0].getValue ();

  if (val.getValueType () == CQLValue::Null_type)
    return "NULL_VALUE";

  return o.toString ();
}

//
// Routine to map the CQL type to CMPIPredOp
//
CMPIPredOp
CQL2PredOp (ExpressionOpType op, Boolean isInverted)
{

  static CMPIPredOp ops[] = {
    CMPI_PredOp_LessThan,
    CMPI_PredOp_GreaterThan,
    CMPI_PredOp_Equals,
    CMPI_PredOp_LessThanOrEquals,
    CMPI_PredOp_GreaterThanOrEquals,
    CMPI_PredOp_NotEquals,
    (CMPIPredOp) 0,             /* Is NULL */
    (CMPIPredOp) 0,             /* Is Not NULL */
    CMPI_PredOp_Isa,
    CMPI_PredOp_Like
  };
  CMPIPredOp op_type = (CMPIPredOp) 0;

  if ((int) op <= (int) sizeof (ops))
    op_type = ops[(int) op];

  /* This is neccesary b/c CQL does not map "NOT (x LIKE a )" to 
     "x NOT_LIKE a". Hence we do it for it. This also applies to
     LIKE operation. */
  if ((isInverted) && (op == ISA))
    op_type = CMPI_PredOp_NotIsa;

  if ((isInverted) && (op == LIKE))
    op_type = CMPI_PredOp_NotLike;

  return op_type;
}

//
// Routine to map the CQL Value type to CMPI_QueryOperand type.
//
CMPI_QueryOperand::Type
CQL2Type (CQLValue::CQLValueType typ)
{
  switch (typ)
    {
    case CQLValue::Null_type:
      return CMPI_QueryOperand::NULL_TYPE;
    case CQLValue::Sint64_type:
      return CMPI_QueryOperand::SINT64_TYPE;
    case CQLValue::Uint64_type:
      return CMPI_QueryOperand::UINT64_TYPE;
    case CQLValue::Real_type:
      return CMPI_QueryOperand::REAL_TYPE;
    case CQLValue::String_type:
      return CMPI_QueryOperand::STRING_TYPE;
    case CQLValue::CIMDateTime_type:
      return CMPI_QueryOperand::DATETIME_TYPE;
    case CQLValue::CIMReference_type:
      return CMPI_QueryOperand::REFERENCE_TYPE;
    case CQLValue::CQLIdentifier_type:
      return CMPI_QueryOperand::PROPERTY_TYPE;
    case CQLValue::CIMObject_type:
      /* IBMKR: Not sure about this one */
      return CMPI_QueryOperand::OBJECT_TYPE;
    case CQLValue::Boolean_type:
      return CMPI_QueryOperand::OBJECT_TYPE;
    default:
      break;
    }
  return CMPI_QueryOperand::NULL_TYPE;
}

void
CMPI_Cql2Dnf::_populateTableau ()
{

  cqs.validate ();
  cqs.applyContext ();
  cqs.normalizeToDOC ();

  CQLPredicate pred = cqs.getPredicate ();
  Array < CQLPredicate > pred_Array;

  if (pred.isSimple ())
    {
      pred_Array.append (pred);
      CQLSimplePredicate aa = pred.getSimplePredicate ();
    }
  else
    {
      pred_Array = pred.getPredicates ();
    }

  _tableau.reserveCapacity (pred_Array.size ());

  for (Uint32 i = 0; i < pred_Array.size (); i++)
    {
      CQLPredicate pred = pred_Array[i];

      if (pred.isSimple ())
        {
          CMPI_TableauRow tr;
          CQLSimplePredicate simple = pred.getSimplePredicate ();

          CQLExpression lhs_cql = simple.getLeftExpression ();
          CQLExpression rhs_cql = simple.getRightExpression ();

          CMPIPredOp opr =
            CQL2PredOp (simple.getOperation (), pred.getInverted ());

          CQLValue lhs_val;
          CQLValue rhs_val;
          if (lhs_cql.getTerms ().size () != 0)
            lhs_val = lhs_cql.getTerms ()[0].getFactors ()[0].getValue ();
          if (rhs_cql.getTerms ().size () != 0)
            rhs_val = rhs_cql.getTerms ()[0].getFactors ()[0].getValue ();

          CMPI_QueryOperand lhs (CQL2String (lhs_cql),
                                 CQL2Type (lhs_val.getValueType ()));
          CMPI_QueryOperand rhs (CQL2String (rhs_cql),
                                 CQL2Type (rhs_val.getValueType ()));

          tr.append (CMPI_term_el (false, opr, lhs, rhs));
          _tableau.append (tr);

        }
    }
}

CMPI_Cql2Dnf::CMPI_Cql2Dnf ()
{
  _tableau.clear ();

}

CMPI_Cql2Dnf::CMPI_Cql2Dnf (const CQLSelectStatement qs):
cqs (qs)
{
  _tableau.clear ();
  _populateTableau ();
}


CMPI_Cql2Dnf::~CMPI_Cql2Dnf ()
{
}


PEGASUS_NAMESPACE_END
