
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
#ifndef PEGASUS_PLATFORM_ZOS_ZSERIES_IBM
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
  CIMFlavor g_flavor = CIMFlavor (CIMFlavor::NONE);
  CIMScope g_scope = CIMScope ();
  //ATTN: BB 2001 BB P1 - Fixed size qualifier list max 10. Make larger or var
  qualifierList g_qualifierList(10);
  CIMMethod *g_currentMethod = 0;
  CIMClass *g_currentClass = 0;
  CIMInstance *g_currentInstance = 0;
  String g_currentAlias = String::EMPTY;
  CIMName g_referenceClassName = CIMName();
  KeyBindingArray g_KeyBindingArray; // it gets created empty
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
  // printf("Error: %s\n", msg);
}

  
#line 110 "cimmof.y"
typedef union {
  struct pragma *pragma;
  int              ival;
  //  char             *strval;
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
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		239
#define	YYFLAG		-32768
#define	YYNTBASE	68

#define YYTRANSLATE(x) ((unsigned)(x) <= 321 ? yytranslate[x] : 151)

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
   134,   136,   138,   140,   142,   146,   148,   150,   152,   154,
   156,   158,   160,   162,   164,   166,   168,   170,   172,   174,
   176,   179,   181,   185,   188,   190,   192,   197,   199,   202,
   203,   205,   209,   211,   215,   219,   221,   224,   225,   227,
   230,   236,   241,   243,   246,   252,   254,   256,   262,   264,
   270,   277,   282,   286,   290,   292,   296,   298,   300,   302,
   304,   306,   308,   310,   312,   314,   319,   320,   323,   325,
   329,   331,   333,   335,   337,   339,   341,   342,   344,   347,
   349,   351,   353,   355,   357,   359,   361,   363,   365,   367,
   369,   371,   373,   375,   377,   379,   383,   384,   386,   388,
   392,   396,   398,   400,   404,   408,   410,   411,   413
};

