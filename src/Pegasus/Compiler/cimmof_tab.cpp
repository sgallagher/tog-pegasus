
/*  A Bison parser, made from cimmof.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse cimmof_parse
#define yylex cimmof_lex
#define yyerror cimmof_error
#define yylval cimmof_lval
#define yychar cimmof_char
#define yydebug cimmof_debug
#define yynerrs cimmof_nerrs
#define	TOK_LEFTCURLYBRACE	257
#define	TOK_RIGHTCURLYBRACE	258
#define	TOK_RIGHTSQUAREBRACKET	259
#define	TOK_LEFTSQUAREBRACKET	260
#define	TOK_LEFTPAREN	261
#define	TOK_RIGHTPAREN	262
#define	TOK_COLON	263
#define	TOK_SEMICOLON	264
#define	TOK_POSITIVE_DECIMAL_VALUE	265
#define	TOK_SIGNED_DECIMAL_VALUE	266
#define	TOK_EQUAL	267
#define	TOK_COMMA	268
#define	TOK_CLASS	269
#define	TOK_REAL_VALUE	270
#define	TOK_CHAR_VALUE	271
#define	TOK_STRING_VALUE	272
#define	TOK_NULL_VALUE	273
#define	TOK_OCTAL_VALUE	274
#define	TOK_HEX_VALUE	275
#define	TOK_BINARY_VALUE	276
#define	TOK_TRUE	277
#define	TOK_FALSE	278
#define	TOK_DQUOTE	279
#define	TOK_PERIOD	280
#define	TOK_SIMPLE_IDENTIFIER	281
#define	TOK_ALIAS_IDENTIFIER	282
#define	TOK_PRAGMA	283
#define	TOK_INCLUDE	284
#define	TOK_AS	285
#define	TOK_INSTANCE	286
#define	TOK_OF	287
#define	TOK_QUALIFIER	288
#define	TOK_SCOPE	289
#define	TOK_SCHEMA	290
#define	TOK_ASSOCIATION	291
#define	TOK_INDICATION	292
#define	TOK_PROPERTY	293
#define	TOK_REFERENCE	294
#define	TOK_METHOD	295
#define	TOK_PARAMETER	296
#define	TOK_ANY	297
#define	TOK_REF	298
#define	TOK_FLAVOR	299
#define	TOK_ENABLEOVERRIDE	300
#define	TOK_DISABLEOVERRIDE	301
#define	TOK_RESTRICTED	302
#define	TOK_TOSUBCLASS	303
#define	TOK_TRANSLATABLE	304
#define	TOK_DT_STR	305
#define	TOK_DT_BOOL	306
#define	TOK_DT_DATETIME	307
#define	TOK_DT_UINT8	308
#define	TOK_DT_SINT8	309
#define	TOK_DT_UINT16	310
#define	TOK_DT_SINT16	311
#define	TOK_DT_UINT32	312
#define	TOK_DT_SINT32	313
#define	TOK_DT_UINT64	314
#define	TOK_DT_SINT64	315
#define	TOK_DT_CHAR8	316
#define	TOK_DT_CHAR16	317
#define	TOK_DT_REAL32	318
#define	TOK_DT_REAL64	319
#define	TOK_UNEXPECTED_CHAR	320
#define	TOK_END_OF_FILE	321

#line 1 "cimmof.y"

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
#include <malloc.h>
#include <cstdio>
#include <cstring>
#include <Pegasus/Common/String.h>
#include "cimmofParser.h"
#include "valueFactory.h"
#include "memobjs.h"
#include "qualifierList.h"
#include "objname.h"

//extern cimmofParser g_cimmofParser;

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
  Uint32 g_flavor = 0;
  Uint32 g_scope = 0;
  qualifierList g_qualifierList(10);  /* FIXME */
  CIMMethod *g_currentMethod = 0;
  CIMClass *g_currentClass = 0;
  CIMInstance *g_currentInstance = 0;
  String g_currentAlias = String::EMPTY;
  String g_referenceClassName = String::EMPTY;
  KeyBindingArray g_KeyBindingArray; // it gets created empty

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
cimmof_error(char *msg) {
  cimmofParser::Instance()->log_parse_error(cimmof_text, msg);
  // printf("Error: %s\n", msg);
}

  
#line 105 "cimmof.y"
typedef union {
  struct pragma *pragma;
  int              ival;
  //  char             *strval;
  String *         strval;
  CIMType::Tag        datatype;
  CIMValue *          value;
  String *         strptr;
  CIMQualifier *      qualifier;
  CIMProperty *       property;
  CIMMethod *         method;
  CIMClass *      mofclass;
  CIMQualifierDecl *   mofqualifier;
  CIMInstance *   instance;
  CIMReference *  reference;
  modelPath *     modelpath;
  KeyBinding *    keybinding;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		232
#define	YYFLAG		-32768
#define	YYNTBASE	68

#define YYTRANSLATE(x) ((unsigned)(x) <= 321 ? yytranslate[x] : 149)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     6,     8,    10,    12,    14,    17,    23,
    25,    28,    29,    34,    38,    40,    43,    45,    47,    49,
    54,    57,    61,    63,    67,    72,    74,    81,    83,    85,
    88,    89,    91,    93,    97,    98,   103,   105,   107,   110,
   112,   114,   118,   121,   122,   125,   126,   128,   130,   132,
   134,   138,   140,   142,   144,   146,   148,   150,   152,   154,
   156,   158,   160,   162,   164,   166,   169,   171,   175,   177,
   179,   184,   186,   189,   190,   192,   196,   198,   202,   206,
   208,   211,   212,   214,   217,   223,   228,   230,   233,   240,
   242,   244,   250,   252,   258,   265,   270,   274,   278,   280,
   284,   286,   288,   290,   292,   294,   296,   298,   300,   302,
   307,   308,   311,   313,   317,   319,   321,   323,   325,   327,
   329,   330,   332,   335,   337,   339,   341,   343,   345,   347,
   349,   351,   353,   355,   357,   359,   361,   363,   365,   367,
   371,   372,   374,   376,   380,   384,   386,   388,   392,   394,
   395,   397
};

