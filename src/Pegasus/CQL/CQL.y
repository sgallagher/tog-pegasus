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
%{
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Query/QueryCommon/QueryException.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/CQL/CQLFactory.h>
#include "CQLObjects.h"
#include <stdio.h>

#define yyparse CQL_parse
#define CQLPREDICATE 0
#define CQLVALUE 1
#define CQLIDENTIFIER 2
#define CQLFUNCTION 3
#define CQLCHAINEDIDENTIFIER 4

#ifdef CQL_DEBUG_GRAMMAR
#define DEBUG_GRAMMAR 1
#else
#define DEBUG_GRAMMAR 0
#endif

int yylex();
char msg[100];
void printf_(char * msg){
	if(DEBUG_GRAMMAR == 1)
		printf("%s\n",msg);
}
extern char * yytext;
int chain_state;
CQLFactory _factory = CQLFactory();
extern int CQL_error(const char *err);

enum CQLType { Id, CId, Val, Func, Fact, Trm, Expr, SPred, Pred, Str };

typedef struct CQLObjPtr {
        void* _ptr;
		  CQLType type;		  
} CQLOBJPTR;

Array<CQLObjPtr> _ptrs;
CQLOBJPTR _ObjPtr;


void CQL_Bison_Cleanup(){
	for(Uint32 i = 0; i < _ptrs.size(); i++){
	  if(_ptrs[i]._ptr){
		switch(_ptrs[i].type){
			case Id:
					delete (CQLIdentifier*)_ptrs[i]._ptr;
					break;
			case CId:
					delete (CQLChainedIdentifier*)_ptrs[i]._ptr;
					break;
			case Val:
					delete (CQLValue*)_ptrs[i]._ptr;
					break;
			case Func:
					delete (CQLFunction*)_ptrs[i]._ptr;
					break;
			case Fact:
					delete (CQLFactor*)_ptrs[i]._ptr;
					break;
			case Trm:
					delete (CQLTerm*)_ptrs[i]._ptr;
					break;
			case Expr:
					delete (CQLExpression*)_ptrs[i]._ptr;
					break;
			case SPred:
					delete (CQLSimplePredicate*)_ptrs[i]._ptr;
					break;
			case Pred:
					delete (CQLPredicate*)_ptrs[i]._ptr;
					break;
			case Str:
					delete (String*)_ptrs[i]._ptr;
		}
	  }
	}
	_ptrs.clear();
   _factory.cleanup();
	_factory = CQLFactory();
}

PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* CQL_globalParserState;
Array<CQLPredicate> _arglist;


PEGASUS_NAMESPACE_END


%}
%union {
   char * strValue;
   String * _string;
   CQLValue * _value;
   CQLIdentifier * _identifier;
   CQLChainedIdentifier * _chainedIdentifier;
   CQLPredicate * _predicate;
   ExpressionOpType _opType;
   void * _node;
}

/* terminals */
%token <strValue> IDENTIFIER 
%token <strValue> STRING_LITERAL
%token <strValue> BINARY
%token <strValue> NEGATIVE_BINARY
%token <strValue> HEXADECIMAL
%token <strValue> NEGATIVE_HEXADECIMAL
%token <strValue> INTEGER 
%token <strValue> NEGATIVE_INTEGER
%token <strValue> REAL 
%token <strValue> NEGATIVE_REAL
%token <strValue> _TRUE 
%token <strValue> _FALSE 
%token <strValue> SCOPED_PROPERTY
%token <strValue> LPAR 
%token <strValue> RPAR 
%token <strValue> HASH
%token <strValue> DOT 
%token <strValue> LBRKT 
%token <strValue> RBRKT 
%token <strValue> UNDERSCORE
%token <strValue> COMMA 
%token <strValue> CONCAT 
%token <strValue> DBL_PIPE
%token <strValue> PLUS
%token <strValue> MINUS 
%token <strValue> TIMES 
%token <strValue> DIV 
%token <strValue> IS 
%token <strValue> _NULL 
%token <_opType> _EQ
%token <_opType> _NE 
%token <_opType> _GT 
%token <_opType> _LT 
%token <_opType> _GE 
%token <_opType> _LE 
%token <_opType> _ISA 
%token <_opType> _LIKE 
%token <strValue> NOT _AND _OR 
%token <strValue> SCOPE 
%token <strValue> ANY EVERY IN SATISFIES 
%token <strValue> STAR 
%token <strValue> DOTDOT 
%token <strValue> SHARP DISTINCT 
%token <strValue> SELECT 
%token <strValue> FIRST 
%token <strValue> FROM 
%token <strValue> WHERE 
%token <strValue> ORDER 
%token <strValue> BY 
%token <strValue> ASC 
%token <strValue> DESC 
%token <strValue> AS 
%token <strValue> UNEXPECTED_CHAR