static const short yyrhs[] = {    69,
     0,    70,    69,     0,     0,   124,     0,   128,     0,    71,
     0,   119,     0,    72,    75,     0,   143,    15,    73,   117,
    74,     0,    27,     0,     9,    73,     0,     0,     3,    76,
     4,    10,     0,     3,     4,    10,     0,    77,     0,    76,
    77,     0,    82,     0,    78,     0,    85,     0,   143,    79,
    80,    81,     0,   140,    89,     0,     7,    90,     8,     0,
    10,     0,   143,    83,    84,     0,   140,    95,    96,    97,
     0,    10,     0,   143,    86,    44,    87,    88,    10,     0,
    27,     0,    27,     0,    13,   106,     0,     0,    27,     0,
    91,     0,    90,    14,    91,     0,     0,   143,    92,    94,
    96,     0,   140,     0,    93,     0,    73,    44,     0,    27,
     0,    27,     0,     6,    11,     5,     0,     6,     5,     0,
     0,    13,    99,     0,     0,   101,     0,   107,     0,   108,
     0,   102,     0,    19,     0,   107,     0,   108,     0,   101,
     0,   100,    14,   101,     0,   102,     0,    19,     0,   103,
     0,    16,     0,    17,     0,   105,     0,   104,     0,    11,
     0,    12,     0,    20,     0,    21,     0,    22,     0,    24,
     0,    23,     0,   106,     0,   105,   106,     0,    18,     0,
     3,   100,     4,     0,     3,     4,     0,   109,     0,   110,
     0,    25,   111,   113,    25,     0,   118,     0,   112,     9,
     0,     0,   106,     0,    73,    26,   114,     0,   115,     0,
   114,    14,   115,     0,   116,    13,    98,     0,    27,     0,
    31,   118,     0,     0,    28,     0,   120,   121,     0,   143,
    32,    33,    73,   117,     0,     3,   122,     4,    10,     0,
   123,     0,   122,   123,     0,   143,    27,    13,    99,    10,
     0,   125,     0,   127,     0,    29,    30,     7,   126,     8,
     0,   106,     0,    29,   149,     7,   150,     8,     0,    34,
   147,   129,   130,   134,    10,     0,     9,   140,    96,    97,
     0,   131,   132,     8,     0,    14,    35,     7,     0,   133,
     0,   132,    14,   133,     0,    15,     0,    36,     0,    37,
     0,    38,     0,    39,     0,    40,     0,    41,     0,    42,
     0,    43,     0,    14,   135,   136,     8,     0,     0,    45,
     7,     0,   137,     0,   136,    14,   137,     0,    46,     0,
    47,     0,    48,     0,    49,     0,    50,     0,   139,     0,
     0,   137,     0,   139,   137,     0,   141,     0,   142,     0,
    51,     0,    52,     0,    53,     0,    54,     0,    55,     0,
    56,     0,    57,     0,    58,     0,    59,     0,    60,     0,
    61,     0,    63,     0,    64,     0,    65,     0,   144,   145,
     5,     0,     0,     6,     0,   146,     0,   145,    14,   146,
     0,   147,   148,   138,     0,    27,     0,   133,     0,     7,
   102,     8,     0,     7,    19,     8,     0,   107,     0,     0,
    27,     0,    18,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   236,   238,   239,   241,   242,   244,   246,   249,   255,   268,
   270,   271,   273,   274,   276,   277,   279,   281,   283,   286,
   292,   302,   304,   306,   313,   328,   330,   346,   348,   350,
   351,   353,   355,   356,   357,   359,   375,   376,   378,   381,
   383,   385,   390,   391,   393,   394,   400,   401,   402,   407,
   413,   419,   425,   432,   433,   443,   444,   446,   447,   448,
   449,   450,   452,   453,   454,   457,   460,   464,   465,   467,
   468,   473,   501,   504,   507,   508,   510,   525,   530,   532,
   534,   536,   541,   542,   544,   553,   555,   556,   558,   560,
   567,   579,   582,   583,   588,   630,   634,   639,   646,   648,
   655,   665,   672,   674,   677,   678,   682,   684,   685,   686,
   688,   689,   690,   691,   692,   695,   697,   702,   704,   706,
   717,   719,   720,   721,   722,   724,   725,   727,   728,   731,
   732,   733,   734,   735,   737,   738,   739,   740,   741,   742,
   743,   744,   745,   747,   748,   750,   751,   753,   755,   756,
   758,   773,   775,   779,   785,   791,   797,   803,   805
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
"parameterName","propertyName","array","typedDefaultValue","initializer","typedInitializer",
"constantValues","constantValue","nonNullConstantValue","integerValue","booleanValue",
"stringValues","stringValue","arrayInitializer","referenceInitializer","objectHandle",
"aliasInitializer","namespaceHandleRef","namespaceHandle","modelPath","keyValuePairList",
"keyValuePair","keyValuePairName","alias","aliasIdentifier","instanceDeclaration",
"instanceHead","instanceBody","valueInitializers","valueInitializer","compilerDirective",
"compilerDirectiveInclude","fileName","compilerDirectivePragma","qualifierDeclaration",
"qualifierValue","scope","scope_begin","metaElements","metaElement","defaultFlavor",
"flavorHead","explicitFlavors","explicitFlavor","flavor","overrideFlavors","dataType",
"intDataType","realDataType","qualifierList","qualifierListBegin","qualifiers",
"qualifier","qualifierName","typedQualifierParameter","pragmaName","pragmaVal", NULL
};
#endif

static const short yyr1[] = {     0,
    68,    69,    69,    70,    70,    70,    70,    71,    72,    73,
    74,    74,    75,    75,    76,    76,    77,    77,    77,    78,
    79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
    88,    89,    90,    90,    90,    91,    92,    92,    93,    94,
    95,    96,    96,    96,    97,    97,    98,    98,    98,    99,
    99,    99,    99,   100,   100,   101,   101,   102,   102,   102,
   102,   102,   103,   103,   103,   103,   103,   104,   104,   105,
   105,   106,   107,   107,   108,   108,   109,   110,   111,   111,
   112,   113,   114,   114,   115,   116,   117,   117,   118,   119,
   120,   121,   122,   122,   123,   124,   124,   125,   126,   127,
   128,   129,   130,   131,   132,   132,   133,   133,   133,   133,
   133,   133,   133,   133,   133,   134,   134,   135,   136,   136,
   137,   137,   137,   137,   137,   138,   138,   139,   139,   140,
   140,   140,   140,   140,   141,   141,   141,   141,   141,   141,
   141,   141,   141,   142,   142,   143,   143,   144,   145,   145,
   146,   147,   147,   148,   148,   148,   148,   149,   150
};