static const short yyrhs[] = {    69,
     0,    70,    69,     0,     0,   122,     0,   126,     0,    71,
     0,   117,     0,    72,    75,     0,   141,    15,    73,   115,
    74,     0,    27,     0,     9,    73,     0,     0,     3,    76,
     4,    10,     0,     3,     4,    10,     0,    77,     0,    76,
    77,     0,    82,     0,    78,     0,    85,     0,   141,    79,
    80,    81,     0,   138,    89,     0,     7,    90,     8,     0,
    10,     0,   141,    83,    84,     0,   138,    95,    96,    97,
     0,    10,     0,   141,    86,    44,    87,    88,    10,     0,
    27,     0,    27,     0,    13,   104,     0,     0,    27,     0,
    91,     0,    90,    14,    91,     0,     0,   141,    92,    94,
    96,     0,   138,     0,    93,     0,    73,    44,     0,    27,
     0,    27,     0,     6,    11,     5,     0,     6,     5,     0,
     0,    13,    98,     0,     0,   100,     0,   105,     0,   106,
     0,   100,     0,    99,    14,   100,     0,   101,     0,    16,
     0,    17,     0,   103,     0,   102,     0,    19,     0,    11,
     0,    12,     0,    20,     0,    21,     0,    22,     0,    24,
     0,    23,     0,   104,     0,   103,   104,     0,    18,     0,
     3,    99,     4,     0,   107,     0,   108,     0,    25,   109,
   111,    25,     0,   116,     0,   110,     9,     0,     0,   104,
     0,    73,    26,   112,     0,   113,     0,   112,    14,   113,
     0,   114,    13,    98,     0,    27,     0,    31,   116,     0,
     0,    28,     0,   118,   119,     0,   141,    32,    33,    73,
   115,     0,     3,   120,     4,    10,     0,   121,     0,   120,
   121,     0,   141,    27,    96,    13,    98,    10,     0,   123,
     0,   125,     0,    29,    30,     7,   124,     8,     0,   104,
     0,    29,   147,     7,   148,     8,     0,    34,   145,   127,
   128,   132,    10,     0,     9,   138,    96,    97,     0,   129,
   130,     8,     0,    14,    35,     7,     0,   131,     0,   130,
    14,   131,     0,    15,     0,    36,     0,    37,     0,    38,
     0,    39,     0,    40,     0,    41,     0,    42,     0,    43,
     0,    14,   133,   134,     8,     0,     0,    45,     7,     0,
   135,     0,   134,    14,   135,     0,    46,     0,    47,     0,
    48,     0,    49,     0,    50,     0,   137,     0,     0,   135,
     0,   137,   135,     0,   139,     0,   140,     0,    51,     0,
    52,     0,    53,     0,    54,     0,    55,     0,    56,     0,
    57,     0,    58,     0,    59,     0,    60,     0,    61,     0,
    63,     0,    64,     0,    65,     0,   142,   143,     5,     0,
     0,     6,     0,   144,     0,   143,    14,   144,     0,   145,
   146,   136,     0,    27,     0,   131,     0,     7,   100,     8,
     0,   105,     0,     0,    27,     0,    18,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   219,   221,   222,   224,   225,   227,   229,   232,   238,   251,
   253,   254,   256,   257,   259,   260,   262,   264,   266,   269,
   275,   285,   287,   289,   295,   304,   306,   321,   323,   325,
   326,   328,   330,   331,   332,   334,   350,   351,   353,   356,
   358,   360,   365,   366,   368,   369,   371,   372,   373,   375,
   376,   383,   384,   385,   386,   387,   388,   390,   391,   392,
   395,   398,   402,   403,   405,   406,   411,   439,   442,   443,
   445,   460,   465,   467,   469,   471,   476,   477,   479,   488,
   490,   491,   493,   495,   502,   514,   517,   518,   520,   550,
   554,   559,   566,   568,   575,   585,   591,   593,   595,   596,
   599,   601,   602,   603,   605,   606,   607,   608,   609,   611,
   613,   615,   617,   619,   621,   622,   623,   624,   625,   627,
   628,   630,   631,   634,   635,   636,   637,   638,   640,   641,
   642,   643,   644,   645,   646,   647,   648,   650,   651,   653,
   654,   656,   658,   659,   661,   675,   676,   680,   681,   682,
   685,   687
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TOK_LEFTCURLYBRACE",
"TOK_RIGHTCURLYBRACE","TOK_RIGHTSQUAREBRACKET","TOK_LEFTSQUAREBRACKET","TOK_LEFTPAREN",
"TOK_RIGHTPAREN","TOK_COLON","TOK_SEMICOLON","TOK_POSITIVE_DECIMAL_VALUE","TOK_SIGNED_DECIMAL_VALUE",
"TOK_EQUAL","TOK_COMMA","TOK_CLASS","TOK_REAL_VALUE","TOK_CHAR_VALUE","TOK_STRING_VALUE",
"TOK_NULL_VALUE","TOK_OCTAL_VALUE","TOK_HEX_VALUE","TOK_BINARY_VALUE","TOK_TRUE",
"TOK_FALSE","TOK_DQUOTE","TOK_PERIOD","TOK_SIMPLE_IDENTIFIER","TOK_ALIAS_IDENTIFIER",
"TOK_PRAGMA","TOK_INCLUDE","TOK_AS","TOK_INSTANCE","TOK_OF","TOK_QUALIFIER",
"TOK_SCOPE","TOK_SCHEMA","TOK_ASSOCIATION","TOK_INDICATION","TOK_PROPERTY","TOK_REFERENCE",
"TOK_METHOD","TOK_PARAMETER","TOK_ANY","TOK_REF","TOK_FLAVOR","TOK_ENABLEOVERRIDE",
"TOK_DISABLEOVERRIDE","TOK_RESTRICTED","TOK_TOSUBCLASS","TOK_TRANSLATABLE","TOK_DT_STR",
"TOK_DT_BOOL","TOK_DT_DATETIME","TOK_DT_UINT8","TOK_DT_SINT8","TOK_DT_UINT16",
"TOK_DT_SINT16","TOK_DT_UINT32","TOK_DT_SINT32","TOK_DT_UINT64","TOK_DT_SINT64",
"TOK_DT_CHAR8","TOK_DT_CHAR16","TOK_DT_REAL32","TOK_DT_REAL64","TOK_UNEXPECTED_CHAR",
"TOK_END_OF_FILE","mofSpec","mofProductions","mofProduction","classDeclaration",
"classHead","className","superClass","classBody","classFeatures","classFeature",
"methodDeclaration","methodStart","methodBody","methodEnd","propertyDeclaration",
"propertyBody","propertyEnd","referenceDeclaration","referencedObject","referenceName",
"referencePath","methodName","parameters","parameter","parameterType","objectRef",
"parameterName","propertyName","array","defaultValue","initializer","constantValues",
"constantValue","integerValue","booleanValue","stringValues","stringValue","arrayInitializer",
"referenceInitializer","objectHandle","aliasInitializer","namespaceHandleRef",
"namespaceHandle","modelPath","keyValuePairList","keyValuePair","keyValuePairName",
"alias","aliasIdentifier","instanceDeclaration","instanceHead","instanceBody",
"valueInitializers","valueInitializer","compilerDirective","compilerDirectiveInclude",
"fileName","compilerDirectivePragma","qualifierDeclaration","qualifierValue",
"scope","scope_begin","metaElements","metaElement","defaultFlavor","flavorHead",
"explicitFlavors","explicitFlavor","flavor","overrideFlavors","dataType","intDataType",
"realDataType","qualifierList","qualifierListBegin","qualifiers","qualifier",
"qualifierName","qualifierParameter","pragmaName","pragmaVal", NULL
};
#endif

