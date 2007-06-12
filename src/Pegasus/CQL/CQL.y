//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

//#define CQL_DEBUG_GRAMMAR

#ifdef CQL_DEBUG_GRAMMAR
#define DEBUG_GRAMMAR 1
#else
#define DEBUG_GRAMMAR 0
#endif

int yylex();
static char msg[200];
void printf_(const char * msg)
{
    if(DEBUG_GRAMMAR == 1)
        printf("%s\n",msg);
}


#define CQL_DEBUG_GRAMMAR

#ifdef CQL_DEBUG_GRAMMAR
#define CQLTRACE(X){ \
   CQL_globalParserState->currentRule = X; \
   sprintf(msg,"BISON::%s", X ); \
   printf_(msg); \
}
#else
#define CQLTRACE(X)
#endif

#ifdef CQL_DEBUG_GRAMMAR
#define CQLTRACE2( X, Y ){ \
   CQL_globalParserState->currentRule = X; \
   sprintf(msg,msg,"BISON::%s = %s\n", X, Y ); \
   printf_(msg); \
}
#else
#define CQLTRACE2( X,Y )
#endif

#define CQLTRACEMSG(X, Y) { \
   sprintf(msg,"BISON::%s-> %s\n", X, Y); \
   printf_(msg); \
}

#define CQLTRACEDECIMAL(X, Y) { \
CQL_globalParserState->currentRule = X; \
sprintf(msg,"BISON::chain-> X : Y = %d\n",Y); \
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

PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* CQL_globalParserState;
Array<CQLPredicate> _arglist;


PEGASUS_NAMESPACE_END