/* grammar - non terminals */
%type <_identifier> identifier
%type <_identifier> class_name
%type <_identifier> class_path
%type <_identifier> scoped_property
%type <_string> literal_string
%type <_value> binary_value
%type <_value> hex_value
%type <_value> decimal_value
%type <_value> real_value
%type <_value> literal
%type <_string> array_index
%type <_string> array_index_list
%type <_node> chain
%type <_predicate> concat
%type <_predicate> factor
%type <_predicate> term
%type <_predicate> arith
%type <_value> value_symbol
%type <_predicate> arith_or_value_symbol
%type <_opType> comp_op
%type <_predicate> comp
%type <_predicate> expr_factor
%type <_predicate> expr_term
%type <_predicate> expr
%type <_node> arg_list
%type <_node> from_specifier
%type <_node> from_criteria
%type <_chainedIdentifier> star_expr
%type <_node> selected_entry
%type <_node> select_list
%type <_node> select_list_tail
%type <_node> search_condition
%type <_node> optional_where
%type <_node> select_statement

%start select_statement

%%

	/**
		The general pattern:  We construct small objects first (CQLIdentifiers, CQLValues etc) which
		get forwared to more complex rules where more complex objects are made.  Eventually we have constructed
		a top level CQLPredicate that gets added to the CQL_globalParserState select statement.

		Along the way we keep track of which rule we are processing so that any errors encountered are specific
		enough to actually do us some good.

		The CQLFactory object is used primarily to save space/coding efforts.  It enables us to build complex CQL
		objects with one simple call, or query complex objects through one simple call.
		
	*/

/* CQLIdentifier */
identifier  : IDENTIFIER 
             { 
		 CQL_globalParserState->currentRule = "identifier";
                 sprintf(msg,"BISON::identifier\n");
		 printf_(msg);
                 if(isUTF8Str(CQL_lval.strValue)){	
                    $$ = new CQLIdentifier(String(CQL_lval.strValue));
					     _ObjPtr._ptr = $$;
					     _ObjPtr.type = Id;
					     _ptrs.append(_ObjPtr);
					  }else{
                    sprintf(msg,"BISON::identifier-> BAD UTF\n");
		              printf_(msg);
		              throw CQLSyntaxErrorException(
					         MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
							   String("Bad UTF8 encountered parsing rule $0 in position $1."),
							   String("identifier"),
							   CQL_globalParserState->currentTokenPos)
						  );
					  }
             }
;

class_name : identifier  
             {
		 CQL_globalParserState->currentRule = "class_name";
                 sprintf(msg,"BISON::class_name = %s\n", (const char *)($1->getName().getString().getCString())); 
		 printf_(msg);
		$$ = $1;
             }
;

class_path : class_name 
             { 
		CQL_globalParserState->currentRule = "class_path";
                 sprintf(msg,"BISON::class_path\n"); 
		 printf_(msg);
		 $$ = $1;
             }
;
/*
property_scope : class_path SCOPE
             { 
                 sprintf(msg,"BISON::property_scope = %s\n",$1); 
		 printf_(msg);

                
             }*/
;

/* CQLIdentifier */
scoped_property : SCOPED_PROPERTY
                  {
							/*
			   			SCOPED_PROPERTY can be:
			   			- "A::prop"
			   			- "A::class.prop"
			   			- "A::class.prop#'OK'
			   			- "A::class.prop[4]"
							*/
							CQL_globalParserState->currentRule = "scoped_property";
							sprintf(msg,"BISON::scoped_property = %s\n",CQL_lval.strValue);
							printf_(msg);
                    if(isUTF8Str(CQL_lval.strValue)){
                       String tmp(CQL_lval.strValue);
		        			  $$ = new CQLIdentifier(tmp);
				  			  _ObjPtr._ptr = $$;
              			  _ObjPtr.type = Id;
              			  _ptrs.append(_ObjPtr);

                    }else{
                       sprintf(msg,"BISON::scoped_property-> BAD UTF\n");
		                 printf_(msg);
		                 throw CQLSyntaxErrorException(
					         MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
							   String("Bad UTF8 encountered parsing rule $0 in position $1."),
							   String("scoped_property"),
							   CQL_globalParserState->currentTokenPos)
						     );
					     }
		        	  }
;   

/* String */
literal_string : STRING_LITERAL 
             { 
		/*
		   We make sure the literal is valid UTF8, then make a String
		*/
		CQL_globalParserState->currentRule = "literal_string";
                sprintf(msg,"BISON::literal_string-> %s\n",CQL_lval.strValue); 
		printf_(msg);

		if(isUTF8Str(CQL_lval.strValue)){
		     $$ = new String(CQL_lval.strValue);
			  _ObjPtr._ptr = $$;
           _ObjPtr.type = Str;
           _ptrs.append(_ObjPtr);
		}else{
		    sprintf(msg,"BISON::literal_string-> BAD UTF\n");
		    printf_(msg);
		    throw CQLSyntaxErrorException(
					MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
							   String("Bad UTF8 encountered parsing rule $0 in position $1."),
							   String("literal_string"),
							   CQL_globalParserState->currentTokenPos)
						 );
		}
             }
;

/* CQLValue */
binary_value : BINARY 
	       { 
		   CQL_globalParserState->currentRule = "binary_value->BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary); 
						 _ObjPtr._ptr = $$;
                   _ObjPtr.type = Val;
                   _ptrs.append(_ObjPtr);
               }
             | NEGATIVE_BINARY 
               { 
		   CQL_globalParserState->currentRule = "binary_value->NEGATIVE_BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary, false); 
						 _ObjPtr._ptr = $$;
                   _ObjPtr.type = Val;
                   _ptrs.append(_ObjPtr);
               }
