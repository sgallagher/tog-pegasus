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
#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define yyparse CQL_parse
#define yylex CQL_lex
#define yyerror CQL_error
#define yychar CQL_char
#define yyval CQL_val
#define yylval CQL_lval
#define yydebug CQL_debug
#define yynerrs CQL_nerrs
#define yyerrflag CQL_errflag
#define yyss CQL_ss
#define yyssp CQL_ssp
#define yyvs CQL_vs
#define yyvsp CQL_vsp
#define yylhs CQL_lhs
#define yylen CQL_len
#define yydefred CQL_defred
#define yydgoto CQL_dgoto
#define yysindex CQL_sindex
#define yyrindex CQL_rindex
#define yygindex CQL_gindex
#define yytable CQL_table
#define yycheck CQL_check
#define yyname CQL_name
#define yyrule CQL_rule
#define yysslim CQL_sslim
#define yystacksize CQL_stacksize
#define YYPREFIX "CQL_"
#line 2 "CQL.y"
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


#line 97 "CQL.y"
typedef union {
   char * strValue;
   String * _string;
   CQLValue * _value;
   CQLIdentifier * _identifier;
   CQLChainedIdentifier * _chainedIdentifier;
   CQLPredicate * _predicate;
   ExpressionOpType _opType;
   void * _node;
} YYSTYPE;
#line 149 "y.tab.c"
#define YYERRCODE 256
#define IDENTIFIER 257
#define STRING_LITERAL 258
#define BINARY 259
#define NEGATIVE_BINARY 260
#define HEXADECIMAL 261
#define NEGATIVE_HEXADECIMAL 262
#define INTEGER 263
#define NEGATIVE_INTEGER 264
#define REAL 265
#define NEGATIVE_REAL 266
#define _TRUE 267
#define _FALSE 268
#define SCOPED_PROPERTY 269
#define LPAR 270
#define RPAR 271
#define HASH 272
#define DOT 273
#define LBRKT 274
#define RBRKT 275
#define UNDERSCORE 276
#define COMMA 277
#define CONCAT 278
#define DBL_PIPE 279
#define PLUS 280
#define MINUS 281
#define TIMES 282
#define DIV 283
#define IS 284
#define _NULL 285
#define _EQ 286
#define _NE 287
#define _GT 288
#define _LT 289
#define _GE 290
#define _LE 291
#define _ISA 292
#define _LIKE 293
#define NOT 294
#define _AND 295
#define _OR 296
#define SCOPE 297
#define ANY 298
#define EVERY 299
#define IN 300
#define SATISFIES 301
#define STAR 302
#define DOTDOT 303
#define SHARP 304
#define DISTINCT 305
#define SELECT 306
#define FIRST 307
#define FROM 308
#define WHERE 309
#define ORDER 310
#define BY 311
#define ASC 312
#define DESC 313
#define AS 314
#define UNEXPECTED_CHAR 315
const short CQL_lhs[] = {                                        -1,
    1,    2,    3,    4,    5,    6,    6,    7,    7,    8,
    8,    9,    9,   10,   10,   10,   10,   10,   10,   10,
   11,   12,   13,   13,   13,   13,   13,   13,   13,   13,
   13,   13,   14,   14,   15,   16,   17,   18,   19,   19,
   20,   20,   20,   20,   20,   20,   21,   21,   21,   21,
   21,   21,   21,   22,   22,   23,   23,   24,   24,   25,
   25,   26,   26,   26,   27,   28,   28,   29,   29,   30,
   31,   31,   32,   33,   33,    0,
};
const short CQL_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    3,    1,    3,    1,    4,    3,    3,
    5,    4,    1,    3,    1,    1,    1,    2,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    4,    3,    3,
    3,    3,    3,    1,    2,    1,    3,    1,    3,    0,
    1,    1,    3,    2,    1,    1,    3,    1,    1,    2,
    0,    3,    1,    0,    2,    5,
};
const short CQL_defred[] = {                                      0,
    0,    0,    1,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   19,   20,    4,    0,    0,    0,   66,    0,
   27,   14,   16,   17,   15,   18,   23,    0,    0,   36,
   37,    0,    0,   54,   56,    0,    0,   69,    0,    0,
    0,    0,   38,   55,    0,    0,    0,    0,    0,    0,
   41,   42,   43,   44,   45,   46,    0,    0,    0,    0,
    0,    0,    0,   70,    0,    0,   24,    0,    0,   26,
   67,    0,   29,   22,    0,    0,   34,   49,    0,   52,
   53,   39,   40,   50,   51,   57,    0,    0,    2,    3,
    0,   65,    0,   28,    0,   32,   48,   72,    0,   64,
    0,   76,   31,   63,    0,   75,
};
const short CQL_dgoto[] = {                                       2,
   20,   90,   91,   21,   22,   23,   24,   25,   26,   27,
   74,   75,   41,   29,   30,   31,   32,   33,   84,   59,
   34,   35,   36,   37,   69,   92,   93,   38,   39,   40,
   64,  106,  102,
};
const short CQL_sindex[] = {                                   -295,
 -152,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -136, -241,  -98,    0, -256,
    0,    0,    0,    0,    0,    0,    0, -255, -257,    0,
    0, -196, -247,    0,    0, -265, -264,    0, -240, -260,
 -227, -261,    0,    0, -136, -241, -249, -136, -241, -273,
    0,    0,    0,    0,    0,    0, -207, -241,  -98,  -82,
 -136, -136, -152,    0, -207, -244,    0, -264, -220,    0,
    0, -218,    0,    0, -216, -264,    0,    0, -230,    0,
    0,    0,    0,    0,    0,    0, -265, -240,    0,    0,
 -251,    0, -253,    0, -241,    0,    0,    0, -207,    0,
 -136,    0,    0,    0, -264,    0,
};
const short CQL_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0, -219,   75,    0,
    0,   78,    0,    0,    0,  101, -270,    0, -242,    0,
   52,    0,    0,    0, -210,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -192,    0,    0,
    0,   29,    0,    0,    0, -211,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  104, -242,    0,    0,
    1,    0,   80,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   81,    0,
};
const short CQL_gindex[] = {                                      0,
  -42,    0,    0,  -38,  -13,    0,    0,    0,    0,    0,
    0,    0,   -1,    0,    0,    0,  -33,   24,    0,   51,
   67,   25,   36,  -14,    0,    0,    0,    0,   37,    0,
   11,    0,    0,
};
#define YYTABLESIZE 412
const short CQL_table[] = {                                      28,
   62,   42,   25,   43,   72,    3,   68,    3,   73,   67,
    1,   78,    3,   45,   80,   46,    4,   47,   48,   15,
   79,   49,   89,   72,   15,   82,   85,   73,   30,   61,
   68,   62,   70,   76,   62,   77,   63,   68,   51,   52,
   53,   54,   55,   56,   81,   66,   48,   65,  100,    3,
   94,   33,   71,   95,   97,  101,  104,   33,   96,   33,
   60,   28,   99,   21,   33,   71,   33,   33,   33,   33,
   33,   33,   33,   33,   35,   33,   33,   47,   61,   74,
   73,  103,   83,   60,   44,   86,  105,   50,   33,   51,
   52,   53,   54,   55,   56,   57,   58,   87,   98,   88,
   58,    0,    0,   59,    3,    4,    5,    6,    7,    8,
    9,   10,   11,   12,   13,   14,   15,   16,    0,   17,
    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
   13,   14,   15,   16,    0,   17,    0,    0,    0,    0,
    0,   18,    0,    0,    0,    0,    0,    0,    0,   19,
    0,    0,    0,    0,    0,    0,    0,   18,    3,    4,
    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
   15,   16,    0,   17,    3,    4,    5,    6,    7,    8,
    9,   10,   11,   12,   13,   14,   15,   16,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   25,    0,   25,   25,   25,    0,   25,
    0,   25,    0,    0,    0,    0,   25,    0,   25,   25,
   25,   25,   25,   25,   25,   25,    0,   25,   25,   30,
    0,   30,   30,   30,    0,   30,    0,   30,    0,   62,
   25,    0,   30,    0,   30,   30,   30,   30,   30,   30,
   30,   30,   33,   30,   30,    0,   33,    0,   33,    0,
   33,    0,    0,    0,    0,   33,   30,   33,   33,   33,
   33,   33,   33,   33,   33,   35,   33,   33,   47,   35,
    0,   35,   47,    0,   47,    0,    0,    0,   35,   33,
   35,   35,   35,   35,   35,   35,   35,   35,    0,   35,
   35,   58,   47,   47,   59,   58,    0,   58,   59,    0,
   59,    0,   35,    0,    0,   47,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   58,    0,    0,   59,
    0,    0,    0,    0,    0,    0,    0,    0,   58,    0,
    0,   59,
};
const short CQL_check[] = {                                       1,
    0,   16,    0,   17,   47,  257,  277,  257,   47,  271,
  306,  285,  257,  270,   57,  272,  258,  273,  274,  269,
  294,  279,   65,   66,  269,   59,   60,   66,    0,  295,
   45,  296,   46,   48,  296,   49,  277,  308,  286,  287,
  288,  289,  290,  291,   58,  273,  274,  308,   91,  257,
  271,    0,  302,  272,  285,  309,   99,  277,  275,  279,
  271,   63,  314,  275,  284,  308,  286,  287,  288,  289,
  290,  291,  292,  293,    0,  295,  296,    0,  271,    0,
    0,   95,   59,   33,   18,   61,  101,  284,  308,  286,
  287,  288,  289,  290,  291,  292,  293,   62,   88,   63,
    0,   -1,   -1,    0,  257,  258,  259,  260,  261,  262,
  263,  264,  265,  266,  267,  268,  269,  270,   -1,  272,
  257,  258,  259,  260,  261,  262,  263,  264,  265,  266,
  267,  268,  269,  270,   -1,  272,   -1,   -1,   -1,   -1,
   -1,  294,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  302,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  294,  257,  258,
  259,  260,  261,  262,  263,  264,  265,  266,  267,  268,
  269,  270,   -1,  272,  257,  258,  259,  260,  261,  262,
  263,  264,  265,  266,  267,  268,  269,  270,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  271,   -1,  273,  274,  275,   -1,  277,
   -1,  279,   -1,   -1,   -1,   -1,  284,   -1,  286,  287,
  288,  289,  290,  291,  292,  293,   -1,  295,  296,  271,
   -1,  273,  274,  275,   -1,  277,   -1,  279,   -1,  309,
  308,   -1,  284,   -1,  286,  287,  288,  289,  290,  291,
  292,  293,  271,  295,  296,   -1,  275,   -1,  277,   -1,
  279,   -1,   -1,   -1,   -1,  284,  308,  286,  287,  288,
  289,  290,  291,  292,  293,  271,  295,  296,  271,  275,
   -1,  277,  275,   -1,  277,   -1,   -1,   -1,  284,  308,
  286,  287,  288,  289,  290,  291,  292,  293,   -1,  295,
  296,  271,  295,  296,  271,  275,   -1,  277,  275,   -1,
  277,   -1,  308,   -1,   -1,  308,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  296,   -1,   -1,  296,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  308,   -1,
   -1,  308,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 315
#if YYDEBUG
const char * const CQL_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"IDENTIFIER","STRING_LITERAL",
"BINARY","NEGATIVE_BINARY","HEXADECIMAL","NEGATIVE_HEXADECIMAL","INTEGER",
"NEGATIVE_INTEGER","REAL","NEGATIVE_REAL","_TRUE","_FALSE","SCOPED_PROPERTY",
"LPAR","RPAR","HASH","DOT","LBRKT","RBRKT","UNDERSCORE","COMMA","CONCAT",
"DBL_PIPE","PLUS","MINUS","TIMES","DIV","IS","_NULL","_EQ","_NE","_GT","_LT",
"_GE","_LE","_ISA","_LIKE","NOT","_AND","_OR","SCOPE","ANY","EVERY","IN",
"SATISFIES","STAR","DOTDOT","SHARP","DISTINCT","SELECT","FIRST","FROM","WHERE",
"ORDER","BY","ASC","DESC","AS","UNEXPECTED_CHAR",
};
const char * const CQL_rule[] = {
"$accept : select_statement",
"identifier : IDENTIFIER",
"class_name : identifier",
"class_path : class_name",
"scoped_property : SCOPED_PROPERTY",
"literal_string : STRING_LITERAL",
"binary_value : BINARY",
"binary_value : NEGATIVE_BINARY",
"hex_value : HEXADECIMAL",
"hex_value : NEGATIVE_HEXADECIMAL",
"decimal_value : INTEGER",
"decimal_value : NEGATIVE_INTEGER",
"real_value : REAL",
"real_value : NEGATIVE_REAL",
"literal : literal_string",
"literal : decimal_value",
"literal : binary_value",
"literal : hex_value",
"literal : real_value",
"literal : _TRUE",
"literal : _FALSE",
"array_index : expr",
"array_index_list : array_index",
"chain : literal",
"chain : LPAR expr RPAR",
"chain : identifier",
"chain : identifier HASH literal_string",
"chain : scoped_property",
"chain : identifier LPAR arg_list RPAR",
"chain : chain DOT scoped_property",
"chain : chain DOT identifier",
"chain : chain DOT identifier HASH literal_string",
"chain : chain LBRKT array_index_list RBRKT",
"concat : chain",
"concat : concat DBL_PIPE literal_string",
"factor : concat",
"term : factor",
"arith : term",
"value_symbol : HASH literal_string",
"arith_or_value_symbol : arith",
"arith_or_value_symbol : value_symbol",
"comp_op : _EQ",
"comp_op : _NE",
"comp_op : _GT",
"comp_op : _LT",
"comp_op : _GE",
"comp_op : _LE",
"comp : arith",
"comp : arith IS NOT _NULL",
"comp : arith IS _NULL",
"comp : arith comp_op arith_or_value_symbol",
"comp : value_symbol comp_op arith",
"comp : arith _ISA identifier",
"comp : arith _LIKE literal_string",
"expr_factor : comp",
"expr_factor : NOT comp",
"expr_term : expr_factor",
"expr_term : expr_term _AND expr_factor",
"expr : expr_term",
"expr : expr _OR expr_term",
"arg_list :",
"arg_list : expr",
"from_specifier : class_path",
"from_specifier : class_path AS identifier",
"from_specifier : class_path identifier",
"from_criteria : from_specifier",
"star_expr : STAR",
"star_expr : chain DOT STAR",
"selected_entry : expr",
"selected_entry : star_expr",
"select_list : selected_entry select_list_tail",
"select_list_tail :",
"select_list_tail : COMMA selected_entry select_list_tail",
"search_condition : expr",
"optional_where :",
"optional_where : WHERE search_condition",
"select_statement : SELECT select_list FROM from_criteria optional_where",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;
#line 1344 "CQL.y"

/*int yyerror(char * err){yyclearin; yyerrok;throw Exception(String(err));return 1;}*/
#line 502 "y.tab.c"
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 219 "CQL.y"
{ 
		 CQL_globalParserState->currentRule = "identifier";
                 sprintf(msg,"BISON::identifier\n");
		 printf_(msg);
                 if(isUTF8Str(CQL_lval.strValue)){	
                    yyval._identifier = new CQLIdentifier(String(CQL_lval.strValue));
					     _ObjPtr._ptr = yyval._identifier;
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
break;
case 2:
#line 242 "CQL.y"
{
		 CQL_globalParserState->currentRule = "class_name";
                 sprintf(msg,"BISON::class_name = %s\n", (const char *)(yyvsp[0]._identifier->getName().getString().getCString())); 
		 printf_(msg);
		yyval._identifier = yyvsp[0]._identifier;
             }
break;
case 3:
#line 251 "CQL.y"
{ 
		CQL_globalParserState->currentRule = "class_path";
                 sprintf(msg,"BISON::class_path\n"); 
		 printf_(msg);
		 yyval._identifier = yyvsp[0]._identifier;
             }
break;
case 4:
#line 270 "CQL.y"
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
		        			  yyval._identifier = new CQLIdentifier(tmp);
				  			  _ObjPtr._ptr = yyval._identifier;
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
break;
case 5:
#line 303 "CQL.y"
{ 
		/*
		   We make sure the literal is valid UTF8, then make a String
		*/
		CQL_globalParserState->currentRule = "literal_string";
                sprintf(msg,"BISON::literal_string-> %s\n",CQL_lval.strValue); 
		printf_(msg);

		if(isUTF8Str(CQL_lval.strValue)){
		     yyval._string = new String(CQL_lval.strValue);
			  _ObjPtr._ptr = yyval._string;
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
break;
case 6:
#line 331 "CQL.y"
{ 
		   CQL_globalParserState->currentRule = "binary_value->BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Binary); 
						 _ObjPtr._ptr = yyval._value;
                   _ObjPtr.type = Val;
                   _ptrs.append(_ObjPtr);
               }
break;
case 7:
#line 342 "CQL.y"
{ 
		   CQL_globalParserState->currentRule = "binary_value->NEGATIVE_BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Binary, false); 
						 _ObjPtr._ptr = yyval._value;
                   _ObjPtr.type = Val;
                   _ptrs.append(_ObjPtr);
               }
break;
case 8:
#line 356 "CQL.y"
{ 
		CQL_globalParserState->currentRule = "hex_value->HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Hex);
					 _ObjPtr._ptr = yyval._value;
                _ObjPtr.type = Val;
                _ptrs.append(_ObjPtr);
            }
break;
case 9:
#line 367 "CQL.y"
{ 
		CQL_globalParserState->currentRule = "hex_value->NEGATIVE_HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Hex, false);
				    _ObjPtr._ptr = yyval._value;
                _ObjPtr.type = Val;
                _ptrs.append(_ObjPtr);
            }
break;
case 10:
#line 381 "CQL.y"
{ 
		    CQL_globalParserState->currentRule = "decimal_value->INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Decimal); 
						  _ObjPtr._ptr = yyval._value;
                    _ObjPtr.type = Val;
                    _ptrs.append(_ObjPtr);
                }
break;
case 11:
#line 392 "CQL.y"
{ 
		    CQL_globalParserState->currentRule = "decimal_value->NEGATIVE_INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Decimal, false);
						  _ObjPtr._ptr = yyval._value;
                    _ObjPtr.type = Val;
                    _ptrs.append(_ObjPtr);
                }
break;
case 12:
#line 406 "CQL.y"
{ 
		 CQL_globalParserState->currentRule = "real_value->REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Real);
					  _ObjPtr._ptr = yyval._value;
                 _ObjPtr.type = Val;
                 _ptrs.append(_ObjPtr);
             }
break;
case 13:
#line 416 "CQL.y"
{ 
		 CQL_globalParserState->currentRule = "real_value->NEGATIVE_REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Real, false);
					  _ObjPtr._ptr = yyval._value;
                 _ObjPtr.type = Val;
                 _ptrs.append(_ObjPtr);
             }
break;
case 14:
#line 429 "CQL.y"
{
	      CQL_globalParserState->currentRule = "literal->literal_string";
              sprintf(msg,"BISON::literal->literal_string\n");
	      printf_(msg);
              yyval._value = new CQLValue(*yyvsp[0]._string);
				  _ObjPtr._ptr = yyval._value;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
break;
case 15:
#line 439 "CQL.y"
{
	      CQL_globalParserState->currentRule = "literal->decimal_value";
              sprintf(msg,"BISON::literal->decimal_value\n");
	      printf_(msg);

          }
break;
case 16:
#line 446 "CQL.y"
{
              CQL_globalParserState->currentRule = "literal->binary_value";
              sprintf(msg,"BISON::literal->binary_value\n");
	      printf_(msg);

          }
break;
case 17:
#line 453 "CQL.y"
{
              CQL_globalParserState->currentRule = "literal->hex_value";
              sprintf(msg,"BISON::literal->hex_value\n");
	      printf_(msg);

          }
break;
case 18:
#line 460 "CQL.y"
{
              CQL_globalParserState->currentRule = "literal->real_value";
              sprintf(msg,"BISON::literal->real_value\n");
	      printf_(msg);

          }
break;
case 19:
#line 467 "CQL.y"
{
	      CQL_globalParserState->currentRule = "literal->_TRUE";
              sprintf(msg,"BISON::literal->_TRUE\n");
	      printf_(msg);

              yyval._value = new CQLValue(Boolean(true));
				  _ObjPtr._ptr = yyval._value;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
break;
case 20:
#line 478 "CQL.y"
{
	      CQL_globalParserState->currentRule = "literal->_FALSE";
              sprintf(msg,"BISON::literal->_FALSE\n");
	      printf_(msg);

              yyval._value = new CQLValue(Boolean(false));
				  _ObjPtr._ptr = yyval._value;
              _ObjPtr.type = Val;
              _ptrs.append(_ObjPtr);
          }
break;
case 21:
#line 492 "CQL.y"
{
		  CQL_globalParserState->currentRule = "array_index->expr";
                  sprintf(msg,"BISON::array_index->expr\n");
		  printf_(msg);

		  CQLValue* _val = (CQLValue*)_factory.getObject(yyvsp[0]._predicate,Predicate,Value);
		  yyval._string = new String(_val->toString());
		  _ObjPtr._ptr = yyval._string;
        _ObjPtr.type = Str;
        _ptrs.append(_ObjPtr);
              }
break;
case 22:
#line 507 "CQL.y"
{
		       CQL_globalParserState->currentRule = "array_index_list->array_index";
                       sprintf(msg,"BISON::array_index_list->array_index\n");
		       printf_(msg);
 		       yyval._string = yyvsp[0]._string;
                   }
break;
case 23:
#line 517 "CQL.y"
{
            CQL_globalParserState->currentRule = "chain->literal";
            sprintf(msg,"BISON::chain->literal\n");
	    printf_(msg);

            chain_state = CQLVALUE;
	    yyval._node = _factory.makeObject(yyvsp[0]._value,Predicate);  
        }
break;
case 24:
#line 526 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain-> ( expr )";
            sprintf(msg,"BISON::chain-> ( expr )\n");
	    printf_(msg);

            chain_state = CQLPREDICATE;
	    yyval._node = yyvsp[-1]._predicate;
        }
break;
case 25:
#line 535 "CQL.y"
{
	   CQL_globalParserState->currentRule = "chain->identifier";
           sprintf(msg,"BISON::chain->identifier\n");
	   printf_(msg);

           chain_state = CQLIDENTIFIER;
	   yyval._node = _factory.makeObject(yyvsp[0]._identifier,Predicate);
        }
break;
case 26:
#line 544 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->identifier#literal_string";
            sprintf(msg,"BISON::chain->identifier#literal_string\n");
	    printf_(msg);

            String tmp = yyvsp[-2]._identifier->getName().getString();
            tmp.append("#").append(*yyvsp[0]._string);
            CQLIdentifier _id(tmp);
   	    yyval._node = _factory.makeObject(&_id,Predicate);
	    chain_state = CQLIDENTIFIER;
        }
break;
case 27:
#line 556 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->scoped_property";
	    sprintf(msg,"BISON::chain-> scoped_property\n");
	    printf_(msg);

            chain_state = CQLIDENTIFIER;
	    yyval._node = _factory.makeObject(yyvsp[0]._identifier,Predicate);
        }
break;
case 28:
#line 565 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->identifier( arg_list )";
            sprintf(msg,"BISON::chain-> identifier( arg_list )\n");
	    printf_(msg);
            chain_state = CQLFUNCTION;
	    CQLFunction _func(*yyvsp[-3]._identifier,_arglist);
	    yyval._node = (CQLPredicate*)(_factory.makeObject(&_func,Predicate));
	    _arglist.clear();
        }
break;
case 29:
#line 575 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->chain.scoped_property";
	    sprintf(msg,"BISON::chain-> chain DOT scoped_property : chain_state = %d\n",chain_state);
	    printf_(msg);

	    CQLIdentifier *_id;
	    if(chain_state == CQLIDENTIFIER){
	        _id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*yyvsp[0]._identifier);
		yyval._node = _factory.makeObject(&_cid,Predicate);
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid;
		_cid = ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier)));
		_cid->append(*yyvsp[0]._identifier);
		_factory.setObject(((CQLPredicate*)yyvsp[-2]._node),_cid,ChainedIdentifier);
		yyval._node = yyvsp[-2]._node;
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
break;
case 30:
#line 606 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->chain.identifier";
            sprintf(msg,"BISON::chain->chain.identifier : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*yyvsp[0]._identifier);
                yyval._node = _factory.makeObject(&_cid,Predicate);
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier)));
                _cid->append(*yyvsp[0]._identifier);
                _factory.setObject(((CQLPredicate*)yyvsp[-2]._node),_cid,ChainedIdentifier);
                yyval._node = yyvsp[-2]._node;
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
break;
case 31:
#line 635 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->chain.identifier#literal_string";
            sprintf(msg,"BISON::chain->chain.identifier#literal_string : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
              CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-4]._node,Predicate,Identifier)));	
              CQLChainedIdentifier _cid(*_id);
                String tmp(yyvsp[-2]._identifier->getName().getString());
                tmp.append("#").append(*yyvsp[0]._string);
                CQLIdentifier _id1(tmp);
                _cid.append(_id1);
                _factory.setObject(((CQLPredicate*)yyvsp[-4]._node),&_cid,ChainedIdentifier);
                yyval._node = yyvsp[-4]._node;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
              CQLChainedIdentifier *_cid =  ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-4]._node,Predicate,ChainedIdentifier)));
              String tmp(yyvsp[-2]._identifier->getName().getString());
                tmp.append("#").append(*yyvsp[0]._string);
                CQLIdentifier _id1(tmp);
                _cid->append(_id1);
                _factory.setObject(((CQLPredicate*)yyvsp[-4]._node),_cid,ChainedIdentifier);
                yyval._node = yyvsp[-4]._node;
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
break;
case 32:
#line 672 "CQL.y"
{
	    CQL_globalParserState->currentRule = "chain->chain[ array_index_list ]";
            sprintf(msg,"BISON::chain->chain[ array_index_list ] : chain_state = %d\n",chain_state);
	    printf_(msg);
	
            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-3]._node,Predicate,Identifier)));
		String tmp = _id->getName().getString();
		tmp.append("[").append(*yyvsp[-1]._string).append("]");
		CQLIdentifier _id1(tmp);
		CQLChainedIdentifier _cid(_id1);
		_factory.setObject(((CQLPredicate*)yyvsp[-3]._node),&_cid,ChainedIdentifier);
                yyval._node = yyvsp[-3]._node;	
	    }else if(chain_state == CQLCHAINEDIDENTIFIER || chain_state == CQLVALUE){
		CQLPredicate* _pred = (CQLPredicate*)yyvsp[-3]._node;
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-3]._node,Predicate,ChainedIdentifier)));
		CQLIdentifier tmpid = _cid->getLastIdentifier();
		String tmp = tmpid.getName().getString();
                tmp.append("[").append(*yyvsp[-1]._string).append("]");
		CQLIdentifier _id1(tmp);
		CQLChainedIdentifier _tmpcid(_id1);
		if(_cid->size() == 1){
			_cid = &_tmpcid;
		}else{
			_cid->append(_id1);
		}
		_factory.setObject(((CQLPredicate*)yyvsp[-3]._node),_cid,ChainedIdentifier);
                yyval._node = yyvsp[-3]._node;
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
break;
case 33:
#line 714 "CQL.y"
{
	     CQL_globalParserState->currentRule = "concat->chain";
             sprintf(msg,"BISON::concat->chain\n");
	     printf_(msg);

	     yyval._predicate = ((CQLPredicate*)yyvsp[0]._node);
         }
