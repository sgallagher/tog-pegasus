//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
#define yyparse cimmof_parse
#define yylex cimmof_lex
#define yyerror cimmof_error
#define yychar cimmof_char
#define yyval cimmof_val
#define yylval cimmof_lval
#define yydebug cimmof_debug
#define yynerrs cimmof_nerrs
#define yyerrflag cimmof_errflag
#define yyss cimmof_ss
#define yyssp cimmof_ssp
#define yyvs cimmof_vs
#define yyvsp cimmof_vsp
#define yylhs cimmof_lhs
#define yylen cimmof_len
#define yydefred cimmof_defred
#define yydgoto cimmof_dgoto
#define yysindex cimmof_sindex
#define yyrindex cimmof_rindex
#define yygindex cimmof_gindex
#define yytable cimmof_table
#define yycheck cimmof_check
#define yyname cimmof_name
#define yyrule cimmof_rule
#define yysslim cimmof_sslim
#define yystacksize cimmof_stacksize
#define YYPREFIX "cimmof_"
#line 2 "cimmof.y"
  /* Flex grammar created from CIM Specification Version 2.2 Appendix A */

  /*
     Note the following implementation details:

       1. The MOF specification has a production of type assocDeclaration,
       but an association is just a type of classDeclaration with a few
       special rules.  At least for the first pass, I'm treating an
       associationDeclaration as a classDeclaration and applying its
       syntactical rules outside of the grammar definition.

       2. Same with the indicationDeclaration.  It appears to be a normal
       classDeclaration with the INDICATION qualifier and no special
       syntactical rules.

       3. The Parser uses String objects throughout to represent
       character data.  However, the tokenizer underneath is probably
       working with 8-bit chars.  If we later use an extended character
       compatible tokenizer, I anticipate NO CHANGE to this parser.

       4. Besides the tokenizer, this parser uses 2 sets of outside
       services:
          1)Class valueFactory.  This has a couple of static methods
	  that assist in creating CIMValue objects from Strings.
	  2)Class cimmofParser.  This has a wide variety of methods
	  that fall into these catagories:
            a) Interfaces to the Repository.  You call cimmofParser::
            methods to query and store compiled CIM elements.
	    b) Error handling.
            c) Data format conversions.
            d) Tokenizer manipulation
            e) Pragma handling
            f) Alias Handling
  */


#define YYSTACKSIZE 2000

#include <cstdlib>
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(__OS400__)
#if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#endif
#include <cstdio>
#include <cstring>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMName.h>
#include "cimmofParser.h"
#include "valueFactory.h"
#include "memobjs.h"
#include "qualifierList.h"
#include "objname.h"

/*include any useful debugging stuff here*/

/* Debugging the parser.  Debugging is provided through
   1. debug functions in Bison that are controlled by a compile time
      flag (YYDEBUG) and a runtime flag (yydebug) which is redefined
      to cimmof_debug.
   2. Debug functions defined through YACCTRACE, a macro defined
      in cimmofparser.h and turned on and off manually.
   All debugging must be turned on manually at this point by
   setting the YYDEBUG compile flag and also setting YACCTRACE.
   ATTN: TODO: automate the debug information flags.
*/
/* Enable this define to compie Bison/Yacc tracing*/
/* ATTN: p3 03092003 ks Enabling this flag currently causes a compile error*/

#define YYDEBUG 1
/*static int cimmof_debug;*/

/*extern cimmofParser g_cimmofParser;*/

extern int   cimmof_lex(void);
extern int   cimmof_error(...);
extern char *cimmof_text;
extern void  cimmof_yy_less(int n);
extern int   cimmof_leng;


/* ------------------------------------------------------------------- */
/* These globals provide continuity between various pieces of a        */
/* declaration.  They are usually interpreted as "these modifiers were */
/* encountered and need to be applied to the finished object".  For    */
/* example, qualifiers are accumulated in g_qualifierList[] as they    */
/* encountered, then applied to the production they qualify when it    */
/* is completed.                                                       */
/* ------------------------------------------------------------------- */
  CIMFlavor g_flavor = CIMFlavor (CIMFlavor::NONE);
  CIMScope g_scope = CIMScope ();
  /*ATTN: BB 2001 BB P1 - Fixed size qualifier list max 20. Make larger or var*/
  qualifierList g_qualifierList(20);
  CIMMethod *g_currentMethod = 0;
  CIMClass *g_currentClass = 0;
  CIMInstance *g_currentInstance = 0;
  String g_currentAlias = String::EMPTY;
  CIMName g_referenceClassName = CIMName();
  Array<CIMKeyBinding> g_KeyBindingArray; /* it gets created empty*/
  TYPED_INITIALIZER_VALUE g_typedInitializerValue; 

/* ------------------------------------------------------------------- */
/* Pragmas, except for the Include pragma, are not handled yet    */
/* I don't understand them, so it may be a while before they are       */ 
/* ------------------------------------------------------------------- */
  struct pragma {
    String name;
    String value;
  };

/* ---------------------------------------------------------------- */
/* Use our general wrap manager to handle end-of-file               */
/* ---------------------------------------------------------------- */
extern "C" {
int
cimmof_wrap() {
  return cimmofParser::Instance()->wrapCurrentBuffer();
}
}

/* ---------------------------------------------------------------- */
/* Pass errors to our general log manager.                          */
/* ---------------------------------------------------------------- */
void
cimmof_error(const char *msg) {
  cimmofParser::Instance()->log_parse_error(cimmof_text, msg);
  /* printf("Error: %s\n", msg);*/
}