;

/* CQLValue */
hex_value : HEXADECIMAL 
            { 
		CQL_globalParserState->currentRule = "hex_value->HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex);
					 _ObjPtr._ptr = $$;
                _ObjPtr.type = Val;
                _ptrs.append(_ObjPtr);
            }
          | NEGATIVE_HEXADECIMAL 
            { 
		CQL_globalParserState->currentRule = "hex_value->NEGATIVE_HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex, false);
				    _ObjPtr._ptr = $$;
                _ObjPtr.type = Val;
                _ptrs.append(_ObjPtr);
            }
;

/* CQLValue */
decimal_value : INTEGER 
                { 
		    CQL_globalParserState->currentRule = "decimal_value->INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal); 
						  _ObjPtr._ptr = $$;
                    _ObjPtr.type = Val;
                    _ptrs.append(_ObjPtr);
                }
              | NEGATIVE_INTEGER 
                { 
		    CQL_globalParserState->currentRule = "decimal_value->NEGATIVE_INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal, false);
						  _ObjPtr._ptr = $$;
                    _ObjPtr.type = Val;
                    _ptrs.append(_ObjPtr);
                }
;

/* CQLValue */
real_value : REAL 
             { 
		 CQL_globalParserState->currentRule = "real_value->REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real);
					  _ObjPtr._ptr = $$;
                 _ObjPtr.type = Val;
                 _ptrs.append(_ObjPtr);
             }
           | NEGATIVE_REAL 
             { 
		 CQL_globalParserState->currentRule = "real_value->NEGATIVE_REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real, false);
					  _ObjPtr._ptr = $$;
                 _ObjPtr.type = Val;
                 _ptrs.append(_ObjPtr);
             }
;

