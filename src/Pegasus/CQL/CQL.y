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
PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* globalParserState;
Array<CQLPredicate> _arglist;                                                                                
PEGASUS_NAMESPACE_END


%}
%union {
   char * strValue;
   int lineno;
   int tokenpos;
   char * linebuf;
   String * _string;
   CQLValue * _value;
   CQLSelectStatement * _ss;
   CQLIdentifier * _identifier;
   CQLChainedIdentifier * _chainedIdentifier;
   CQLTerm * _term;
   CQLFactor * _factor;
   CQLPredicate * _predicate;
   CQLSimplePredicate * _simplePredicate;
   ExpressionOpType _opType;
   CQLExpression * _expression;
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
		a top level CQLPredicate that gets added to the globalParserState select statement.

		Along the way we keep track of which rule we are processing so that any errors encountered are specific
		enough to actually do us some good.

		The CQLFactory object is used primarily to save space/coding efforts.  It enables us to build complex CQL
		objects with one simple call, or query complex objects through one simple call.
		
	*/

/* CQLIdentifier */
identifier  : IDENTIFIER 
             { 
		 globalParserState->currentRule = "identifier";
                 sprintf(msg,"BISON::identifier\n");
		 printf_(msg);
	
                 $$ = new CQLIdentifier(String(CQL_lval.strValue));
             }
;

class_name : identifier  
             {
		 globalParserState->currentRule = "class_name";
                 sprintf(msg,"BISON::class_name = %s\n", (const char *)($1->getName().getString().getCString())); 
		 printf_(msg);
		$$ = $1;
             }
;

class_path : class_name 
             { 
		globalParserState->currentRule = "class_path";
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
			globalParserState->currentRule = "scoped_property";
			sprintf(msg,"BISON::scoped_property = %s\n",CQL_lval.strValue);
			printf_(msg);

		        String tmp(CQL_lval.strValue);
		        $$ = new CQLIdentifier(tmp);
                  }
;   

/* String */
literal_string : STRING_LITERAL 
             { 
		/*
		   We make sure the literal is valid UTF8, then make a String
		*/
		globalParserState->currentRule = "literal_string";
                sprintf(msg,"BISON::literal_string-> %s\n",CQL_lval.strValue); 
		printf_(msg);

		if(isUTF8Str(CQL_lval.strValue)){
		     $$ = new String(CQL_lval.strValue);
		}else{
		    sprintf(msg,"BISON::literal_string-> BAD UTF\n");
		    printf_(msg);
		    throw CQLSyntaxErrorException(
					MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
							   String("Bad UTF8 encountered parsing rule $0 in position $1."),
							   String("literal_string"),
							   globalParserState->currentTokenPos)
						 );
		}
             }
;

/* CQLValue */
binary_value : BINARY 
	       { 
		   globalParserState->currentRule = "binary_value->BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary); 
               }
             | NEGATIVE_BINARY 
               { 
		   globalParserState->currentRule = "binary_value->NEGATIVE_BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary, false); 
               }
;

/* CQLValue */
hex_value : HEXADECIMAL 
            { 
		globalParserState->currentRule = "hex_value->HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex);
            }
          | NEGATIVE_HEXADECIMAL 
            { 
		globalParserState->currentRule = "hex_value->NEGATIVE_HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex, false);
            }
;

/* CQLValue */
decimal_value : INTEGER 
                { 
		    globalParserState->currentRule = "decimal_value->INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal); 
                }
              | NEGATIVE_INTEGER 
                { 
		    globalParserState->currentRule = "decimal_value->NEGATIVE_INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal, false);
                }
;

/* CQLValue */
real_value : REAL 
             { 
		 globalParserState->currentRule = "real_value->REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real);
             }
           | NEGATIVE_REAL 
             { 
		 globalParserState->currentRule = "real_value->NEGATIVE_REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real, false);
             }
;