static const short yyr1[] = {     0,
    68,    69,    69,    70,    70,    70,    70,    71,    72,    73,
    74,    74,    75,    75,    76,    76,    77,    77,    77,    78,
    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
    88,    89,    90,    90,    90,    91,    92,    92,    93,    94,
    95,    96,    96,    96,    97,    97,    98,    98,    98,    99,
    99,   100,   100,   100,   100,   100,   100,   101,   101,   101,
   101,   101,   102,   102,   103,   103,   104,   105,   106,   106,
   107,   108,   109,   109,   110,   111,   112,   112,   113,   114,
   115,   115,   116,   117,   118,   119,   120,   120,   121,   122,
   122,   123,   124,   125,   126,   127,   128,   129,   130,   130,
   131,   131,   131,   131,   131,   131,   131,   131,   131,   132,
   132,   133,   134,   134,   135,   135,   135,   135,   135,   136,
   136,   137,   137,   138,   138,   138,   138,   138,   139,   139,
   139,   139,   139,   139,   139,   139,   139,   140,   140,   141,
   141,   142,   143,   143,   144,   145,   145,   146,   146,   146,
   147,   148
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     2,     5,     1,
     2,     0,     4,     3,     1,     2,     1,     1,     1,     4,
     2,     3,     1,     3,     4,     1,     6,     1,     1,     2,
     0,     1,     1,     3,     0,     4,     1,     1,     2,     1,
     1,     3,     2,     0,     2,     0,     1,     1,     1,     1,
     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     1,     3,     1,     1,
     4,     1,     2,     0,     1,     3,     1,     3,     3,     1,
     2,     0,     1,     2,     5,     4,     1,     2,     6,     1,
     1,     5,     1,     5,     6,     4,     3,     3,     1,     3,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
     0,     2,     1,     3,     1,     1,     1,     1,     1,     1,
     0,     1,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
     0,     1,     1,     3,     3,     1,     1,     3,     1,     0,
     1,     1
};