static const short yyr2[] = {     0,
     1,     2,     0,     1,     1,     1,     1,     2,     5,     1,
     2,     0,     4,     3,     1,     2,     1,     1,     1,     4,
     2,     3,     1,     3,     4,     1,     6,     1,     1,     2,
     0,     1,     1,     3,     0,     4,     1,     1,     2,     1,
     1,     3,     2,     0,     2,     0,     1,     1,     1,     1,
     1,     1,     1,     1,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     2,     1,     3,     2,     1,     1,     4,     1,     2,     0,
     1,     3,     1,     3,     3,     1,     2,     0,     1,     2,
     5,     4,     1,     2,     5,     1,     1,     5,     1,     5,
     6,     4,     3,     3,     1,     3,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     4,     0,     2,     1,     3,
     1,     1,     1,     1,     1,     1,     0,     1,     2,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     3,     0,     1,     1,     3,
     3,     1,     1,     3,     3,     1,     0,     1,     1
};

static const short yydefact[] = {   147,
   148,     0,     0,     1,   147,     6,     0,     7,     0,     4,
    96,    97,     5,     0,     0,   158,     0,     0,   107,   152,
   108,   109,   110,   111,   112,   113,   114,   115,   153,     0,
     2,   147,     8,   147,    90,     0,     0,     0,   149,   157,
     0,     0,     0,     0,     0,   147,    15,    18,    17,    19,
     0,   147,    93,     0,    10,    88,     0,   146,     0,     0,
     0,   156,   127,    72,    99,     0,   159,     0,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,    44,   130,   131,     0,   117,     0,    14,     0,
    16,    28,     0,     0,     0,     0,     0,    94,     0,     0,
    12,    88,   150,    74,    63,    64,    59,    60,    57,    65,
    66,    67,    69,    68,     0,    54,    56,    58,    62,    61,
    70,     0,     0,   121,   122,   123,   124,   125,   128,   151,
   126,    98,   100,     0,    46,     0,     0,     0,     0,   105,
    13,   147,     0,    26,    24,     0,    41,    21,    44,    92,
     0,    89,    87,     0,     9,    91,    73,     0,    71,   155,
   154,   129,    43,     0,     0,   102,   104,     0,     0,   101,
   103,     0,     0,    33,     0,    23,    20,    29,    31,    46,
    51,    80,     0,    50,    52,    53,    75,    76,    78,    11,
    55,    42,    45,   118,     0,   119,   106,    22,   147,     0,
     0,    38,    37,     0,     0,    25,    81,     0,     0,    95,
   116,     0,    34,    39,    40,    44,    30,    27,     0,     0,
    79,   120,    36,     0,    77,    86,    82,    83,     0,     0,
     0,    84,    85,    47,    48,    49,     0,     0,     0
};

static const short yydefgoto[] = {   237,
     4,     5,     6,     7,    56,   155,    33,    46,    47,    48,
    93,   143,   177,    49,    94,   145,    50,    95,   179,   205,
   148,   173,   174,   201,   202,   216,   149,   135,   166,   233,
   183,   115,   116,   117,   118,   119,   120,   121,   185,   186,
   187,   188,   208,   209,   220,   227,   228,   229,   101,   189,
     8,     9,    35,    52,    53,    10,    11,    66,    12,    13,
    44,    87,    88,   139,    29,   138,   169,   195,   129,   130,
   131,    83,    84,    85,    14,    15,    38,    39,    40,    63,
    18,    68
};

static const short yypact[] = {    61,
-32768,   -13,    77,-32768,    61,-32768,    16,-32768,    28,-32768,
-32768,-32768,-32768,    -3,    77,-32768,    25,    62,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    71,
-32768,    64,-32768,    72,-32768,    52,    55,    11,-32768,    27,
    75,    76,   174,    83,    88,    69,-32768,-32768,-32768,-32768,
   -16,    70,-32768,    73,-32768,    68,    52,-32768,    77,   200,
   128,-32768,    37,-32768,-32768,    95,-32768,    97,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    96,-32768,-32768,    86,    92,    94,-32768,    98,
-32768,-32768,   100,   102,    79,    99,   114,-32768,   112,   110,
   118,    68,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,     9,-32768,-32768,-32768,-32768,    75,
-32768,   120,   133,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    37,-32768,-32768,    13,   129,   146,   109,   147,    19,-32768,
-32768,    14,   149,-32768,-32768,   134,   153,-32768,    96,-32768,
   155,-32768,-32768,    52,-32768,-32768,-32768,   229,-32768,-32768,
-32768,-32768,-32768,   151,   155,-32768,-32768,   156,    37,-32768,
-32768,    94,    57,-32768,    -1,-32768,-32768,-32768,   157,   129,
-32768,    75,   152,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    58,-32768,-32768,-32768,    72,   125,
   137,-32768,-32768,    75,   172,-32768,-32768,    52,   175,-32768,
-32768,    37,-32768,-32768,-32768,    96,-32768,-32768,   159,   161,
-32768,-32768,-32768,   160,-32768,-32768,   179,-32768,   192,   160,
   178,-32768,-32768,-32768,-32768,-32768,   188,   207,-32768
};

