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
/*	if(DEBUG_GRAMMAR == 1)*/
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


#line 42 "CQL.y"
typedef union {
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
} YYSTYPE;
#line 102 "y.tab.c"
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
   21,   21,   21,   21,   22,   22,   23,   23,   24,   24,
   25,   25,   25,   26,   26,   26,   27,   28,   28,   29,
   29,   30,   31,   31,   32,   33,   33,    0,
};
const short CQL_len[] = {                                         2,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    3,    1,    3,    1,    4,    3,    3,
    5,    4,    1,    3,    1,    1,    1,    2,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    4,    3,    3,
    3,    3,    3,    3,    1,    2,    1,    3,    1,    3,
    0,    1,    1,    1,    3,    2,    1,    1,    3,    1,
    1,    2,    0,    3,    1,    0,    2,    5,
};
const short CQL_defred[] = {                                      0,
    0,    0,    1,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   19,   20,    4,    0,    0,    0,   68,    0,
   27,   14,   16,   17,   15,   18,   23,    0,    0,   36,
   37,    0,    0,   55,   57,    0,    0,   71,    0,    0,
    0,    0,   38,   56,    0,    0,    0,    0,    0,    0,
   41,   42,   43,   44,   45,   46,    0,    0,    0,    0,
    0,    0,    0,   72,    0,    0,   24,   62,    0,    0,
   26,   69,    0,   29,   22,    0,    0,    0,   49,    0,
   53,   54,   39,   40,   50,   51,   52,   58,    0,    0,
    2,    3,    0,   67,    0,   28,    0,   32,   48,   74,
    0,   66,    0,   78,   31,   65,    0,   77,
};
const short CQL_dgoto[] = {                                       2,
   20,   92,   93,   21,   22,   23,   24,   25,   26,   27,
   75,   76,   41,   29,   30,   31,   32,   33,   85,   59,
   34,   35,   36,   37,   70,   94,   95,   38,   39,   40,
   64,  108,  104,
};
const short CQL_sindex[] = {                                   -295,
 -155,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,  -93, -241,  -30,    0, -256,
    0,    0,    0,    0,    0,    0,    0, -255, -257,    0,
    0, -220, -247,    0,    0, -265, -264,    0, -231, -254,
 -237, -261,    0,    0, -109, -241, -249,  -93,  -12, -273,
    0,    0,    0,    0,    0,    0, -210, -241,  -30,  -30,
  -93,  -93, -155,    0, -210, -244,    0,    0, -264, -216,
    0,    0, -215,    0,    0, -219, -264, -237,    0, -227,
    0,    0,    0,    0,    0,    0,    0,    0, -265, -231,
    0,    0, -251,    0, -248,    0, -241,    0,    0,    0,
 -210,    0,  -93,    0,    0,    0, -264,    0,
};
const short CQL_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,    0,    0,    0,  -82,   98,    0,
    0,  101,    0,    0,    0,  124, -270,    0, -243,    0,
   52,    0,    0,    0, -211,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -197,    0,
    0,    0,   29,    0,    0,    0, -199,   75,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  127, -243,
    0,    0,    1,    0,   77,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   78,    0,
};
const short CQL_gindex[] = {                                      0,
  -42,    0,    0,  -38,  -13,    0,    0,    0,    0,    0,
    0,    0,   -1,    0,    0,    0,  -33,  -10,    0,   46,
   62,   20,   21,  -14,    0,    0,    0,    0,   19,    0,
   -5,    0,    0,
};
#define YYTABLESIZE 435
const short CQL_table[] = {                                      28,
   64,   42,   25,   43,   73,    3,   70,    3,   74,   67,
    1,   79,    3,   45,   81,   46,    4,   47,   48,   15,
   80,   49,   91,   73,   15,   83,   86,   74,   30,   61,
   69,   62,   71,   77,   62,   66,   48,   70,   51,   52,
   53,   54,   55,   56,   82,   63,    3,   78,   84,   87,
  102,   33,   72,   65,   96,   98,   97,   99,  106,   61,
  103,   28,  101,   50,   73,   51,   52,   53,   54,   55,
   56,   57,   58,   63,   34,   21,   76,   75,   60,   44,
   88,   90,   89,  105,  100,    0,    0,    0,  107,    0,
    0,    0,    0,    0,    0,    0,    0,   35,    0,    0,
   47,    3,    4,    5,    6,    7,    8,    9,   10,   11,
   12,   13,   14,   15,   16,    0,   17,    0,    0,    0,
    0,    0,    0,   59,    0,    0,   60,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   18,    0,
    0,    0,    0,    0,    0,    0,   19,    3,    4,    5,
    6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
   16,    0,   17,    3,    4,    5,    6,    7,    8,    9,
   10,   11,   12,   13,   14,   15,   16,    0,   17,    0,
    0,    0,    0,    0,   18,    0,    0,    0,    0,    0,
    0,    0,   68,    0,   33,    0,   33,    0,    0,    0,
   18,   33,    0,   33,   33,   33,   33,   33,   33,   33,
   33,    0,   33,   33,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,   33,    3,    4,    5,    6,
    7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
    0,   17,    0,    0,    3,    4,    5,    6,    7,    8,
    9,   10,   11,   12,   13,   14,   15,   16,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   25,    0,   25,   25,   25,    0,   25,
    0,   25,    0,    0,    0,    0,   25,    0,   25,   25,
   25,   25,   25,   25,   25,   25,    0,   25,   25,   30,
    0,   30,   30,   30,    0,   30,    0,   30,    0,   64,
   25,    0,   30,    0,   30,   30,   30,   30,   30,   30,
   30,   30,   33,   30,   30,    0,   33,    0,   33,    0,
   33,    0,    0,    0,    0,   33,   30,   33,   33,   33,
   33,   33,   33,   33,   33,   34,   33,   33,    0,   34,
    0,   34,    0,   34,    0,    0,    0,    0,   34,   33,
   34,   34,   34,   34,   34,   34,   34,   34,   35,   34,
   34,   47,   35,    0,   35,   47,    0,   47,    0,    0,
    0,   35,   34,   35,   35,   35,   35,   35,   35,   35,
   35,    0,   35,   35,   59,   47,   47,   60,   59,    0,
   59,   60,    0,   60,    0,   35,    0,    0,   47,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   59,
    0,    0,   60,    0,    0,    0,    0,    0,    0,    0,
    0,   59,    0,    0,   60,
};
const short CQL_check[] = {                                       1,
    0,   16,    0,   17,   47,  257,  277,  257,   47,  271,
  306,  285,  257,  270,   57,  272,  258,  273,  274,  269,
  294,  279,   65,   66,  269,   59,   60,   66,    0,  295,
   45,  296,   46,   48,  296,  273,  274,  308,  286,  287,
  288,  289,  290,  291,   58,  277,  257,   49,   59,   60,
   93,    0,  302,  308,  271,  275,  272,  285,  101,  271,
  309,   63,  314,  284,  308,  286,  287,  288,  289,  290,
  291,  292,  293,  271,    0,  275,    0,    0,   33,   18,
   61,   63,   62,   97,   90,   -1,   -1,   -1,  103,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
    0,  257,  258,  259,  260,  261,  262,  263,  264,  265,
  266,  267,  268,  269,  270,   -1,  272,   -1,   -1,   -1,
   -1,   -1,   -1,    0,   -1,   -1,    0,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  294,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  302,  257,  258,  259,
  260,  261,  262,  263,  264,  265,  266,  267,  268,  269,
  270,   -1,  272,  257,  258,  259,  260,  261,  262,  263,
  264,  265,  266,  267,  268,  269,  270,   -1,  272,   -1,
   -1,   -1,   -1,   -1,  294,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  302,   -1,  277,   -1,  279,   -1,   -1,   -1,
  294,  284,   -1,  286,  287,  288,  289,  290,  291,  292,
  293,   -1,  295,  296,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  308,  257,  258,  259,  260,
  261,  262,  263,  264,  265,  266,  267,  268,  269,  270,
   -1,  272,   -1,   -1,  257,  258,  259,  260,  261,  262,
  263,  264,  265,  266,  267,  268,  269,  270,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  271,   -1,  273,  274,  275,   -1,  277,
   -1,  279,   -1,   -1,   -1,   -1,  284,   -1,  286,  287,
  288,  289,  290,  291,  292,  293,   -1,  295,  296,  271,
   -1,  273,  274,  275,   -1,  277,   -1,  279,   -1,  309,
  308,   -1,  284,   -1,  286,  287,  288,  289,  290,  291,
  292,  293,  271,  295,  296,   -1,  275,   -1,  277,   -1,
  279,   -1,   -1,   -1,   -1,  284,  308,  286,  287,  288,
  289,  290,  291,  292,  293,  271,  295,  296,   -1,  275,
   -1,  277,   -1,  279,   -1,   -1,   -1,   -1,  284,  308,
  286,  287,  288,  289,  290,  291,  292,  293,  271,  295,
  296,  271,  275,   -1,  277,  275,   -1,  277,   -1,   -1,
   -1,  284,  308,  286,  287,  288,  289,  290,  291,  292,
  293,   -1,  295,  296,  271,  295,  296,  271,  275,   -1,
  277,  275,   -1,  277,   -1,  308,   -1,   -1,  308,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  296,
   -1,   -1,  296,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  308,   -1,   -1,  308,
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
"concat : concat DBL_PIPE chain",
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
"comp : value_symbol comp_op value_symbol",
"comp : arith _ISA identifier",
"comp : arith _LIKE literal_string",
"expr_factor : comp",
"expr_factor : NOT comp",
"expr_term : expr_factor",
"expr_term : expr_term _AND expr_factor",
"expr : expr_term",
"expr : expr _OR expr_term",
"arg_list :",
"arg_list : STAR",
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
#line 1195 "CQL.y"

/*int yyerror(char * err){yyclearin; yyerrok;throw Exception(String(err));return 1;}*/
#line 461 "y.tab.c"
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
#line 172 "CQL.y"
{ 
		 globalParserState->currentRule = "identifier";
                 sprintf(msg,"BISON::identifier\n");
		 printf_(msg);
	
                 yyval._identifier = new CQLIdentifier(String(CQL_lval.strValue));
             }
break;
case 2:
#line 182 "CQL.y"
{
		 globalParserState->currentRule = "class_name";
                 sprintf(msg,"BISON::class_name = %s\n", (const char *)(yyvsp[0]._identifier->getName().getString().getCString())); 
		 printf_(msg);
		yyval._identifier = yyvsp[0]._identifier;
             }
break;
case 3:
#line 191 "CQL.y"
{ 
		globalParserState->currentRule = "class_path";
                 sprintf(msg,"BISON::class_path\n"); 
		 printf_(msg);
		 yyval._identifier = yyvsp[0]._identifier;
             }
break;
case 4:
#line 210 "CQL.y"
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
		        yyval._identifier = new CQLIdentifier(tmp);
                  }