#line 135 "cimmof.y"
typedef union {
  /*char                     *strval;*/
  CIMClass                 *mofclass;
  CIMFlavor                *flavor;
  CIMInstance              *instance;
  CIMKeyBinding            *keybinding;
  CIMMethod                *method;
  CIMName                  *cimnameval;
  CIMObjectPath            *reference;
  CIMProperty              *property;
  CIMQualifier             *qualifier;
  CIMQualifierDecl         *mofqualifier;
  CIMScope                 *scope;
  CIMType                   datatype;
  CIMValue                 *value;
  int                       ival;
  modelPath                *modelpath;
  String                   *strptr;
  String                   *strval;
  struct pragma            *pragma;
  TYPED_INITIALIZER_VALUE  *typedinitializer;
} YYSTYPE;
#line 198 "y.tab.c"
#define YYERRCODE 256
#define TOK_ALIAS_IDENTIFIER 257
#define TOK_ANY 258
#define TOK_AS 259
#define TOK_ASSOCIATION 260
#define TOK_BINARY_VALUE 261
#define TOK_CHAR_VALUE 262
#define TOK_CLASS 263
#define TOK_COLON 264
#define TOK_COMMA 265
#define TOK_DISABLEOVERRIDE 266
#define TOK_DQUOTE 267
#define TOK_DT_BOOL 268
#define TOK_DT_CHAR16 269
#define TOK_DT_CHAR8 270
#define TOK_DT_DATETIME 271
#define TOK_DT_REAL32 272
#define TOK_DT_REAL64 273
#define TOK_DT_SINT16 274
#define TOK_DT_SINT32 275
#define TOK_DT_SINT64 276
#define TOK_DT_SINT8 277
#define TOK_DT_STR 278
#define TOK_DT_UINT16 279
#define TOK_DT_UINT32 280
#define TOK_DT_UINT64 281
#define TOK_DT_UINT8 282
#define TOK_ENABLEOVERRIDE 283
#define TOK_END_OF_FILE 284
#define TOK_EQUAL 285
#define TOK_FALSE 286
#define TOK_FLAVOR 287
#define TOK_HEX_VALUE 288
#define TOK_INCLUDE 289
#define TOK_INDICATION 290
#define TOK_INSTANCE 291
#define TOK_LEFTCURLYBRACE 292
#define TOK_LEFTPAREN 293
#define TOK_LEFTSQUAREBRACKET 294
#define TOK_METHOD 295
#define TOK_NULL_VALUE 296
#define TOK_OCTAL_VALUE 297
#define TOK_OF 298
#define TOK_PARAMETER 299
#define TOK_PERIOD 300
#define TOK_POSITIVE_DECIMAL_VALUE 301
#define TOK_PRAGMA 302
#define TOK_PROPERTY 303
#define TOK_QUALIFIER 304
#define TOK_REAL_VALUE 305
#define TOK_REF 306
#define TOK_REFERENCE 307
#define TOK_RESTRICTED 308
#define TOK_RIGHTCURLYBRACE 309
#define TOK_RIGHTPAREN 310
#define TOK_RIGHTSQUAREBRACKET 311
#define TOK_SCHEMA 312
#define TOK_SCOPE 313
#define TOK_SEMICOLON 314
#define TOK_SIGNED_DECIMAL_VALUE 315
#define TOK_SIMPLE_IDENTIFIER 316
#define TOK_STRING_VALUE 317
#define TOK_TOSUBCLASS 318
#define TOK_TRANSLATABLE 319
#define TOK_TRUE 320
#define TOK_UNEXPECTED_CHAR 321
const short cimmof_lhs[] = {                                        -1,
    0,   60,   60,   61,   61,   61,   61,   23,   22,    4,
    5,    5,   63,   63,   65,   65,   66,   66,   66,   20,
   19,   67,   68,    3,   69,   69,   69,   70,    9,    9,
   27,   26,   71,   28,   36,   37,   38,   38,   10,    2,
    1,   15,   15,   15,   57,   57,   52,   52,   52,   56,
   56,   56,   56,   34,   34,   53,   53,   42,   42,   42,
   42,   42,   39,   39,   39,   39,   39,   16,   16,   51,
   51,   50,   33,   33,   47,   47,   49,   48,   41,   41,
   40,   21,   17,   17,   18,   45,   54,   54,   55,   14,
   13,   72,   73,   73,   74,   62,   62,   75,   35,   25,
   24,   59,   30,   76,   31,   31,   32,   32,   32,   32,
   32,   32,   32,   32,   32,   12,   12,   77,   78,   78,
   79,   79,   79,   79,   79,   11,   11,   80,   80,    6,
    6,    6,    6,    6,    7,    7,    7,    7,    7,    7,
    7,    7,    7,    8,    8,   64,   64,   81,   82,   82,
   29,   46,   46,   58,   58,   58,   58,   43,   44,
};
const short cimmof_len[] = {                                         2,
    1,    2,    0,    1,    1,    1,    1,    2,    5,    1,
    2,    0,    4,    3,    1,    2,    1,    1,    1,    4,
    2,    3,    1,    1,    1,    3,    0,    4,    1,    1,
    3,    4,    1,    6,    1,    1,    2,    0,    2,    1,
    1,    3,    2,    0,    2,    0,    1,    1,    1,    1,
    1,    1,    1,    1,    3,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    2,    1,    3,    2,    1,    1,    4,    1,    2,    0,
    1,    3,    1,    3,    3,    1,    2,    0,    1,    2,
    5,    4,    1,    2,    5,    1,    1,    5,    1,    5,
    6,    4,    3,    3,    1,    3,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    4,    0,    2,    1,    3,
    1,    1,    1,    1,    1,    1,    0,    1,    2,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    3,    0,    1,    1,    3,
    3,    1,    1,    3,    3,    1,    0,    1,    1,
};
const short cimmof_defred[] = {                                      0,
  148,    0,    0,    0,    0,    7,    0,    6,    5,   97,
    1,    0,    4,    0,   96,    0,    0,  158,    0,  115,
  109,  107,  110,  113,  114,  111,  112,  108,  152,  153,
    0,    0,   90,    0,    8,    2,    0,    0,  149,    0,
    0,    0,    0,    0,    0,    0,    0,   93,    0,   18,
   17,   19,    0,    0,   15,   10,    0,    0,    0,    0,
  156,    0,    0,  146,   72,    0,   99,  159,    0,  133,
  143,  134,  144,  145,  138,  140,  142,  136,  132,  137,
  139,  141,  135,    0,  130,  131,    0,    0,    0,    0,
    0,   94,   14,   35,    0,    0,    0,    0,    0,   16,
    0,    0,    0,   67,   60,   68,   66,   57,   65,   63,
   59,   74,   64,   69,   62,    0,   58,   56,   70,    0,
   54,    0,    0,  122,  121,  123,  124,  125,  151,  128,
    0,  150,   98,  100,    0,    0,    0,    0,    0,    0,
  105,    0,   92,    0,    0,   21,    0,    0,   33,   31,
    0,   13,   89,   87,    0,    9,   91,    0,   73,   71,
  155,  154,  129,    0,   43,    0,  102,  104,    0,    0,
  101,    0,  103,    0,   51,   52,   50,   53,   76,   75,
   78,    0,    0,    0,    0,   25,   23,   20,   36,    0,
   11,   55,   42,   45,  118,    0,  119,  106,    0,    0,
   81,   95,   32,    0,   29,    0,   30,    0,   22,    0,
    0,    0,  116,   79,    0,    0,   39,   40,    0,   26,
   37,   34,  120,    0,   77,   28,   86,    0,   83,    0,
    0,    0,   84,   48,   49,   85,   47,
};
const short cimmof_dgoto[] = {                                       4,
  145,  219,  146,   57,  156,   84,   85,   86,  206,  207,
  129,  139,    5,    6,  136,  115,  228,  229,   96,   50,
  216,    7,    8,    9,   10,   97,   51,   52,   39,   88,
  140,   30,  176,  116,   66,   98,  190,  211,  117,  199,
  200,  118,   19,   69,  230,   40,  178,  179,  180,  119,
  120,  236,  121,  102,  181,  182,  167,   62,   45,   11,
   12,   13,   35,   14,   54,   55,  148,  188,  185,  186,
  150,   33,   47,   48,   15,   89,  170,  196,  130,  131,
   16,   41,
};
const short cimmof_sindex[] = {                                   -142,
    0, -266,  -78,    0, -264,    0, -260,    0,    0,    0,
    0, -142,    0, -150,    0,  -78, -253,    0, -244,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -198, -232,    0, -280,    0,    0, -263, -205,    0, -262,
 -252, -230, -212,   58, -161, -207, -276,    0, -194,    0,
    0,    0,   -3, -203,    0,    0, -143, -263, -167, -129,
    0, -247,  -78,    0,    0, -188,    0,    0, -185,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -168,    0,    0, -186, -137,  -64, -149,
 -171,    0,    0,    0, -165, -148, -160, -157, -159,    0,
 -101, -106, -143,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -226,    0,    0,    0, -230,
    0, -145, -141,    0,    0,    0,    0,    0,    0,    0,
 -247,    0,    0,    0, -274, -115, -132, -114, -151, -240,
    0, -241,    0,    0, -168,    0, -232, -133,    0,    0,
 -139,    0,    0,    0, -263,    0,    0, -122,    0,    0,
    0,    0,    0, -127,    0, -241,    0,    0, -104, -247,
    0,  -64,    0, -230,    0,    0,    0,    0,    0,    0,
    0, -117, -115,   27, -221,    0,    0,    0,    0,  -98,
    0,    0,    0,    0,    0, -213,    0,    0,  -74, -263,
    0,    0,    0, -105,    0, -116,    0, -232,    0, -230,
 -112, -247,    0,    0,  -97,  -63,    0,    0, -168,    0,
    0,    0,    0, -111,    0,    0,    0,  -59,    0,  -77,
 -111, -219,    0,    0,    0,    0,    0,
};
const short cimmof_rindex[] = {                                      1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -109,    0,   43,    0,    0,    0,    0,    0, -208,
    0,    0,    0,    0,    0,    0, -109,    0,    0,    0,
    0,    0,    0,   43,    0,    0, -146,    0,    0,    0,
    0, -243,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -250,    0,    0,    0, -103,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -83,  -82,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -202,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -231,    0,    0,    0,    0,  -51,    0,    0,    0,    0,
    0,    0,    0, -170, -273,    0,  -19,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -100,    0,    0,    0,    0,    0,    0,
    0,    0,  -99,    0,    0,    0,    0,    0,    0,  -96,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   43,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -211,    0,
    0,    0,    0,    0,    0,    0,    0,  -54,    0,    0,
    0,    0,    0,    0,    0,    0,    0,
};
const short cimmof_gindex[] = {                                      0,
    0,    0,    0,  -53,    0,  -47,    0,    0,    0,    0,
    0,    0,    0,    0, -135,    0,    0,  -12,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  157,    0,
    0,  -80,  -40,    0,    0,    0,    0,    0,    0,    0,
    0,  -52,    0,    0,    0,  219,   -9,    0,    0,  -39,
    0,    0, -147,  121,  126,   62,   47,    0,    0,  220,
    0,    0,    0,  -30,    0,  179,    0,    0,    0,   28,
    0,    0,    0,  190,    0,    0,    0,    0, -124,    0,
    0,    0,
};
#define YYTABLESIZE 359
const short cimmof_table[] = {                                      61,
    3,   46,   67,   53,  103,   95,  163,  123,  141,  183,
  192,   44,   63,    1,   44,  153,   46,    1,  124,  104,
  105,  127,   17,   53,  172,  174,  164,   32,   49,   59,
   60,   34,   91,  126,   44,  125,  165,  153,  158,   42,
   44,  104,  105,  208,  106,  197,  107,  174,   43,   18,
   59,  212,   56,   44,  175,  109,  157,  157,   64,  110,
  126,    1,   61,  111,   61,   44,  106,  127,  107,  173,
  127,  128,   59,  113,  157,   65,  108,  109,  114,  126,
  160,  110,  159,  226,  237,  111,   65,  223,  209,  177,
    1,  198,   58,  104,  105,  113,  213,   65,   44,  157,
  114,  191,  157,   87,   68,   99,   61,   61,   90,  157,
  157,   61,   37,  177,   41,  101,  184,   88,  106,   93,
  107,  133,   24,   41,  134,  135,  137,  138,  108,  109,
  204,  104,  105,  110,  201,  142,  205,  111,  104,  105,
   38,  112,  143,   41,  147,   88,  215,  113,  151,   65,
  144,    1,  114,  149,  152,  153,  106,  155,  107,    2,
  168,    3,  171,  106,  161,  107,  122,  109,  162,  166,
  221,  110,  169,  108,  109,  111,  189,  184,  110,   20,
  187,   21,  111,  193,   22,  113,  210,   65,  195,  214,
  114,  234,  113,   20,   65,   21,  202,  114,   22,  218,
  217,  222,  224,  225,  227,  231,  147,  232,   12,   88,
  117,   23,   82,   46,   46,   80,   24,   38,  233,  132,
   25,   31,  235,  157,   26,   23,  154,  194,   27,  203,
   24,   36,  100,   28,   25,  220,   92,   29,   26,    0,
    0,    0,   27,    0,    0,   27,    0,   28,  147,  147,
    0,  147,  147,  147,  147,  147,  147,  147,  147,  147,
  147,  147,  147,  147,   70,   71,    0,   72,   73,   74,
   75,   76,   77,   78,   79,   80,   81,   82,   83,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   27,  147,    0,    0,   70,   71,  147,   72,   73,   74,
   75,   76,   77,   78,   79,   80,   81,   82,   83,    0,
  147,  147,   94,  147,  147,  147,  147,  147,  147,  147,
  147,  147,  147,  147,  147,   70,   71,    0,   72,   73,
   74,   75,   76,   77,   78,   79,   80,   81,   82,   83,
    0,    0,   56,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  147,
};
const short cimmof_check[] = {                                      40,
    0,   32,   42,   34,   58,   53,  131,   60,   89,  145,
  158,  285,  265,  294,  265,  257,   47,  294,  266,  261,
  262,  265,  289,   54,  265,  267,  301,  292,  309,  292,
  293,  292,  309,  265,  285,  283,  311,  257,  265,  293,
  314,  261,  262,  265,  286,  170,  288,  267,  293,  316,
  292,  265,  316,  265,  296,  297,  265,  266,  311,  301,
  308,  294,  265,  305,  267,  264,  286,  311,  288,  310,
  318,  319,  292,  315,  283,  317,  296,  297,  320,  311,
  120,  301,  309,  219,  232,  305,  317,  212,  310,  142,
  294,  172,  298,  261,  262,  315,  310,  317,  310,  308,
  320,  155,  311,  265,  317,  309,  309,  310,  316,  318,
  319,  314,  263,  166,  285,  259,  147,  264,  286,  314,
  288,  310,  293,  294,  310,  294,  313,  265,  296,  297,
  184,  261,  262,  301,  174,  285,  184,  305,  261,  262,
  291,  309,  314,  314,  293,  292,  200,  315,  306,  317,
  316,  294,  320,  314,  314,  257,  286,  264,  288,  302,
  293,  304,  314,  286,  310,  288,  296,  297,  310,  285,
  210,  301,  287,  296,  297,  305,  316,  208,  301,  258,
  314,  260,  305,  311,  263,  315,  285,  317,  293,  264,
  320,  232,  315,  258,  317,  260,  314,  320,  263,  316,
  306,  314,  300,  267,  316,  265,  316,  285,  292,  292,
  314,  290,  267,  265,  314,  316,  295,  314,  231,   63,
  299,    3,  232,  103,  303,  290,  101,  166,  307,  183,
  295,   12,   54,  312,  299,  208,   47,  316,  303,   -1,
   -1,   -1,  307,   -1,   -1,  265,   -1,  312,  268,  269,
   -1,  271,  272,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,  263,  268,  269,   -1,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  310,  291,   -1,   -1,  268,  269,  316,  271,  272,  273,
  274,  275,  276,  277,  278,  279,  280,  281,  282,   -1,
  268,  269,  316,  271,  272,  273,  274,  275,  276,  277,
  278,  279,  280,  281,  282,  268,  269,   -1,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  280,  281,  282,
   -1,   -1,  316,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  316,
};
#define YYFINAL 4
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 321
#if YYDEBUG
const char * const cimmof_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOK_ALIAS_IDENTIFIER","TOK_ANY",
"TOK_AS","TOK_ASSOCIATION","TOK_BINARY_VALUE","TOK_CHAR_VALUE","TOK_CLASS",
"TOK_COLON","TOK_COMMA","TOK_DISABLEOVERRIDE","TOK_DQUOTE","TOK_DT_BOOL",
"TOK_DT_CHAR16","TOK_DT_CHAR8","TOK_DT_DATETIME","TOK_DT_REAL32",
"TOK_DT_REAL64","TOK_DT_SINT16","TOK_DT_SINT32","TOK_DT_SINT64","TOK_DT_SINT8",
"TOK_DT_STR","TOK_DT_UINT16","TOK_DT_UINT32","TOK_DT_UINT64","TOK_DT_UINT8",
"TOK_ENABLEOVERRIDE","TOK_END_OF_FILE","TOK_EQUAL","TOK_FALSE","TOK_FLAVOR",
"TOK_HEX_VALUE","TOK_INCLUDE","TOK_INDICATION","TOK_INSTANCE",
"TOK_LEFTCURLYBRACE","TOK_LEFTPAREN","TOK_LEFTSQUAREBRACKET","TOK_METHOD",
"TOK_NULL_VALUE","TOK_OCTAL_VALUE","TOK_OF","TOK_PARAMETER","TOK_PERIOD",
"TOK_POSITIVE_DECIMAL_VALUE","TOK_PRAGMA","TOK_PROPERTY","TOK_QUALIFIER",
"TOK_REAL_VALUE","TOK_REF","TOK_REFERENCE","TOK_RESTRICTED",
"TOK_RIGHTCURLYBRACE","TOK_RIGHTPAREN","TOK_RIGHTSQUAREBRACKET","TOK_SCHEMA",
"TOK_SCOPE","TOK_SEMICOLON","TOK_SIGNED_DECIMAL_VALUE","TOK_SIMPLE_IDENTIFIER",
"TOK_STRING_VALUE","TOK_TOSUBCLASS","TOK_TRANSLATABLE","TOK_TRUE",
"TOK_UNEXPECTED_CHAR",
};
const char * const cimmof_rule[] = {
"$accept : mofSpec",
"mofSpec : mofProductions",
"mofProductions : mofProduction mofProductions",
"mofProductions :",
"mofProduction : compilerDirective",
"mofProduction : qualifierDeclaration",
"mofProduction : classDeclaration",
"mofProduction : instanceDeclaration",
"classDeclaration : classHead classBody",
"classHead : qualifierList TOK_CLASS className alias superClass",
"className : TOK_SIMPLE_IDENTIFIER",
"superClass : TOK_COLON className",
"superClass :",
"classBody : TOK_LEFTCURLYBRACE classFeatures TOK_RIGHTCURLYBRACE TOK_SEMICOLON",
"classBody : TOK_LEFTCURLYBRACE TOK_RIGHTCURLYBRACE TOK_SEMICOLON",
"classFeatures : classFeature",
"classFeatures : classFeatures classFeature",
"classFeature : propertyDeclaration",
"classFeature : methodDeclaration",
"classFeature : referenceDeclaration",
"methodDeclaration : qualifierList methodHead methodBody methodEnd",
"methodHead : dataType methodName",
"methodBody : TOK_LEFTPAREN parameters TOK_RIGHTPAREN",
"methodEnd : TOK_SEMICOLON",
"methodName : TOK_SIMPLE_IDENTIFIER",
"parameters : parameter",
"parameters : parameters TOK_COMMA parameter",
"parameters :",
"parameter : qualifierList parameterType parameterName array",
"parameterType : dataType",
"parameterType : objectRef",
"propertyDeclaration : qualifierList propertyBody propertyEnd",
"propertyBody : dataType propertyName array typedDefaultValue",
"propertyEnd : TOK_SEMICOLON",
"referenceDeclaration : qualifierList referencedObject TOK_REF referenceName referencePath TOK_SEMICOLON",
"referencedObject : TOK_SIMPLE_IDENTIFIER",
"referenceName : TOK_SIMPLE_IDENTIFIER",
"referencePath : TOK_EQUAL stringValue",
"referencePath :",
"objectRef : className TOK_REF",
"parameterName : TOK_SIMPLE_IDENTIFIER",
"propertyName : TOK_SIMPLE_IDENTIFIER",
"array : TOK_LEFTSQUAREBRACKET TOK_POSITIVE_DECIMAL_VALUE TOK_RIGHTSQUAREBRACKET",
"array : TOK_LEFTSQUAREBRACKET TOK_RIGHTSQUAREBRACKET",
"array :",
"typedDefaultValue : TOK_EQUAL typedInitializer",
"typedDefaultValue :",
"initializer : constantValue",
"initializer : arrayInitializer",
"initializer : referenceInitializer",
"typedInitializer : nonNullConstantValue",
"typedInitializer : TOK_NULL_VALUE",
"typedInitializer : arrayInitializer",
"typedInitializer : referenceInitializer",
"constantValues : constantValue",
"constantValues : constantValues TOK_COMMA constantValue",
"constantValue : nonNullConstantValue",
"constantValue : TOK_NULL_VALUE",
"nonNullConstantValue : integerValue",
"nonNullConstantValue : TOK_REAL_VALUE",
"nonNullConstantValue : TOK_CHAR_VALUE",
"nonNullConstantValue : stringValues",
"nonNullConstantValue : booleanValue",
"integerValue : TOK_POSITIVE_DECIMAL_VALUE",
"integerValue : TOK_SIGNED_DECIMAL_VALUE",
"integerValue : TOK_OCTAL_VALUE",
"integerValue : TOK_HEX_VALUE",
"integerValue : TOK_BINARY_VALUE",
"booleanValue : TOK_FALSE",
"booleanValue : TOK_TRUE",
"stringValues : stringValue",
"stringValues : stringValues stringValue",
"stringValue : TOK_STRING_VALUE",
"arrayInitializer : TOK_LEFTCURLYBRACE constantValues TOK_RIGHTCURLYBRACE",
"arrayInitializer : TOK_LEFTCURLYBRACE TOK_RIGHTCURLYBRACE",
"referenceInitializer : objectHandle",
"referenceInitializer : aliasInitializer",
"objectHandle : TOK_DQUOTE namespaceHandleRef modelPath TOK_DQUOTE",
"aliasInitializer : aliasIdentifier",
"namespaceHandleRef : namespaceHandle TOK_COLON",
"namespaceHandleRef :",
"namespaceHandle : stringValue",
"modelPath : className TOK_PERIOD keyValuePairList",
"keyValuePairList : keyValuePair",
"keyValuePairList : keyValuePairList TOK_COMMA keyValuePair",
"keyValuePair : keyValuePairName TOK_EQUAL initializer",
"keyValuePairName : TOK_SIMPLE_IDENTIFIER",
"alias : TOK_AS aliasIdentifier",
"alias :",
"aliasIdentifier : TOK_ALIAS_IDENTIFIER",
"instanceDeclaration : instanceHead instanceBody",
"instanceHead : qualifierList TOK_INSTANCE TOK_OF className alias",
"instanceBody : TOK_LEFTCURLYBRACE valueInitializers TOK_RIGHTCURLYBRACE TOK_SEMICOLON",
"valueInitializers : valueInitializer",
"valueInitializers : valueInitializers valueInitializer",
"valueInitializer : qualifierList TOK_SIMPLE_IDENTIFIER TOK_EQUAL typedInitializer TOK_SEMICOLON",
"compilerDirective : compilerDirectiveInclude",
"compilerDirective : compilerDirectivePragma",
"compilerDirectiveInclude : TOK_PRAGMA TOK_INCLUDE TOK_LEFTPAREN fileName TOK_RIGHTPAREN",
"fileName : stringValue",
"compilerDirectivePragma : TOK_PRAGMA pragmaName TOK_LEFTPAREN pragmaVal TOK_RIGHTPAREN",
"qualifierDeclaration : TOK_QUALIFIER qualifierName qualifierValue scope defaultFlavor TOK_SEMICOLON",
"qualifierValue : TOK_COLON dataType array typedDefaultValue",
"scope : scope_begin metaElements TOK_RIGHTPAREN",
"scope_begin : TOK_COMMA TOK_SCOPE TOK_LEFTPAREN",
"metaElements : metaElement",
"metaElements : metaElements TOK_COMMA metaElement",
"metaElement : TOK_CLASS",
"metaElement : TOK_SCHEMA",
"metaElement : TOK_ASSOCIATION",
"metaElement : TOK_INDICATION",
"metaElement : TOK_PROPERTY",
"metaElement : TOK_REFERENCE",
"metaElement : TOK_METHOD",
"metaElement : TOK_PARAMETER",
"metaElement : TOK_ANY",
"defaultFlavor : TOK_COMMA flavorHead explicitFlavors TOK_RIGHTPAREN",
"defaultFlavor :",
"flavorHead : TOK_FLAVOR TOK_LEFTPAREN",
"explicitFlavors : explicitFlavor",
"explicitFlavors : explicitFlavors TOK_COMMA explicitFlavor",
"explicitFlavor : TOK_ENABLEOVERRIDE",
"explicitFlavor : TOK_DISABLEOVERRIDE",
"explicitFlavor : TOK_RESTRICTED",
"explicitFlavor : TOK_TOSUBCLASS",
"explicitFlavor : TOK_TRANSLATABLE",
"flavor : overrideFlavors",
"flavor :",
"overrideFlavors : explicitFlavor",
"overrideFlavors : overrideFlavors explicitFlavor",
"dataType : intDataType",
"dataType : realDataType",
"dataType : TOK_DT_STR",
"dataType : TOK_DT_BOOL",
"dataType : TOK_DT_DATETIME",
"intDataType : TOK_DT_UINT8",
"intDataType : TOK_DT_SINT8",
"intDataType : TOK_DT_UINT16",
"intDataType : TOK_DT_SINT16",
"intDataType : TOK_DT_UINT32",
"intDataType : TOK_DT_SINT32",
"intDataType : TOK_DT_UINT64",
"intDataType : TOK_DT_SINT64",
"intDataType : TOK_DT_CHAR16",
"realDataType : TOK_DT_REAL32",
"realDataType : TOK_DT_REAL64",
"qualifierList : qualifierListBegin qualifiers TOK_RIGHTSQUAREBRACKET",
"qualifierList :",
"qualifierListBegin : TOK_LEFTSQUAREBRACKET",
"qualifiers : qualifier",
"qualifiers : qualifiers TOK_COMMA qualifier",
"qualifier : qualifierName typedQualifierParameter flavor",
"qualifierName : TOK_SIMPLE_IDENTIFIER",
"qualifierName : metaElement",
"typedQualifierParameter : TOK_LEFTPAREN nonNullConstantValue TOK_RIGHTPAREN",
"typedQualifierParameter : TOK_LEFTPAREN TOK_NULL_VALUE TOK_RIGHTPAREN",
"typedQualifierParameter : arrayInitializer",
"typedQualifierParameter :",
"pragmaName : TOK_SIMPLE_IDENTIFIER",
"pragmaVal : TOK_STRING_VALUE",
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
case 4:
#line 284 "cimmof.y"
{ /* FIXME: Where do we put directives? */ }
break;
case 5:
#line 286 "cimmof.y"
{ cimmofParser::Instance()->addQualifier(yyvsp[0].mofqualifier); delete yyvsp[0].mofqualifier; }
break;
case 6:
#line 288 "cimmof.y"
{ cimmofParser::Instance()->addClass(yyvsp[0].mofclass); }
break;
case 7:
#line 290 "cimmof.y"
{ cimmofParser::Instance()->addInstance(yyvsp[0].instance); }
break;
case 8:
#line 303 "cimmof.y"
{
    YACCTRACE("classDeclaration");
    if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addClassAlias(g_currentAlias, yyval.mofclass, false);
}
break;
case 9:
#line 311 "cimmof.y"
{
    /* create new instance of class with className and superclassName*/
    /* put returned class object on stack*/
    YACCTRACE("classHead:");
    yyval.mofclass = cimmofParser::Instance()->newClassDecl(*yyvsp[-2].cimnameval, *yyvsp[0].cimnameval);
    
    /* put list of qualifiers into class*/
    applyQualifierList(&g_qualifierList, yyval.mofclass);
    
    g_currentAlias = *yyvsp[-1].strval;
    if (g_currentClass)
        delete g_currentClass;
    g_currentClass = yyval.mofclass;
    delete yyvsp[-2].cimnameval;
    delete yyvsp[-1].strval;
    delete yyvsp[0].cimnameval;
}
break;
case 10:
#line 330 "cimmof.y"
{}
break;
case 11:
#line 333 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].cimnameval); }
break;
case 12:
#line 334 "cimmof.y"
{ yyval.cimnameval = new CIMName(); }
break;
case 17:
#line 345 "cimmof.y"
{
  YACCTRACE("classFeature:applyProperty");
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; }
break;
case 18:
#line 348 "cimmof.y"
{
  YACCTRACE("classFeature:applyMethod");
  cimmofParser::Instance()->applyMethod(*g_currentClass, *yyvsp[0].method); }
break;
case 19:
#line 351 "cimmof.y"
{
  YACCTRACE("classFeature:applyProperty");
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; }
break;
case 20:
#line 369 "cimmof.y"
{
  YACCTRACE("methodDeclaration");
  yyval.method = yyvsp[-2].method;
}
break;
case 21:
#line 380 "cimmof.y"
{
    YACCTRACE("methodHead");
    if (g_currentMethod)
    delete g_currentMethod;

  /* create new method instance with pointer to method name and datatype*/
  g_currentMethod = cimmofParser::Instance()->newMethod(*yyvsp[0].cimnameval, yyvsp[-1].datatype) ;
  
  /* put new method on stack*/
  yyval.method = g_currentMethod;

  /* apply the method qualifier list.*/
  applyQualifierList(&g_qualifierList, yyval.method);

  delete yyvsp[0].cimnameval;
}
break;
case 24:
#line 404 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 28:
#line 416 "cimmof.y"
{ 
  /* ATTN: P2 2002 Question Need to create default value including type?*/
  
  YACCTRACE("parameter:");
  CIMParameter *p = 0;
  cimmofParser *cp = cimmofParser::Instance();

  /* Create new parameter with name, type, isArray, array, referenceClassName*/
  if (yyvsp[0].ival == -1) {
    p = cp->newParameter(*yyvsp[-1].cimnameval, yyvsp[-2].datatype, false, 0, g_referenceClassName);
  } else {
    p = cp->newParameter(*yyvsp[-1].cimnameval, yyvsp[-2].datatype, true, yyvsp[0].ival, g_referenceClassName);
  }

  g_referenceClassName = CIMName();

  YACCTRACE("parameter:applyQualifierList");
  applyQualifierList(&g_qualifierList, p);

  cp->applyParameter(*g_currentMethod, *p);
  delete p;
  delete yyvsp[-1].cimnameval;
}
break;
case 29:
#line 441 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 30:
#line 442 "cimmof.y"
{ yyval.datatype = CIMTYPE_REFERENCE; }
break;
case 31:
#line 458 "cimmof.y"
{
    /* set body to stack and apply qualifier list*/
    /* ATTN: the apply qualifer only works here because*/
    /* there are not lower level qualifiers.  We do productions*/
    /* that might have lower level qualifiers differently by*/
    /* setting up a xxxHead production where qualifiers are */
    /* applied.*/
    YACCTRACE("propertyDeclaration:");
    yyval.property = yyvsp[-1].property;
    applyQualifierList(&g_qualifierList, yyval.property);
}
break;
case 32:
#line 472 "cimmof.y"
{
  CIMValue *v = valueFactory::createValue(yyvsp[-3].datatype, yyvsp[-1].ival, 
                      (yyvsp[0].typedinitializer->type == CIMMOF_NULL_VALUE), yyvsp[0].typedinitializer->value);
  if (yyvsp[-1].ival == -1) {
    yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].cimnameval, *v, false, 0);
} else {                                           
    yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].cimnameval, *v, true, yyvsp[-1].ival);
  }

  delete yyvsp[-2].cimnameval;
  delete yyvsp[0].typedinitializer->value;
  delete v;
}
break;
case 34:
#line 504 "cimmof.y"
{
  String s(*yyvsp[-4].strval);
  if (!String::equal(*yyvsp[-1].strval, String::EMPTY))
    s.append("." + *yyvsp[-1].strval);
  CIMValue *v = valueFactory::createValue(CIMTYPE_REFERENCE, -1, true, &s);
  /*KS add the isArray and arraysize parameters. 8 mar 2002*/
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v, false,0, *yyvsp[-4].strval);
  applyQualifierList(&g_qualifierList, yyval.property);
  delete yyvsp[-4].strval;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval; 
  delete v;
}
break;
case 35:
#line 519 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 36:
#line 522 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 37:
#line 525 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 38:
#line 526 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 39:
#line 529 "cimmof.y"
{  
                          g_referenceClassName = *yyvsp[-1].cimnameval; }