/* CQLValue */
literal : literal_string 
          {
	      CQL_globalParserState->currentRule = "literal->literal_string";
              sprintf(msg,"BISON::literal->literal_string\n");
	      printf_(msg);
              $$ = new CQLValue(*$1);
				  _ObjPtr._ptr = $$;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
        | decimal_value
          {
	      CQL_globalParserState->currentRule = "literal->decimal_value";
              sprintf(msg,"BISON::literal->decimal_value\n");
	      printf_(msg);

          }
        | binary_value
          {
              CQL_globalParserState->currentRule = "literal->binary_value";
              sprintf(msg,"BISON::literal->binary_value\n");
	      printf_(msg);

          }
        | hex_value
          {
              CQL_globalParserState->currentRule = "literal->hex_value";
              sprintf(msg,"BISON::literal->hex_value\n");
	      printf_(msg);

          }
        | real_value
          {
              CQL_globalParserState->currentRule = "literal->real_value";
              sprintf(msg,"BISON::literal->real_value\n");
	      printf_(msg);

          }
        | _TRUE
          {
	      CQL_globalParserState->currentRule = "literal->_TRUE";
              sprintf(msg,"BISON::literal->_TRUE\n");
	      printf_(msg);

              $$ = new CQLValue(Boolean(true));
				  _ObjPtr._ptr = $$;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
        | _FALSE
          {
	      CQL_globalParserState->currentRule = "literal->_FALSE";
              sprintf(msg,"BISON::literal->_FALSE\n");
	      printf_(msg);

              $$ = new CQLValue(Boolean(false));
				  _ObjPtr._ptr = $$;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
;

/* String */
array_index : expr
              {
		  CQL_globalParserState->currentRule = "array_index->expr";
                  sprintf(msg,"BISON::array_index->expr\n");
		  printf_(msg);

		  CQLValue* _val = (CQLValue*)_factory.getObject($1,Predicate,Value);
		  $$ = new String(_val->toString());
		  _ObjPtr._ptr = $$;
        _ObjPtr.type = Str;
        _ptrs.append(_ObjPtr);
              }
;

/* String */
array_index_list : array_index
                   {
		       CQL_globalParserState->currentRule = "array_index_list->array_index";
                       sprintf(msg,"BISON::array_index_list->array_index\n");
		       printf_(msg);
 		       $$ = $1;
                   }
;

/* void* */
chain : literal
        {
            CQL_globalParserState->currentRule = "chain->literal";
            sprintf(msg,"BISON::chain->literal\n");
	    printf_(msg);

            chain_state = CQLVALUE;
	    $$ = _factory.makeObject($1,Predicate);  
        }
      | LPAR expr RPAR
        {
	    CQL_globalParserState->currentRule = "chain-> ( expr )";
            sprintf(msg,"BISON::chain-> ( expr )\n");
	    printf_(msg);

            chain_state = CQLPREDICATE;
	    $$ = $2;
        }
      | identifier
        {
	   CQL_globalParserState->currentRule = "chain->identifier";
           sprintf(msg,"BISON::chain->identifier\n");
	   printf_(msg);

           chain_state = CQLIDENTIFIER;
	   $$ = _factory.makeObject($1,Predicate);
        }
      | identifier HASH literal_string
        {
	    CQL_globalParserState->currentRule = "chain->identifier#literal_string";
            sprintf(msg,"BISON::chain->identifier#literal_string\n");
	    printf_(msg);

            String tmp = $1->getName().getString();
            tmp.append("#").append(*$3);
            CQLIdentifier _id(tmp);
   	    $$ = _factory.makeObject(&_id,Predicate);
	    chain_state = CQLIDENTIFIER;
        }
      | scoped_property
        {
	    CQL_globalParserState->currentRule = "chain->scoped_property";
	    sprintf(msg,"BISON::chain-> scoped_property\n");
	    printf_(msg);

            chain_state = CQLIDENTIFIER;
	    $$ = _factory.makeObject($1,Predicate);
        }
      | identifier LPAR arg_list RPAR
        {
	    CQL_globalParserState->currentRule = "chain->identifier( arg_list )";
            sprintf(msg,"BISON::chain-> identifier( arg_list )\n");
	    printf_(msg);
            chain_state = CQLFUNCTION;
	    CQLFunction _func(*$1,_arglist);
	    $$ = (CQLPredicate*)(_factory.makeObject(&_func,Predicate));
	    _arglist.clear();
        }
      | chain DOT scoped_property
        {
	    CQL_globalParserState->currentRule = "chain->chain.scoped_property";
	    sprintf(msg,"BISON::chain-> chain DOT scoped_property : chain_state = %d\n",chain_state);
	    printf_(msg);

	    CQLIdentifier *_id;
	    if(chain_state == CQLIDENTIFIER){
	        _id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*$3);
		$$ = _factory.makeObject(&_cid,Predicate);
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid;
		_cid = ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
		_cid->append(*$3);
		_factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
		$$ = $1;
	    }else{
		/* error */
		String _msg("chain-> chain DOT scoped_property : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.scoped_property"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );
            }

            chain_state = CQLCHAINEDIDENTIFIER;
        }
      | chain DOT identifier
        {
	    CQL_globalParserState->currentRule = "chain->chain.identifier";
            sprintf(msg,"BISON::chain->chain.identifier : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*$3);
                $$ = _factory.makeObject(&_cid,Predicate);
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
                _cid->append(*$3);
                _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
                $$ = $1;
            }else{
                /* error */
		String _msg("chain-> chain DOT identifier : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.identifier"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );
            }
	    chain_state = CQLCHAINEDIDENTIFIER;

        }
      | chain DOT identifier HASH literal_string
        {
	    CQL_globalParserState->currentRule = "chain->chain.identifier#literal_string";
            sprintf(msg,"BISON::chain->chain.identifier#literal_string : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
              CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));	
              CQLChainedIdentifier _cid(*_id);
                String tmp($3->getName().getString());
                tmp.append("#").append(*$5);
                CQLIdentifier _id1(tmp);
                _cid.append(_id1);
                _factory.setObject(((CQLPredicate*)$1),&_cid,ChainedIdentifier);
                $$ = $1;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
              CQLChainedIdentifier *_cid =  ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
              String tmp($3->getName().getString());
                tmp.append("#").append(*$5);
                CQLIdentifier _id1(tmp);
                _cid->append(_id1);
                _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
                $$ = $1;
            }else{
                /* error */
		String _msg("chain->chain.identifier#literal_string : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.identifier#literal_string"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );
            }
                                                                                                        
            chain_state = CQLCHAINEDIDENTIFIER;

        }
      | chain LBRKT array_index_list RBRKT
        {
	    CQL_globalParserState->currentRule = "chain->chain[ array_index_list ]";
            sprintf(msg,"BISON::chain->chain[ array_index_list ] : chain_state = %d\n",chain_state);
	    printf_(msg);
	
            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));
		String tmp = _id->getName().getString();
		tmp.append("[").append(*$3).append("]");
		CQLIdentifier _id1(tmp);
		CQLChainedIdentifier _cid(_id1);
		_factory.setObject(((CQLPredicate*)$1),&_cid,ChainedIdentifier);
                $$ = $1;	
	    }else if(chain_state == CQLCHAINEDIDENTIFIER || chain_state == CQLVALUE){
		CQLPredicate* _pred = (CQLPredicate*)$1;
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
		CQLIdentifier tmpid = _cid->getLastIdentifier();
		String tmp = tmpid.getName().getString();
                tmp.append("[").append(*$3).append("]");
		CQLIdentifier _id1(tmp);
		CQLChainedIdentifier _tmpcid(_id1);
		if(_cid->size() == 1){
			_cid = &_tmpcid;
		}else{
			_cid->append(_id1);
		}
		_factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
                $$ = $1;
	    }else{
		/* error */
		String _msg("chain->chain[ array_index_list ] : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER or CQLVALUE");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER_OR_VALUE"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier or a CQLValue while parsing rule $0 in position $1."),
							   String("chain->chain[ array_index_list ]"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );
	    }
        }
;