/* CQLValue */
literal : literal_string 
          {
	      globalParserState->currentRule = "literal->literal_string";
              sprintf(msg,"BISON::literal->literal_string\n");
	      printf_(msg);
              $$ = new CQLValue(*$1);
	      delete $1;
          }
        | decimal_value
          {
	      globalParserState->currentRule = "literal->decimal_value";
              sprintf(msg,"BISON::literal->decimal_value\n");
	      printf_(msg);

          }
        | binary_value
          {
              globalParserState->currentRule = "literal->binary_value";
              sprintf(msg,"BISON::literal->binary_value\n");
	      printf_(msg);

          }
        | hex_value
          {
              globalParserState->currentRule = "literal->hex_value";
              sprintf(msg,"BISON::literal->hex_value\n");
	      printf_(msg);

          }
        | real_value
          {
              globalParserState->currentRule = "literal->real_value";
              sprintf(msg,"BISON::literal->real_value\n");
	      printf_(msg);

          }
        | _TRUE
          {
	      globalParserState->currentRule = "literal->_TRUE";
              sprintf(msg,"BISON::literal->_TRUE\n");
	      printf_(msg);

              $$ = new CQLValue(Boolean(true));
          }
        | _FALSE
          {
	      globalParserState->currentRule = "literal->_FALSE";
              sprintf(msg,"BISON::literal->_FALSE\n");
	      printf_(msg);

              $$ = new CQLValue(Boolean(false));
          }
;

/* String */
array_index : expr
              {
		  globalParserState->currentRule = "array_index->expr";
                  sprintf(msg,"BISON::array_index->expr\n");
		  printf_(msg);

		  //CQLValue tmp = _factory.getValue((CQLPredicate*)$1);
		  //$$ = new String(tmp.toString());
		  CQLValue* _val = (CQLValue*)_factory.getObject($1,Predicate,Value);
		  $$ = new String(_val->toString());
              }
;

/* String */
array_index_list : array_index
                   {
		       globalParserState->currentRule = "array_index_list->array_index";
                       sprintf(msg,"BISON::array_index_list->array_index\n");
		       printf_(msg);
 		       $$ = $1;
                   }
;