break;
case 40:
#line 533 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 41:
#line 536 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 42:
#line 541 "cimmof.y"
{ yyval.ival = valueFactory::Stoi(*yyvsp[-1].strval);
		   delete yyvsp[-1].strval;
                 }
break;
case 43:
#line 544 "cimmof.y"
{ yyval.ival = 0; }
break;
case 44:
#line 545 "cimmof.y"
{ yyval.ival = -1; }
break;
case 45:
#line 548 "cimmof.y"
{ yyval.typedinitializer = yyvsp[0].typedinitializer; }
break;
case 46:
#line 549 "cimmof.y"
{   /* empty */
                  g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                  g_typedInitializerValue.value = new String(String::EMPTY); 
                  yyval.typedinitializer = &g_typedInitializerValue;
              }
break;
case 47:
#line 556 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 48:
#line 557 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 49:
#line 558 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 50:
#line 565 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 51:
#line 571 "cimmof.y"
{
           g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
           g_typedInitializerValue.value = new String(String::EMPTY); 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 52:
#line 577 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 53:
#line 583 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_REFERENCE_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 54:
#line 592 "cimmof.y"
{ 
            *yyval.strval = valueFactory::stringWComma(String(*yyvsp[0].strval)); }
break;
case 55:
#line 595 "cimmof.y"
{
                YACCTRACE("constantValues:1, Value= " << *yyvsp[0].strval);
                (*yyval.strval).append(","); 
                /*(*$$).append(*$3);*/
                (*yyval.strval).append(valueFactory::stringWComma(String(*yyvsp[0].strval)));
                delete yyvsp[0].strval;
              }
break;
case 56:
#line 607 "cimmof.y"
{yyval.strval = yyvsp[0].strval;}
break;
case 57:
#line 608 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 58:
#line 611 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 59:
#line 612 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 60:
#line 613 "cimmof.y"
{ yyval.strval =  yyvsp[0].strval; }
break;
case 61:
#line 614 "cimmof.y"
{ }
break;
case 62:
#line 615 "cimmof.y"
{ yyval.strval = new String(yyvsp[0].ival ? "T" : "F"); }
break;
case 65:
#line 620 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->oct_to_dec(*yyvsp[0].strval));
                   delete yyvsp[0].strval; }