break;
case 5:
#line 229 "CQL.y"
{ 
		/*
		   We make sure the literal is valid UTF8, then make a String
		*/
		globalParserState->currentRule = "literal_string";
                sprintf(msg,"BISON::literal_string-> %s\n",CQL_lval.strValue); 
		printf_(msg);

		if(isUTF8Str(CQL_lval.strValue)){
		     yyval._string = new String(CQL_lval.strValue);
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
break;
case 6:
#line 254 "CQL.y"
{ 
		   globalParserState->currentRule = "binary_value->BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Binary); 
               }
break;
case 7:
#line 262 "CQL.y"
{ 
		   globalParserState->currentRule = "binary_value->NEGATIVE_BINARY";
                   sprintf(msg,"BISON::binary_value-> %s\n",CQL_lval.strValue); 
		   printf_(msg);

                   yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Binary, false); 
               }
break;
case 8:
#line 273 "CQL.y"
{ 
		globalParserState->currentRule = "hex_value->HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Hex);
            }
break;
case 9:
#line 281 "CQL.y"
{ 
		globalParserState->currentRule = "hex_value->NEGATIVE_HEXADECIMAL";
                sprintf(msg,"BISON::hex_value-> %s\n",CQL_lval.strValue); 
		printf_(msg);

                yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Hex, false);
            }