break;
case 34:
#line 722 "CQL.y"
{
	         CQL_globalParserState->currentRule = "concat->concat || literal_string";
            sprintf(msg,"BISON::concat||literal_string\n");
	         printf_(msg);

		      CQLValue* tmpval = new CQLValue(*yyvsp[0]._string);
	         _ObjPtr._ptr = tmpval;
            _ObjPtr.type = Val;
            _ptrs.append(_ObjPtr);  

			   if((CQLPredicate*)yyvsp[-2]._predicate->isSimple())
            {
               CQLSimplePredicate sp = ((CQLPredicate*)yyvsp[-2]._predicate)->getSimplePredicate();
					if(sp.isSimple())
					{
                  CQLExpression exp = sp.getLeftExpression();
						if(exp.isSimple())
						{
                    CQLTerm* _term = ((CQLTerm*)(_factory.getObject(yyvsp[-2]._predicate, Predicate, Term)));
                    /* check for simple literal values*/
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
                    yyval._predicate = (CQLPredicate*)(_factory.makeObject(_term,Predicate)); 
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
break;
case 35:
#line 793 "CQL.y"
{
	     CQL_globalParserState->currentRule = "factor->concat";
             sprintf(msg,"BISON::factor->concat\n");
	     printf_(msg);

	     yyval._predicate = yyvsp[0]._predicate;
         }
break;
case 36:
#line 820 "CQL.y"
{
	   CQL_globalParserState->currentRule = "term->factor";
           sprintf(msg,"BISON::term->factor\n");
	   printf_(msg);

           yyval._predicate = yyvsp[0]._predicate;
       }
break;
case 37:
#line 846 "CQL.y"
{
	    CQL_globalParserState->currentRule = "arith->term";
            sprintf(msg,"BISON::arith->term\n");
	    printf_(msg);

	    /*CQLPredicate* _pred = new CQLPredicate(*$1);*/
/*	    _factory._predicates.append(_pred);*/
            yyval._predicate = yyvsp[0]._predicate;
        }
break;
case 38:
#line 874 "CQL.y"
{
	  	   CQL_globalParserState->currentRule = "value_symbol->#literal_string";
                   sprintf(msg,"BISON::value_symbol->#literal_string\n");
                   printf_(msg);

		   String tmp("#");
		   tmp.append(*yyvsp[0]._string);
		   CQLIdentifier tmpid(tmp);
		   yyval._value = new CQLValue(tmpid);
			_ObjPtr._ptr = yyval._value;
         _ObjPtr.type = Val;
         _ptrs.append(_ObjPtr);
               }
break;
case 39:
#line 890 "CQL.y"
{
			    CQL_globalParserState->currentRule = "arith_or_value_symbol->arith";
                            sprintf(msg,"BISON::arith_or_value_symbol->arith\n");
			    printf_(msg);

			    yyval._predicate = yyvsp[0]._predicate;
                        }
break;
case 40:
#line 898 "CQL.y"
{
			    /* make into predicate */
			    CQL_globalParserState->currentRule = "arith_or_value_symbol->value_symbol";
                            sprintf(msg,"BISON::arith_or_value_symbol->value_symbol\n");
			    printf_(msg);

			    CQLFactor _fctr(*yyvsp[0]._value);
			    yyval._predicate = (CQLPredicate*)(_factory.makeObject(&_fctr, Predicate));
                        }
break;
case 41:
#line 910 "CQL.y"
{
	      CQL_globalParserState->currentRule = "comp_op->_EQ";
              sprintf(msg,"BISON::comp_op->_EQ\n");
	      printf_(msg);
	      yyval._opType = EQ;
          }
break;
case 42:
#line 917 "CQL.y"
{
	      CQL_globalParserState->currentRule = "comp_op->_NE";
              sprintf(msg,"BISON::comp_op->_NE\n");
	      printf_(msg);
	      yyval._opType = NE;
          }
break;
case 43:
#line 924 "CQL.y"
{
	      CQL_globalParserState->currentRule = "comp_op->_GT";
              sprintf(msg,"BISON::comp_op->_GT\n");
	      printf_(msg);
	      yyval._opType = GT;
          }
break;
case 44:
#line 931 "CQL.y"
{
 	      CQL_globalParserState->currentRule = "comp_op->_LT";
              sprintf(msg,"BISON::comp_op->_LT\n");
	      printf_(msg);
	      yyval._opType = LT;
          }
break;
case 45:
#line 938 "CQL.y"
{
	      CQL_globalParserState->currentRule = "comp_op->_GE";
              sprintf(msg,"BISON::comp_op->_GE\n");
	      printf_(msg);
	      yyval._opType = GE;
          }
break;
case 46:
#line 945 "CQL.y"
{
	      CQL_globalParserState->currentRule = "comp_op->_LE";
              sprintf(msg,"BISON::comp_op->_LE\n");
	      printf_(msg);
	      yyval._opType = LE;
          }
break;
case 47:
#line 954 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith";
           sprintf(msg,"BISON::comp->arith\n");
	   printf_(msg);

	   yyval._predicate = yyvsp[0]._predicate;
       }
break;
case 48:
#line 962 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith IS NOT _NULL";
           sprintf(msg,"BISON::comp->arith IS NOT _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject(yyvsp[-3]._predicate,Expression));
	   CQLSimplePredicate _sp(*_expr, IS_NOT_NULL);
           _factory.setObject(yyvsp[-3]._predicate,&_sp,SimplePredicate);
	   yyval._predicate = yyvsp[-3]._predicate;
       }
break;
case 49:
#line 973 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith IS _NULL";
           sprintf(msg,"BISON::comp->arith IS _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Expression));
           CQLSimplePredicate _sp(*_expr, IS_NULL);
           _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
       }
break;
case 50:
#line 984 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith comp_op arith_or_value_symbol";
           sprintf(msg,"BISON::comp->arith comp_op arith_or_value_symbol\n");
	   printf_(msg);
	   if(yyvsp[-2]._predicate->isSimple() && yyvsp[0]._predicate->isSimple()){
		CQLExpression* _exp1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
		CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject(yyvsp[0]._predicate,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, yyvsp[-1]._opType);
           	yyval._predicate = new CQLPredicate(_sp);
				_ObjPtr._ptr = yyval._predicate;
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
break;
case 51:
#line 1008 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->value_symbol comp_op arith";
           sprintf(msg,"BISON::comp->value_symbol comp_op arith\n");
	   printf_(msg);

	   if(yyvsp[0]._predicate->isSimple()){
           	CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject(yyvsp[-2]._value, Expression));
	        CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject(yyvsp[0]._predicate,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, yyvsp[-1]._opType);
           	yyval._predicate = new CQLPredicate(_sp);
				_ObjPtr._ptr = yyval._predicate;
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
break;
case 52:
#line 1034 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith _ISA identifier";
	   /* make sure $1 isSimple(), get its expression, make simplepred->predicate */
           sprintf(msg,"BISON::comp->arith _ISA identifier\n");
	   printf_(msg);

	   CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
	   CQLChainedIdentifier _cid(*yyvsp[0]._identifier);
	   CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_cid,Expression));
           CQLSimplePredicate _sp(*_expr1, *_expr2, ISA);
	   _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
       }
break;
case 53:
#line 1048 "CQL.y"
{
	   CQL_globalParserState->currentRule = "comp->arith _LIKE literal_string";
           sprintf(msg,"BISON::comp->arith _LIKE literal_string\n");
	   printf_(msg);

           CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
	   CQLValue _val(*yyvsp[0]._string);
           CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_val,Expression));
	   CQLSimplePredicate _sp(*_expr1, *_expr2, LIKE);
           _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
       }