/* void* */
chain : literal
        {
            globalParserState->currentRule = "chain->literal";
            sprintf(msg,"BISON::chain->literal\n");
	    printf_(msg);

            chain_state = CQLVALUE;
	    $$ = _factory.makeObject($1,Predicate);  
	    delete $1;
        }
      | LPAR expr RPAR
        {
	    globalParserState->currentRule = "chain-> ( expr )";
            sprintf(msg,"BISON::chain-> ( expr )\n");
	    printf_(msg);

            chain_state = CQLPREDICATE;
	    $$ = $2;
        }
      | identifier
        {
	   globalParserState->currentRule = "chain->identifier";
           sprintf(msg,"BISON::chain->identifier\n");
	   printf_(msg);

           chain_state = CQLIDENTIFIER;
	   $$ = _factory.makeObject($1,Predicate);
	   delete $1;
        }
      | identifier HASH literal_string
        {
	    globalParserState->currentRule = "chain->identifier#literal_string";
            sprintf(msg,"BISON::chain->identifier#literal_string\n");
	    printf_(msg);

            String tmp = $1->getName().getString();
            tmp.append("#").append(*$3);
            CQLIdentifier _id(tmp);
   	    $$ = _factory.makeObject(&_id,Predicate);
	    chain_state = CQLIDENTIFIER;
	    delete $1; delete $3;
        }
      | scoped_property
        {
	    globalParserState->currentRule = "chain->scoped_property";
	    sprintf(msg,"BISON::chain-> scoped_property\n");
	    printf_(msg);

            chain_state = CQLIDENTIFIER;
	    $$ = _factory.makeObject($1,Predicate);
	    delete $1;
        }
      | identifier LPAR arg_list RPAR
        {
	    globalParserState->currentRule = "chain->identifier( arg_list )";
            sprintf(msg,"BISON::chain-> identifier( arg_list )\n");
	    printf_(msg);
            chain_state = CQLFUNCTION;
	    CQLFunction _func(*$1,_arglist);
	    $$ = (CQLPredicate*)(_factory.makeObject(&_func,Predicate));
	    _arglist.clear();
	    delete $1; 
        }
      | chain DOT scoped_property
        {
	    globalParserState->currentRule = "chain->chain.scoped_property";
	    sprintf(msg,"BISON::chain-> chain DOT scoped_property : chain_state = %d\n",chain_state);
	    printf_(msg);

	    CQLIdentifier *_id;
	    if(chain_state == CQLIDENTIFIER){
	        _id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*$3);
		$$ = _factory.makeObject(&_cid,Predicate);
		delete $3;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid;
		_cid = ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
		_cid->append(*$3);
		_factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
		$$ = $1;
		delete $3;
	    }else{
		/* error */
		String _msg("chain-> chain DOT scoped_property : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.scoped_property"),
                                                           globalParserState->currentTokenPos)
                                                 );
            }

            chain_state = CQLCHAINEDIDENTIFIER;
        }
      | chain DOT identifier
        {
	    globalParserState->currentRule = "chain->chain.identifier";
            sprintf(msg,"BISON::chain->chain.identifier : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject($1,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*$3);
                $$ = _factory.makeObject(&_cid,Predicate);
		delete $3;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
                _cid->append(*$3);
                _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
                $$ = $1;
		delete $3;
            }else{
                /* error */
		String _msg("chain-> chain DOT identifier : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.identifier"),
                                                           globalParserState->currentTokenPos)
                                                 );
            }
	    chain_state = CQLCHAINEDIDENTIFIER;

        }
      | chain DOT identifier HASH literal_string
        {
	    globalParserState->currentRule = "chain->chain.identifier#literal_string";
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
                delete $3; delete $5;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
              CQLChainedIdentifier *_cid =  ((CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier)));
              String tmp($3->getName().getString());
                tmp.append("#").append(*$5);
                CQLIdentifier _id1(tmp);
                _cid->append(_id1);
                _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
                $$ = $1;
                delete $3; delete $5;
            }else{
                /* error */
		String _msg("chain->chain.identifier#literal_string : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier while parsing rule $0 in position $1."),
							   String("chain.identifier#literal_string"),
                                                           globalParserState->currentTokenPos)
                                                 );
            }
                                                                                                        
            chain_state = CQLCHAINEDIDENTIFIER;

        }
      | chain LBRKT array_index_list RBRKT
        {
	    globalParserState->currentRule = "chain->chain[ array_index_list ]";
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
		delete $3;	
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
		delete $3;
	    }else{
		/* error */
		String _msg("chain->chain[ array_index_list ] : chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER or CQLVALUE");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER_OR_VALUE"),
                                                           String("Chain state not a CQLIdentifier or a CQLChainedIdentifier or a CQLValue while parsing rule $0 in position $1."),
							   String("chain->chain[ array_index_list ]"),
                                                           globalParserState->currentTokenPos)
                                                 );
	    }
        }
;

concat : chain
         {
	     globalParserState->currentRule = "concat->chain";
             sprintf(msg,"BISON::concat->chain\n");
	     printf_(msg);

	     $$ = ((CQLPredicate*)$1);
         }
       | concat DBL_PIPE chain
         {
	     globalParserState->currentRule = "concat->concat || chain";
             sprintf(msg,"BISON::concat||chain\n");
	     printf_(msg);

	     if($1->isSimpleValue() && ((CQLPredicate*)$3)->isSimpleValue()){
		CQLFactor* _fctr1 = ((CQLFactor*)(_factory.getObject($1, Predicate, Factor)));
		CQLFactor* _fctr2 = ((CQLFactor*)(_factory.getObject($3, Predicate, Factor)));	
		CQLTerm _term1(*_fctr1);
		_term1.appendOperation(concat,*_fctr2);
		$$ = (CQLPredicate*)(_factory.makeObject(&_term1,Predicate));
		delete $1; 
		CQLPredicate* _pred = (CQLPredicate*)$3;
		delete _pred;
	     }
         }
;