break;
case 10:
#line 292 "CQL.y"
{ 
		    globalParserState->currentRule = "decimal_value->INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Decimal); 
                }
break;
case 11:
#line 300 "CQL.y"
{ 
		    globalParserState->currentRule = "decimal_value->NEGATIVE_INTEGER";
                    sprintf(msg,"BISON::decimal_value-> %s\n",CQL_lval.strValue); 
		    printf_(msg);

                    yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Decimal, false);
                }
break;
case 12:
#line 311 "CQL.y"
{ 
		 globalParserState->currentRule = "real_value->REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Real);
             }
break;
case 13:
#line 318 "CQL.y"
{ 
		 globalParserState->currentRule = "real_value->NEGATIVE_REAL";
                 sprintf(msg,"BISON::real_value-> %s\n",CQL_lval.strValue); 
		 printf_(msg);
                 yyval._value = new CQLValue(CQL_lval.strValue, CQLValue::Real, false);
             }
break;
case 14:
#line 328 "CQL.y"
{
	      globalParserState->currentRule = "literal->literal_string";
              sprintf(msg,"BISON::literal->literal_string\n");
	      printf_(msg);
              yyval._value = new CQLValue(*yyvsp[0]._string);
	      delete yyvsp[0]._string;
          }
break;
case 15:
#line 336 "CQL.y"
{
	      globalParserState->currentRule = "literal->decimal_value";
              sprintf(msg,"BISON::literal->decimal_value\n");
	      printf_(msg);

          }
break;
case 16:
#line 343 "CQL.y"
{
              globalParserState->currentRule = "literal->binary_value";
              sprintf(msg,"BISON::literal->binary_value\n");
	      printf_(msg);

          }
break;
case 17:
#line 350 "CQL.y"
{
              globalParserState->currentRule = "literal->hex_value";
              sprintf(msg,"BISON::literal->hex_value\n");
	      printf_(msg);

          }