static const short yypgoto[] = {-32768,
   187,-32768,-32768,-32768,   -53,-32768,-32768,-32768,   162,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,    10,-32768,-32768,-32768,-32768,  -139,    30,-32768,
    48,-32768,  -149,   -55,-32768,-32768,-32768,   -39,   -40,   -17,
-32768,-32768,-32768,-32768,-32768,-32768,   -15,-32768,   140,   136,
-32768,-32768,-32768,-32768,   191,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   -81,-32768,-32768,-32768,  -123,-32768,
-32768,   -46,-32768,-32768,   -31,-32768,-32768,   185,   251,-32768,
-32768,-32768
};


#define	YYLAST		254


static const short yytable[] = {    62,
    51,    65,    54,   102,    96,   123,   140,   162,   191,   180,
    92,    36,   157,    16,    51,    58,    17,   163,    32,     1,
    54,   -35,   158,   164,    59,    55,   171,   -35,    37,    60,
    34,    41,   172,    61,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,   196,    80,    81,    82,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    -3,    80,    81,    82,   198,   211,     1,    45,    42,     1,
   199,   212,    90,    97,     1,     1,   223,     1,    55,    43,
   159,   234,   124,   125,   126,   127,   128,    57,   222,     2,
   197,    19,    64,    67,     3,   184,    86,    89,   100,    99,
   190,   134,   132,    20,   133,   137,   142,   141,    19,   184,
   175,   144,    21,    22,    23,    24,    25,    26,    27,    28,
   136,   200,   146,   150,   151,   147,   154,   160,   203,    21,
    22,    23,    24,    25,    26,    27,    28,   152,   105,   106,
   161,   165,   207,   107,   108,    64,   122,   110,   111,   112,
   113,   114,   167,   168,   219,   192,   170,    60,   176,   -32,
   178,   210,   194,   215,   217,   105,   106,   175,   214,   204,
   107,   108,    64,   181,   110,   111,   112,   113,   114,   182,
    60,   218,   152,   221,   224,   225,   226,   238,   105,   106,
   235,    31,   230,   107,   108,    64,   109,   110,   111,   112,
   113,   114,   182,   104,   231,   152,   239,    91,   213,   206,
   105,   106,   193,   236,   232,   107,   108,    64,   109,   110,
   111,   112,   113,   114,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,   153,    80,    81,    82,   105,
   106,   156,    98,   103,   107,   108,    64,   109,   110,   111,
   112,   113,   114,    30
};