static const short yydefact[] = {   141,
   142,     0,     0,     1,   141,     6,     0,     7,     0,     4,
    90,    91,     5,     0,     0,   151,     0,     0,   101,   146,
   102,   103,   104,   105,   106,   107,   108,   109,   147,     0,
     2,   141,     8,   141,    84,     0,     0,     0,   143,   150,
     0,     0,     0,     0,     0,   141,    15,    18,    17,    19,
     0,   141,    87,     0,    10,    82,     0,   140,     0,     0,
     0,   149,   121,    67,    93,     0,   152,     0,   126,   127,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,    44,   124,   125,     0,   111,     0,    14,     0,
    16,    28,     0,     0,     0,     0,     0,    88,    44,     0,
    12,    82,   144,    58,    59,    53,    54,    57,    60,    61,
    62,    64,    63,     0,    50,    52,    56,    55,    65,     0,
   115,   116,   117,   118,   119,   122,   145,   120,    92,    94,
     0,    46,     0,     0,     0,     0,    99,    13,   141,     0,
    26,    24,     0,    41,    21,    44,    86,     0,    83,    81,
     0,     9,    85,    68,     0,    66,   148,   123,    43,     0,
     0,    96,    98,     0,     0,    95,    97,     0,     0,    33,
     0,    23,    20,    29,    31,    46,     0,    11,    51,    42,
    74,    45,    47,    48,    49,    69,    70,    72,   112,     0,
   113,   100,    22,   141,     0,     0,    38,    37,     0,     0,
    25,     0,    75,     0,     0,   110,     0,    34,    39,    40,
    44,    30,    27,    89,     0,     0,    73,   114,    36,     0,
    71,    80,    76,    77,     0,     0,     0,    78,    79,     0,
     0,     0
};

static const short yydefgoto[] = {   230,
     4,     5,     6,     7,    56,   152,    33,    46,    47,    48,
    93,   140,   173,    49,    94,   142,    50,    95,   175,   200,
   145,   169,   170,   196,   197,   211,   146,   132,   162,   182,
   114,   183,   116,   117,   118,   119,   184,   185,   186,   187,
   204,   205,   216,   223,   224,   225,   101,   188,     8,     9,
    35,    52,    53,    10,    11,    66,    12,    13,    44,    87,
    88,   136,    29,   135,   165,   190,   126,   127,   128,    83,
    84,    85,    14,    15,    38,    39,    40,    63,    18,    68
};

static const short yypact[] = {    21,
-32768,    29,    53,-32768,    21,-32768,    28,-32768,    30,-32768,
-32768,-32768,-32768,    -3,    53,-32768,    47,    59,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    62,
-32768,    12,-32768,    67,-32768,    48,    41,    10,-32768,    27,
    58,    61,   127,    68,    71,    63,-32768,-32768,-32768,-32768,
   -16,    66,-32768,    50,-32768,    52,    48,-32768,    53,   182,
   182,-32768,    15,-32768,-32768,    76,-32768,    78,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    92,-32768,-32768,    64,    86,    72,-32768,    93,
-32768,-32768,    97,    95,    75,    79,   106,-32768,    92,    94,
   111,    52,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,     9,-32768,-32768,-32768,    58,-32768,   113,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    15,-32768,-32768,
    17,   110,   129,    98,   130,    18,-32768,-32768,    11,   132,
-32768,-32768,   117,   138,-32768,    92,-32768,   133,-32768,-32768,
    48,-32768,-32768,-32768,   182,-32768,-32768,-32768,-32768,   142,
   149,-32768,-32768,   141,    15,-32768,-32768,    72,    43,-32768,
    74,-32768,-32768,-32768,   136,   110,   149,-32768,-32768,-32768,
    58,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    44,
-32768,-32768,-32768,    67,   107,   126,-32768,-32768,    58,   144,
-32768,   145,-32768,    48,   147,-32768,    15,-32768,-32768,-32768,
    92,-32768,-32768,-32768,   131,   139,-32768,-32768,-32768,   148,
-32768,-32768,   162,-32768,   150,   148,   149,-32768,-32768,   158,
   189,-32768
};

static const short yypgoto[] = {-32768,
   190,-32768,-32768,-32768,   -54,-32768,-32768,-32768,   151,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,     2,-32768,-32768,-32768,-32768,   -93,    31,  -167,
-32768,   -53,-32768,-32768,-32768,   -40,   168,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -17,-32768,   108,   112,-32768,-32768,
-32768,-32768,   159,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   -83,-32768,-32768,-32768,  -119,-32768,-32768,   -47,
-32768,-32768,   -32,-32768,-32768,   154,   211,-32768,-32768,-32768
};


#define	YYLAST		214