break;
case 18:
#line 357 "CQL.y"
{
              globalParserState->currentRule = "literal->real_value";
              sprintf(msg,"BISON::literal->real_value\n");
	      printf_(msg);

          }
break;
case 19:
#line 364 "CQL.y"
{
	      globalParserState->currentRule = "literal->_TRUE";
              sprintf(msg,"BISON::literal->_TRUE\n");
	      printf_(msg);

              yyval._value = new CQLValue(Boolean(true));
          }
break;
case 20:
#line 372 "CQL.y"
{
	      globalParserState->currentRule = "literal->_FALSE";
              sprintf(msg,"BISON::literal->_FALSE\n");
	      printf_(msg);

              yyval._value = new CQLValue(Boolean(false));
          }
break;
case 21:
#line 383 "CQL.y"
{
		  globalParserState->currentRule = "array_index->expr";
                  sprintf(msg,"BISON::array_index->expr\n");
		  printf_(msg);

		  /*CQLValue tmp = _factory.getValue((CQLPredicate*)$1);*/
		  /*$$ = new String(tmp.toString());*/
		  CQLValue* _val = (CQLValue*)_factory.getObject(yyvsp[0]._predicate,Predicate,Value);
		  yyval._string = new String(_val->toString());
              }
break;
case 22:
#line 397 "CQL.y"
{
		       globalParserState->currentRule = "array_index_list->array_index";
                       sprintf(msg,"BISON::array_index_list->array_index\n");
		       printf_(msg);
 		       yyval._string = yyvsp[0]._string;
                   }
break;
case 23:
#line 407 "CQL.y"
{
            globalParserState->currentRule = "chain->literal";
            sprintf(msg,"BISON::chain->literal\n");
	    printf_(msg);

            chain_state = CQLVALUE;
				printf("********************* BEFORE MAKEOBJECT **************\n");
	    yyval._node = _factory.makeObject(yyvsp[0]._value,Predicate);  
		 	   printf("**************************************** AFTER **********\n");
	    delete yyvsp[0]._value;
        }
break;
case 24:
#line 419 "CQL.y"
{
	    globalParserState->currentRule = "chain-> ( expr )";
            sprintf(msg,"BISON::chain-> ( expr )\n");
	    printf_(msg);

            chain_state = CQLPREDICATE;
	    yyval._node = yyvsp[-1]._predicate;
        }
break;
case 25:
#line 428 "CQL.y"
{
	   globalParserState->currentRule = "chain->identifier";
           sprintf(msg,"BISON::chain->identifier\n");
	   printf_(msg);

           chain_state = CQLIDENTIFIER;
	   yyval._node = _factory.makeObject(yyvsp[0]._identifier,Predicate);
	   delete yyvsp[0]._identifier;
        }
break;
case 26:
#line 438 "CQL.y"
{
	    globalParserState->currentRule = "chain->identifier#literal_string";
            sprintf(msg,"BISON::chain->identifier#literal_string\n");
	    printf_(msg);

            String tmp = yyvsp[-2]._identifier->getName().getString();
            tmp.append("#").append(*yyvsp[0]._string);
            CQLIdentifier _id(tmp);
   	    yyval._node = _factory.makeObject(&_id,Predicate);
	    chain_state = CQLIDENTIFIER;
	    delete yyvsp[-2]._identifier; delete yyvsp[0]._string;
        }
break;
case 27:
#line 451 "CQL.y"
{
	    globalParserState->currentRule = "chain->scoped_property";
	    sprintf(msg,"BISON::chain-> scoped_property\n");
	    printf_(msg);

            chain_state = CQLIDENTIFIER;
	    yyval._node = _factory.makeObject(yyvsp[0]._identifier,Predicate);
	    delete yyvsp[0]._identifier;
        }
break;
case 28:
#line 461 "CQL.y"
{
	    globalParserState->currentRule = "chain->identifier( arg_list )";
            sprintf(msg,"BISON::chain-> identifier( arg_list )\n");
	    printf_(msg);
            chain_state = CQLFUNCTION;
	    CQLFunction _func(*yyvsp[-3]._identifier,_arglist);
	    yyval._node = (CQLPredicate*)(_factory.makeObject(&_func,Predicate));
	    _arglist.clear();
	    delete yyvsp[-3]._identifier; 
        }