factor : concat
	 {
	     globalParserState->currentRule = "factor->concat";
             sprintf(msg,"BISON::factor->concat\n");
	     printf_(msg);

	     $$ = $1;
         }         
      /* | PLUS concat
         {
	      add enum instead of _invert to CQLFactor,has to be either nothing, + or -
              get the factor and set the optype appropriately
	     globalParserState->currentRule = "concat->PLUS concat"; 
             printf("BISON::factor->PLUS concat\n");
             $$ = new CQLFactor(*(CQLValue*)$2);
         }
       | MINUS concat
         {
              get the factor and set the optype appropriately 
	     globalParserState->currentRule = "concat->MINUS concat";
             printf("BISON::factor->MINUS concat\n");
             CQLValue *tmp = (CQLValue*)$2;
             tmp->invert();
	     $$ = new CQLFactor(*tmp);
         }*/
;

term : factor
       {
	   globalParserState->currentRule = "term->factor";
           sprintf(msg,"BISON::term->factor\n");
	   printf_(msg);

           $$ = $1;
       }
    /* | term STAR factor
       {
	    get factor out of $1, get factor out of $3, appendoperation, then construct predicate and forward it 
           printf("BISON::term->term STAR factor\n");
	   globalParserState->currentRule = "term->term STAR factor";
           $1->appendOperation(mult, *$3);
           $$ = $1;
       }
     | term DIV factor
       {
            get factor out of $1, get factor out of $3, appendoperation, then construct predicate and forward it 
	   globalParserState->currentRule = "term->term DIV factor";
           printf("BISON::term->term DIV factor\n");
           $1->appendOperation(divide, *$3);
           $$ = $1;
       }*/
;

arith : term
        {
	    globalParserState->currentRule = "arith->term";
            sprintf(msg,"BISON::arith->term\n");
	    printf_(msg);

	    //CQLPredicate* _pred = new CQLPredicate(*$1);
//	    _factory._predicates.append(_pred);
            $$ = $1;
        }
     /* | arith PLUS term
        {
	     get term out of $1, get term out of $3, appendoperation, then construct predicate and forward it
	    globalParserState->currentRule = "arith->arith PLUS term";
            printf("BISON::arith->arith PLUS term\n");
            $1->appendOperation(TERM_ADD, *$3);
            $$ = $1;
        }
      | arith MINUS term
        {
	    get term out of $1, get term out of $3, appendoperation, then construct predicate and forward it 
	    globalParserState->currentRule = "arith->arith MINUS term";
            printf("BISON::arith->arith MINUS term\n");
	    $1->appendOperation(TERM_SUBTRACT, *$3);
            $$ = $1;
        }*/
;

value_symbol : HASH literal_string
               {
	  	   globalParserState->currentRule = "value_symbol->#literal_string";
                   sprintf(msg,"BISON::value_symbol->#literal_string\n");
                   printf_(msg);

		   String tmp("#");
		   tmp.append(*$2);
		   CQLIdentifier tmpid(tmp);
		   $$ = new CQLValue(tmpid);
		   delete $2;
               }
;

arith_or_value_symbol : arith
                        {
			    globalParserState->currentRule = "arith_or_value_symbol->arith";
                            sprintf(msg,"BISON::arith_or_value_symbol->arith\n");
			    printf_(msg);

			    $$ = $1;
                        }
                      | value_symbol
                        {
			    /* make into predicate */
			    globalParserState->currentRule = "arith_or_value_symbol->value_symbol";
                            sprintf(msg,"BISON::arith_or_value_symbol->value_symbol\n");
			    printf_(msg);

			    CQLFactor _fctr(*$1);
			    $$ = (CQLPredicate*)(_factory.makeObject(&_fctr, Predicate));
			    delete $1;
                        }
;