concat : chain
         {
	     CQL_globalParserState->currentRule = "concat->chain";
             sprintf(msg,"BISON::concat->chain\n");
	     printf_(msg);

	     $$ = ((CQLPredicate*)$1);
         }
       | concat DBL_PIPE literal_string
         {
	         CQL_globalParserState->currentRule = "concat->concat || literal_string";
            sprintf(msg,"BISON::concat||literal_string\n");
	         printf_(msg);

		      CQLValue* tmpval = new CQLValue(*$3);
	         _ObjPtr._ptr = tmpval;
            _ObjPtr.type = Val;
            _ptrs.append(_ObjPtr);  

			   if((CQLPredicate*)$1->isSimple())
            {
               CQLSimplePredicate sp = ((CQLPredicate*)$1)->getSimplePredicate();
					if(sp.isSimple())
					{
                  CQLExpression exp = sp.getLeftExpression();
						if(exp.isSimple())
						{
                    CQLTerm* _term = ((CQLTerm*)(_factory.getObject($1, Predicate, Term)));
                    // check for simple literal values
                    Array<CQLFactor> factors = _term->getFactors();
                    for(Uint32 i = 0; i < factors.size(); i++){
                       if(!factors[i].isSimpleValue()){
                          MessageLoaderParms mparms("CQL.CQL_y.CONCAT_PRODUCTION_FACTORS_NOT_SIMPLE",
                                                   "The CQLFactors are not simple while processing rule $0.",
                                                    CQL_globalParserState->currentRule);
                          throw CQLSyntaxErrorException(mparms);
                       }else{
                          CQLValue val = factors[i].getValue();
                          if(val.getValueType() != CQLValue::String_type){
                             MessageLoaderParms mparms("CQL.CQL_y.CONCAT_PRODUCTION_VALUE_NOT_LITERAL",
                                                       "The CQLValue is not a string literal while processing rule $0.",
                                                       CQL_globalParserState->currentRule);
                             throw CQLSyntaxErrorException(mparms);
		                    }
                       }
                    }
                    CQLFactor* _fctr2 = ((CQLFactor*)(_factory.makeObject(tmpval, Factor)));
                    _term->appendOperation(concat,*_fctr2);
                    $$ = (CQLPredicate*)(_factory.makeObject(_term,Predicate)); 
						}
                  else
                  {
                     MessageLoaderParms mparms("CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                                               "The $0 is not simple while processing rule $1.",
															  String("CQLExpression"),
                                               CQL_globalParserState->currentRule);
                     throw CQLSyntaxErrorException(mparms);
                  }
					 }
                else
                {
                     MessageLoaderParms mparms("CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                                               "The $0 is not simple while processing rule $1.",
															  String("CQLSimplePredicate"),
                                               CQL_globalParserState->currentRule);
                     throw CQLSyntaxErrorException(mparms);
                }
				 }
             else
             {
                     MessageLoaderParms mparms("CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                                               "The $0 is not simple while processing rule $1.",
															  String("CQLPredicate"),
                                               CQL_globalParserState->currentRule);
                     throw CQLSyntaxErrorException(mparms);
             }
        }
;

factor : concat
	 {
	     CQL_globalParserState->currentRule = "factor->concat";
             sprintf(msg,"BISON::factor->concat\n");
	     printf_(msg);

	     $$ = $1;
         }         
      /* | PLUS concat
         {
	      add enum instead of _invert to CQLFactor,has to be either nothing, + or -
              get the factor and set the optype appropriately
	     CQL_globalParserState->currentRule = "concat->PLUS concat"; 
             printf("BISON::factor->PLUS concat\n");
             $$ = new CQLFactor(*(CQLValue*)$2);
         }
       | MINUS concat
         {
              get the factor and set the optype appropriately 
	     CQL_globalParserState->currentRule = "concat->MINUS concat";
             printf("BISON::factor->MINUS concat\n");
             CQLValue *tmp = (CQLValue*)$2;
             tmp->invert();
	     $$ = new CQLFactor(*tmp);
         }*/
;

term : factor
       {
	   CQL_globalParserState->currentRule = "term->factor";
           sprintf(msg,"BISON::term->factor\n");
	   printf_(msg);

           $$ = $1;
       }
    /* | term STAR factor
       {
	    get factor out of $1, get factor out of $3, appendoperation, then construct predicate and forward it 
           printf("BISON::term->term STAR factor\n");
	   CQL_globalParserState->currentRule = "term->term STAR factor";
           $1->appendOperation(mult, *$3);
           $$ = $1;
       }
     | term DIV factor
       {
            get factor out of $1, get factor out of $3, appendoperation, then construct predicate and forward it 
	   CQL_globalParserState->currentRule = "term->term DIV factor";
           printf("BISON::term->term DIV factor\n");
           $1->appendOperation(divide, *$3);
           $$ = $1;
       }*/
;

arith : term
        {
	    CQL_globalParserState->currentRule = "arith->term";
            sprintf(msg,"BISON::arith->term\n");
	    printf_(msg);

	    //CQLPredicate* _pred = new CQLPredicate(*$1);
//	    _factory._predicates.append(_pred);
            $$ = $1;
        }
     /* | arith PLUS term
        {
	     get term out of $1, get term out of $3, appendoperation, then construct predicate and forward it
	    CQL_globalParserState->currentRule = "arith->arith PLUS term";
            printf("BISON::arith->arith PLUS term\n");
            $1->appendOperation(TERM_ADD, *$3);
            $$ = $1;
        }
      | arith MINUS term
        {
	    get term out of $1, get term out of $3, appendoperation, then construct predicate and forward it 
	    CQL_globalParserState->currentRule = "arith->arith MINUS term";
            printf("BISON::arith->arith MINUS term\n");
	    $1->appendOperation(TERM_SUBTRACT, *$3);
            $$ = $1;
        }*/