break;
case 29:
#line 472 "CQL.y"
{
	    globalParserState->currentRule = "chain->chain.scoped_property";
	    sprintf(msg,"BISON::chain-> chain DOT scoped_property : chain_state = %d\n",chain_state);
	    printf_(msg);

	    CQLIdentifier *_id;
	    if(chain_state == CQLIDENTIFIER){
	        _id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*yyvsp[0]._identifier);
		yyval._node = _factory.makeObject(&_cid,Predicate);
		delete yyvsp[0]._identifier;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid;
		_cid = ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier)));
		_cid->append(*yyvsp[0]._identifier);
		_factory.setObject(((CQLPredicate*)yyvsp[-2]._node),_cid,ChainedIdentifier);
		yyval._node = yyvsp[-2]._node;
		delete yyvsp[0]._identifier;
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
break;
case 30:
#line 505 "CQL.y"
{
	    globalParserState->currentRule = "chain->chain.identifier";
            sprintf(msg,"BISON::chain->chain.identifier : chain_state = %d\n",chain_state);
	    printf_(msg);

            if(chain_state == CQLIDENTIFIER){
		CQLIdentifier *_id = ((CQLIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,Identifier)));
                CQLChainedIdentifier _cid(*_id);
                _cid.append(*yyvsp[0]._identifier);
                yyval._node = _factory.makeObject(&_cid,Predicate);
		delete yyvsp[0]._identifier;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
		CQLChainedIdentifier *_cid = ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier)));
                _cid->append(*yyvsp[0]._identifier);
                _factory.setObject(((CQLPredicate*)yyvsp[-2]._node),_cid,ChainedIdentifier);
                yyval._node = yyvsp[-2]._node;
		delete yyvsp[0]._identifier;
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
break;
case 31:
#line 536 "CQL.y"
{
	    globalParserState->currentRule = "chain->chain.identifier#literal_string";
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
                delete yyvsp[-2]._identifier; delete yyvsp[0]._string;
            }else if(chain_state == CQLCHAINEDIDENTIFIER){
              CQLChainedIdentifier *_cid =  ((CQLChainedIdentifier*)(_factory.getObject(yyvsp[-4]._node,Predicate,ChainedIdentifier)));
              String tmp(yyvsp[-2]._identifier->getName().getString());
                tmp.append("#").append(*yyvsp[0]._string);
                CQLIdentifier _id1(tmp);
                _cid->append(_id1);
                _factory.setObject(((CQLPredicate*)yyvsp[-4]._node),_cid,ChainedIdentifier);
                yyval._node = yyvsp[-4]._node;
                delete yyvsp[-2]._identifier; delete yyvsp[0]._string;
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
break;
case 32:
#line 575 "CQL.y"
{
	    globalParserState->currentRule = "chain->chain[ array_index_list ]";
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
		delete yyvsp[-1]._string;	
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
		delete yyvsp[-1]._string;
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
break;
case 33:
#line 619 "CQL.y"
{
	     globalParserState->currentRule = "concat->chain";
             sprintf(msg,"BISON::concat->chain\n");
	     printf_(msg);

	     yyval._predicate = ((CQLPredicate*)yyvsp[0]._node);
         }
break;
case 34:
#line 627 "CQL.y"
{
	     globalParserState->currentRule = "concat->concat || chain";
             sprintf(msg,"BISON::concat||chain\n");
	     printf_(msg);

	     if(yyvsp[-2]._predicate->isSimpleValue() && ((CQLPredicate*)yyvsp[0]._node)->isSimpleValue()){
		CQLFactor* _fctr1 = ((CQLFactor*)(_factory.getObject(yyvsp[-2]._predicate, Predicate, Factor)));
		CQLFactor* _fctr2 = ((CQLFactor*)(_factory.getObject(yyvsp[0]._node, Predicate, Factor)));	
		CQLTerm _term1(*_fctr1);
		_term1.appendOperation(concat,*_fctr2);
		yyval._predicate = (CQLPredicate*)(_factory.makeObject(&_term1,Predicate));
		delete yyvsp[-2]._predicate; 
		CQLPredicate* _pred = (CQLPredicate*)yyvsp[0]._node;
		delete _pred;
	     }
         }
break;
case 35:
#line 646 "CQL.y"
{
	     globalParserState->currentRule = "factor->concat";
             sprintf(msg,"BISON::factor->concat\n");
	     printf_(msg);

	     yyval._predicate = yyvsp[0]._predicate;
         }
break;
case 36:
#line 673 "CQL.y"
{
	   globalParserState->currentRule = "term->factor";
           sprintf(msg,"BISON::term->factor\n");
	   printf_(msg);

           yyval._predicate = yyvsp[0]._predicate;
       }
break;
case 37:
#line 699 "CQL.y"
{
	    globalParserState->currentRule = "arith->term";
            sprintf(msg,"BISON::arith->term\n");
	    printf_(msg);

	    /*CQLPredicate* _pred = new CQLPredicate(*$1);*/
/*	    _factory._predicates.append(_pred);*/
            yyval._predicate = yyvsp[0]._predicate;
        }
break;
case 38:
#line 727 "CQL.y"
{
	  	   globalParserState->currentRule = "value_symbol->#literal_string";
                   sprintf(msg,"BISON::value_symbol->#literal_string\n");
                   printf_(msg);

		   String tmp("#");
		   tmp.append(*yyvsp[0]._string);
		   CQLIdentifier tmpid(tmp);
		   yyval._value = new CQLValue(tmpid);
		   delete yyvsp[0]._string;
               }
break;
case 39:
#line 741 "CQL.y"
{
			    globalParserState->currentRule = "arith_or_value_symbol->arith";
                            sprintf(msg,"BISON::arith_or_value_symbol->arith\n");
			    printf_(msg);

			    yyval._predicate = yyvsp[0]._predicate;
                        }
break;
case 40:
#line 749 "CQL.y"
{
			    /* make into predicate */
			    globalParserState->currentRule = "arith_or_value_symbol->value_symbol";
                            sprintf(msg,"BISON::arith_or_value_symbol->value_symbol\n");
			    printf_(msg);

			    CQLFactor _fctr(*yyvsp[0]._value);
			    yyval._predicate = (CQLPredicate*)(_factory.makeObject(&_fctr, Predicate));
			    delete yyvsp[0]._value;
                        }
break;
case 41:
#line 762 "CQL.y"
{
	      globalParserState->currentRule = "comp_op->_EQ";
              sprintf(msg,"BISON::comp_op->_EQ\n");
	      printf_(msg);
	      yyval._opType = EQ;
          }
break;
case 42:
#line 769 "CQL.y"
{
	      globalParserState->currentRule = "comp_op->_NE";
              sprintf(msg,"BISON::comp_op->_NE\n");
	      printf_(msg);
	      yyval._opType = NE;
          }
break;
case 43:
#line 776 "CQL.y"
{
	      globalParserState->currentRule = "comp_op->_GT";
              sprintf(msg,"BISON::comp_op->_GT\n");
	      printf_(msg);
	      yyval._opType = GT;
          }
break;
case 44:
#line 783 "CQL.y"
{
 	      globalParserState->currentRule = "comp_op->_LT";
              sprintf(msg,"BISON::comp_op->_LT\n");
	      printf_(msg);
	      yyval._opType = LT;
          }
break;
case 45:
#line 790 "CQL.y"
{
	      globalParserState->currentRule = "comp_op->_GE";
              sprintf(msg,"BISON::comp_op->_GE\n");
	      printf_(msg);
	      yyval._opType = GE;
          }
break;
case 46:
#line 797 "CQL.y"
{
	      globalParserState->currentRule = "comp_op->_LE";
              sprintf(msg,"BISON::comp_op->_LE\n");
	      printf_(msg);
	      yyval._opType = LE;
          }
break;
case 47:
#line 806 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith";
           sprintf(msg,"BISON::comp->arith\n");
	   printf_(msg);

	   yyval._predicate = yyvsp[0]._predicate;
       }
break;
case 48:
#line 814 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith IS NOT _NULL";
           sprintf(msg,"BISON::comp->arith IS NOT _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject(yyvsp[-3]._predicate,Expression));
	   CQLSimplePredicate _sp(*_expr, IS_NOT_NULL);
           _factory.setObject(yyvsp[-3]._predicate,&_sp,SimplePredicate);
	   yyval._predicate = yyvsp[-3]._predicate;
       }
break;
case 49:
#line 825 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith IS _NULL";
           sprintf(msg,"BISON::comp->arith IS _NULL\n");
	   printf_(msg);

	   CQLExpression *_expr = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Expression));
           CQLSimplePredicate _sp(*_expr, IS_NULL);
           _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
       }