static const short yycheck[] = {    40,
    32,    41,    34,    57,    51,    61,    88,   131,   158,   149,
    27,    15,     4,    27,    46,     5,    30,     5,     3,     6,
    52,     8,    14,    11,    14,    27,     8,    14,    32,     3,
     3,     7,    14,     7,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,   169,    63,    64,    65,    51,
    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
     0,    63,    64,    65,     8,     8,     6,     4,     7,     6,
    14,    14,     4,     4,     6,     6,   216,     6,    27,     9,
   120,   231,    46,    47,    48,    49,    50,    33,   212,    29,
   172,    15,    18,    18,    34,   151,    14,    10,    31,    27,
   154,     6,     8,    27,     8,    14,     7,    10,    15,   165,
   142,    10,    36,    37,    38,    39,    40,    41,    42,    43,
    35,   175,    44,    10,    13,    27,     9,     8,   175,    36,
    37,    38,    39,    40,    41,    42,    43,    28,    11,    12,
     8,    13,   182,    16,    17,    18,    19,    20,    21,    22,
    23,    24,     7,    45,   208,     5,    10,     3,    10,     7,
    27,    10,     7,    27,   204,    11,    12,   199,    44,    13,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
     3,    10,    28,     9,    26,    25,    27,     0,    11,    12,
   231,     5,    14,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,     4,    13,    28,     0,    46,   199,   180,
    11,    12,   165,   231,   230,    16,    17,    18,    19,    20,
    21,    22,    23,    24,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,   100,    63,    64,    65,    11,
    12,   102,    52,    59,    16,    17,    18,    19,    20,    21,
    22,    23,    24,     3
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
#line 241 "cimmof.y"
{ /* FIXME: Where do we put directives? */ ;
    break;}
case 5:
#line 243 "cimmof.y"
{ cimmofParser::Instance()->addQualifier(yyvsp[0].mofqualifier); delete yyvsp[0].mofqualifier; ;
    break;}
case 6:
#line 245 "cimmof.y"
{ cimmofParser::Instance()->addClass(yyvsp[0].mofclass); ;
    break;}
case 7:
#line 247 "cimmof.y"
{ cimmofParser::Instance()->addInstance(yyvsp[0].instance); ;
    break;}
case 8:
#line 250 "cimmof.y"
{
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addClassAlias(g_currentAlias, yyval.mofclass, false);
;
    break;}
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
;
    break;}
case 10:
#line 268 "cimmof.y"
{  ;
    break;}
case 11:
#line 270 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].cimnameval); ;
    break;}
case 12:
#line 271 "cimmof.y"
{ yyval.cimnameval = new CIMName(); ;
    break;}
case 17:
#line 279 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;
    break;}
case 18:
#line 281 "cimmof.y"
{
  cimmofParser::Instance()->applyMethod(*g_currentClass, *yyvsp[0].method); ;
    break;}
case 19:
#line 283 "cimmof.y"
{
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;
    break;}
case 20:
#line 287 "cimmof.y"
{
  yyval.method = yyvsp[-2].method;
  apply(&g_qualifierList, yyval.method);
;
    break;}
case 21:
#line 293 "cimmof.y"
{
  if (g_currentMethod)
    delete g_currentMethod;
  g_currentMethod = 
                 cimmofParser::Instance()->newMethod(*yyvsp[0].cimnameval, yyvsp[-1].datatype) ;
  yyval.method = g_currentMethod;
  delete yyvsp[0].cimnameval;
;
    break;}
case 24:
#line 307 "cimmof.y"
{
   yyval.property = yyvsp[-1].property;
  apply(&g_qualifierList, yyval.property);
;
    break;}
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
;
    break;}
case 27:
#line 332 "cimmof.y"
{
  String s(*yyvsp[-4].strval);
  if (!String::equal(*yyvsp[-1].strval, String::EMPTY))
    s += "." + *yyvsp[-1].strval;
  CIMValue *v = valueFactory::createValue(CIMTYPE_REFERENCE, -1, true, &s);
  //KS add the isArray and arraysize parameters. 8 mar 2002
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v, false,0, *yyvsp[-4].strval);
  apply(&g_qualifierList, yyval.property);
  delete yyvsp[-4].strval;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval; 
  delete v;
;
    break;}
case 28:
#line 346 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 29:
#line 348 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 30:
#line 350 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 31:
#line 351 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 32:
#line 353 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;
    break;}
case 36:
#line 360 "cimmof.y"
{ // ATTN: P2 2002 Question Need to create default value including type?
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
;
    break;}
case 37:
#line 375 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 38:
#line 376 "cimmof.y"
{ yyval.datatype = CIMTYPE_REFERENCE; ;
    break;}
case 39:
#line 378 "cimmof.y"
{  
                          g_referenceClassName = *yyvsp[-1].cimnameval; ;
    break;}
case 40:
#line 381 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;
    break;}
case 41:
#line 383 "cimmof.y"
{ yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;
    break;}
case 42:
#line 387 "cimmof.y"
{ yyval.ival = valueFactory::Stoi(*yyvsp[-1].strval);
		   delete yyvsp[-1].strval;
                 ;
    break;}
case 43:
#line 390 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 44:
#line 391 "cimmof.y"
{ yyval.ival = -1; ;
    break;}
case 45:
#line 393 "cimmof.y"
{ yyval.typedinitializer = yyvsp[0].typedinitializer; ;
    break;}
