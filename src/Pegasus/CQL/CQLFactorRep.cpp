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

#include "CQLFactorRep.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLScope.h>
#include <Pegasus/CQL/QueryContext.h>
#include <Pegasus/Common/Tracer.h>
 
PEGASUS_NAMESPACE_BEGIN

CQLFactorRep::CQLFactorRep(const CQLFactorRep* rep)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::CQLFactorRep()");

  _CQLVal = rep->_CQLVal;
  _CQLFunct = rep->_CQLFunct;
  _CQLExp = rep->_CQLExp;
  _invert = rep->_invert;
  _simpleValue = rep->_simpleValue;
  _containedType = rep->_containedType;

  PEG_METHOD_EXIT();
}

CQLFactorRep::CQLFactorRep(const CQLValue& inCQLVal)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::CQLFactorRep()");

  _CQLVal = inCQLVal;
  _simpleValue = true;
  _containedType = VALUE;
  
  PEG_METHOD_EXIT();
}

CQLFactorRep::CQLFactorRep(const CQLExpression& inCQLExp)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::CQLFactorRep()");

  _CQLExp = inCQLExp;
  _simpleValue = false;
  _containedType = EXPRESSION;

  PEG_METHOD_EXIT();
}

CQLFactorRep::CQLFactorRep(const CQLFunction& inCQLFunc)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::CQLFactorRep()");

  _CQLFunct = inCQLFunc;
  _simpleValue = false;
  _containedType = FUNCTION;

  PEG_METHOD_EXIT();
}

CQLValue CQLFactorRep::getValue()const
{
   return _CQLVal;
}

CQLValue CQLFactorRep::resolveValue(const CIMInstance& CI, const QueryContext& QueryCtx)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::resolveValue()");

  if(_containedType == EXPRESSION)
    {
      PEG_METHOD_EXIT();
      return _CQLExp.resolveValue(CI,QueryCtx);
    }
  else if (_containedType == FUNCTION)
    {
      PEG_METHOD_EXIT();
      return _CQLFunct.resolveValue(CI,QueryCtx);
    }
  else
    {
      _CQLVal.resolve(CI,QueryCtx);
      PEG_METHOD_EXIT();
      return _CQLVal;
    }
}

Boolean CQLFactorRep::isSimple()const
{
   return _simpleValue;
}

Boolean CQLFactorRep::isSimpleValue()const
{
   return _simpleValue;
}

CQLFunction CQLFactorRep::getCQLFunction()const
{
   return _CQLFunct;
}

CQLExpression CQLFactorRep::getCQLExpression()const
{
   return _CQLExp;
}

String CQLFactorRep::toString()const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::toString()");

  if(_containedType == VALUE)
    {
      PEG_METHOD_EXIT();
      return _CQLVal.toString();
    }
  
  if(_containedType == FUNCTION)
    {
      PEG_METHOD_EXIT();
      return _CQLFunct.toString();
    }
  else
    {
      PEG_METHOD_EXIT();
      return _CQLExp.toString();
    }
}

void CQLFactorRep::applyContext(QueryContext& inContext,
                                CQLChainedIdentifier& inCid)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::applyContext()");
  
  if(_containedType == FUNCTION)
    {
      _CQLFunct.applyContext(inContext);
    }
  else if(_containedType == EXPRESSION)
    {
      _CQLExp.applyContext(inContext);
    }
  else 
    {
      _CQLVal.applyContext(inContext,inCid);
    }

  PEG_METHOD_EXIT();
  return;
}

Boolean CQLFactorRep::operator==(const CQLFactorRep& rep)const
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLFactorRep::operator==()");
  
  if(_CQLExp != rep._CQLExp)
    {
      PEG_METHOD_EXIT();
      return false;
    }
  if(CQLValue(_CQLVal) != rep._CQLVal) // Why?
    {
      PEG_METHOD_EXIT();
      return false;
    }
  if(_CQLFunct != rep._CQLFunct)
    {
      PEG_METHOD_EXIT();
      return false;
    }
  if(_invert != rep._invert)
    {
      PEG_METHOD_EXIT();
      return false;
    }
  if(_simpleValue != rep._simpleValue)
    {
      PEG_METHOD_EXIT();
      return false;
    }
  if(_containedType != rep._containedType)
    {
      PEG_METHOD_EXIT();
      return false;
    }
  
  PEG_METHOD_EXIT();
  return true;
}

Boolean CQLFactorRep::operator!=(const CQLFactorRep& rep)const
{
  return (!operator==(rep));
}
PEGASUS_NAMESPACE_END