break;
case 50:
#line 836 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith comp_op arith_or_value_symbol";
           sprintf(msg,"BISON::comp->arith comp_op arith_or_value_symbol\n");
	   printf_(msg);
	   if(yyvsp[-2]._predicate->isSimple() && yyvsp[0]._predicate->isSimple()){
		CQLExpression* _exp1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
		CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject(yyvsp[0]._predicate,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, yyvsp[-1]._opType);
           	yyval._predicate = new CQLPredicate(_sp);
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
break;
case 51:
#line 857 "CQL.y"
{
	   globalParserState->currentRule = "comp->value_symbol comp_op arith";
           sprintf(msg,"BISON::comp->value_symbol comp_op arith\n");
	   printf_(msg);

	   if(yyvsp[0]._predicate->isSimple()){
           	CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject(yyvsp[-2]._value, Expression));
	        CQLExpression* _exp2 = (CQLExpression*)(_factory.getObject(yyvsp[0]._predicate,Predicate,Expression));
	   	CQLSimplePredicate _sp(*_exp1, *_exp2, yyvsp[-1]._opType);
           	yyval._predicate = new CQLPredicate(_sp);
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
break;
case 52:
#line 880 "CQL.y"
{
		globalParserState->currentRule = "comp->value_symbol comp_op value_symbol";
		sprintf(msg,"BISON::comp->value_symbol comp_op value_symbol\n");
           	printf_(msg);
                                                                                                                                                             
                CQLExpression* _exp1 = (CQLExpression*)(_factory.makeObject(yyvsp[-2]._value, Expression));
                CQLExpression* _exp2 = (CQLExpression*)(_factory.makeObject(yyvsp[0]._value,Expression));
                CQLSimplePredicate _sp(*_exp1, *_exp2, yyvsp[-1]._opType);
                yyval._predicate = new CQLPredicate(_sp);
       }
break;
case 53:
#line 891 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith _ISA identifier";
	   /* make sure $1 isSimple(), get its expression, make simplepred->predicate */
           sprintf(msg,"BISON::comp->arith _ISA identifier\n");
	   printf_(msg);

	   CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
	   CQLChainedIdentifier _cid(*yyvsp[0]._identifier);
	   CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_cid,Expression));
           CQLSimplePredicate _sp(*_expr1, *_expr2, ISA);
	   _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
	   delete yyvsp[0]._identifier;
       }