comp_op : _EQ 
          {
	      globalParserState->currentRule = "comp_op->_EQ";
              sprintf(msg,"BISON::comp_op->_EQ\n");
	      printf_(msg);
	      $$ = EQ;
          }
        | _NE
          {
	      globalParserState->currentRule = "comp_op->_NE";
              sprintf(msg,"BISON::comp_op->_NE\n");
	      printf_(msg);
	      $$ = NE;
          }
        | _GT 
          {
	      globalParserState->currentRule = "comp_op->_GT";
              sprintf(msg,"BISON::comp_op->_GT\n");
	      printf_(msg);
	      $$ = GT;
          }
        | _LT
          {
 	      globalParserState->currentRule = "comp_op->_LT";
              sprintf(msg,"BISON::comp_op->_LT\n");
	      printf_(msg);
	      $$ = LT;
          }
        | _GE
          {
	      globalParserState->currentRule = "comp_op->_GE";
              sprintf(msg,"BISON::comp_op->_GE\n");
	      printf_(msg);
	      $$ = GE;
          }
        | _LE
          {
	      globalParserState->currentRule = "comp_op->_LE";
              sprintf(msg,"BISON::comp_op->_LE\n");
	      printf_(msg);
	      $$ = LE;
          }
;

comp : arith
       {
	   globalParserState->currentRule = "comp->arith";
           sprintf(msg,"BISON::comp->arith\n");
	   printf_(msg);

	   $$ = $1;
       }
     | arith IS NOT _NULL
       {
	   globalParserState->currentRule = "comp->arith IS NOT _NULL";
           sprintf(msg,"BISON::comp->arith IS NOT _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject($1,Expression));
	   CQLSimplePredicate _sp(*_expr, IS_NOT_NULL);
           _factory.setObject($1,&_sp,SimplePredicate);
	   $$ = $1;
       }
     | arith IS _NULL
       {
	   globalParserState->currentRule = "comp->arith IS _NULL";
           sprintf(msg,"BISON::comp->arith IS _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject($1,Expression));
           CQLSimplePredicate _sp(*_expr, IS_NULL);
           _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
       }
     | arith comp_op arith_or_value_symbol
       {
	   globalParserState->currentRule = "comp->arith comp_op arith_or_value_symbol";
           sprintf(msg,"BISON::comp->arith comp_op arith_or_value_symbol\n");
	   printf_(msg);
	   if($1->isSimple() && $3->isSimple()){
		CQLExpression* _exp1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
		CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject($3,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
           	$$ = new CQLPredicate(_sp);
	   }else{
		/* error */
		String _msg("comp->arith comp_op arith_or_value_symbol : $1 is not simple OR $3 is not simple");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE"),
                                                           String("The CQLSimplePredicate is not simple while parsing rule $0 in position $1."),
							   String("comp->arith comp_op arith_or_value_symbol"),
                                                           globalParserState->currentTokenPos)
                                                 );
	   }
       }
     | value_symbol comp_op arith
       {
	   globalParserState->currentRule = "comp->value_symbol comp_op arith";
           sprintf(msg,"BISON::comp->value_symbol comp_op arith\n");
	   printf_(msg);

	   if($3->isSimple()){
           	CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject($1, Expression));
	        CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject($3,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
           	$$ = new CQLPredicate(_sp);
	   }else{
		/* error */
		String _msg("comp->value_symbol comp_op arith : $3 is not simple");
		throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE"),
                                                           String("The CQLSimplePredicate is not simple while parsing rule $0 in position $1."),
                                                           String("comp->value_symbol comp_op arith"),
                                                           globalParserState->currentTokenPos)
                                                 );

	   }
       }
     | value_symbol comp_op value_symbol
       {
		globalParserState->currentRule = "comp->value_symbol comp_op value_symbol";
		sprintf(msg,"BISON::comp->value_symbol comp_op value_symbol\n");
           	printf_(msg);
                                                                                                                                                             
                CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject($1, Expression));
                CQLExpression* _exp2 = (CQLExpression*)(_factory.makeObject($3,Expression));
                CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
                $$ = new CQLPredicate(_sp);
       }
     | arith _ISA identifier
       {
	   globalParserState->currentRule = "comp->arith _ISA identifier";
	   /* make sure $1 isSimple(), get its expression, make simplepred->predicate */
           sprintf(msg,"BISON::comp->arith _ISA identifier\n");
	   printf_(msg);

	   CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
	   CQLChainedIdentifier _cid(*$3);
	   CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_cid,Expression));
           CQLSimplePredicate _sp(*_expr1, *_expr2, ISA);
	   _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
	   delete $3;
       }
     | arith _LIKE literal_string
       {
	   globalParserState->currentRule = "comp->arith _LIKE literal_string";
           sprintf(msg,"BISON::comp->arith _LIKE literal_string\n");
	   printf_(msg);

           CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject($1,Predicate,Expression));
	   CQLValue _val(*$3);
           CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_val,Expression));
	   CQLSimplePredicate _sp(*_expr1, *_expr2, LIKE);
           _factory.setObject($1,&_sp,SimplePredicate);
           $$ = $1;
	   delete $3;
       }