break;
case 66:
#line 623 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->hex_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; }
break;
case 67:
#line 626 "cimmof.y"
{
                 yyval.strval = new String(cimmofParser::Instance()->binary_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; }
break;
case 68:
#line 631 "cimmof.y"
{ yyval.ival = 0; }
break;
case 69:
#line 632 "cimmof.y"
{ yyval.ival = 1; }
break;
case 70:
#line 635 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 71:
#line 637 "cimmof.y"
{ 
                (*yyval.strval).append(*yyvsp[0].strval);  delete yyvsp[0].strval;
              }
break;
case 72:
#line 643 "cimmof.y"
{ 
   /*String oldrep = *$1;*/
   /*String s(oldrep), s1(String::EMPTY);*/
   /* Handle quoted quote*/
   /*int len = s.size();*/
   /*if (s[len] == '\n') {*/
       /*error: new line inside a string constant unless it is quoted*/
       /*if (s[len - 2] == '\\') {*/
           /*if (len > 3)*/
	        /*s1 = s.subString(1, len-3);*/
       /*} else {*/
           /*cimmof_error("New line in string constant");*/
           /*}*/
       /*cimmofParser::Instance()->increment_lineno();*/
   /*} else { // Can only be a quotation mark*/
       /*if (s[len - 2] == '\\') {  // if it is quoted*/
           /*if (len > 3) s1 = s.subString(1, len-3);*/
           /*s1.append('\"');*/
           /*cimmof_yy_less(len-1);*/
       /*} else { // This is the normal case:  real quotes on both end*/
           /*s1 = s.subString(1, len - 2) ;*/
           /*}*/
       /*}*/
   /*delete $1;*/
   yyval.strval = /*new String(s1);*/
        new String(*yyvsp[0].strval);
   delete yyvsp[0].strval;
}
break;
case 73:
#line 675 "cimmof.y"
{ yyval.strval = yyvsp[-1].strval; }
break;
case 74:
#line 677 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 75:
#line 680 "cimmof.y"
{}
break;
case 76:
#line 681 "cimmof.y"
{  }
break;
case 77:
#line 685 "cimmof.y"
{ 
  /* The objectName string is decomposed for syntactical purposes */
  /* and reassembled here for later parsing in creation of an objname instance */
  String *s = new String(*yyvsp[-2].strval);
  if (!String::equal(*s, String::EMPTY) && yyvsp[-1].modelpath)
    (*s).append(":");
  if (yyvsp[-1].modelpath) {
    (*s).append(yyvsp[-1].modelpath->Stringrep());
  }
  yyval.strval = s;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].modelpath;
}
break;
case 78:
#line 700 "cimmof.y"
{
        /* convert somehow from alias to a CIM object name*/
        yyerror("'alias' is not yet supported (see bugzilla 14).");
        YYABORT;
        delete yyvsp[0].strval;
        }