break;
case 54:
#line 906 "CQL.y"
{
	   globalParserState->currentRule = "comp->arith _LIKE literal_string";
           sprintf(msg,"BISON::comp->arith _LIKE literal_string\n");
	   printf_(msg);

           CQLExpression *_expr1 = (CQLExpression*)(_factory.getObject(yyvsp[-2]._predicate,Predicate,Expression));
	   CQLValue _val(*yyvsp[0]._string);
           CQLExpression *_expr2 = (CQLExpression*)(_factory.makeObject(&_val,Expression));
	   CQLSimplePredicate _sp(*_expr1, *_expr2, LIKE);
           _factory.setObject(yyvsp[-2]._predicate,&_sp,SimplePredicate);
           yyval._predicate = yyvsp[-2]._predicate;
	   delete yyvsp[0]._string;
       }
break;
case 55:
#line 921 "CQL.y"
{
		  globalParserState->currentRule = "expr_factor->comp";
                  sprintf(msg,"BISON::expr_factor->comp\n");
	          printf_(msg);

		  yyval._predicate = yyvsp[0]._predicate;
              }
break;
case 56:
#line 929 "CQL.y"
{
		  globalParserState->currentRule = "expr_factor->NOT comp";
                  sprintf(msg,"BISON::expr_factor->NOT comp\n");
	 	  printf_(msg);

		  yyvsp[0]._predicate->setInverted();
		  yyval._predicate = yyvsp[0]._predicate;
              }
break;
case 57:
#line 940 "CQL.y"
{
	        globalParserState->currentRule = "expr_term->expr_factor";
                sprintf(msg,"BISON::expr_term->expr_factor\n");
		printf_(msg);

		yyval._predicate = yyvsp[0]._predicate;
            }
break;
case 58:
#line 948 "CQL.y"
{
		globalParserState->currentRule = "expr_term->expr_term AND expr_factor";
		sprintf(msg,"BISON::expr_term->expr_term AND expr_factor\n");
		printf_(msg);

		yyval._predicate = new CQLPredicate();
           	yyval._predicate->appendPredicate(*yyvsp[-2]._predicate);
           	yyval._predicate->appendPredicate(*yyvsp[0]._predicate, AND);	
            }
break;
case 59:
#line 960 "CQL.y"
{
	  globalParserState->currentRule = "expr->expr_term";
          sprintf(msg,"BISON::expr->expr_term\n");
	  printf_(msg);

	  yyval._predicate = yyvsp[0]._predicate; 	   
       }
break;
case 60:
#line 968 "CQL.y"
{
	   globalParserState->currentRule = "expr->expr OR expr_term";
           sprintf(msg,"BISON::expr->expr OR expr_term\n");
	   printf_(msg);
	   yyval._predicate = new CQLPredicate();
	   yyval._predicate->appendPredicate(*yyvsp[-2]._predicate);
	   yyval._predicate->appendPredicate(*yyvsp[0]._predicate, OR);
       }
