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
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "QueryExpression.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <Pegasus/WQL/WQLParser.h>

#include <iostream>

PEGASUS_NAMESPACE_BEGIN

QueryExpression::QueryExpression():
  _ss(NULL)
{
}

QueryExpression::QueryExpression(String queryLang, String query, QueryContext& ctx):
  _queryLang(queryLang)
{
   String cql("CIM:CQL");
   String wql("WQL");

   if(String::compareNoCase(queryLang,cql) == 0)
   {
     CQLSelectStatement* cqlss = new CQLSelectStatement(queryLang, query, ctx);

     // Compile the statement
     CQLParser::parse(query, *cqlss);

     // Finish checking the statement for CQL by applying the class contexts to
     // the chained identifiers.
     cqlss->applyContext();

     _ss = cqlss;
   }
   else if(String::compareNoCase(queryLang,wql) == 0)
   {
     WQLSelectStatement* wqlss = new WQLSelectStatement(queryLang, query, ctx);

     // Compile the statement
     WQLParser::parse(query, *wqlss);

     _ss = wqlss;
   }
   else
   {
     throw QueryLanguageInvalidException(
				MessageLoaderParms(String("Query.QueryExpression.INVALID_QUERY_LANGUAGE"),
						   String("The query language specified is invalid: $0."),
						   queryLang));  
   }
}

QueryExpression::QueryExpression(String queryLang, String query):
  _queryLang(queryLang)
{
   String cql("CIM:CQL");
   String wql("WQL");

   if(String::compareNoCase(queryLang,cql) == 0)
   {
     CQLSelectStatement* cqlss = new CQLSelectStatement(queryLang, query);

     // Note: cannot call parse the CQLSelectStatement
     // because there is no QueryContext.
     // The parse will happen when setQueryContext is called.

     _ss = cqlss;
   }
   else if(String::compareNoCase(queryLang,wql) == 0)
   {
     WQLSelectStatement* wqlss = new WQLSelectStatement(queryLang, query);

     // Compile the statement
     WQLParser::parse(query, *wqlss);

     _ss = wqlss;
   }
   else
   {
     throw QueryLanguageInvalidException(
            MessageLoaderParms(String("Query.QueryExpression.INVALID_QUERY_LANGUAGE"),
            String("The query language specified is invalid: $0."),
            queryLang));
   }
}

QueryExpression::QueryExpression(const QueryExpression& expr):
  _queryLang(expr._queryLang)
{
  if (expr._ss == NULL)
  { 
    _ss = NULL;
  }
  else
  {
    _ss = NULL;

    String cql("CIM:CQL");
    String wql("WQL");

    if(String::compareNoCase(expr._queryLang, cql) == 0)
    {
      CQLSelectStatement* tempSS = dynamic_cast<CQLSelectStatement*>(expr._ss);
      if (tempSS != NULL)
        _ss = new CQLSelectStatement(*tempSS);
    }
    else if (String::compareNoCase(expr._queryLang, wql) == 0)
    {
      WQLSelectStatement* tempSS = dynamic_cast<WQLSelectStatement*>(expr._ss);
      if (tempSS != NULL)
        _ss = new WQLSelectStatement(*tempSS);
    }
  }
}

QueryExpression::~QueryExpression()
{
  if (_ss != NULL)
   delete _ss;
}

QueryExpression QueryExpression::operator=(const QueryExpression& rhs)
{
  if (this == &rhs)
    return *this;

  if (_ss != NULL)
    delete _ss;
  _ss = NULL;
 
  if (rhs._ss != NULL)
  {
    String cql("CIM:CQL");
    String wql("WQL");

    if(String::compareNoCase(rhs._queryLang, cql) == 0)
    {
      CQLSelectStatement* tempSS = dynamic_cast<CQLSelectStatement*>(rhs._ss);
      if (tempSS != NULL)
        _ss = new CQLSelectStatement(*tempSS);
    }
    else if (String::compareNoCase(rhs._queryLang, wql) == 0)
    {
      WQLSelectStatement* tempSS = dynamic_cast<WQLSelectStatement*>(rhs._ss);
      if (tempSS != NULL)
        _ss = new WQLSelectStatement(*tempSS);
    }
  }

  _queryLang = rhs._queryLang;

  return *this;
}

String QueryExpression::getQueryLanguage() const
{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

   return _ss->getQueryLanguage();
}

String QueryExpression::getQuery() const
{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  return _ss->getQuery();
}

Boolean QueryExpression::evaluate(const CIMInstance & inst) const
{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  try
  {
    return _ss->evaluate(inst);
  }
  catch (QueryException&)
  {
    throw;
  }
  catch (Exception& e)
  {
    throw PEGASUS_QUERY_EXCEPTION(e.getContentLanguages(), e.getMessage());
  }
}

CIMPropertyList QueryExpression::getPropertyList(const CIMObjectPath& objectPath) const
{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  try
  {
    return _ss->getPropertyList(objectPath);
  }
  catch (QueryException&)
  {
    throw;
  }
  catch (Exception& e)
  {
    throw PEGASUS_QUERY_EXCEPTION(e.getContentLanguages(), e.getMessage());
  }
}

void QueryExpression::applyProjection(CIMInstance instance){
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  try
  {
    _ss->applyProjection(instance);
  }
  catch (QueryException&)
  {
    throw;
  }
  catch (Exception& e)
  {
    throw PEGASUS_QUERY_EXCEPTION(e.getContentLanguages(), e.getMessage());
  }
}

void QueryExpression::validate(){
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  try
  {
    _ss->validate();
  }
  catch (QueryException&)
  {
    throw;
  }
  catch (Exception& e)
  {
    throw PEGASUS_QUERY_EXCEPTION(e.getContentLanguages(), e.getMessage());
  }
}
 
Array<CIMObjectPath> QueryExpression::getClassPathList() const{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  try
  {
   return _ss->getClassPathList();
  }
  catch (QueryException&)
  {
    throw;
  }
  catch (Exception& e)
  {
    throw PEGASUS_QUERY_EXCEPTION(e.getContentLanguages(), e.getMessage());
  }   
}

SelectStatement* QueryExpression::getSelectStatement(){
   return _ss;
}

void QueryExpression::setQueryContext(QueryContext& inCtx) throw (Exception)
{
  if(_ss == NULL){
    MessageLoaderParms parms("Query.QueryExpression.SS_IS_NULL",
                             "Trying to process a query with a NULL SelectStatement.");
    throw QueryException(parms);
  }

  // SelectStatement only allows this to be called once.
  _ss->setQueryContext(inCtx);

  String cql("CIM:CQL");

  if(String::compareNoCase(_queryLang, cql) == 0)
  {
    // Now that we have a QueryContext, we can finish compiling 
    // the CQL statement.
    CQLSelectStatement* tempSS = dynamic_cast<CQLSelectStatement*>(_ss);
    if (tempSS != NULL)
    {
      CQLParser::parse(getQuery(), *tempSS);
      tempSS->applyContext();
    }
  }
}

PEGASUS_NAMESPACE_END