;
expr_factor : comp
              {
		  globalParserState->currentRule = "expr_factor->comp";
                  sprintf(msg,"BISON::expr_factor->comp\n");
	          printf_(msg);

		  $$ = $1;
              }
            | NOT comp
              {
		  globalParserState->currentRule = "expr_factor->NOT comp";
                  sprintf(msg,"BISON::expr_factor->NOT comp\n");
	 	  printf_(msg);

		  $2->setInverted();
		  $$ = $2;
              }
;

expr_term : expr_factor
            {
	        globalParserState->currentRule = "expr_term->expr_factor";
                sprintf(msg,"BISON::expr_term->expr_factor\n");
		printf_(msg);

		$$ = $1;
            }
          | expr_term _AND expr_factor
            {
		globalParserState->currentRule = "expr_term->expr_term AND expr_factor";
		sprintf(msg,"BISON::expr_term->expr_term AND expr_factor\n");
		printf_(msg);

		$$ = new CQLPredicate();
           	$$->appendPredicate(*$1);
           	$$->appendPredicate(*$3, AND);	
            }
;

expr : expr_term 
       {
	  globalParserState->currentRule = "expr->expr_term";
          sprintf(msg,"BISON::expr->expr_term\n");
	  printf_(msg);

	  $$ = $1; 	   
       }
     | expr _OR expr_term
       {
	   globalParserState->currentRule = "expr->expr OR expr_term";
           sprintf(msg,"BISON::expr->expr OR expr_term\n");
	   printf_(msg);
	   $$ = new CQLPredicate();
	   $$->appendPredicate(*$1);
	   $$->appendPredicate(*$3, OR);
       }
;

arg_list : {;}
         | STAR
           {
	       globalParserState->currentRule = "arg_list->STAR";
               sprintf(msg,"BISON::arg_list->STAR\n");
	       printf_(msg);

	       CQLIdentifier _id("*");
	       CQLPredicate* _pred = (CQLPredicate*)(_factory.makeObject(&_id,Predicate));
	       _arglist.append(*_pred); /* 
					   since arg_list can loop back on itself, 
					   we need to store away previous solutions 
					   production.  We keep track of previous productions
					   in the _arglist array and later pass that to CQLFunction
					   as part of chain: identifier LPAR arg_list RPAR
					*/
           }
         | expr
	   {
		   globalParserState->currentRule = "arg_list->arg_list_sub->expr";
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
		     globalParserState->currentRule = "from_specifier->class_path";
                     sprintf(msg,"BISON::from_specifier->class_path\n");
		     printf_(msg);

		     globalParserState->statement->appendClassPath(*$1);
		     delete $1;
                 } 

		| class_path AS identifier
		  {
			globalParserState->currentRule = "from_specifier->class_path AS identifier";
			sprintf(msg,"BISON::from_specifier->class_path AS identifier\n");
			printf_(msg);

			CQLIdentifier _class(*$1);
			String _alias($3->getName().getString());
			globalParserState->statement->insertClassPathAlias(_class,_alias);
			globalParserState->statement->appendClassPath(_class);
			delete $1; delete $3;
		  }
		| class_path identifier
		  {
			globalParserState->currentRule = "from_specifier->class_path identifier";
			sprintf(msg,"BISON::from_specifier->class_path identifier\n");
			printf_(msg);

			CQLIdentifier _class(*$1);
                        String _alias($2->getName().getString());
                        globalParserState->statement->insertClassPathAlias(_class,_alias);
                        globalParserState->statement->appendClassPath(_class);
			delete $1; delete $2;
		  }