break;
case 54:
#line 1062 "CQL.y"
{
		  CQL_globalParserState->currentRule = "expr_factor->comp";
                  sprintf(msg,"BISON::expr_factor->comp\n");
	          printf_(msg);

		  yyval._predicate = yyvsp[0]._predicate;
              }
break;
case 55:
#line 1070 "CQL.y"
{
		           CQL_globalParserState->currentRule = "expr_factor->NOT comp";
                 sprintf(msg,"BISON::expr_factor->NOT comp\n");
	 	           printf_(msg);
        
		           yyvsp[0]._predicate->setInverted(!(yyvsp[0]._predicate->getInverted()));
		           yyval._predicate = yyvsp[0]._predicate;
              }
break;
case 56:
#line 1081 "CQL.y"
{
	        CQL_globalParserState->currentRule = "expr_term->expr_factor";
                sprintf(msg,"BISON::expr_term->expr_factor\n");
		printf_(msg);

		yyval._predicate = yyvsp[0]._predicate;
            }
break;
case 57:
#line 1089 "CQL.y"
{
		CQL_globalParserState->currentRule = "expr_term->expr_term AND expr_factor";
		sprintf(msg,"BISON::expr_term->expr_term AND expr_factor\n");
		printf_(msg);

		yyval._predicate = new CQLPredicate();
           	yyval._predicate->appendPredicate(*yyvsp[-2]._predicate);
           	yyval._predicate->appendPredicate(*yyvsp[0]._predicate, AND);	
				_ObjPtr._ptr = yyval._predicate;
            _ObjPtr.type = Pred;
            _ptrs.append(_ObjPtr);
            }