;

value_symbol : HASH literal_string
               {
	  	   CQL_globalParserState->currentRule = "value_symbol->#literal_string";
                   sprintf(msg,"BISON::value_symbol->#literal_string\n");
                   printf_(msg);

		   String tmp("#");
		   tmp.append(*$2);
		   CQLIdentifier tmpid(tmp);
		   $$ = new CQLValue(tmpid);
			_ObjPtr._ptr = $$;
         _ObjPtr.type = Val;
         _ptrs.append(_ObjPtr);
               }
;

arith_or_value_symbol : arith
                        {
			    CQL_globalParserState->currentRule = "arith_or_value_symbol->arith";
                            sprintf(msg,"BISON::arith_or_value_symbol->arith\n");
			    printf_(msg);

			    $$ = $1;
                        }
                      | value_symbol
                        {
			    /* make into predicate */
			    CQL_globalParserState->currentRule = "arith_or_value_symbol->value_symbol";
                            sprintf(msg,"BISON::arith_or_value_symbol->value_symbol\n");
			    printf_(msg);

			    CQLFactor _fctr(*$1);
			    $$ = (CQLPredicate*)(_factory.makeObject(&_fctr, Predicate));
                        }
;

comp_op : _EQ 
          {
	      CQL_globalParserState->currentRule = "comp_op->_EQ";
              sprintf(msg,"BISON::comp_op->_EQ\n");
	      printf_(msg);
	      $$ = EQ;
          }
        | _NE
          {
	      CQL_globalParserState->currentRule = "comp_op->_NE";
              sprintf(msg,"BISON::comp_op->_NE\n");
	      printf_(msg);
	      $$ = NE;
          }
        | _GT 
          {
	      CQL_globalParserState->currentRule = "comp_op->_GT";
              sprintf(msg,"BISON::comp_op->_GT\n");
	      printf_(msg);
	      $$ = GT;
          }
        | _LT
          {
 	      CQL_globalParserState->currentRule = "comp_op->_LT";
              sprintf(msg,"BISON::comp_op->_LT\n");
	      printf_(msg);
	      $$ = LT;
          }
        | _GE
          {
	      CQL_globalParserState->currentRule = "comp_op->_GE";
              sprintf(msg,"BISON::comp_op->_GE\n");
	      printf_(msg);
	      $$ = GE;
          }
        | _LE
          {
	      CQL_globalParserState->currentRule = "comp_op->_LE";
              sprintf(msg,"BISON::comp_op->_LE\n");
	      printf_(msg);
	      $$ = LE;
          }
;

comp : arith
       {
	   CQL_globalParserState->currentRule = "comp->arith";
           sprintf(msg,"BISON::comp->arith\n");
	   printf_(msg);

	   $$ = $1;
       }
     | arith IS NOT _NULL
       {
	   CQL_globalParserState->currentRule = "comp->arith IS NOT _NULL";
           sprintf(msg,"BISON::comp->arith IS NOT _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject($1,Expression));
	   CQLSimplePredicate _sp(*_expr, IS_NOT_NULL);
           _factory.setObject($1,&_sp,SimplePredicate);
	   $$ = $1;
       }
     | arith IS _NULL
       {
	   CQL_globalParserState->currentRule = "comp->arith IS _NULL";
           sprintf(msg,"BISON::comp->arith IS _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject($1,Expression));
           CQLSimplePredicate _sp(*_expr, IS_NULL);
           _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
       }
     | arith comp_op arith_or_value_symbol
       {
	   CQL_globalParserState->currentRule = "comp->arith comp_op arith_or_value_symbol";
           sprintf(msg,"BISON::comp->arith comp_op arith_or_value_symbol\n");
	   printf_(msg);
	   if($1->isSimple() && $3->isSimple()){
		CQLExpression* _exp1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
		CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject($3,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
           	$$ = new CQLPredicate(_sp);
				_ObjPtr._ptr = $$;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
	   }else{
		/* error */
		String _msg("comp->arith comp_op arith_or_value_symbol : $1 is not simple OR $3 is not simple");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE"),
                                                           String("The CQLSimplePredicate is not simple while parsing rule $0 in position $1."),
							   String("comp->arith comp_op arith_or_value_symbol"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );
	   }
       }
     | value_symbol comp_op arith
       {
	   CQL_globalParserState->currentRule = "comp->value_symbol comp_op arith";
           sprintf(msg,"BISON::comp->value_symbol comp_op arith\n");
	   printf_(msg);

	   if($3->isSimple()){
           	CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject($1, Expression));
	        CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject($3,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
           	$$ = new CQLPredicate(_sp);
				_ObjPtr._ptr = $$;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
	   }else{
		/* error */
		String _msg("comp->value_symbol comp_op arith : $3 is not simple");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE"),
                                                           String("The CQLSimplePredicate is not simple while parsing rule $0 in position $1."),
                                                           String("comp->value_symbol comp_op arith"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );

	   }
       }
     | arith _ISA identifier
       {
	   CQL_globalParserState->currentRule = "comp->arith _ISA identifier";
	   /* make sure $1 isSimple(), get its expression, make simplepred->predicate */
           sprintf(msg,"BISON::comp->arith _ISA identifier\n");
	   printf_(msg);

	   CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
	   CQLChainedIdentifier _cid(*$3);
	   CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_cid,Expression));
           CQLSimplePredicate _sp(*_expr1, *_expr2, ISA);
	   _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
       }
     | arith _LIKE literal_string
       {
	   CQL_globalParserState->currentRule = "comp->arith _LIKE literal_string";
           sprintf(msg,"BISON::comp->arith _LIKE literal_string\n");
	   printf_(msg);

           CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
	   CQLValue _val(*$3);
           CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_val,Expression));
	   CQLSimplePredicate _sp(*_expr1, *_expr2, LIKE);
           _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
       }