;

from_criteria : from_specifier
                {
		    globalParserState->currentRule = "from_criteria->from_specifier";
                    sprintf(msg,"BISON::from_criteria->from_specifier\n");
		    printf_(msg);
                }
;

star_expr : STAR
            {
		globalParserState->currentRule = "star_expr->STAR";
                sprintf(msg,"BISON::star_expr->STAR\n");
		printf_(msg);

		CQLIdentifier _id("*");
		$$ = (CQLChainedIdentifier*)(_factory.makeObject(&_id,ChainedIdentifier));
            }
	  | chain DOT STAR
	    {
		globalParserState->currentRule = "star_expr->chain.*";
		sprintf(msg,"BISON::star_expr->chain.*\n");
                printf_(msg);
		CQLChainedIdentifier* _tmp = (CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier));
		CQLChainedIdentifier* _cid = new CQLChainedIdentifier(*_tmp);
                CQLIdentifier _id("*");
		_cid->append(_id);
                $$ = _cid;
	    }
;

selected_entry : expr 
                 {
		     globalParserState->currentRule = "selected_entry->expr";
                     sprintf(msg,"BISON::selected_entry->expr\n");
		     printf_(msg);
		     if($1->isSimpleValue()){
		        CQLChainedIdentifier *_cid = (CQLChainedIdentifier*)(_factory.getObject($1,Predicate,ChainedIdentifier));
		        globalParserState->statement->appendSelectIdentifier(*_cid);
		     }else{
			/* error */
			String _msg("selected_entry->expr : $1 is not a simple value");
		 	throw CQLSyntaxErrorException(
                                        MessageLoaderParms(String("CQL.CQL_y.NOT_SIMPLE_VALUE"),
                                                           String("The CQLPredicate is not a simple value while parsing rule $0 in position $1."),
                                                           String("selected_entry->expr"),
                                                           globalParserState->currentTokenPos)
                                                 );	
		     }
                 }
               | star_expr
                 {
		     globalParserState->currentRule = "selected_entry->star_expr";
                     sprintf(msg,"BISON::selected_entry->star_expr\n");
		     printf_(msg);
		     globalParserState->statement->appendSelectIdentifier(*$1);
                 }
;

select_list : selected_entry select_list_tail
            {
		globalParserState->currentRule = "select_list->selected_entry select_list_tail";
                sprintf(msg,"BISON::select_list->selected_entry select_list_tail\n");
		printf_(msg);
            }
;

select_list_tail : {;} /* empty */
                 | COMMA selected_entry select_list_tail
                   {
		       globalParserState->currentRule = "select_list_tail->COMMA selected_entry select_list_tail";
                       sprintf(msg,"BISON::select_list_tail->COMMA selected_entry select_list_tail\n");
		       printf_(msg);
                   }
;

search_condition : expr
                   {
			globalParserState->currentRule = "search_condition->expr";
                        sprintf(msg,"BISON::search_condition->expr\n");
			printf_(msg);
			globalParserState->statement->setPredicate(*$1);
                   }
;

optional_where : {;}
               | WHERE search_condition
                 {
		     globalParserState->currentRule = "optional_where->WHERE search_condition";
                     sprintf(msg,"BISON::optional_where->WHERE search_condition\n");
		     printf_(msg);
		     globalParserState->statement->setHasWhereClause();
                 }
;

select_statement : SELECT select_list FROM from_criteria optional_where 
                   {
		       globalParserState->currentRule = "select_statement";
                       sprintf(msg,"select_statement\n\n");
		       printf_(msg);
                   }
;

%%

/*int yyerror(char * err){yyclearin; yyerrok;throw Exception(String(err));return 1;}*/