break;
case 58:
#line 1104 "CQL.y"
{
	  CQL_globalParserState->currentRule = "expr->expr_term";
          sprintf(msg,"BISON::expr->expr_term\n");
	  printf_(msg);

	  yyval._predicate = yyvsp[0]._predicate; 	   
       }
break;
case 59:
#line 1112 "CQL.y"
{
	   CQL_globalParserState->currentRule = "expr->expr OR expr_term";
           sprintf(msg,"BISON::expr->expr OR expr_term\n");
	   printf_(msg);
	   yyval._predicate = new CQLPredicate();
	   yyval._predicate->appendPredicate(*yyvsp[-2]._predicate);
	   yyval._predicate->appendPredicate(*yyvsp[0]._predicate, OR);
		_ObjPtr._ptr = yyval._predicate;
      _ObjPtr.type = Pred;
      _ptrs.append(_ObjPtr);
       }
break;
case 60:
#line 1125 "CQL.y"
{;}
break;
case 61:
#line 1143 "CQL.y"
{
		   CQL_globalParserState->currentRule = "arg_list->arg_list_sub->expr";
                   sprintf(msg,"BISON::arg_list_sub->expr\n");
                   printf_(msg);

                   _arglist.append(*yyvsp[0]._predicate);/*
                                           since arg_list can loop back on itself,
                                           we need to store away previous solutions
                                           production.  We keep track of previous productions
                                           in the _arglist array and later pass that to CQLFunction
                                           as part of chain: identifier LPAR arg_list RPAR
                                        */
           }