;
expr_factor : comp
              {
		  CQL_globalParserState->currentRule = "expr_factor->comp";
                  sprintf(msg,"BISON::expr_factor->comp\n");
	          printf_(msg);

		  $$ = $1;
              }
            | NOT comp
              {
		           CQL_globalParserState->currentRule = "expr_factor->NOT comp";
                 sprintf(msg,"BISON::expr_factor->NOT comp\n");
	 	           printf_(msg);
        
		           $2->setInverted(!($2->getInverted()));
		           $$ = $2;
              }
;

expr_term : expr_factor
            {
	        CQL_globalParserState->currentRule = "expr_term->expr_factor";
                sprintf(msg,"BISON::expr_term->expr_factor\n");
		printf_(msg);

		$$ = $1;
            }
          | expr_term _AND expr_factor
            {
		CQL_globalParserState->currentRule = "expr_term->expr_term AND expr_factor";
		sprintf(msg,"BISON::expr_term->expr_term AND expr_factor\n");
		printf_(msg);

		$$ = new CQLPredicate();
           	$$->appendPredicate(*$1);
           	$$->appendPredicate(*$3, AND);	
				_ObjPtr._ptr = $$;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
            }
;

expr : expr_term 
       {
	  CQL_globalParserState->currentRule = "expr->expr_term";
          sprintf(msg,"BISON::expr->expr_term\n");
	  printf_(msg);

	  $$ = $1; 	   
       }
     | expr _OR expr_term
       {
	   CQL_globalParserState->currentRule = "expr->expr OR expr_term";
           sprintf(msg,"BISON::expr->expr OR expr_term\n");
	   printf_(msg);
	   $$ = new CQLPredicate();
	   $$->appendPredicate(*$1);
	   $$->appendPredicate(*$3, OR);
		_ObjPtr._ptr = $$;
      _ObjPtr.type = Pred;
      _ptrs.append(_ObjPtr);
       }
;

arg_list : {;}
        /* | STAR
           {
	       CQL_globalParserState->currentRule = "arg_list->STAR";
               sprintf(msg,"BISON::arg_list->STAR\n");
	       printf_(msg);

	       CQLIdentifier _id("*");
	       CQLPredicate* _pred = (CQLPredicate*)(_factory.makeObject(&_id,Predicate));
	       _arglist.append(*_pred); 
					   since arg_list can loop back on itself, 
					   we need to store away previous solutions 
					   production.  We keep track of previous productions
					   in the _arglist array and later pass that to CQLFunction
					   as part of chain: identifier LPAR arg_list RPAR
					
           }*/
         | expr
	   {
		   CQL_globalParserState->currentRule = "arg_list->arg_list_sub->expr";
                   sprintf(msg,"BISON::arg_list_sub->expr\n");
                   printf_(msg);

                   _arglist.append(*$1);/*
                                           since arg_list can loop back on itself,
                                           we need to store away previous solutions
                                           production.  We keep track of previous productions
                                           in the _arglist array and later pass that to CQLFunction
                                           as part of chain: identifier LPAR arg_list RPAR
                                        */
           }