break;
case 79:
#line 709 "cimmof.y"
{ }
break;
case 80:
#line 710 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 81:
#line 713 "cimmof.y"
{}
break;
case 82:
#line 716 "cimmof.y"
{
             modelPath *m = new modelPath((*yyvsp[-2].cimnameval).getString(), g_KeyBindingArray);
             g_KeyBindingArray.clear(); 
             delete yyvsp[-2].cimnameval;}
break;
case 83:
#line 722 "cimmof.y"
{ yyval.ival = 0; }
break;
case 84:
#line 723 "cimmof.y"
{ yyval.ival = 0; }
break;
case 85:
#line 727 "cimmof.y"
{
		CIMKeyBinding *kb = new CIMKeyBinding(*yyvsp[-2].strval, *yyvsp[0].strval,
                               modelPath::KeyBindingTypeOf(*yyvsp[0].strval));
		g_KeyBindingArray.append(*kb);
		delete kb;
		delete yyvsp[-2].strval;
	        delete yyvsp[0].strval; }
break;
case 87:
#line 739 "cimmof.y"
{ 
              yyerror("'alias' is not yet supported (see bugzilla 14).");
              YYABORT;
              yyval.strval = yyvsp[0].strval;
              }
break;
case 88:
#line 744 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 90:
#line 763 "cimmof.y"
{ 
  yyval.instance = g_currentInstance; 
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addInstanceAlias(g_currentAlias, yyvsp[-1].instance, true);
}
break;
case 91:
#line 772 "cimmof.y"
{
  if (g_currentInstance)
    delete g_currentInstance;
  g_currentAlias = *yyvsp[0].strval;
  g_currentInstance = cimmofParser::Instance()->newInstance(*yyvsp[-1].cimnameval);
  /* apply the qualifierlist to the current instance*/
  yyval.instance = g_currentInstance;
  applyQualifierList(&g_qualifierList, yyval.instance);
  delete yyvsp[-1].cimnameval;
  delete yyvsp[0].strval;
}
break;
case 95:
#line 801 "cimmof.y"
{
  cimmofParser *cp = cimmofParser::Instance();
  /* ATTN: P1 InstanceUpdate function 2001 BB  Instance update needs work here and CIMOM */
  /* a property.  It must be fixed in the Common code first.*/
  /* What we have to do here is create a CIMProperty  and initialize it with*/
  /* the value provided.  The name of the property is $2 and it belongs*/
  /* to the class whose name is in g_currentInstance->getClassName().*/
  /* The steps are*/
  /*   2. Get  property declaration's value object*/
  CIMProperty *oldprop = cp->PropertyFromInstance(*g_currentInstance,
							*yyvsp[-3].strval);
  CIMValue *oldv = cp->ValueFromProperty(*oldprop);

  /*   3. create the new Value object of the same type*/

  /* We want createValue to interpret a value as an array if is enclosed */
  /* in {}s (e.g., { 2 } or {2, 3, 5}) or it is NULL and the property is */
  /* defined as an array. createValue is responsible for the actual*/
  /* validation.*/

  CIMValue *v = valueFactory::createValue(oldv->getType(),
                 ((yyvsp[-1].typedinitializer->type == CIMMOF_ARRAY_VALUE) |
                  ((yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE) & oldprop->isArray()))?0:-1,
                 (yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE),
                 yyvsp[-1].typedinitializer->value);


  /*   4. create a clone property with the new value*/
  CIMProperty *newprop = cp->copyPropertyWithNewValue(*oldprop, *v);

  /*   5. apply the qualifiers; */
  applyQualifierList(&g_qualifierList, newprop);

  /*   6. and apply the CIMProperty to g_currentInstance.*/
  cp->applyProperty(*g_currentInstance, *newprop);
  delete yyvsp[-3].strval;
  delete yyvsp[-1].typedinitializer->value;
  delete oldprop;
  delete oldv;
  delete v;
  delete newprop;
}
break;
case 96:
#line 857 "cimmof.y"
{
    /*printf("compilerDirectiveInclude "); */
}
break;
case 97:
#line 861 "cimmof.y"
{
    /*printf("compilerDirectivePragma ");*/
}
break;
case 98:
#line 868 "cimmof.y"
{
  cimmofParser::Instance()->enterInlineInclude(*yyvsp[-1].strval); delete yyvsp[-1].strval;
}
break;
case 99:
#line 873 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 100:
#line 878 "cimmof.y"
{ cimmofParser::Instance()->processPragma(*yyvsp[-3].strval, *yyvsp[-1].strval); 
		   delete yyvsp[-3].strval;
		   delete yyvsp[-1].strval;
		   }