break;
case 62:
#line 1204 "CQL.y"
{
		     CQL_globalParserState->currentRule = "from_specifier->class_path";
                     sprintf(msg,"BISON::from_specifier->class_path\n");
		     printf_(msg);

		     CQL_globalParserState->statement->appendClassPath(*yyvsp[0]._identifier);
                 }
break;
case 63:
#line 1213 "CQL.y"
{
			CQL_globalParserState->currentRule = "from_specifier->class_path AS identifier";
			sprintf(msg,"BISON::from_specifier->class_path AS identifier\n");
			printf_(msg);

			CQLIdentifier _class(*yyvsp[-2]._identifier);
			String _alias(yyvsp[0]._identifier->getName().getString());
			CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
			CQL_globalParserState->statement->appendClassPath(_class);
		  }
break;
case 64:
#line 1224 "CQL.y"
{
			CQL_globalParserState->currentRule = "from_specifier->class_path identifier";
			sprintf(msg,"BISON::from_specifier->class_path identifier\n");
			printf_(msg);

			CQLIdentifier _class(*yyvsp[-1]._identifier);
                        String _alias(yyvsp[0]._identifier->getName().getString());
                        CQL_globalParserState->statement->insertClassPathAlias(_class,_alias);
                        CQL_globalParserState->statement->appendClassPath(_class);
		  }