/*
         | DISTINCT STAR
           {
               sprintf(msg,"BISON::arg_list->DISTINCT STAR\n");
	       printf_(msg);

	       CQLIdentifier _id("DISTINCTSTAR");
               CQLPredicate* _pred = (CQLPredicate*)(_factory.makeObject(&_id,Predicate));
               _arglist.append(*_pred);
           }
         | arg_list_sub arg_list_tail
           {
               sprintf(msg,"BISON::arg_list->arg_list_sub arg_list_tail\n");
	       printf_(msg);
           }
;

arg_list_sub : expr
               {
                   sprintf(msg,"BISON::arg_list_sub->expr\n");
		   printf_(msg);
		
		   _arlist.append(*$1);   		   
               }   
             | DISTINCT expr
               {
                   sprintf(msg,"BISON::arg_list_sub->DISTINCT expr\n");
		   printf_(msg);
	
		   String tmp1("DISTINCT");
		   CQLIdentifier* _id = (CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier));
		   String tmp2(_id->getName().getString());
		   tmp1.append(tmp2);
		   CQLIdentifier _id1(tmp1);
		   
   		   
               }
;

arg_list_tail : {;}
              | COMMA arg_list_sub arg_list_tail
              {
                  sprintf(msg,"BISON::arg_list_tail->COMMA arg_list_sub arg_list_tail\n");
		  printf_(msg);
              }
;
*/
from_specifier : class_path
                 {
		     CQL_globalParserState->currentRule = "from_specifier->class_path";
                     sprintf(msg,"BISON::from_specifier->class_path\n");
		     printf_(msg);

		     CQL_globalParserState->statement->appendClassPath(*$1);
                 } 

		| class_path AS identifier
		  {
			CQL_globalParserState->currentRule = "from_specifier->class_path AS identifier";
			sprintf(msg,"BISON::from_specifier->class_path AS identifier\n");
			printf_(msg);

			CQLIdentifier _class(*$1);
			String _alias($3->getName().getString());
			CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
			CQL_globalParserState->statement->appendClassPath(_class);
		  }
		| class_path identifier
		  {
			CQL_globalParserState->currentRule = "from_specifier->class_path identifier";
			sprintf(msg,"BISON::from_specifier->class_path identifier\n");
			printf_(msg);

			CQLIdentifier _class(*$1);
                        String _alias($2->getName().getString());
                        CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
                        CQL_globalParserState->statement->appendClassPath(_class);
		  }
;

from_criteria : from_specifier
                {
		    CQL_globalParserState->currentRule = "from_criteria->from_specifier";
                    sprintf(msg,"BISON::from_criteria->from_specifier\n");
		    printf_(msg);
                }
;

star_expr : STAR
            {
		CQL_globalParserState->currentRule = "star_expr->STAR";
                sprintf(msg,"BISON::star_expr->STAR\n");
		printf_(msg);

		CQLIdentifier _id("*");
		$$ = (CQLChainedIdentifier*)(_factory.makeObject(&_id,ChainedIdentifier));
            }
	  | chain DOT STAR
	    {
		CQL_globalParserState->currentRule = "star_expr->chain.*";
		sprintf(msg,"BISON::star_expr->chain.*\n");
                printf_(msg);
		CQLChainedIdentifier* _tmp = (CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier));
		CQLChainedIdentifier* _cid = new CQLChainedIdentifier(*_tmp);
                CQLIdentifier _id("*");
		_cid->append(_id);
                $$ = _cid;
					 _ObjPtr._ptr = $$;
                _ObjPtr.type = CId;
                _ptrs.append(_ObjPtr);
	    }
;

selected_entry : expr 
                 {
		     CQL_globalParserState->currentRule = "selected_entry->expr";
                     sprintf(msg,"BISON::selected_entry->expr\n");
		     printf_(msg);
		     if($1->isSimpleValue()){
		        CQLChainedIdentifier *_cid = (CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier));
		        CQL_globalParserState->statement->appendSelectIdentifier(*_cid);
		     }else{
			/* error */
			String _msg("selected_entry->expr : $1 is not a simple value");
		 	throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE_VALUE"),
                                                           String("The CQLPredicate is not a simple value while parsing rule $0 in position $1."),
                                                           String("selected_entry->expr"),
                                                           CQL_globalParserState->currentTokenPos)
                                                 );	
		     }
                 }
               | star_expr
                 {
		     CQL_globalParserState->currentRule = "selected_entry->star_expr";
                     sprintf(msg,"BISON::selected_entry->star_expr\n");
		     printf_(msg);
		     CQL_globalParserState->statement->appendSelectIdentifier(*$1);
                 }
;

select_list : selected_entry select_list_tail
            {
		CQL_globalParserState->currentRule = "select_list->selected_entry select_list_tail";
                sprintf(msg,"BISON::select_list->selected_entry select_list_tail\n");
		printf_(msg);
            }
;

select_list_tail : {;} /* empty */
                 | COMMA selected_entry select_list_tail
                   {
		       CQL_globalParserState->currentRule = "select_list_tail->COMMA selected_entry select_list_tail";
                       sprintf(msg,"BISON::select_list_tail->COMMA selected_entry select_list_tail\n");
		       printf_(msg);
                   }
;

search_condition : expr
                   {
			CQL_globalParserState->currentRule = "search_condition->expr";
                        sprintf(msg,"BISON::search_condition->expr\n");
			printf_(msg);
			CQL_globalParserState->statement->setPredicate(*$1);
                   }
;

optional_where : {}
               | WHERE search_condition
                 {
		     CQL_globalParserState->currentRule = "optional_where->WHERE search_condition";
                     sprintf(msg,"BISON::optional_where->WHERE search_condition\n");
		     printf_(msg);
		     CQL_globalParserState->statement->setHasWhereClause();
                 }
;

select_statement : SELECT select_list FROM from_criteria optional_where 
                   {
		       CQL_globalParserState->currentRule = "select_statement";
                       sprintf(msg,"select_statement\n\n");
		       printf_(msg);
				 CQL_Bison_Cleanup();
                   }
						 
;

%%

/*int yyerror(char * err){yyclearin; yyerrok;throw Exception(String(err));return 1;}*/