static const short yytable[] = {    51,
    65,    54,   102,    96,   137,   148,   115,   120,   158,   202,
    92,    36,   154,    51,    58,    45,     1,     1,   -35,    54,
    -3,   159,   155,    59,   -35,   167,     1,   160,    37,    60,
    32,   168,    34,    61,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,   191,    80,    81,    82,     2,
   193,   206,   176,    41,     3,    16,   194,   207,    17,   229,
   121,   122,   123,   124,   125,    42,    90,    19,     1,    97,
    43,     1,     1,    57,    55,    64,    99,   156,    67,    20,
    89,    86,   100,   129,   192,   130,    19,   218,    21,    22,
    23,    24,    25,    26,    27,    28,   178,   131,   133,   134,
    55,   179,   138,   139,   141,   144,   171,    21,    22,    23,
    24,    25,    26,    27,    28,   147,   195,   219,   143,   151,
   157,   149,   161,   198,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,   163,    80,    81,    82,   166,
   203,   172,   164,   174,   -32,   177,   180,   189,   199,   215,
   209,    60,   210,   213,   214,   217,   220,   231,   212,   104,
   105,   171,   227,   221,   106,   107,    64,   108,   109,   110,
   111,   112,   113,   181,   222,   226,   149,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,   232,    80,
    81,    82,   104,   105,    31,   208,    91,   106,   107,    64,
   108,   109,   110,   111,   112,   113,   201,    62,   228,   153,
    98,   150,   103,    30
};

static const short yycheck[] = {    32,
    41,    34,    57,    51,    88,    99,    60,    61,   128,   177,
    27,    15,     4,    46,     5,     4,     6,     6,     8,    52,
     0,     5,    14,    14,    14,     8,     6,    11,    32,     3,
     3,    14,     3,     7,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,   165,    63,    64,    65,    29,
     8,     8,   146,     7,    34,    27,    14,    14,    30,   227,
    46,    47,    48,    49,    50,     7,     4,    15,     6,     4,
     9,     6,     6,    33,    27,    18,    27,   118,    18,    27,
    10,    14,    31,     8,   168,     8,    15,   207,    36,    37,
    38,    39,    40,    41,    42,    43,   151,     6,    35,    14,
    27,   155,    10,     7,    10,    27,   139,    36,    37,    38,
    39,    40,    41,    42,    43,    10,   171,   211,    44,     9,
     8,    28,    13,   171,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,     7,    63,    64,    65,    10,
   181,    10,    45,    27,     7,    13,     5,     7,    13,   204,
    44,     3,    27,    10,    10,     9,    26,     0,   199,    11,
    12,   194,    13,    25,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    27,    14,    28,    51,    52,    53,
    54,    55,    56,    57,    58,    59,    60,    61,     0,    63,
    64,    65,    11,    12,     5,   194,    46,    16,    17,    18,
    19,    20,    21,    22,    23,    24,   176,    40,   226,   102,
    52,   100,    59,     3
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"
/* This file comes from bison-@bison_version@.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 224 "cimmof.y"
{ /* FIXME: Where do we put directives? */ ;
    break;}
case 5:
#line 226 "cimmof.y"
{ cimmofParser::Instance()->addQualifier(yyvsp[0].mofqualifier); delete yyvsp[0].mofqualifier; ;
    break;}
case 6:
#line 228 "cimmof.y"
{ cimmofParser::Instance()->addClass(yyvsp[0].mofclass); ;
    break;}
case 7:
#line 230 "cimmof.y"
{ cimmofParser::Instance()->addInstance(yyvsp[0].instance); ;
    break;}
case 8:
#line 233 "cimmof.y"
{
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addClassAlias(g_currentAlias, yyval.mofclass, false);
;
    break;}
case 9:
#line 239 "cimmof.y"
{
  yyval.mofclass = cimmofParser::Instance()->newClassDecl(*yyvsp[-2].strval, *yyvsp[0].strval);
  apply(&g_qualifierList, yyval.mofclass);
  g_currentAlias = *yyvsp[-1].strval;
  if (g_currentClass)
    delete g_currentClass;
  g_currentClass = yyval.mofclass;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval;
  delete yyvsp[0].strval;
;
    break;}
case 10:
#line 251 "cimmof.y"
{  ;
    break;}
case 11:
#line 253 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 12:
#line 254 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 17:
#line 262 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;
    break;}
case 18:
#line 264 "cimmof.y"
{
  cimmofParser::Instance()->applyMethod(*g_currentClass, *yyvsp[0].method); ;
    break;}
case 19:
#line 266 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;
    break;}
case 20:
#line 270 "cimmof.y"
{
  yyval.method = yyvsp[-2].method;
  apply(&g_qualifierList, yyval.method);
;
    break;}
case 21:
#line 276 "cimmof.y"
{
  if (g_currentMethod)
    delete g_currentMethod;
  g_currentMethod = 
                 cimmofParser::Instance()->newMethod(*yyvsp[0].strval, yyvsp[-1].datatype) ;
  yyval.method = g_currentMethod;
  delete yyvsp[0].strval;
;
    break;}
case 24:
#line 290 "cimmof.y"
{
   yyval.property = yyvsp[-1].property;
  apply(&g_qualifierList, yyval.property);
;
    break;}
case 25:
#line 296 "cimmof.y"
{
  CIMValue *v = valueFactory::createValue(yyvsp[-3].datatype, yyvsp[-1].ival, yyvsp[0].strval);
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v);
  delete yyvsp[-2].strval;
  delete yyvsp[0].strval;
  delete v;