break;
case 101:
#line 896 "cimmof.y"
{
/*    CIMQualifierDecl *qd = new CIMQualifierDecl($2, $3, $4, $5);*/
	yyval.mofqualifier = cimmofParser::Instance()->newQualifierDecl(*yyvsp[-4].strval, yyvsp[-3].value, *yyvsp[-2].scope, *yyvsp[-1].flavor);
        delete yyvsp[-4].strval;
	delete yyvsp[-3].value;  /* CIMValue object created in qualifierValue production*/
}
break;
case 102:
#line 905 "cimmof.y"
{
    yyval.value = valueFactory::createValue(yyvsp[-2].datatype, yyvsp[-1].ival, 
                yyvsp[0].typedinitializer->type == CIMMOF_NULL_VALUE, yyvsp[0].typedinitializer->value);
    delete yyvsp[0].typedinitializer->value;
}
break;
case 103:
#line 912 "cimmof.y"
{ yyval.scope = yyvsp[-1].scope; }
break;
case 104:
#line 915 "cimmof.y"
{ 
    g_scope = CIMScope (CIMScope::NONE); }
break;
case 105:
#line 919 "cimmof.y"
{ yyval.scope = yyvsp[0].scope; }
break;
case 106:
#line 921 "cimmof.y"
{ yyval.scope->addScope(*yyvsp[0].scope); }
break;
case 107:
#line 925 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS);        }
break;
case 108:
#line 927 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS); }
break;
case 109:
#line 928 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ASSOCIATION);  }
break;
case 110:
#line 929 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::INDICATION);   }
break;
case 111:
#line 931 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PROPERTY);     }
break;
case 112:
#line 932 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::REFERENCE);    }
break;
case 113:
#line 933 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::METHOD);       }
break;
case 114:
#line 934 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PARAMETER);    }
break;
case 115:
#line 935 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ANY);          }
break;
case 116:
#line 940 "cimmof.y"
{ yyval.flavor = &g_flavor; }
break;
case 117:
#line 941 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); }
break;
case 118:
#line 947 "cimmof.y"
{g_flavor = CIMFlavor (CIMFlavor::NONE);}
break;
case 121:
#line 964 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::ENABLEOVERRIDE); }
break;
case 122:
#line 965 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::DISABLEOVERRIDE); }
break;
case 123:
#line 966 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::RESTRICTED); }
break;
case 124:
#line 967 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TOSUBELEMENTS); }
break;
case 125:
#line 968 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TRANSLATABLE); }
break;
case 126:
#line 971 "cimmof.y"
{ yyval.flavor = &g_flavor; }
break;
case 127:
#line 972 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); }
break;
case 130:
#line 980 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 131:
#line 981 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 132:
#line 982 "cimmof.y"
{ yyval.datatype = CIMTYPE_STRING;   }
break;
case 133:
#line 983 "cimmof.y"
{ yyval.datatype = CIMTYPE_BOOLEAN;  }
break;
case 134:
#line 984 "cimmof.y"
{ yyval.datatype = CIMTYPE_DATETIME; }
break;
case 135:
#line 987 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT8;  }
break;
case 136:
#line 988 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT8;  }
break;
case 137:
#line 989 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT16; }
break;
case 138:
#line 990 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT16; }
break;
case 139:
#line 991 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT32; }
break;
case 140:
#line 992 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT32; }
break;
case 141:
#line 993 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT64; }
break;
case 142:
#line 994 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT64; }
break;
case 143:
#line 995 "cimmof.y"
{ yyval.datatype = CIMTYPE_CHAR16; }
break;
case 144:
#line 998 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL32; }
break;
case 145:
#line 999 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL64; }
break;
case 147:
#line 1012 "cimmof.y"
{ 
                 /*yydebug = 1; stderr = stdout;*/
                 }