break;
case 65:
#line 1237 "CQL.y"
{
		    CQL_globalParserState->currentRule = "from_criteria->from_specifier";
                    sprintf(msg,"BISON::from_criteria->from_specifier\n");
		    printf_(msg);
                }
break;
case 66:
#line 1245 "CQL.y"
{
		CQL_globalParserState->currentRule = "star_expr->STAR";
                sprintf(msg,"BISON::star_expr->STAR\n");
		printf_(msg);

		CQLIdentifier _id("*");
		yyval._chainedIdentifier = (CQLChainedIdentifier*)(_factory.makeObject(&_id,ChainedIdentifier));
            }
break;
case 67:
#line 1254 "CQL.y"
{
		CQL_globalParserState->currentRule = "star_expr->chain.*";
		sprintf(msg,"BISON::star_expr->chain.*\n");
                printf_(msg);
		CQLChainedIdentifier* _tmp = (CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier));
		CQLChainedIdentifier* _cid = new CQLChainedIdentifier(*_tmp);
                CQLIdentifier _id("*");
		_cid->append(_id);
                yyval._chainedIdentifier = _cid;
					 _ObjPtr._ptr = yyval._chainedIdentifier;
                _ObjPtr.type = CId;
                _ptrs.append(_ObjPtr);
	    }