;
    break;}
case 27:
#line 308 "cimmof.y"
{
  String s(*yyvsp[-4].strval);
  if (!String::equal(*yyvsp[-1].strval, String::EMPTY))
    s += "." + *yyvsp[-1].strval;
  CIMValue *v = valueFactory::createValue(CIMType::REFERENCE, -1, &s);
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v, *yyvsp[-4].strval);
  apply(&g_qualifierList, yyval.property);
  delete yyvsp[-4].strval;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval; 
  delete v;
;
    break;}
case 28:
#line 321 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 29:
#line 323 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 30:
#line 325 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 31:
#line 326 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 36:
#line 335 "cimmof.y"
{ // FIXME:  Need to create default value including type?
  CIMParameter *p = 0;
  cimmofParser *cp = cimmofParser::Instance();
  if (yyvsp[0].ival == -1) {
    p = cp->newParameter(*yyvsp[-1].strval, yyvsp[-2].datatype, false, 0, g_referenceClassName);
  } else {
    p = cp->newParameter(*yyvsp[-1].strval, yyvsp[-2].datatype, true, yyvsp[0].ival, g_referenceClassName);
  }
  g_referenceClassName = String::EMPTY;
  apply(&g_qualifierList, p);
  cp->applyParameter(*g_currentMethod, *p);
  delete p;
  delete yyvsp[-1].strval;
;
    break;}
case 37:
#line 350 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 38:
#line 351 "cimmof.y"
{ yyval.datatype = CIMType::REFERENCE; ;
    break;}
case 39:
#line 353 "cimmof.y"
{  
                          g_referenceClassName = *yyvsp[-1].strval; ;
    break;}
case 42:
#line 362 "cimmof.y"
{ yyval.ival = valueFactory::Stoi(*yyvsp[-1].strval);
		   delete yyvsp[-1].strval;
                 ;
    break;}
case 43:
#line 365 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 44:
#line 366 "cimmof.y"
{ yyval.ival = -1; ;
    break;}
case 45:
#line 368 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 46:
#line 369 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 47:
#line 371 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 48:
#line 372 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 49:
#line 373 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 50:
#line 375 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 51:
#line 377 "cimmof.y"
{
                                *yyval.strval += ","; 
                                *yyval.strval += *yyvsp[0].strval;
				delete yyvsp[0].strval;
                              ;
    break;}
case 52:
#line 383 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 53:
#line 384 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 54:
#line 385 "cimmof.y"
{ yyval.strval =  yyvsp[0].strval; ;
    break;}
case 55:
#line 386 "cimmof.y"
{ ;
    break;}
case 56:
#line 387 "cimmof.y"
{ yyval.strval = new String(yyvsp[0].ival ? "T" : "F"); ;
    break;}
case 57:
#line 388 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 60:
#line 392 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->oct_to_dec(*yyvsp[0].strval));
                   delete yyvsp[0].strval; ;
    break;}
case 61:
#line 395 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->hex_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;
    break;}
case 62:
#line 398 "cimmof.y"
{
                 yyval.strval = new String(cimmofParser::Instance()->binary_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;
    break;}
case 63:
#line 402 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 64:
#line 403 "cimmof.y"
{ yyval.ival = 1; ;
    break;}
case 65:
#line 405 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 66:
#line 407 "cimmof.y"
{ 
                *yyval.strval += *yyvsp[0].strval;  delete yyvsp[0].strval;
              ;
    break;}
case 67:
#line 412 "cimmof.y"
{ 
   String oldrep = *yyvsp[0].strval;
   String s(oldrep), s1(String::EMPTY);
   // Handle quoted quote
   int len = s.size();
   if (s[len] == '\n') {
     // error: new line inside a string constant unless it is quoted
     if (s[len - 2] == '\\') {
       if (len > 3)
	 s1 = s.subString(1, len-3);
     } else {
       cimmof_error("New line in string constant");
     }
     cimmofParser::Instance()->increment_lineno();
   } else { // Can only be a quotation mark
     if (s[len - 2] == '\\') {  // if it is quoted
       if (len > 3)
	 s1 = s.subString(1, len-3);
       s1 += '\"';
       cimmof_yy_less(len-1);
     } else { // This is the normal case:  real quotes on both end
       s1 = s.subString(1, len - 2) ;
     }
   }
   delete yyvsp[0].strval; yyval.strval = new String(s1);
;
    break;}
case 68:
#line 440 "cimmof.y"
{ yyval.strval = yyvsp[-1].strval; ;
    break;}
case 69:
#line 442 "cimmof.y"
{;
    break;}
case 70:
#line 443 "cimmof.y"
{  ;
    break;}
case 71:
#line 446 "cimmof.y"
{ 
  // The objectName string is decomposed for syntactical purposes 
  // and reassembled here for later parsing in creation of an objname instance 
  String *s = new String(*yyvsp[-2].strval);
  if (!String::equal(*s, String::EMPTY) && yyvsp[-1].modelpath)
    *s += ":";
  if (yyvsp[-1].modelpath) {
    *s += yyvsp[-1].modelpath->Stringrep();
  }
  yyval.strval = s;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].modelpath;
;
    break;}