break;
case 148:
#line 1018 "cimmof.y"
{ 

    /*yydebug = 1; stderr = stdout;*/
    YACCTRACE("qualifierListbegin");
    g_qualifierList.init(); }
break;
case 149:
#line 1026 "cimmof.y"
{ }
break;
case 150:
#line 1027 "cimmof.y"
{ }
break;
case 151:
#line 1032 "cimmof.y"
{
  cimmofParser *p = cimmofParser::Instance();
  /* The qualifier value can't be set until we know the contents of the*/
  /* QualifierDeclaration.  That's what QualifierValue() does.*/
  CIMValue *v = p->QualifierValue(*yyvsp[-2].strval, 
                  (yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE), *yyvsp[-1].typedinitializer->value); 
  yyval.qualifier = p->newQualifier(*yyvsp[-2].strval, *v, g_flavor);
  g_qualifierList.add(yyval.qualifier);
  delete yyvsp[-2].strval;
  delete yyvsp[-1].typedinitializer->value;
  delete v;
 }
break;
case 152:
#line 1047 "cimmof.y"
{ 
    g_flavor = CIMFlavor (CIMFlavor::NONE); }
break;
case 153:
#line 1049 "cimmof.y"
{ 
                        yyval.strval = new String((*yyvsp[0].scope).toString ());
                        g_flavor = CIMFlavor (CIMFlavor::NONE); }
break;
case 154:
#line 1056 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
                    g_typedInitializerValue.value =  yyvsp[-1].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 155:
#line 1062 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 156:
#line 1068 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
                    g_typedInitializerValue.value =  yyvsp[0].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 157:
#line 1073 "cimmof.y"
{   /* empty */
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 158:
#line 1080 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 159:
#line 1083 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
#line 1733 "y.tab.c"
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