break;
case 68:
#line 1270 "CQL.y"
{
		     CQL_globalParserState->currentRule = "selected_entry->expr";
                     sprintf(msg,"BISON::selected_entry->expr\n");
		     printf_(msg);
		     if(yyvsp[0]._predicate->isSimpleValue()){
		        CQLChainedIdentifier *_cid = (CQLChainedIdentifier*)(_factory.getObject(yyvsp[0]._predicate,Predicate,ChainedIdentifier));
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
break;
case 69:
#line 1289 "CQL.y"
{
		     CQL_globalParserState->currentRule = "selected_entry->star_expr";
                     sprintf(msg,"BISON::selected_entry->star_expr\n");
		     printf_(msg);
		     CQL_globalParserState->statement->appendSelectIdentifier(*yyvsp[0]._chainedIdentifier);
                 }
break;
case 70:
#line 1298 "CQL.y"
{
		CQL_globalParserState->currentRule = "select_list->selected_entry select_list_tail";
                sprintf(msg,"BISON::select_list->selected_entry select_list_tail\n");
		printf_(msg);
            }
break;
case 71:
#line 1305 "CQL.y"
{;}
break;
case 72:
#line 1307 "CQL.y"
{
		       CQL_globalParserState->currentRule = "select_list_tail->COMMA selected_entry select_list_tail";
                       sprintf(msg,"BISON::select_list_tail->COMMA selected_entry select_list_tail\n");
		       printf_(msg);
                   }
break;
case 73:
#line 1315 "CQL.y"
{
			CQL_globalParserState->currentRule = "search_condition->expr";
                        sprintf(msg,"BISON::search_condition->expr\n");
			printf_(msg);
			CQL_globalParserState->statement->setPredicate(*yyvsp[0]._predicate);
                   }
break;
case 74:
#line 1323 "CQL.y"
{}
break;
case 75:
#line 1325 "CQL.y"
{
		     CQL_globalParserState->currentRule = "optional_where->WHERE search_condition";
                     sprintf(msg,"BISON::optional_where->WHERE search_condition\n");
		     printf_(msg);
		     CQL_globalParserState->statement->setHasWhereClause();
                 }
break;
case 76:
#line 1334 "CQL.y"
{
		       CQL_globalParserState->currentRule = "select_statement";
                       sprintf(msg,"select_statement\n\n");
		       printf_(msg);
				 CQL_Bison_Cleanup();
                   }
break;
#line 1779 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