void CQL_Bison_Cleanup(){
    for(Uint32 i = 0; i < _ptrs.size(); i++)
    {
      if(_ptrs[i]._ptr)
      {
        switch(_ptrs[i].type)
        {
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
%token <strValue> TOK_IDENTIFIER 
%token <strValue> TOK_STRING_LITERAL
%token <strValue> TOK_BINARY
%token <strValue> TOK_NEGATIVE_BINARY
%token <strValue> TOK_HEXADECIMAL
%token <strValue> TOK_NEGATIVE_HEXADECIMAL
%token <strValue> TOK_INTEGER 
%token <strValue> TOK_NEGATIVE_INTEGER
%token <strValue> TOK_REAL 
%token <strValue> TOK_NEGATIVE_REAL
%token <strValue> TOK_TRUE 
%token <strValue> TOK_FALSE 
%token <strValue> TOK_SCOPED_PROPERTY
%token <strValue> TOK_LPAR 
%token <strValue> TOK_RPAR 
%token <strValue> TOK_HASH
%token <strValue> TOK_DOT 
%token <strValue> TOK_LBRKT 
%token <strValue> TOK_RBRKT 
%token <strValue> TOK_UNDERSCORE
%token <strValue> TOK_COMMA 
%token <strValue> TOK_CONCAT 
%token <strValue> TOK_DBL_PIPE
%token <strValue> TOK_PLUS
%token <strValue> TOK_MINUS 
%token <strValue> TOK_TIMES 
%token <strValue> TOK_DIV 
%token <strValue> TOK_IS 
%token <strValue> TOK_NULL 
%token <_opType> TOK_EQ
%token <_opType> TOK_NE 
%token <_opType> TOK_GT 
%token <_opType> TOK_LT 
%token <_opType> TOK_GE 
%token <_opType> TOK_LE 
%token <_opType> TOK_ISA 
%token <_opType> TOK_LIKE 
%token <strValue> TOK_NOT TOK_AND TOK_OR 
%token <strValue> TOK_SCOPE 
%token <strValue> TOK_ANY TOK_EVERY TOK_IN TOK_SATISFIES 
%token <strValue> TOK_STAR 
%token <strValue> TOK_DOTDOT 
%token <strValue> TOK_SHARP TOK_DISTINCT 
%token <strValue> TOK_SELECT 
%token <strValue> TOK_FIRST 
%token <strValue> TOK_FROM 
%token <strValue> TOK_WHERE 
%token <strValue> TOK_ORDER 
%token <strValue> TOK_BY 
%token <strValue> TOK_ASC 
%token <strValue> TOK_DESC 
%token <strValue> TOK_AS 
%token <strValue> TOK_UNEXPECTED_CHAR


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
        The general pattern:  We construct small objects first (CQLIdentifiers,
        CQLValues etc) which get forwared to more complex rules where more
        complex objects are made.  Eventually we have constructed a top level
        CQLPredicate that gets added to the CQL_globalParserState select
        statement.

        Along the way we keep track of which rule we are processing so that
        any errors encountered are specific enough to actually do us some good.

        The CQLFactory object is used primarily to save space/coding efforts.
        It enables us to build complex CQL objects with one simple call,
        or query complex objects through one simple call.
        
    */

/* CQLIdentifier */
identifier  : TOK_IDENTIFIER 
    { 
        CQLTRACE("identifier");

        if(isUTF8Str(CQL_lval.strValue))
        {  
            $$ = new CQLIdentifier(String(CQL_lval.strValue));
                _ObjPtr._ptr = $$;
                _ObjPtr.type = Id;
                _ptrs.append(_ObjPtr);
        }
        else
        {
            sprintf(msg,"BISON::identifier-> BAD UTF\n");
            printf_(msg);
            CQLTRACEMSG("identifier", "BAD UTF");

            throw CQLSyntaxErrorException(
                MessageLoaderParms("CQL.CQL_y.BAD_UTF8",
                "Bad UTF8 encountered parsing rule $0"
                   " in position $1.",
                "identifier",
                CQL_globalParserState->currentTokenPos) );
        }
    }
;

class_name : identifier  
    {
        CQLTRACE2("class_name",(const char *)($1->getName().getString()
                                              .getCString()) );
        $$ = $1;
    }
;

class_path : class_name 
    { 
        CQLTRACE("class_path");
        $$ = $1;
    }
;
/**********************************
property_scope : class_path TOK_SCOPE
    { 
        sprintf(msg,"BISON::property_scope = %s\n",$1); 
        printf_(msg);
    }
***********************************/
;

/* CQLIdentifier */
scoped_property : TOK_SCOPED_PROPERTY
    {
        /*
        TOK_SCOPED_PROPERTY can be:
        - "A::prop"
        - "A::class.prop"
        - "A::class.prop#'OK'
        - "A::class.prop[4]"
        */
        CQLTRACE2("scoped_property",CQL_lval.strValue );
        
        if(isUTF8Str(CQL_lval.strValue))
        {
            String tmp(CQL_lval.strValue);
            $$ = new CQLIdentifier(tmp);
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Id;
            _ptrs.append(_ObjPtr);
        }
        else
        {
            sprintf(msg,"BISON::scoped_property-> BAD UTF\n");
            printf_(msg);
            throw CQLSyntaxErrorException(
               MessageLoaderParms("CQL.CQL_y.BAD_UTF8",
                  "Bad UTF8 encountered parsing rule $0 in position $1.",
                  "scoped_property",
                  CQL_globalParserState->currentTokenPos) );
        }
    }
;   

/* String */
literal_string : TOK_STRING_LITERAL 
    { 
        /*
        Make sure the literal is valid UTF8, then make a String
        */
        CQLTRACE2("literal_string",CQL_lval.strValue );
        
        if(isUTF8Str(CQL_lval.strValue))
        {
            $$ = new String(CQL_lval.strValue);
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Str;
            _ptrs.append(_ObjPtr);
        }
        else
        {
            sprintf(msg,"BISON::literal_string-> BAD UTF\n");
            printf_(msg);
            CQLTRACEMSG("literal_string", "BAD UTF");
            
            throw CQLSyntaxErrorException(
               MessageLoaderParms("CQL.CQL_y.BAD_UTF8",
                  "Bad UTF8 encountered parsing rule $0 in position $1.",
                  "literal_string",
                  CQL_globalParserState->currentTokenPos));
        }
    }
;

/* CQLValue */
binary_value : TOK_BINARY 
    { 
         CQLTRACE2("binary_value->TOK_BINARY", CQL_lval.strValue);
         
         $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary); 
         _ObjPtr._ptr = $$;
         _ObjPtr.type = Val;
         _ptrs.append(_ObjPtr);
    }
| TOK_NEGATIVE_BINARY 
    { 
         printf_(msg);
        
         CQLTRACE2("binary_value->TOK_NEGATIVE_BINARY", CQL_lval.strValue);
        
         $$ = new CQLValue(CQL_lval.strValue, CQLValue::Binary, false); 
         _ObjPtr._ptr = $$;
         _ObjPtr.type = Val;
         _ptrs.append(_ObjPtr);
    }
;

/* CQLValue */
hex_value : TOK_HEXADECIMAL 
    { 
        CQLTRACE2("hex_value->TOK_HEXADECIMAL", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
| TOK_NEGATIVE_HEXADECIMAL 
    { 
        CQLTRACE2("hex_value->TOK_NEGATIVE_HEXADECIMAL", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Hex, false);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
;

/* CQLValue */
decimal_value : TOK_INTEGER 
    { 
        CQLTRACE2("decimal_value->TOK_INTEGER", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal); 
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
  | TOK_NEGATIVE_INTEGER 
    { 
        CQLTRACE2("decimal_value->TOK_NEGATIVE_INTEGER", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Decimal, false);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
;

/* CQLValue */
real_value : TOK_REAL 
    { 
        CQLTRACE2("real_value->TOK_REAL", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
  | TOK_NEGATIVE_REAL 
    { 
        CQLTRACE2("real_value->TOK_NEGATIVE_REAL", CQL_lval.strValue);
        
        $$ = new CQLValue(CQL_lval.strValue, CQLValue::Real, false);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Val;
        _ptrs.append(_ObjPtr);
    }
;

/* CQLValue */
literal : literal_string 
        {
            $$ = new CQLValue(*$1);
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Val;
            _ptrs.append(_ObjPtr);
        }
    | decimal_value
        {
            CQLTRACE("literal->decimal_value");
        }
    | binary_value
        {
            CQLTRACE("literal->binary_value");
        }
    | hex_value
        {
            CQLTRACE("literal->hex_value");
        }
    | real_value
        {
            CQLTRACE("literal->real_value");
        }
    | TOK_TRUE
        {
            CQLTRACE("literal->TOK_TRUE");
        
            $$ = new CQLValue(Boolean(true));
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Val;
            _ptrs.append(_ObjPtr);
        }
    | TOK_FALSE
        {
             CQLTRACE("literal->TOK_FALSE");
            
             $$ = new CQLValue(Boolean(false));
             _ObjPtr._ptr = $$;
             _ObjPtr.type = Val;
             _ptrs.append(_ObjPtr);
        }
;

/* String */
array_index : expr
    {
        CQLTRACE("array_index->expr");
        
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
        CQLTRACE("array_index_list->array_index");
        
        $$ = $1;
    }
;

/* void* */
chain : literal
    {
        CQLTRACE("chain->literal");
        
        chain_state = CQLVALUE;
        $$ = _factory.makeObject($1,Predicate);  
    }

  | TOK_LPAR expr TOK_RPAR
    {
        CQLTRACE("chain-> ( expr )");

        chain_state = CQLPREDICATE;
        $$ = $2;
    }

  | identifier
    {
        CQLTRACE("chain->identifier");
        
        chain_state = CQLIDENTIFIER;
        $$ = _factory.makeObject($1,Predicate);
    }

  | identifier TOK_HASH literal_string
    {
        CQLTRACE("chain->identifier#literal_string");
        
        String tmp = $1->getName().getString();
        tmp.append("#").append(*$3);
        CQLIdentifier _id(tmp);
        $$ = _factory.makeObject(&_id,Predicate);
        chain_state = CQLIDENTIFIER;
    }

  | scoped_property
    {
         CQLTRACE("chain->scoped_property");
        
         chain_state = CQLIDENTIFIER;
         $$ = _factory.makeObject($1,Predicate);
    }

  | identifier TOK_LPAR arg_list TOK_RPAR
    {
         CQLTRACE("chain->identifier( arg_list )");
        
         chain_state = CQLFUNCTION;
         CQLFunction _func(*$1,_arglist);
         $$ = (CQLPredicate*)(_factory.makeObject(&_func,Predicate));
         _arglist.clear();
    }

  | chain TOK_DOT scoped_property
    {
        CQLTRACEDECIMAL("chain TOK_DOT scoped_property", chain_state);
        
        CQLIdentifier *_id;
        if(chain_state == CQLIDENTIFIER)
        {
            _id = ((CQLIdentifier*)(_factory.getObject(
                $1,Predicate,Identifier)));
            CQLChainedIdentifier _cid(*_id);
            _cid.append(*$3);
            $$ = _factory.makeObject(&_cid,Predicate);
        }
        else if(chain_state == CQLCHAINEDIDENTIFIER)
        {
            CQLChainedIdentifier *_cid;
            _cid = ((CQLChainedIdentifier*)(_factory.getObject(
                $1,Predicate,ChainedIdentifier)));
            _cid->append(*$3);
            _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
            $$ = $1;
        }
        else  /* error */
        {
            String _msg("chain-> chain TOK_DOT scoped_property :"
                " chain state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");

            throw CQLSyntaxErrorException(
                MessageLoaderParms("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER",
                    "Chain state not a CQLIdentifier or a"
                        " CQLChainedIdentifier while parsing rule $0 in"
                        " position $1.",
                    "chain.scoped_property",
                    CQL_globalParserState->currentTokenPos));
        }
    
    chain_state = CQLCHAINEDIDENTIFIER;
    }

  | chain TOK_DOT identifier
    {
        CQLTRACEDECIMAL("chain->chain.identifier", chain_state);
        
        if(chain_state == CQLIDENTIFIER)
        {
            CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(
            $1,Predicate,Identifier)));
            CQLChainedIdentifier _cid(*_id);
            _cid.append(*$3);
            $$ = _factory.makeObject(&_cid,Predicate);
        }
        else if(chain_state == CQLCHAINEDIDENTIFIER)
        {
            CQLChainedIdentifier *_cid = 
            ((CQLChainedIdentifier*)(_factory.getObject(
            $1,Predicate,ChainedIdentifier)));
            _cid->append(*$3);
            _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
            $$ = $1;
        }
        else  /* error */
        {
            String _msg("chain-> chain TOK_DOT identifier : chain state not"
                " CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");

            throw CQLSyntaxErrorException(
            MessageLoaderParms("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER",
                "Chain state not a CQLIdentifier or a CQLChainedId"
                    "Identifier while parsing rule $0 in position $1.",
                "chain.identifier",
                CQL_globalParserState->currentTokenPos));
        }
        chain_state = CQLCHAINEDIDENTIFIER;
    }

  | chain TOK_DOT identifier TOK_HASH literal_string
    {
        CQLTRACEDECIMAL("chain->chain.identifier#literal_string", chain_state);
        
        if(chain_state == CQLIDENTIFIER)
        {
            CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(
                $1,Predicate,Identifier))); 
            CQLChainedIdentifier _cid(*_id);
            String tmp($3->getName().getString());
            tmp.append("#").append(*$5);
            CQLIdentifier _id1(tmp);
            _cid.append(_id1);
            _factory.setObject(((CQLPredicate*)$1),&_cid,ChainedIdentifier);
            $$ = $1;
        }
        else if(chain_state == CQLCHAINEDIDENTIFIER)
        {
            CQLChainedIdentifier *_cid =  ((CQLChainedIdentifier*)(
                _factory.getObject($1,Predicate,ChainedIdentifier)));
            String tmp($3->getName().getString());
            tmp.append("#").append(*$5);
            CQLIdentifier _id1(tmp);
            _cid->append(_id1);
            _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
            $$ = $1;
        }
        else  /* error */
        {
            String _msg("chain->chain.identifier#literal_string : chain"
                " state not CQLIDENTIFIER or CQLCHAINEDIDENTIFIER");

            throw CQLSyntaxErrorException(
            MessageLoaderParms("CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER",
                "Chain state not a CQLIdentifier or a CQLChained"
                    "Identifier while parsing rule $0 in position $1.",
                "chain.identifier#literal_string",
                CQL_globalParserState->currentTokenPos) );
            }
        chain_state = CQLCHAINEDIDENTIFIER;
    }

  | chain TOK_LBRKT array_index_list TOK_RBRKT
    {
        CQLTRACEDECIMAL("chain->chain[ array_index_list ]", chain_state);
        
        if(chain_state == CQLIDENTIFIER)
        {
            CQLIdentifier *_id = ((CQLIdentifier*)
                (_factory.getObject($1,Predicate,Identifier)));
            String tmp = _id->getName().getString();
            tmp.append("[").append(*$3).append("]");
            CQLIdentifier _id1(tmp);
            CQLChainedIdentifier _cid(_id1);
            _factory.setObject(((CQLPredicate*)$1),&_cid,ChainedIdentifier);
            $$ = $1;    
        }
        else if(chain_state == CQLCHAINEDIDENTIFIER || chain_state == CQLVALUE)
        {
            CQLPredicate* _pred = (CQLPredicate*)$1;
            CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)
                (_factory.getObject
                ($1,Predicate,ChainedIdentifier)));
            CQLIdentifier tmpid = _cid->getLastIdentifier();
            String tmp = tmpid.getName().getString();
            tmp.append("[").append(*$3).append("]");
            CQLIdentifier _id1(tmp);
            CQLChainedIdentifier _tmpcid(_id1);

            if(_cid->size() == 1)
            {
                _cid = &_tmpcid;
            }
            else
            {
                _cid->append(_id1);
            }
            _factory.setObject(((CQLPredicate*)$1),_cid,ChainedIdentifier);
            $$ = $1;
        }
        else  /* error */
        {
            String _msg("chain->chain[ array_index_list ] : chain state not"
                " CQLIDENTIFIER or CQLCHAINEDIDENTIFIER or CQLVALUE");

            throw CQLSyntaxErrorException(
               MessageLoaderParms(
                  "CQL.CQL_y.NOT_CHAINID_OR_IDENTIFIER_OR_VALUE",
                  "Chain state not a CQLIdentifier or a "
                    "CQLChainedIdentifier or a CQLValue while parsing rule"
                        " $0 in position $1.",
                  "chain->chain[ array_index_list ]",
                  CQL_globalParserState->currentTokenPos) );
        }
    }
;

concat : chain
    {
        CQLTRACE("concat->chain");
        
        $$ = ((CQLPredicate*)$1);
    }

  | concat TOK_DBL_PIPE literal_string
    {
        CQLTRACE("concat->concat || literal_string");
        
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
                    CQLTerm* _term = ((CQLTerm*)
                        (_factory.getObject($1, Predicate, Term)));
                    // check for simple literal values
                    Array<CQLFactor> factors = _term->getFactors();
                    for(Uint32 i = 0; i < factors.size(); i++)
                    {
                        if(!factors[i].isSimpleValue())
                        {
                            MessageLoaderParms mparms(
                                "CQL.CQL_y.CONCAT_PRODUCTION_FACTORS_NOT"
                                "_SIMPLE",
                                "The CQLFactors are not simple while"
                                " processing rule $0.",
                                CQL_globalParserState->currentRule);
                            throw CQLSyntaxErrorException(mparms);
                        }
                        else /* !factors[i].isSimpleValue() */
                        {
                            CQLValue val = factors[i].getValue();
                            if(val.getValueType() != CQLValue::String_type)
                            {
                                MessageLoaderParms mparms(
                                 "CQL.CQL_y."
                                     "CONCAT_PRODUCTION_VALUE_NOT_LITERAL",
                                 "The CQLValue is not a string literal while"
                                     " processing rule $0.",
                                 CQL_globalParserState->currentRule);
                                throw CQLSyntaxErrorException(mparms);
                            }
                        }
                    }  // End For factors.size() Loop

                    CQLFactor* _fctr2 = ((CQLFactor*)
                        (_factory.makeObject(tmpval, Factor)));
                    _term->appendOperation(concat,*_fctr2);
                    $$ = (CQLPredicate*)(_factory.makeObject
                        (_term,Predicate)); 
                }
                else /* exp.isSimple() */
                {
                    MessageLoaderParms mparms(
                        "CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                        "The $0 is not simple while processing rule $1.",
                        "CQLExpression",
                        CQL_globalParserState->currentRule);
                    throw CQLSyntaxErrorException(mparms);
                }
            }
            else /*sp.isSimple() */
            {
                 MessageLoaderParms mparms(
                    "CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                    "The $0 is not simple while processing rule $1.",
                    "CQLSimplePredicate",
                    CQL_globalParserState->currentRule);
                 throw CQLSyntaxErrorException(mparms);
            }
        }
        else /* end if((CQLPredicate*)$1->isSimple()) */
        {
            MessageLoaderParms mparms(
                "CQL.CQL_y.CONCAT_PRODUCTION_NOT_SIMPLE",
                "The $0 is not simple while processing rule $1.",
                "CQLPredicate",
                CQL_globalParserState->currentRule);
            throw CQLSyntaxErrorException(mparms);
        }
    }
;

factor : concat
    {
        CQLTRACE("factor->concat");
        
        $$ = $1;
    }         
    /*
  | TOK_PLUS concat
    {
        add enum instead of _invert to CQLFactor,has to be either nothing,
        + or -
        get the factor and set the optype appropriately
        CQL_globalParserState->currentRule = "concat->TOK_PLUS concat"; 
        printf("BISON::factor->TOK_PLUS concat\n");
        ...
        $$ = new CQLFactor(*(CQLValue*)$2);
    }
  | TOK_MINUS concat
    {
        get the factor and set the optype appropriately 
        CQL_globalParserState->currentRule = "concat->TOK_MINUS concat";
        printf("BISON::factor->TOK_MINUS concat\n");
        ...
        CQLValue *tmp = (CQLValue*)$2;
        tmp->invert();
        $$ = new CQLFactor(*tmp);
    }
    */
;

term : factor
    {
        CQLTRACE("term->factor");
        
        $$ = $1;
    }
 /*
  | term TOK_STAR factor
    {
        get factor out of $1, get factor out of $3, appendoperation, 
        then construct predicate and forward it 
        printf("BISON::term->term TOK_STAR factor\n");
        CQL_globalParserState->currentRule = "term->term TOK_STAR factor";
        $1->appendOperation(mult, *$3);
        $$ = $1;
    }
  | term TOK_DIV factor
    {
        get factor out of $1, get factor out of $3, appendoperation, 
        then construct predicate and forward it 
        CQL_globalParserState->currentRule = "term->term TOK_DIV factor";
        printf("BISON::term->term TOK_DIV factor\n");
        $1->appendOperation(divide, *$3);
        $$ = $1;
    }
 */
;

arith : term
   {
      CQLTRACE("arith->term");
      
      //CQLPredicate* _pred = new CQLPredicate(*$1);
      //      _factory._predicates.append(_pred);
      $$ = $1;
   }
/***********************8
   | arith TOK_PLUS term
   {
         get term out of $1, get term out of $3, appendoperation, then 
         construct predicate and forward it
         CQL_globalParserState->currentRule = "arith->arith TOK_PLUS term";
         printf("BISON::arith->arith TOK_PLUS term\n");
         $1->appendOperation(TERM_ADD, *$3);
         $$ = $1;
    }
   | arith TOK_MINUS term
    {
         get term out of $1, get term out of $3, append operation, then 
         construct predicate and forward it 
         CQL_globalParserState->currentRule = "arith->arith TOK_MINUS term";
         printf("BISON::arith->arith TOK_MINUS term\n");
         $1->appendOperation(TERM_SUBTRACT, *$3);
         $$ = $1;
    }
************************/
;

value_symbol : TOK_HASH literal_string
    {
        CQLTRACE("value_symbol->#literal_string");
        
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
        CQLTRACE("arith_or_value_symbol->arith");
        
        $$ = $1;
    }
  | value_symbol
    {
        /* make into predicate */
        CQLTRACE("arith_or_value_symbol->value_symbol");
        
        CQLFactor _fctr(*$1);
        $$ = (CQLPredicate*)(_factory.makeObject(&_fctr, Predicate));
    }
;

comp_op : TOK_EQ 
    {
         CQLTRACE("comp_op->TOK_EQ");
        
         $$ = EQ;
    }
  | TOK_NE
    {
         CQLTRACE("comp_op->TOK_NE");
         $$ = NE;
    }
  | TOK_GT 
    {
         CQLTRACE("comp_op->TOK_GT");
        
         $$ = GT;
    }
  | TOK_LT
    {
         CQLTRACE("comp_op->TOK_LT");
         $$ = LT;
    }
  | TOK_GE
    {
         CQLTRACE("comp_op->TOK_GE");
         $$ = GE;
    }
  | TOK_LE
    {
         CQLTRACE("comp_op->TOK_LE");
         $$ = LE;
    }
;

comp : arith
    {
        CQLTRACE("comp->arith");
        
        $$ = $1;
    }
  | arith TOK_IS TOK_NOT TOK_NULL
    {
        CQLTRACE("comp->arith TOK_IS TOK_NOT TOK_NULL");
        
        CQLExpression *_expr =
         (CQLExpression*)(_factory.getObject($1,Expression));
        CQLSimplePredicate _sp(*_expr, IS_NOT_NULL);
        _factory.setObject($1,&_sp,SimplePredicate);
        $$ = $1;
    }
  | arith TOK_IS TOK_NULL
    {
        CQLTRACE("comp->arith TOK_IS TOK_NULL");
        
        CQLExpression *_expr =
             (CQLExpression*)(_factory.getObject($1,Expression));
        CQLSimplePredicate _sp(*_expr, IS_NULL);
        _factory.setObject($1,&_sp,SimplePredicate);
        $$ = $1;
    }
  | arith comp_op arith_or_value_symbol
    {
        CQLTRACE("comp->arith comp_op arith_or_value_symbol");
        
        if($1->isSimple() && $3->isSimple())
        {
            CQLExpression* _exp1 = (CQLExpression*)
            (_factory.getObject($1,Predicate,Expression));
            CQLExpression* _exp2 = (CQLExpression*)
            (_factory.getObject($3,Predicate,Expression));
            CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
            $$ = new CQLPredicate(_sp);
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
        }
        else  /* error */
        {
           String _msg("comp->arith comp_op arith_or_value_symbol : $1 is "
               "not simple OR $3 is not simple");
           throw CQLSyntaxErrorException(
               MessageLoaderParms("CQL.CQL_y.NOT_SIMPLE",
                   "The CQLSimplePredicate is not simple while parsing "
                   "rule $0 in position $1.",
                       "comp->arith comp_op arith_or_value_symbol",
                   CQL_globalParserState->currentTokenPos) );
        }
    }
  | value_symbol comp_op arith
    {
        CQLTRACE("comp->value_symbol comp_op arith");
        
        if($3->isSimple())
        {
            CQLExpression* _exp1 = (CQLExpression*)
                (_factory.makeObject($1, Expression));
            CQLExpression* _exp2 = (CQLExpression*)
                (_factory.getObject($3,Predicate,Expression));
            CQLSimplePredicate _sp(*_exp1, *_exp2, $2);
            $$ = new CQLPredicate(_sp);
            _ObjPtr._ptr = $$;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
        }
        else  /* error */
        {
            String _msg("comp->value_symbol comp_op arith : $3 is not simple");
            throw CQLSyntaxErrorException(
                MessageLoaderParms("CQL.CQL_y.NOT_SIMPLE",
                    "The CQLSimplePredicate is not simple while parsing"
                        " rule $0 in position $1.",
                    "comp->value_symbol comp_op arith",
                CQL_globalParserState->currentTokenPos) );
    
        }
    }
  | arith TOK_ISA identifier
    {
        /* make sure $1 isSimple(), get its expression, make 
           simplepred->predicate 
        */
        CQLTRACE("comp->arith _TOK_ISA identifier");
        
        CQLExpression *_expr1 = (CQLExpression*)
        (_factory.getObject($1,Predicate,Expression));
        CQLChainedIdentifier _cid(*$3);
        CQLExpression *_expr2 = (CQLExpression*)
        (_factory.makeObject(&_cid,Expression));
        CQLSimplePredicate _sp(*_expr1, *_expr2, ISA);
        _factory.setObject($1,&_sp,SimplePredicate);
        $$ = $1;
    }
  | arith TOK_LIKE literal_string
    {
         CQLTRACE("comp->arith TOK_LIKE literal_string");
        
         CQLExpression *_expr1 = (CQLExpression*)
            (_factory.getObject($1,Predicate,Expression));
         CQLValue _val(*$3);
         CQLExpression *_expr2 = (CQLExpression*)
            (_factory.makeObject(&_val,Expression));
         CQLSimplePredicate _sp(*_expr1, *_expr2, LIKE);
         _factory.setObject($1,&_sp,SimplePredicate);
         $$ = $1;
    }
;
expr_factor : comp
    {
        CQLTRACE("expr_factor->comp");
        
        $$ = $1;
    }
  | TOK_NOT comp
    {
        CQLTRACE("expr_factor->TOK_NOT comp");
        
        $2->setInverted(!($2->getInverted()));
        $$ = $2;
    }
;

expr_term : expr_factor
    {
        CQLTRACE("expr_term->expr_factor");
        
        $$ = $1;
    }
| expr_term TOK_AND expr_factor
    {
        CQLTRACE("expr_term->expr_term AND expr_factor");
        
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
        CQLTRACE("expr->expr_term");
        
        $$ = $1;     
        }
| expr TOK_OR expr_term
    {
        CQLTRACE("expr->expr OR expr_term");
        
        $$ = new CQLPredicate();
        $$->appendPredicate(*$1);
        $$->appendPredicate(*$3, OR);
        _ObjPtr._ptr = $$;
        _ObjPtr.type = Pred;
        _ptrs.append(_ObjPtr);
    }
;

arg_list : {;}
/****************
   | TOK_STAR
      {
         CQLTRACE("arg_list->TOK_STAR");
         
         CQLIdentifier _id("*");
         CQLPredicate* _pred = (CQLPredicate*)
            (_factory.makeObject(&_id,Predicate));
         _arglist.append(*_pred); 
                    since arg_list can loop back on itself, 
                    we need to store away previous solutions 
                    production.  We keep track of previous productions
                    in the _arglist array and later pass that to CQLFunction
                    as part of chain: identifier TOK_LPAR arg_list TOK_RPAR
                    
      }
*******************/
    | expr
    {
        CQLTRACE("arg_list->arg_list_sub->expr");
        
        _arglist.append(*$1);
        /*
        since arg_list can loop back on itself,
        we need to store away previous solutions
        production.  We keep track of previous productions
        in the _arglist array and later pass that to CQLFunction
        as part of chain: identifier TOK_LPAR arg_list TOK_RPAR
        */
    }
/****************************
   | TOK_DISTINCT TOK_STAR
      {
         CQLTRACE("arg_list->TOK_DISTINCT TOK_STAR");

         CQLIdentifier _id("DISTINCTSTAR");
         CQLPredicate* _pred = (CQLPredicate*)
            (_factory.makeObject(&_id,Predicate));
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
   | TOK_DISTINCT expr
      {
         sprintf(msg,"BISON::arg_list_sub->TOK_DISTINCT expr\n");
         printf_(msg);
    
         String tmp1("TOK_DISTINCT");
         CQLIdentifier* _id = (CQLIdentifier*)
            (_factory.getObject($1,Predicate,Identifier));
         String tmp2(_id->getName().getString());
         tmp1.append(tmp2);
         CQLIdentifier _id1(tmp1);
      }
;

arg_list_tail : {;}
   | TOK_COMMA arg_list_sub arg_list_tail
      {
         sprintf(msg,"BISON::arg_list_tail->TOK_COMMA arg_list_sub"
             " arg_list_tail\n");
         printf_(msg);
      }
;
*******************/

from_specifier : class_path
    {
        CQLTRACE("from_specifier->class_path");
        
        CQL_globalParserState->statement->appendClassPath(*$1);
    } 
  | class_path TOK_AS identifier
    {
        CQLTRACE("from_specifier->class_path TOK_AS identifier");
        
        CQLIdentifier _class(*$1);
        String _alias($3->getName().getString());
        CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
        CQL_globalParserState->statement->appendClassPath(_class);
    }
  | class_path identifier
    {
        CQLTRACE("from_specifier->class_path identifier");
        
        CQLIdentifier _class(*$1);
        String _alias($2->getName().getString());
        CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
        CQL_globalParserState->statement->appendClassPath(_class);
    }
;

from_criteria : from_specifier
    {
        CQLTRACE("from_criteria->from_specifier");
    }
;

star_expr : TOK_STAR
    {
        CQLTRACE("star_expr->TOK_STAR");
        
        CQLIdentifier _id("*");
        $$ = (CQLChainedIdentifier*)
             (_factory.makeObject(&_id,ChainedIdentifier));
    }
  | chain TOK_DOT TOK_STAR
    {
        CQLTRACE("star_expr->chain.*");
        
        CQLChainedIdentifier* _tmp = (CQLChainedIdentifier*)
        (_factory.getObject($1,Predicate,ChainedIdentifier));
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
        CQLTRACE("selected_entry->expr");
        
        if($1->isSimpleValue())
        {
            CQLChainedIdentifier *_cid = (CQLChainedIdentifier*)
                (_factory.getObject($1,Predicate,ChainedIdentifier));
            CQL_globalParserState->statement->appendSelectIdentifier(*_cid);
        }
        else  /* errot*/
        {
            String _msg("selected_entry->expr : $1 is not a simple value");
            throw CQLSyntaxErrorException(
               MessageLoaderParms("CQL.CQL_y.NOT_SIMPLE_VALUE",
                  "The CQLPredicate is not a simple value while "
                      "parsing rule $0 in position $1.",
                  "selected_entry->expr",
                  CQL_globalParserState->currentTokenPos) ); 
        }
    }
| star_expr
    {
        CQLTRACE("selected_entry->star_expr");
        
        CQL_globalParserState->statement->appendSelectIdentifier(*$1);
    }
;

select_list : selected_entry select_list_tail
    {
         CQLTRACE("select_list->selected_entry select_list_tail");
    }
;

select_list_tail : {;} /* empty */

  | TOK_COMMA selected_entry select_list_tail
    {
        CQLTRACE("select_list_tail->TOK_COMMA selected_entry"
                 " select_list_tail");
    }
;

search_condition : expr
    {
        CQLTRACE("search_condition->expr");
        
        CQL_globalParserState->statement->setPredicate(*$1);
    }
;

optional_where : {}

  | TOK_WHERE search_condition
    {
        CQLTRACE( "optional_where->TOK_WHERE search_condition");
        
        CQL_globalParserState->statement->setHasWhereClause();
    }
;

select_statement : TOK_SELECT select_list TOK_FROM from_criteria optional_where 
    {
        CQLTRACE("select_statement");
        
        CQL_Bison_Cleanup();
    }
                         
;

%%
/*int yyerror(char * err){yyclearin; yyerrok;throw Exception(String(err));
return 1;}*/