case 46:
#line 394 "cimmof.y"
{   /* empty */
                  g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                  g_typedInitializerValue.value = new String(String::EMPTY); 
                  yyval.typedinitializer = &g_typedInitializerValue;
              ;
    break;}
case 47:
#line 400 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 48:
#line 401 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 49:
#line 402 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 50:
#line 408 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;
    break;}
case 51:
#line 414 "cimmof.y"
{
           g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
           g_typedInitializerValue.value = new String(String::EMPTY); 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;
    break;}
case 52:
#line 420 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;
    break;}
case 53:
#line 426 "cimmof.y"
{ 
           g_typedInitializerValue.type = CIMMOF_REFERENCE_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;
    break;}
case 54:
#line 432 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 55:
#line 434 "cimmof.y"
{
                                *yyval.strval += ","; 
                                *yyval.strval += *yyvsp[0].strval;
				delete yyvsp[0].strval;
                              ;
    break;}
case 56:
#line 443 "cimmof.y"
{yyval.strval = yyvsp[0].strval;;
    break;}
case 57:
#line 444 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 58:
#line 446 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 59:
#line 447 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 60:
#line 448 "cimmof.y"
{ yyval.strval =  yyvsp[0].strval; ;
    break;}
case 61:
#line 449 "cimmof.y"
{ ;
    break;}
case 62:
#line 450 "cimmof.y"
{ yyval.strval = new String(yyvsp[0].ival ? "T" : "F"); ;
    break;}
case 65:
#line 454 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->oct_to_dec(*yyvsp[0].strval));
                   delete yyvsp[0].strval; ;
    break;}