case 72:
#line 460 "cimmof.y"
{
  // convert somehow from alias to a CIM object name
  delete yyvsp[0].strval;
;
    break;}
case 73:
#line 466 "cimmof.y"
{ ;
    break;}
case 74:
#line 467 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 75:
#line 469 "cimmof.y"
{;
    break;}
case 76:
#line 471 "cimmof.y"
{
             modelPath *m = new modelPath(*yyvsp[-2].strval, g_KeyBindingArray);
             g_KeyBindingArray.clear(); 
             delete yyvsp[-2].strval;;
    break;}
case 77:
#line 476 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 78:
#line 477 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 79:
#line 480 "cimmof.y"
{
		KeyBinding *kb = new KeyBinding(*yyvsp[-2].strval, *yyvsp[0].strval,
                               modelPath::KeyBindingTypeOf(*yyvsp[0].strval));
		g_KeyBindingArray.append(*kb);
		delete kb;
		delete yyvsp[-2].strval;
	        delete yyvsp[0].strval; ;
    break;}
case 81:
#line 490 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 82:
#line 491 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 84:
#line 496 "cimmof.y"
{ 
  yyval.instance = g_currentInstance; 
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addInstanceAlias(g_currentAlias, yyvsp[-1].instance, true);
;
    break;}
case 85:
#line 503 "cimmof.y"
{
  if (g_currentInstance)
    delete g_currentInstance;
  g_currentAlias = *yyvsp[0].strval;
  g_currentInstance = cimmofParser::Instance()->newInstance(*yyvsp[-1].strval);
  yyval.instance = g_currentInstance;
  apply(&g_qualifierList, yyval.instance);
  delete yyvsp[-1].strval;
  delete yyvsp[0].strval;
;
    break;}
case 89:
#line 522 "cimmof.y"
{
  cimmofParser *cp = cimmofParser::Instance();
  // FIXME:  This still doesn't work because there is no way to update 
  // a property.  It must be fixed in the Common code first.
  // What we have to do here is create a CIMProperty  and initialize it with
  // the value provided.  The name of the property is $2 and it belongs
  // to the class whose name is in g_currentInstance->getClassName().
  // The steps are
  //   2. Get  property declaration's value object
  CIMProperty *oldprop = cp->PropertyFromInstance(*g_currentInstance,
							*yyvsp[-4].strval);
  CIMValue *oldv = cp->ValueFromProperty(*oldprop);
  //   3. create the new Value object of the same type
  CIMValue *v = valueFactory::createValue(oldv->getType(), yyvsp[-3].ival, yyvsp[-1].strval);
  //   4. create a clone property with the new value
  CIMProperty *newprop = cp->copyPropertyWithNewValue(*oldprop, *v);
  //   5. apply the qualifiers; 
  apply(&g_qualifierList, newprop);
  //   6. and apply the CIMProperty to g_currentInstance.
  cp->applyProperty(*g_currentInstance, *newprop);
  delete yyvsp[-4].strval;
  delete yyvsp[-1].strval;
  delete oldprop;
  delete oldv;
  delete v;
  delete newprop;
;
    break;}
case 90:
#line 551 "cimmof.y"
{
    //printf("compilerDirectiveInclude "); 
;
    break;}
case 91:
#line 555 "cimmof.y"
{
    //printf("compilerDirectivePragma ");
;
    break;}
case 92:
#line 561 "cimmof.y"
{
  cimmofParser::Instance()->enterInlineInclude(*yyvsp[-1].strval); delete yyvsp[-1].strval;
;
    break;}
case 93:
#line 566 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 94:
#line 570 "cimmof.y"
{ cimmofParser::Instance()->processPragma(*yyvsp[-3].strval, *yyvsp[-1].strval); 
		   delete yyvsp[-3].strval;
		   delete yyvsp[-1].strval;
		   ;
    break;}
case 95:
#line 577 "cimmof.y"
{
//    CIMQualifierDecl *qd = new CIMQualifierDecl($2, $3, $4, $5);
	yyval.mofqualifier = cimmofParser::Instance()->newQualifierDecl(*yyvsp[-4].strval, yyvsp[-3].value, yyvsp[-2].ival, yyvsp[-1].ival);
        delete yyvsp[-4].strval;
	delete yyvsp[-3].value;  // CIMValue object created in qualifierValue production
;
    break;}
case 96:
#line 586 "cimmof.y"
{
    yyval.value = valueFactory::createValue(yyvsp[-2].datatype, yyvsp[-1].ival, yyvsp[0].strval);
    delete yyvsp[0].strval;
;
    break;}
case 97:
#line 591 "cimmof.y"
{ yyval.ival = yyvsp[-1].ival; ;
    break;}
case 98:
#line 593 "cimmof.y"
{ g_scope = CIMScope::NONE; ;
    break;}
case 99:
#line 595 "cimmof.y"
{ yyval.ival = yyvsp[0].ival; ;
    break;}
case 100:
#line 597 "cimmof.y"
{ yyval.ival |= yyvsp[0].ival; ;
    break;}
case 101:
#line 599 "cimmof.y"
{ yyval.ival = CIMScope::CLASS;        ;
    break;}
