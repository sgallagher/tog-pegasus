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
#include <malloc.h>
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

/*extern cimmofParser g_cimmofParser;*/

extern int cimmof_lex(void);
extern int cimmof_error(...);
extern char *cimmof_text;
extern void cimmof_yy_less(int n);

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
  /*ATTN: BB 2001 BB P1 - Fixed size qualifier list max 10. Make larger or var*/
  qualifierList g_qualifierList(10);
  CIMMethod *g_currentMethod = 0;
  CIMClass *g_currentClass = 0;
  CIMInstance *g_currentInstance = 0;
  String g_currentAlias = String::EMPTY;
  CIMName g_referenceClassName = CIMName();
  KeyBindingArray g_KeyBindingArray; /* it gets created empty*/
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

  
#line 110 "cimmof.y"
typedef union {
  struct pragma *pragma;
  int              ival;
  /*  char             *strval;*/
  String *         strval;
  CIMName *         cimnameval;
  CIMType        datatype;
  CIMValue *          value;
  String *         strptr;
  CIMQualifier *      qualifier;
  CIMScope *          scope;
  CIMFlavor *         flavor;
  CIMProperty *       property;
  CIMMethod *         method;
  CIMClass *      mofclass;
  CIMQualifierDecl *   mofqualifier;
  CIMInstance *   instance;
  CIMObjectPath *  reference;
  modelPath *     modelpath;
  KeyBinding *    keybinding;
  TYPED_INITIALIZER_VALUE * typedinitializer;
} YYSTYPE;
#line 174 "y.tab.c"
#define YYERRCODE 256
#define TOK_LEFTCURLYBRACE 257
#define TOK_RIGHTCURLYBRACE 258
#define TOK_RIGHTSQUAREBRACKET 259
#define TOK_LEFTSQUAREBRACKET 260
#define TOK_LEFTPAREN 261
#define TOK_RIGHTPAREN 262
#define TOK_COLON 263
#define TOK_SEMICOLON 264
#define TOK_POSITIVE_DECIMAL_VALUE 265
#define TOK_SIGNED_DECIMAL_VALUE 266
#define TOK_EQUAL 267
#define TOK_COMMA 268
#define TOK_CLASS 269
#define TOK_REAL_VALUE 270
#define TOK_CHAR_VALUE 271
#define TOK_STRING_VALUE 272
#define TOK_NULL_VALUE 273
#define TOK_OCTAL_VALUE 274
#define TOK_HEX_VALUE 275
#define TOK_BINARY_VALUE 276
#define TOK_TRUE 277
#define TOK_FALSE 278
#define TOK_DQUOTE 279
#define TOK_PERIOD 280
#define TOK_SIMPLE_IDENTIFIER 281
#define TOK_ALIAS_IDENTIFIER 282
#define TOK_PRAGMA 283
#define TOK_INCLUDE 284
#define TOK_AS 285
#define TOK_INSTANCE 286
#define TOK_OF 287
#define TOK_QUALIFIER 288
#define TOK_SCOPE 289
#define TOK_SCHEMA 290
#define TOK_ASSOCIATION 291
#define TOK_INDICATION 292
#define TOK_PROPERTY 293
#define TOK_REFERENCE 294
#define TOK_METHOD 295
#define TOK_PARAMETER 296
#define TOK_ANY 297
#define TOK_REF 298
#define TOK_FLAVOR 299
#define TOK_ENABLEOVERRIDE 300
#define TOK_DISABLEOVERRIDE 301
#define TOK_RESTRICTED 302
#define TOK_TOSUBCLASS 303
#define TOK_TRANSLATABLE 304
#define TOK_DT_STR 305
#define TOK_DT_BOOL 306
#define TOK_DT_DATETIME 307
#define TOK_DT_UINT8 308
#define TOK_DT_SINT8 309
#define TOK_DT_UINT16 310
#define TOK_DT_SINT16 311
#define TOK_DT_UINT32 312
#define TOK_DT_SINT32 313
#define TOK_DT_UINT64 314
#define TOK_DT_SINT64 315
#define TOK_DT_CHAR8 316
#define TOK_DT_CHAR16 317
#define TOK_DT_REAL32 318
#define TOK_DT_REAL64 319
#define TOK_UNEXPECTED_CHAR 320
#define TOK_END_OF_FILE 321
const short cimmof_lhs[] = {                                        -1,
    0,   60,   60,   61,   61,   61,   61,   56,   55,   27,
   28,   28,   63,   63,   65,   65,   66,   66,   66,   54,
   53,   67,   68,   51,   50,   70,   52,    6,    7,    8,
    8,   26,   69,   69,   69,   71,   46,   46,   47,   25,
   24,   39,   39,   39,   30,   30,   11,   11,   11,   29,
   29,   29,   29,   15,   15,   12,   12,   13,   13,   13,
   13,   13,   18,   18,   18,   18,   18,   40,   40,   10,
   10,    9,   14,   14,   21,   21,   23,   22,   20,   20,
   19,   32,   41,   41,   33,    3,   16,   16,   17,   59,
   58,   72,   73,   73,   74,   62,   62,   75,    5,   42,
   57,   48,   34,   76,   35,   35,   36,   36,   36,   36,
   36,   36,   36,   36,   36,   38,   38,   77,   78,   78,
   79,   79,   79,   79,   79,   37,   37,   80,   80,   43,
   43,   43,   43,   43,   44,   44,   44,   44,   44,   44,
   44,   44,   44,   45,   45,   64,   64,   81,   82,   82,
   49,    4,    4,   31,   31,   31,   31,    1,    2,
};
const short cimmof_len[] = {                                         2,
    1,    2,    0,    1,    1,    1,    1,    2,    5,    1,
    2,    0,    4,    3,    1,    2,    1,    1,    1,    4,
    2,    3,    1,    3,    4,    1,    6,    1,    1,    2,
    0,    1,    1,    3,    0,    4,    1,    1,    2,    1,
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
  148,    0,    0,    0,   97,    0,    6,    5,    0,    7,
    1,    0,    4,    0,   96,    0,  158,    0,    0,  107,
  152,  108,  109,  110,  111,  112,  113,  114,  115,    0,
  153,    0,    8,    0,   90,    2,    0,    0,    0,  149,
    0,    0,    0,    0,    0,    0,   17,   19,   18,    0,
    0,   15,    0,    0,   93,   10,    0,    0,    0,    0,
  156,    0,  146,    0,   72,    0,   99,  159,    0,  132,
  133,  134,  135,  136,  137,  138,  139,  140,  141,  142,
  143,  144,  145,    0,  130,  131,    0,    0,    0,   14,
   28,    0,    0,    0,    0,    0,   16,    0,    0,   94,
    0,    0,    0,   74,   63,   64,   59,   60,   57,   65,
   66,   67,   69,   68,   70,    0,   54,   56,    0,   58,
   62,    0,    0,  121,  122,  123,  124,  125,  151,  128,
    0,  150,   98,  100,    0,    0,    0,    0,    0,    0,
  105,    0,    0,    0,   21,   26,   24,    0,    0,   13,
    0,   92,   89,   87,    0,    9,   91,   71,   73,    0,
  155,  154,  129,   43,    0,    0,  102,  104,    0,    0,
  101,  103,    0,   29,    0,    0,    0,    0,   33,   23,
   20,   51,    0,   50,   52,   78,   53,   76,   75,    0,
   11,   55,   42,   45,  118,    0,  119,  106,    0,    0,
   25,    0,   37,    0,   38,   22,    0,   81,    0,    0,
   95,  116,    0,   30,   27,   39,   40,    0,   34,   79,
    0,    0,  120,   36,    0,   77,   86,    0,   83,    0,
    0,    0,   85,   47,   48,   49,   84,
};
const short cimmof_dgoto[] = {                                       4,
   19,   69,  228,   39,   66,   92,  175,  200,  115,  116,
  233,  117,  118,  185,  119,  102,  186,  120,  209,  210,
  187,  188,  189,  144,  218,  145,   57,  156,  190,  167,
   62,  222,  229,   88,  140,   31,  129,  139,  136,  121,
  230,    5,   84,   85,   86,  204,  205,   45,   40,   94,
   47,   48,   95,   49,    6,    7,    8,    9,   10,   11,
   12,   13,   33,   14,   51,   52,  149,  181,  178,  147,
  179,   35,   54,   55,   15,   89,  170,  196,  130,  131,
   16,   41,
};
const short cimmof_sindex[] = {                                   -247,
    0, -251, -143,    0,    0, -222,    0,    0, -208,    0,
    0, -247,    0, -254,    0, -143,    0, -219, -195,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -175,
    0, -206,    0, -184,    0,    0, -158, -163, -172,    0,
 -245, -145, -144, -108, -138, -135,    0,    0,    0, -203,
 -159,    0, -156, -141,    0,    0, -154, -158,   24,   38,
    0, -207,    0, -143,    0, -130,    0,    0, -128,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -125,    0,    0, -153, -131,  -73,    0,
    0, -157, -142, -121, -117, -119,    0, -127, -106,    0,
 -126, -104, -154,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -145,    0,    0, -241,    0,
    0, -102, -101,    0,    0,    0,    0,    0,    0,    0,
 -207,    0,    0,    0, -215, -103,  -98,  -91,  -99, -217,
    0,  -69,    0, -125,    0,    0,    0, -184,  -51,    0,
  -32,    0,    0,    0, -158,    0,    0,    0,    0,   52,
    0,    0,    0,    0,  -45,  -32,    0,    0,  -46, -207,
    0,    0,  -73,    0,  -41, -103, -139, -187,    0,    0,
    0,    0, -145,    0,    0,    0,    0,    0,    0,  -48,
    0,    0,    0,    0,    0, -185,    0,    0, -145,  -36,
    0,  -71,    0,  -52,    0,    0, -184,    0,  -33, -158,
    0,    0, -207,    0,    0,    0,    0, -125,    0,    0,
  -49,  -47,    0,    0,  -29,    0,    0,  -31,    0,  -20,
    2,  -29,    0,    0,    0,    0,    0,
};
const short cimmof_rindex[] = {                                      1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    1,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -124,    0,  -28,    0,    0,    0,    0, -230,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -124,    0,    0,  -28,    0,    0, -210,    0,    0,    0,
    0, -240,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0, -147,    0,    0,    0,  -27,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  -22,   -8,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -242,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
 -234,    0,    0,    0,    0,  -17,    0,    0,    0,    0,
    0,    0, -221, -177,    0,    0,    0, -250,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,  -10,   -9,    0,    0,    0,    0,
    0,    0,  -25,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0, -124,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0, -182,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  -21,
    0,    0,    0,    0,    0,    0,    0,
};
const short cimmof_gindex[] = {                                      0,
    0,    0,    0,  254,    0,    0,    0,    0,  -37,    0,
    0, -149,  -53,  -39,    0,  157,  160,    0,    0,    0,
   31,    0,    0,    0,    0,    0,  -55,    0,   97,   88,
    0,    0,   33,    0,    0,  -81,    0,    0, -134,    0,
    0,    0,  -44,    0,    0,    0,    0,    0,  202,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  257,
    0,    0,    0,  -30,    0,  220,    0,    0,    0,    0,
   76,    0,    0,  231,    0,    0,    0,    0, -122,    0,
    0,    0,
};
#define YYTABLESIZE 330
const short cimmof_table[] = {                                      61,
    3,   50,  103,   53,   67,   93,  123,  141,  163,  176,
  192,   35,    1,   63,   37,   61,  159,   35,  127,   61,
   50,   61,   64,   53,  126,   61,  160,  127,  157,   17,
  147,   38,   18,  126,   32,    2,   61,  157,   41,   32,
    3,   42,   41,  164,  172,   41,   88,  197,   34,  165,
  173,   46,   88,    1,  147,  147,  147,  147,  147,  147,
  147,  147,  147,  147,  147,   43,  147,  147,  147,  157,
  157,  157,  157,  157,  206,    1,  212,   91,  158,   44,
  207,  234,  213,  224,   59,   44,   44,   44,   60,   44,
  223,  198,  124,  125,  126,  127,  128,  184,   96,  191,
    1,   70,   71,   72,   73,   74,   75,   76,   77,   78,
   79,   80,  184,   81,   82,   83,   99,  177,    1,   44,
   44,  202,   56,   58,   98,   20,   65,   68,   90,   87,
  101,  133,  203,  134,  135,  137,  138,   21,  143,  151,
  142,   56,  146,  148,  150,  208,   22,   23,   24,   25,
   26,   27,   28,   29,  221,  153,  147,  152,  155,  161,
  162,  214,  168,  166,  171,   70,   71,   72,   73,   74,
   75,   76,   77,   78,   79,   80,  177,   81,   82,   83,
  147,  147,  147,  147,  147,  147,  147,  147,  147,  147,
  147,  235,  147,  147,  147,   20,   70,   71,   72,   73,
   74,   75,   76,   77,   78,   79,   80,  169,   81,   82,
   83,  174,  180,  193,  195,  211,   22,   23,   24,   25,
   26,   27,   28,   29,   59,  199,  216,  215,  217,  220,
  225,  226,  105,  106,   12,  231,  117,  107,  108,   65,
  182,  110,  111,  112,  113,  114,  183,  232,   88,  153,
   46,  227,  147,   31,   46,   80,   30,   82,   59,  157,
  154,  236,  194,  201,  237,  132,  105,  106,   36,  147,
   97,  107,  108,   65,  109,  110,  111,  112,  113,  114,
  183,  104,  219,  153,  100,    0,  147,    0,  105,  106,
    0,    0,    0,  107,  108,   65,  109,  110,  111,  112,
  113,  114,  105,  106,    0,    0,    0,  107,  108,   65,
  122,  110,  111,  112,  113,  114,  105,  106,    0,    0,
    0,  107,  108,   65,  109,  110,  111,  112,  113,  114,
};
const short cimmof_check[] = {                                      39,
    0,   32,   58,   34,   42,   50,   60,   89,  131,  144,
  160,  262,  260,  259,  269,  258,  258,  268,  259,  262,
   51,  264,  268,   54,  259,  268,  268,  268,  259,  281,
  281,  286,  284,  268,  257,  283,  279,  268,  260,  261,
  288,  261,  264,  259,  262,  267,  257,  170,  257,  265,
  268,  258,  263,  260,  305,  306,  307,  308,  309,  310,
  311,  312,  313,  314,  315,  261,  317,  318,  319,  300,
  301,  302,  303,  304,  262,  260,  262,  281,  116,  262,
  268,  231,  268,  218,  257,  268,  264,  263,  261,  267,
  213,  173,  300,  301,  302,  303,  304,  151,  258,  155,
  260,  305,  306,  307,  308,  309,  310,  311,  312,  313,
  314,  315,  166,  317,  318,  319,  258,  148,  260,  267,
  268,  177,  281,  287,  281,  269,  272,  272,  264,  268,
  285,  262,  177,  262,  260,  289,  268,  281,  281,  267,
  298,  281,  264,  261,  264,  183,  290,  291,  292,  293,
  294,  295,  296,  297,  210,  282,  281,  264,  263,  262,
  262,  199,  261,  267,  264,  305,  306,  307,  308,  309,
  310,  311,  312,  313,  314,  315,  207,  317,  318,  319,
  305,  306,  307,  308,  309,  310,  311,  312,  313,  314,
  315,  231,  317,  318,  319,  269,  305,  306,  307,  308,
  309,  310,  311,  312,  313,  314,  315,  299,  317,  318,
  319,  281,  264,  259,  261,  264,  290,  291,  292,  293,
  294,  295,  296,  297,  257,  267,  298,  264,  281,  263,
  280,  279,  265,  266,  257,  267,  264,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  279,  268,  257,  282,
  268,  281,  281,  264,  264,  281,    3,  279,  257,  103,
  101,  231,  166,  176,  232,   64,  265,  266,   12,  269,
   51,  270,  271,  272,  273,  274,  275,  276,  277,  278,
  279,  258,  207,  282,   54,   -1,  286,   -1,  265,  266,
   -1,   -1,   -1,  270,  271,  272,  273,  274,  275,  276,
  277,  278,  265,  266,   -1,   -1,   -1,  270,  271,  272,
  273,  274,  275,  276,  277,  278,  265,  266,   -1,   -1,
   -1,  270,  271,  272,  273,  274,  275,  276,  277,  278,
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
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOK_LEFTCURLYBRACE",
"TOK_RIGHTCURLYBRACE","TOK_RIGHTSQUAREBRACKET","TOK_LEFTSQUAREBRACKET",
"TOK_LEFTPAREN","TOK_RIGHTPAREN","TOK_COLON","TOK_SEMICOLON",
"TOK_POSITIVE_DECIMAL_VALUE","TOK_SIGNED_DECIMAL_VALUE","TOK_EQUAL","TOK_COMMA",
"TOK_CLASS","TOK_REAL_VALUE","TOK_CHAR_VALUE","TOK_STRING_VALUE",
"TOK_NULL_VALUE","TOK_OCTAL_VALUE","TOK_HEX_VALUE","TOK_BINARY_VALUE",
"TOK_TRUE","TOK_FALSE","TOK_DQUOTE","TOK_PERIOD","TOK_SIMPLE_IDENTIFIER",
"TOK_ALIAS_IDENTIFIER","TOK_PRAGMA","TOK_INCLUDE","TOK_AS","TOK_INSTANCE",
"TOK_OF","TOK_QUALIFIER","TOK_SCOPE","TOK_SCHEMA","TOK_ASSOCIATION",
"TOK_INDICATION","TOK_PROPERTY","TOK_REFERENCE","TOK_METHOD","TOK_PARAMETER",
"TOK_ANY","TOK_REF","TOK_FLAVOR","TOK_ENABLEOVERRIDE","TOK_DISABLEOVERRIDE",
"TOK_RESTRICTED","TOK_TOSUBCLASS","TOK_TRANSLATABLE","TOK_DT_STR","TOK_DT_BOOL",
"TOK_DT_DATETIME","TOK_DT_UINT8","TOK_DT_SINT8","TOK_DT_UINT16","TOK_DT_SINT16",
"TOK_DT_UINT32","TOK_DT_SINT32","TOK_DT_UINT64","TOK_DT_SINT64","TOK_DT_CHAR8",
"TOK_DT_CHAR16","TOK_DT_REAL32","TOK_DT_REAL64","TOK_UNEXPECTED_CHAR",
"TOK_END_OF_FILE",
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
"methodDeclaration : qualifierList methodStart methodBody methodEnd",
"methodStart : dataType methodName",
"methodBody : TOK_LEFTPAREN parameters TOK_RIGHTPAREN",
"methodEnd : TOK_SEMICOLON",
"propertyDeclaration : qualifierList propertyBody propertyEnd",
"propertyBody : dataType propertyName array typedDefaultValue",
"propertyEnd : TOK_SEMICOLON",
"referenceDeclaration : qualifierList referencedObject TOK_REF referenceName referencePath TOK_SEMICOLON",
"referencedObject : TOK_SIMPLE_IDENTIFIER",
"referenceName : TOK_SIMPLE_IDENTIFIER",
"referencePath : TOK_EQUAL stringValue",
"referencePath :",
"methodName : TOK_SIMPLE_IDENTIFIER",
"parameters : parameter",
"parameters : parameters TOK_COMMA parameter",
"parameters :",
"parameter : qualifierList parameterType parameterName array",
"parameterType : dataType",
"parameterType : objectRef",
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
#line 241 "cimmof.y"
{ /* FIXME: Where do we put directives? */ }
break;
case 5:
#line 243 "cimmof.y"
{ cimmofParser::Instance()->addQualifier(yyvsp[0].mofqualifier); delete yyvsp[0].mofqualifier; }
break;
case 6:
#line 245 "cimmof.y"
{ cimmofParser::Instance()->addClass(yyvsp[0].mofclass); }
break;
case 7:
#line 247 "cimmof.y"
{ cimmofParser::Instance()->addInstance(yyvsp[0].instance); }
break;
case 8:
#line 250 "cimmof.y"
{
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addClassAlias(g_currentAlias, yyval.mofclass, false);
}
break;
case 9:
#line 256 "cimmof.y"
{
  yyval.mofclass = cimmofParser::Instance()->newClassDecl(*yyvsp[-2].cimnameval, *yyvsp[0].cimnameval);
  apply(&g_qualifierList, yyval.mofclass);
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
#line 268 "cimmof.y"
{  }
break;
case 11:
#line 270 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].cimnameval); }
break;
case 12:
#line 271 "cimmof.y"
{ yyval.cimnameval = new CIMName(); }
break;
case 17:
#line 279 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; }
break;
case 18:
#line 281 "cimmof.y"
{
  cimmofParser::Instance()->applyMethod(*g_currentClass, *yyvsp[0].method); }
break;
case 19:
#line 283 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; }
break;
case 20:
#line 287 "cimmof.y"
{
  yyval.method = yyvsp[-2].method;
  apply(&g_qualifierList, yyval.method);
}
break;
case 21:
#line 293 "cimmof.y"
{
  if (g_currentMethod)
    delete g_currentMethod;
  g_currentMethod = 
                 cimmofParser::Instance()->newMethod(*yyvsp[0].cimnameval, yyvsp[-1].datatype) ;
  yyval.method = g_currentMethod;
  delete yyvsp[0].cimnameval;
}
break;
case 24:
#line 307 "cimmof.y"
{
   yyval.property = yyvsp[-1].property;
  apply(&g_qualifierList, yyval.property);
}
break;
case 25:
#line 314 "cimmof.y"
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
case 27:
#line 332 "cimmof.y"
{
  String s(*yyvsp[-4].strval);
  if (!String::equal(*yyvsp[-1].strval, String::EMPTY))
    s += "." + *yyvsp[-1].strval;
  CIMValue *v = valueFactory::createValue(CIMTYPE_REFERENCE, -1, true, &s);
  /*KS add the isArray and arraysize parameters. 8 mar 2002*/
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v, false,0, *yyvsp[-4].strval);
  apply(&g_qualifierList, yyval.property);
  delete yyvsp[-4].strval;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval; 
  delete v;
}
break;
case 28:
#line 346 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 29:
#line 348 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 30:
#line 350 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 31:
#line 351 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 32:
#line 353 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 36:
#line 360 "cimmof.y"
{ /* ATTN: P2 2002 Question Need to create default value including type?*/
  CIMParameter *p = 0;
  cimmofParser *cp = cimmofParser::Instance();
  if (yyvsp[0].ival == -1) {
    p = cp->newParameter(*yyvsp[-1].cimnameval, yyvsp[-2].datatype, false, 0, g_referenceClassName);
  } else {
    p = cp->newParameter(*yyvsp[-1].cimnameval, yyvsp[-2].datatype, true, yyvsp[0].ival, g_referenceClassName);
  }
  g_referenceClassName = CIMName();
  apply(&g_qualifierList, p);
  cp->applyParameter(*g_currentMethod, *p);
  delete p;
  delete yyvsp[-1].cimnameval;
}
break;
case 37:
#line 375 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 38:
#line 376 "cimmof.y"
{ yyval.datatype = CIMTYPE_REFERENCE; }
break;
case 39:
#line 378 "cimmof.y"
{  
                          g_referenceClassName = *yyvsp[-1].cimnameval; }
break;
case 40:
#line 381 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 41:
#line 383 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); }
break;
case 42:
#line 387 "cimmof.y"
{ yyval.ival = valueFactory::Stoi(*yyvsp[-1].strval);
		   delete yyvsp[-1].strval;
                 }
break;
case 43:
#line 390 "cimmof.y"
{ yyval.ival = 0; }
break;
case 44:
#line 391 "cimmof.y"
{ yyval.ival = -1; }
break;
case 45:
#line 393 "cimmof.y"
{ yyval.typedinitializer = yyvsp[0].typedinitializer; }
break;
case 46:
#line 394 "cimmof.y"
{   /* empty */
                  g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                  g_typedInitializerValue.value = new String(String::EMPTY); 
                  yyval.typedinitializer = &g_typedInitializerValue;
              }
break;
case 47:
#line 400 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 48:
#line 401 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 49:
#line 402 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 50:
#line 408 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 51:
#line 414 "cimmof.y"
{
           g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
           g_typedInitializerValue.value = new String(String::EMPTY); 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 52:
#line 420 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 53:
#line 426 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_REFERENCE_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           }
break;
case 54:
#line 432 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 55:
#line 434 "cimmof.y"
{
                                *yyval.strval += ","; 
                                *yyval.strval += *yyvsp[0].strval;
				delete yyvsp[0].strval;
                              }
break;
case 56:
#line 443 "cimmof.y"
{yyval.strval = yyvsp[0].strval;}
break;
case 57:
#line 444 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 58:
#line 446 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 59:
#line 447 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 60:
#line 448 "cimmof.y"
{ yyval.strval =  yyvsp[0].strval; }
break;
case 61:
#line 449 "cimmof.y"
{ }
break;
case 62:
#line 450 "cimmof.y"
{ yyval.strval = new String(yyvsp[0].ival ? "T" : "F"); }
break;
case 65:
#line 454 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->oct_to_dec(*yyvsp[0].strval));
                   delete yyvsp[0].strval; }
break;
case 66:
#line 457 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->hex_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; }
break;
case 67:
#line 460 "cimmof.y"
{
                 yyval.strval = new String(cimmofParser::Instance()->binary_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; }
break;
case 68:
#line 464 "cimmof.y"
{ yyval.ival = 0; }
break;
case 69:
#line 465 "cimmof.y"
{ yyval.ival = 1; }
break;
case 70:
#line 467 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 71:
#line 469 "cimmof.y"
{ 
                *yyval.strval += *yyvsp[0].strval;  delete yyvsp[0].strval;
              }
break;
case 72:
#line 474 "cimmof.y"
{ 
   String oldrep = *yyvsp[0].strval;
   String s(oldrep), s1(String::EMPTY);
   /* Handle quoted quote*/
   int len = s.size();
   if (s[len] == '\n') {
     /* error: new line inside a string constant unless it is quoted*/
     if (s[len - 2] == '\\') {
       if (len > 3)
	 s1 = s.subString(1, len-3);
     } else {
       cimmof_error("New line in string constant");
     }
     cimmofParser::Instance()->increment_lineno();
   } else { /* Can only be a quotation mark*/
     if (s[len - 2] == '\\') {  /* if it is quoted*/
       if (len > 3)
	 s1 = s.subString(1, len-3);
       s1 += '\"';
       cimmof_yy_less(len-1);
     } else { /* This is the normal case:  real quotes on both end*/
       s1 = s.subString(1, len - 2) ;
     }
   }
   delete yyvsp[0].strval; yyval.strval = new String(s1);
}
break;
case 73:
#line 503 "cimmof.y"
{ yyval.strval = yyvsp[-1].strval; }
break;
case 74:
#line 505 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 75:
#line 507 "cimmof.y"
{}
break;
case 76:
#line 508 "cimmof.y"
{  }
break;
case 77:
#line 511 "cimmof.y"
{ 
  /* The objectName string is decomposed for syntactical purposes */
  /* and reassembled here for later parsing in creation of an objname instance */
  String *s = new String(*yyvsp[-2].strval);
  if (!String::equal(*s, String::EMPTY) && yyvsp[-1].modelpath)
    *s += ":";
  if (yyvsp[-1].modelpath) {
    *s += yyvsp[-1].modelpath->Stringrep();
  }
  yyval.strval = s;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].modelpath;
}
break;
case 78:
#line 525 "cimmof.y"
{
  /* convert somehow from alias to a CIM object name*/
  delete yyvsp[0].strval;
}
break;
case 79:
#line 531 "cimmof.y"
{ }
break;
case 80:
#line 532 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 81:
#line 534 "cimmof.y"
{}
break;
case 82:
#line 536 "cimmof.y"
{
             modelPath *m = new modelPath(*yyvsp[-2].cimnameval, g_KeyBindingArray);
             g_KeyBindingArray.clear(); 
             delete yyvsp[-2].cimnameval;}
break;
case 83:
#line 541 "cimmof.y"
{ yyval.ival = 0; }
break;
case 84:
#line 542 "cimmof.y"
{ yyval.ival = 0; }
break;
case 85:
#line 545 "cimmof.y"
{
		KeyBinding *kb = new KeyBinding(*yyvsp[-2].strval, *yyvsp[0].strval,
                               modelPath::KeyBindingTypeOf(*yyvsp[0].strval));
		g_KeyBindingArray.append(*kb);
		delete kb;
		delete yyvsp[-2].strval;
	        delete yyvsp[0].strval; }
break;
case 87:
#line 555 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 88:
#line 556 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); }
break;
case 90:
#line 561 "cimmof.y"
{ 
  yyval.instance = g_currentInstance; 
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addInstanceAlias(g_currentAlias, yyvsp[-1].instance, true);
}
break;
case 91:
#line 568 "cimmof.y"
{
  if (g_currentInstance)
    delete g_currentInstance;
  g_currentAlias = *yyvsp[0].strval;
  g_currentInstance = cimmofParser::Instance()->newInstance(*yyvsp[-1].cimnameval);
  yyval.instance = g_currentInstance;
  apply(&g_qualifierList, yyval.instance);
  delete yyvsp[-1].cimnameval;
  delete yyvsp[0].strval;
}
break;
case 95:
#line 590 "cimmof.y"
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
  apply(&g_qualifierList, newprop);
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
#line 631 "cimmof.y"
{
    /*printf("compilerDirectiveInclude "); */
}
break;
case 97:
#line 635 "cimmof.y"
{
    /*printf("compilerDirectivePragma ");*/
}
break;
case 98:
#line 641 "cimmof.y"
{
  cimmofParser::Instance()->enterInlineInclude(*yyvsp[-1].strval); delete yyvsp[-1].strval;
}
break;
case 99:
#line 646 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 100:
#line 650 "cimmof.y"
{ cimmofParser::Instance()->processPragma(*yyvsp[-3].strval, *yyvsp[-1].strval); 
		   delete yyvsp[-3].strval;
		   delete yyvsp[-1].strval;
		   }
break;
case 101:
#line 657 "cimmof.y"
{
/*    CIMQualifierDecl *qd = new CIMQualifierDecl($2, $3, $4, $5);*/
	yyval.mofqualifier = cimmofParser::Instance()->newQualifierDecl(*yyvsp[-4].strval, yyvsp[-3].value, *yyvsp[-2].scope, *yyvsp[-1].flavor);
        delete yyvsp[-4].strval;
	delete yyvsp[-3].value;  /* CIMValue object created in qualifierValue production*/
}
break;
case 102:
#line 666 "cimmof.y"
{
    yyval.value = valueFactory::createValue(yyvsp[-2].datatype, yyvsp[-1].ival, 
                yyvsp[0].typedinitializer->type == CIMMOF_NULL_VALUE, yyvsp[0].typedinitializer->value);
    delete yyvsp[0].typedinitializer->value;
}
break;
case 103:
#line 672 "cimmof.y"
{ yyval.scope = yyvsp[-1].scope; }
break;
case 104:
#line 674 "cimmof.y"
{ 
    g_scope = CIMScope (CIMScope::NONE); }
break;
case 105:
#line 677 "cimmof.y"
{ yyval.scope = yyvsp[0].scope; }
break;
case 106:
#line 679 "cimmof.y"
{ yyval.scope->addScope(*yyvsp[0].scope); }
break;
case 107:
#line 682 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS);        }
break;
case 108:
#line 684 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS); }
break;
case 109:
#line 685 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ASSOCIATION);  }
break;
case 110:
#line 686 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::INDICATION);   }
break;
case 111:
#line 688 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PROPERTY);     }
break;
case 112:
#line 689 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::REFERENCE);    }
break;
case 113:
#line 690 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::METHOD);       }
break;
case 114:
#line 691 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PARAMETER);    }
break;
case 115:
#line 692 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ANY);          }
break;
case 116:
#line 696 "cimmof.y"
{ yyval.flavor = &g_flavor; }
break;
case 117:
#line 697 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); }
break;
case 118:
#line 702 "cimmof.y"
{g_flavor = CIMFlavor (CIMFlavor::NONE);}
break;
case 121:
#line 718 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::ENABLEOVERRIDE); }
break;
case 122:
#line 719 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::DISABLEOVERRIDE); }
break;
case 123:
#line 720 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::RESTRICTED); }
break;
case 124:
#line 721 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TOSUBELEMENTS); }
break;
case 125:
#line 722 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TRANSLATABLE); }
break;
case 126:
#line 724 "cimmof.y"
{ yyval.flavor = &g_flavor; }
break;
case 127:
#line 725 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); }
break;
case 130:
#line 731 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 131:
#line 732 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; }
break;
case 132:
#line 733 "cimmof.y"
{ yyval.datatype = CIMTYPE_STRING;   }
break;
case 133:
#line 734 "cimmof.y"
{ yyval.datatype = CIMTYPE_BOOLEAN;  }
break;
case 134:
#line 735 "cimmof.y"
{ yyval.datatype = CIMTYPE_DATETIME; }
break;
case 135:
#line 737 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT8;  }
break;
case 136:
#line 738 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT8;  }
break;
case 137:
#line 739 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT16; }
break;
case 138:
#line 740 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT16; }
break;
case 139:
#line 741 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT32; }
break;
case 140:
#line 742 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT32; }
break;
case 141:
#line 743 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT64; }
break;
case 142:
#line 744 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT64; }
break;
case 143:
#line 745 "cimmof.y"
{ yyval.datatype = CIMTYPE_CHAR16; }
break;
case 144:
#line 747 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL32; }
break;
case 145:
#line 748 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL64; }
break;
case 147:
#line 751 "cimmof.y"
{ }
break;
case 148:
#line 753 "cimmof.y"
{ g_qualifierList.init(); }
break;
case 149:
#line 755 "cimmof.y"
{ }
break;
case 150:
#line 756 "cimmof.y"
{ }
break;
case 151:
#line 759 "cimmof.y"
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
#line 773 "cimmof.y"
{ 
    g_flavor = CIMFlavor (CIMFlavor::NONE); }
break;
case 153:
#line 775 "cimmof.y"
{ 
                        yyval.strval = new String((*yyvsp[0].scope).toString ());
                        g_flavor = CIMFlavor (CIMFlavor::NONE); }
break;
case 154:
#line 780 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
                    g_typedInitializerValue.value =  yyvsp[-1].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 155:
#line 786 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 156:
#line 792 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
                    g_typedInitializerValue.value =  yyvsp[0].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 157:
#line 797 "cimmof.y"
{   /* empty */
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    }
break;
case 158:
#line 803 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
case 159:
#line 805 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; }
break;
#line 1648 "y.tab.c"
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