case 66:
#line 457 "cimmof.y"
{
                   yyval.strval = new String(cimmofParser::Instance()->hex_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;
    break;}
case 67:
#line 460 "cimmof.y"
{
                 yyval.strval = new String(cimmofParser::Instance()->binary_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;
    break;}
case 68:
#line 464 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 69:
#line 465 "cimmof.y"
{ yyval.ival = 1; ;
    break;}
case 70:
#line 467 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 71:
#line 469 "cimmof.y"
{ 
                *yyval.strval += *yyvsp[0].strval;  delete yyvsp[0].strval;
              ;
    break;}
case 72:
#line 474 "cimmof.y"
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
case 73:
#line 503 "cimmof.y"
{ yyval.strval = yyvsp[-1].strval; ;
    break;}
case 74:
#line 505 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 75:
#line 507 "cimmof.y"
{;
    break;}
case 76:
#line 508 "cimmof.y"
{  ;
    break;}
case 77:
#line 511 "cimmof.y"
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
case 78:
#line 525 "cimmof.y"
{
  // convert somehow from alias to a CIM object name
  delete yyvsp[0].strval;
;
    break;}
case 79:
#line 531 "cimmof.y"
{ ;
    break;}
case 80:
#line 532 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 81:
#line 534 "cimmof.y"
{;
    break;}
case 82:
#line 536 "cimmof.y"
{
             modelPath *m = new modelPath(*yyvsp[-2].cimnameval, g_KeyBindingArray);
             g_KeyBindingArray.clear(); 
             delete yyvsp[-2].cimnameval;;
    break;}
case 83:
#line 541 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 84:
#line 542 "cimmof.y"
{ yyval.ival = 0; ;
    break;}
case 85:
#line 545 "cimmof.y"
{
		KeyBinding *kb = new KeyBinding(*yyvsp[-2].strval, *yyvsp[0].strval,
                               modelPath::KeyBindingTypeOf(*yyvsp[0].strval));
		g_KeyBindingArray.append(*kb);
		delete kb;
		delete yyvsp[-2].strval;
	        delete yyvsp[0].strval; ;
    break;}
case 87:
#line 555 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 88:
#line 556 "cimmof.y"
{ yyval.strval = new String(String::EMPTY); ;
    break;}
case 90:
#line 561 "cimmof.y"
{ 
  yyval.instance = g_currentInstance; 
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addInstanceAlias(g_currentAlias, yyvsp[-1].instance, true);
;
    break;}
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
;
    break;}
case 95:
#line 590 "cimmof.y"
{
  cimmofParser *cp = cimmofParser::Instance();
  // ATTN: P1 InstanceUpdate function 2001 BB  Instance update needs work here and CIMOM 
  // a property.  It must be fixed in the Common code first.
  // What we have to do here is create a CIMProperty  and initialize it with
  // the value provided.  The name of the property is $2 and it belongs
  // to the class whose name is in g_currentInstance->getClassName().
  // The steps are
  //   2. Get  property declaration's value object
  CIMProperty *oldprop = cp->PropertyFromInstance(*g_currentInstance,
							*yyvsp[-3].strval);
  CIMValue *oldv = cp->ValueFromProperty(*oldprop);

  //   3. create the new Value object of the same type

  // We want createValue to interpret a value as an array if is enclosed 
  // in {}s (e.g., { 2 } or {2, 3, 5}) or it is NULL and the property is 
  // defined as an array. createValue is responsible for the actual
  // validation.

  CIMValue *v = valueFactory::createValue(oldv->getType(),
                 ((yyvsp[-1].typedinitializer->type == CIMMOF_ARRAY_VALUE) |
                  ((yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE) & oldprop->isArray()))?0:-1,
                 (yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE),
                 yyvsp[-1].typedinitializer->value);

  //   4. create a clone property with the new value
  CIMProperty *newprop = cp->copyPropertyWithNewValue(*oldprop, *v);
  //   5. apply the qualifiers; 
  apply(&g_qualifierList, newprop);
  //   6. and apply the CIMProperty to g_currentInstance.
  cp->applyProperty(*g_currentInstance, *newprop);
  delete yyvsp[-3].strval;
  delete yyvsp[-1].typedinitializer->value;
  delete oldprop;
  delete oldv;
  delete v;
  delete newprop;
;
    break;}
case 96:
#line 631 "cimmof.y"
{
    //printf("compilerDirectiveInclude "); 
;
    break;}
case 97:
#line 635 "cimmof.y"
{
    //printf("compilerDirectivePragma ");
;
    break;}
case 98:
#line 641 "cimmof.y"
{
  cimmofParser::Instance()->enterInlineInclude(*yyvsp[-1].strval); delete yyvsp[-1].strval;
;
    break;}
case 99:
#line 646 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 100:
#line 650 "cimmof.y"
{ cimmofParser::Instance()->processPragma(*yyvsp[-3].strval, *yyvsp[-1].strval); 
		   delete yyvsp[-3].strval;
		   delete yyvsp[-1].strval;
		   ;
    break;}
case 101:
#line 657 "cimmof.y"
{
//    CIMQualifierDecl *qd = new CIMQualifierDecl($2, $3, $4, $5);
	yyval.mofqualifier = cimmofParser::Instance()->newQualifierDecl(*yyvsp[-4].strval, yyvsp[-3].value, *yyvsp[-2].scope, *yyvsp[-1].flavor);
        delete yyvsp[-4].strval;
	delete yyvsp[-3].value;  // CIMValue object created in qualifierValue production
;
    break;}
case 102:
#line 666 "cimmof.y"
{
    yyval.value = valueFactory::createValue(yyvsp[-2].datatype, yyvsp[-1].ival, 
                yyvsp[0].typedinitializer->type == CIMMOF_NULL_VALUE, yyvsp[0].typedinitializer->value);
    delete yyvsp[0].typedinitializer->value;
;
    break;}
case 103:
#line 672 "cimmof.y"
{ yyval.scope = yyvsp[-1].scope; ;
    break;}
case 104:
#line 674 "cimmof.y"
{ 
    g_scope = CIMScope (CIMScope::NONE); ;
    break;}
case 105:
#line 677 "cimmof.y"
{ yyval.scope = yyvsp[0].scope; ;
    break;}
case 106:
#line 679 "cimmof.y"
{ yyval.scope->addScope(*yyvsp[0].scope); ;
    break;}
case 107:
#line 682 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS);        ;
    break;}
case 108:
#line 684 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::CLASS); ;
    break;}
case 109:
#line 685 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ASSOCIATION);  ;
    break;}
case 110:
#line 686 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::INDICATION);   ;
    break;}
case 111:
#line 688 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PROPERTY);     ;
    break;}
case 112:
#line 689 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::REFERENCE);    ;
    break;}
case 113:
#line 690 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::METHOD);       ;
    break;}
case 114:
#line 691 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::PARAMETER);    ;
    break;}
case 115:
#line 692 "cimmof.y"
{ yyval.scope = new CIMScope(CIMScope::ANY);          ;
    break;}
case 116:
#line 696 "cimmof.y"
{ yyval.flavor = &g_flavor; ;
    break;}