case 102:
#line 601 "cimmof.y"
{ yyval.ival = CIMScope::CLASS; ;
    break;}
case 103:
#line 602 "cimmof.y"
{ yyval.ival = CIMScope::ASSOCIATION;  ;
    break;}
case 104:
#line 603 "cimmof.y"
{ yyval.ival = CIMScope::INDICATION;   ;
    break;}
case 105:
#line 605 "cimmof.y"
{ yyval.ival = CIMScope::PROPERTY;     ;
    break;}
case 106:
#line 606 "cimmof.y"
{ yyval.ival = CIMScope::REFERENCE;    ;
    break;}
case 107:
#line 607 "cimmof.y"
{ yyval.ival = CIMScope::METHOD;       ;
    break;}
case 108:
#line 608 "cimmof.y"
{ yyval.ival = CIMScope::PARAMETER;    ;
    break;}
case 109:
#line 609 "cimmof.y"
{ yyval.ival = CIMScope::ANY;          ;
    break;}
case 110:
#line 612 "cimmof.y"
{ yyval.ival = g_flavor; ;
    break;}
case 111:
#line 613 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 112:
#line 615 "cimmof.y"
{ g_flavor = 0; ;
    break;}
case 115:
#line 621 "cimmof.y"
{ g_flavor |= CIMFlavor::OVERRIDABLE; ;
    break;}
case 116:
#line 622 "cimmof.y"
{ g_flavor &= ~(CIMFlavor::OVERRIDABLE); ;
    break;}
case 117:
#line 623 "cimmof.y"
{ g_flavor |= CIMFlavor::TOINSTANCE; ;
    break;}
case 118:
#line 624 "cimmof.y"
{ g_flavor |= CIMFlavor::TOSUBCLASS; ;
    break;}
case 119:
#line 625 "cimmof.y"
{ g_flavor |= CIMFlavor::TRANSLATABLE; ;
    break;}
case 120:
#line 627 "cimmof.y"
{ yyval.ival = g_flavor; ;
    break;}
case 121:
#line 628 "cimmof.y"
{ yyval.ival = CIMFlavor::DEFAULTS; ;
    break;}
case 124:
#line 634 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 125:
#line 635 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 126:
#line 636 "cimmof.y"
{ yyval.datatype = CIMType::STRING;   ;
    break;}
case 127:
#line 637 "cimmof.y"
{ yyval.datatype = CIMType::BOOLEAN;  ;
    break;}
case 128:
#line 638 "cimmof.y"
{ yyval.datatype = CIMType::DATETIME; ;
    break;}
case 129:
#line 640 "cimmof.y"
{ yyval.datatype = CIMType::UINT8;  ;
    break;}
case 130:
#line 641 "cimmof.y"
{ yyval.datatype = CIMType::SINT8;  ;
    break;}
case 131:
#line 642 "cimmof.y"
{ yyval.datatype = CIMType::UINT16; ;
    break;}
case 132:
#line 643 "cimmof.y"
{ yyval.datatype = CIMType::SINT16; ;
    break;}
case 133:
#line 644 "cimmof.y"
{ yyval.datatype = CIMType::UINT32; ;
    break;}
case 134:
#line 645 "cimmof.y"
{ yyval.datatype = CIMType::SINT32; ;
    break;}
case 135:
#line 646 "cimmof.y"
{ yyval.datatype = CIMType::UINT64; ;
    break;}
case 136:
#line 647 "cimmof.y"
{ yyval.datatype = CIMType::SINT64; ;
    break;}
case 137:
#line 648 "cimmof.y"
{ yyval.datatype = CIMType::CHAR16; ;
    break;}
case 138:
#line 650 "cimmof.y"
{ yyval.datatype =CIMType::REAL32; ;
    break;}
case 139:
#line 651 "cimmof.y"
{ yyval.datatype =CIMType::REAL64; ;
    break;}
case 141:
#line 654 "cimmof.y"
{ ;
    break;}
case 142:
#line 656 "cimmof.y"
{ g_qualifierList.init(); ;
    break;}
case 143:
#line 658 "cimmof.y"
{ ;
    break;}
case 144:
#line 659 "cimmof.y"
{ ;
    break;}
case 145:
#line 662 "cimmof.y"
{
  cimmofParser *p = cimmofParser::Instance();
  // The qualifier value can't be set until we know the contents of the
  // QualifierDeclaration.  That's what QualifierValue() does.
  CIMValue *v = p->QualifierValue(*yyvsp[-2].strval, *yyvsp[-1].strval); 

  yyval.qualifier = p->newQualifier(*yyvsp[-2].strval, *v, g_flavor);
  g_qualifierList.add(yyval.qualifier);
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval;
  delete v;
 ;
    break;}
case 146:
#line 675 "cimmof.y"
{ g_flavor = 0; ;
    break;}
case 147:
#line 676 "cimmof.y"
{ 
                        yyval.strval = new String(ScopeToString(yyvsp[0].ival));
                        g_flavor = 0; ;
    break;}
case 148:
#line 680 "cimmof.y"
{ yyval.strval = yyvsp[-1].strval; ;
    break;}
case 149:
#line 681 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 150:
#line 682 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 151:
#line 685 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 152:
#line 687 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 689 "cimmof.y"