break;
case 61:
#line 978 "CQL.y"
{;}
break;
case 62:
#line 980 "CQL.y"
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
break;
case 63:
#line 996 "CQL.y"
{
		   globalParserState->currentRule = "arg_list->arg_list_sub->expr";
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
case 64:
#line 1057 "CQL.y"
{
		     globalParserState->currentRule = "from_specifier->class_path";
                     sprintf(msg,"BISON::from_specifier->class_path\n");
		     printf_(msg);

		     globalParserState->statement->appendClassPath(*yyvsp[0]._identifier);
		     delete yyvsp[0]._identifier;
                 }
break;
case 65:
#line 1067 "CQL.y"
{
			globalParserState->currentRule = "from_specifier->class_path AS identifier";
			sprintf(msg,"BISON::from_specifier->class_path AS identifier\n");
			printf_(msg);

			CQLIdentifier _class(*yyvsp[-2]._identifier);
			String _alias(yyvsp[0]._identifier->getName().getString());
			globalParserState->statement->insertClassPathAlias(_class,_alias);
			globalParserState->statement->appendClassPath(_class);
			delete yyvsp[-2]._identifier; delete yyvsp[0]._identifier;
		  }
break;
case 66:
#line 1079 "CQL.y"
{
			globalParserState->currentRule = "from_specifier->class_path identifier";
			sprintf(msg,"BISON::from_specifier->class_path identifier\n");
			printf_(msg);

			CQLIdentifier _class(*yyvsp[-1]._identifier);
                        String _alias(yyvsp[0]._identifier->getName().getString());
                        globalParserState->statement->insertClassPathAlias(_class,_alias);
                        globalParserState->statement->appendClassPath(_class);
			delete yyvsp[-1]._identifier; delete yyvsp[0]._identifier;
		  }
break;
case 67:
#line 1093 "CQL.y"
{
		    globalParserState->currentRule = "from_criteria->from_specifier";
                    sprintf(msg,"BISON::from_criteria->from_specifier\n");
		    printf_(msg);
                }
break;
case 68:
#line 1101 "CQL.y"
{
		globalParserState->currentRule = "star_expr->STAR";
                sprintf(msg,"BISON::star_expr->STAR\n");
		printf_(msg);

		CQLIdentifier _id("*");
		yyval._chainedIdentifier = (CQLChainedIdentifier*)(_factory.makeObject(&_id,ChainedIdentifier));
            }
break;
case 69:
#line 1110 "CQL.y"
{
		globalParserState->currentRule = "star_expr->chain.*";
		sprintf(msg,"BISON::star_expr->chain.*\n");
                printf_(msg);
		CQLChainedIdentifier* _tmp = (CQLChainedIdentifier*)(_factory.getObject(yyvsp[-2]._node,Predicate,ChainedIdentifier));
		CQLChainedIdentifier* _cid = new CQLChainedIdentifier(*_tmp);
                CQLIdentifier _id("*");
		_cid->append(_id);
                yyval._chainedIdentifier = _cid;
	    }
break;
case 70:
#line 1123 "CQL.y"
{
		     globalParserState->currentRule = "selected_entry->expr";
                     sprintf(msg,"BISON::selected_entry->expr\n");
		     printf_(msg);
		     if(yyvsp[0]._predicate->isSimpleValue()){
		        CQLChainedIdentifier *_cid = (CQLChainedIdentifier*)(_factory.getObject(yyvsp[0]._predicate,Predicate,ChainedIdentifier));
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
break;
case 71:
#line 1142 "CQL.y"
{
		     globalParserState->currentRule = "selected_entry->star_expr";
                     sprintf(msg,"BISON::selected_entry->star_expr\n");
		     printf_(msg);
		     globalParserState->statement->appendSelectIdentifier(*yyvsp[0]._chainedIdentifier);
                 }
break;
case 72:
#line 1151 "CQL.y"
{
		globalParserState->currentRule = "select_list->selected_entry select_list_tail";
                sprintf(msg,"BISON::select_list->selected_entry select_list_tail\n");
		printf_(msg);
            }
break;
case 73:
#line 1158 "CQL.y"
{;}
break;
case 74:
#line 1160 "CQL.y"
{
		       globalParserState->currentRule = "select_list_tail->COMMA selected_entry select_list_tail";
                       sprintf(msg,"BISON::select_list_tail->COMMA selected_entry select_list_tail\n");
		       printf_(msg);
                   }
break;
case 75:
#line 1168 "CQL.y"
{
			globalParserState->currentRule = "search_condition->expr";
                        sprintf(msg,"BISON::search_condition->expr\n");
			printf_(msg);
			globalParserState->statement->setPredicate(*yyvsp[0]._predicate);
                   }
break;
case 76:
#line 1176 "CQL.y"
{;}
break;
case 77:
#line 1178 "CQL.y"
{
		     globalParserState->currentRule = "optional_where->WHERE search_condition";
                     sprintf(msg,"BISON::optional_where->WHERE search_condition\n");
		     printf_(msg);
		     globalParserState->statement->setHasWhereClause();
                 }
break;
case 78:
#line 1187 "CQL.y"
{
		       globalParserState->currentRule = "select_statement";
                       sprintf(msg,"select_statement\n\n");
		       printf_(msg);
                   }
break;
#line 1657 "y.tab.c"
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