case 117:
#line 697 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); ;
    break;}
case 118:
#line 702 "cimmof.y"
{g_flavor = CIMFlavor (CIMFlavor::NONE);;
    break;}
case 121:
#line 718 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::ENABLEOVERRIDE); ;
    break;}
case 122:
#line 719 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::DISABLEOVERRIDE); ;
    break;}
case 123:
#line 720 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::RESTRICTED); ;
    break;}
case 124:
#line 721 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TOSUBELEMENTS); ;
    break;}
case 125:
#line 722 "cimmof.y"
{ g_flavor.addFlavor (CIMFlavor::TRANSLATABLE); ;
    break;}
case 126:
#line 724 "cimmof.y"
{ yyval.flavor = &g_flavor; ;
    break;}
case 127:
#line 725 "cimmof.y"
{ yyval.flavor = new CIMFlavor (CIMFlavor::NONE); ;
    break;}
case 130:
#line 731 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 131:
#line 732 "cimmof.y"
{ yyval.datatype = yyvsp[0].datatype; ;
    break;}
case 132:
#line 733 "cimmof.y"
{ yyval.datatype = CIMTYPE_STRING;   ;
    break;}
case 133:
#line 734 "cimmof.y"
{ yyval.datatype = CIMTYPE_BOOLEAN;  ;
    break;}
case 134:
#line 735 "cimmof.y"
{ yyval.datatype = CIMTYPE_DATETIME; ;
    break;}
case 135:
#line 737 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT8;  ;
    break;}
case 136:
#line 738 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT8;  ;
    break;}
case 137:
#line 739 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT16; ;
    break;}
case 138:
#line 740 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT16; ;
    break;}
case 139:
#line 741 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT32; ;
    break;}
case 140:
#line 742 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT32; ;
    break;}
case 141:
#line 743 "cimmof.y"
{ yyval.datatype = CIMTYPE_UINT64; ;
    break;}
case 142:
#line 744 "cimmof.y"
{ yyval.datatype = CIMTYPE_SINT64; ;
    break;}
case 143:
#line 745 "cimmof.y"
{ yyval.datatype = CIMTYPE_CHAR16; ;
    break;}
case 144:
#line 747 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL32; ;
    break;}
case 145:
#line 748 "cimmof.y"
{ yyval.datatype =CIMTYPE_REAL64; ;
    break;}
case 147:
#line 751 "cimmof.y"
{ ;
    break;}
case 148:
#line 753 "cimmof.y"
{ g_qualifierList.init(); ;
    break;}
case 149:
#line 755 "cimmof.y"
{ ;
    break;}
case 150:
#line 756 "cimmof.y"
{ ;
    break;}
case 151:
#line 759 "cimmof.y"
{
  cimmofParser *p = cimmofParser::Instance();
  // The qualifier value can't be set until we know the contents of the
  // QualifierDeclaration.  That's what QualifierValue() does.
  CIMValue *v = p->QualifierValue(*yyvsp[-2].strval, 
                  (yyvsp[-1].typedinitializer->type == CIMMOF_NULL_VALUE), *yyvsp[-1].typedinitializer->value); 
  yyval.qualifier = p->newQualifier(*yyvsp[-2].strval, *v, g_flavor);
  g_qualifierList.add(yyval.qualifier);
  delete yyvsp[-2].strval;
  delete yyvsp[-1].typedinitializer->value;
  delete v;
 ;
    break;}
case 152:
#line 773 "cimmof.y"
{ 
    g_flavor = CIMFlavor (CIMFlavor::NONE); ;
    break;}
case 153:
#line 775 "cimmof.y"
{ 
                        yyval.strval = new String((*yyvsp[0].scope).toString ());
                        g_flavor = CIMFlavor (CIMFlavor::NONE); ;
    break;}
case 154:
#line 780 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
                    g_typedInitializerValue.value =  yyvsp[-1].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;
    break;}
case 155:
#line 786 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;
    break;}
case 156:
#line 792 "cimmof.y"
{
                    g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
                    g_typedInitializerValue.value =  yyvsp[0].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;
    break;}
case 157:
#line 797 "cimmof.y"
{   /* empty */
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;
    break;}
case 158:
#line 803 "cimmof.y"
{ yyval.strval = yyvsp[0].strval; ;
    break;}
case 159:
#line 805 "cimmof.y"
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
#line 807 "cimmof.y"

