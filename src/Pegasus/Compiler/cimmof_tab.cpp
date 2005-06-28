/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse cimmof_parse
#define yylex   cimmof_lex
#define yyerror cimmof_error
#define yylval  cimmof_lval
#define yychar  cimmof_char
#define yydebug cimmof_debug
#define yynerrs cimmof_nerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_ALIAS_IDENTIFIER = 258,
     TOK_ANY = 259,
     TOK_AS = 260,
     TOK_ASSOCIATION = 261,
     TOK_BINARY_VALUE = 262,
     TOK_CHAR_VALUE = 263,
     TOK_CLASS = 264,
     TOK_COLON = 265,
     TOK_COMMA = 266,
     TOK_DISABLEOVERRIDE = 267,
     TOK_DQUOTE = 268,
     TOK_DT_BOOL = 269,
     TOK_DT_CHAR16 = 270,
     TOK_DT_CHAR8 = 271,
     TOK_DT_DATETIME = 272,
     TOK_DT_REAL32 = 273,
     TOK_DT_REAL64 = 274,
     TOK_DT_SINT16 = 275,
     TOK_DT_SINT32 = 276,
     TOK_DT_SINT64 = 277,
     TOK_DT_SINT8 = 278,
     TOK_DT_STR = 279,
     TOK_DT_UINT16 = 280,
     TOK_DT_UINT32 = 281,
     TOK_DT_UINT64 = 282,
     TOK_DT_UINT8 = 283,
     TOK_ENABLEOVERRIDE = 284,
     TOK_END_OF_FILE = 285,
     TOK_EQUAL = 286,
     TOK_FALSE = 287,
     TOK_FLAVOR = 288,
     TOK_HEX_VALUE = 289,
     TOK_INCLUDE = 290,
     TOK_INDICATION = 291,
     TOK_INSTANCE = 292,
     TOK_LEFTCURLYBRACE = 293,
     TOK_LEFTPAREN = 294,
     TOK_LEFTSQUAREBRACKET = 295,
     TOK_METHOD = 296,
     TOK_NULL_VALUE = 297,
     TOK_OCTAL_VALUE = 298,
     TOK_OF = 299,
     TOK_PARAMETER = 300,
     TOK_PERIOD = 301,
     TOK_POSITIVE_DECIMAL_VALUE = 302,
     TOK_PRAGMA = 303,
     TOK_PROPERTY = 304,
     TOK_QUALIFIER = 305,
     TOK_REAL_VALUE = 306,
     TOK_REF = 307,
     TOK_REFERENCE = 308,
     TOK_RESTRICTED = 309,
     TOK_RIGHTCURLYBRACE = 310,
     TOK_RIGHTPAREN = 311,
     TOK_RIGHTSQUAREBRACKET = 312,
     TOK_SCHEMA = 313,
     TOK_SCOPE = 314,
     TOK_SEMICOLON = 315,
     TOK_SIGNED_DECIMAL_VALUE = 316,
     TOK_SIMPLE_IDENTIFIER = 317,
     TOK_STRING_VALUE = 318,
     TOK_TOSUBCLASS = 319,
     TOK_TRANSLATABLE = 320,
     TOK_TRUE = 321,
     TOK_UNEXPECTED_CHAR = 322
   };
#endif
#define TOK_ALIAS_IDENTIFIER 258
#define TOK_ANY 259
#define TOK_AS 260
#define TOK_ASSOCIATION 261
#define TOK_BINARY_VALUE 262
#define TOK_CHAR_VALUE 263
#define TOK_CLASS 264
#define TOK_COLON 265
#define TOK_COMMA 266
#define TOK_DISABLEOVERRIDE 267
#define TOK_DQUOTE 268
#define TOK_DT_BOOL 269
#define TOK_DT_CHAR16 270
#define TOK_DT_CHAR8 271
#define TOK_DT_DATETIME 272
#define TOK_DT_REAL32 273
#define TOK_DT_REAL64 274
#define TOK_DT_SINT16 275
#define TOK_DT_SINT32 276
#define TOK_DT_SINT64 277
#define TOK_DT_SINT8 278
#define TOK_DT_STR 279
#define TOK_DT_UINT16 280
#define TOK_DT_UINT32 281
#define TOK_DT_UINT64 282
#define TOK_DT_UINT8 283
#define TOK_ENABLEOVERRIDE 284
#define TOK_END_OF_FILE 285
#define TOK_EQUAL 286
#define TOK_FALSE 287
#define TOK_FLAVOR 288
#define TOK_HEX_VALUE 289
#define TOK_INCLUDE 290
#define TOK_INDICATION 291
#define TOK_INSTANCE 292
#define TOK_LEFTCURLYBRACE 293
#define TOK_LEFTPAREN 294
#define TOK_LEFTSQUAREBRACKET 295
#define TOK_METHOD 296
#define TOK_NULL_VALUE 297
#define TOK_OCTAL_VALUE 298
#define TOK_OF 299
#define TOK_PARAMETER 300
#define TOK_PERIOD 301
#define TOK_POSITIVE_DECIMAL_VALUE 302
#define TOK_PRAGMA 303
#define TOK_PROPERTY 304
#define TOK_QUALIFIER 305
#define TOK_REAL_VALUE 306
#define TOK_REF 307
#define TOK_REFERENCE 308
#define TOK_RESTRICTED 309
#define TOK_RIGHTCURLYBRACE 310
#define TOK_RIGHTPAREN 311
#define TOK_RIGHTSQUAREBRACKET 312
#define TOK_SCHEMA 313
#define TOK_SCOPE 314
#define TOK_SEMICOLON 315
#define TOK_SIGNED_DECIMAL_VALUE 316
#define TOK_SIMPLE_IDENTIFIER 317
#define TOK_STRING_VALUE 318
#define TOK_TOSUBCLASS 319
#define TOK_TRANSLATABLE 320
#define TOK_TRUE 321
#define TOK_UNEXPECTED_CHAR 322




/* Copy the first part of user declarations.  */
#line 29 "cimmof.y"

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
#if !defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM) && !defined(__OS400__) && !defined(PEGASUS_OS_VMS)
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

//include any useful debugging stuff here

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
// Enable this define to compie Bison/Yacc tracing
// ATTN: p3 03092003 ks Enabling this flag currently causes a compile error

#define YYDEBUG 1
//static int cimmof_debug;

//extern cimmofParser g_cimmofParser;

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
  //ATTN: BB 2001 BB P1 - Fixed size qualifier list max 20. Make larger or var
  qualifierList g_qualifierList(20);
  CIMMethod *g_currentMethod = 0;
  CIMClass *g_currentClass = 0;
  CIMInstance *g_currentInstance = 0;
  String g_currentAlias = String::EMPTY;
  CIMName g_referenceClassName = CIMName();
  Array<CIMKeyBinding> g_KeyBindingArray; // it gets created empty
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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 163 "cimmof.y"
typedef union YYSTYPE {
  //char                     *strval;
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
/* Line 191 of yacc.c.  */
#line 375 "cimmoftemp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 387 "cimmoftemp"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  32
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   292

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  84
/* YYNRULES -- Number of rules. */
#define YYNRULES  160
/* YYNRULES -- Number of states. */
#define YYNSTATES  239

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   322

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     8,     9,    11,    13,    15,    17,
      20,    26,    28,    31,    32,    37,    41,    43,    46,    48,
      50,    52,    57,    60,    64,    66,    68,    70,    74,    75,
      80,    82,    84,    88,    93,    95,   102,   104,   106,   109,
     110,   113,   115,   117,   121,   124,   125,   128,   129,   131,
     133,   135,   137,   139,   141,   143,   145,   149,   151,   153,
     155,   157,   159,   161,   163,   165,   167,   169,   171,   173,
     175,   177,   179,   182,   184,   188,   191,   193,   195,   200,
     202,   205,   206,   208,   212,   214,   218,   222,   224,   227,
     228,   230,   233,   239,   244,   246,   249,   255,   257,   259,
     265,   267,   273,   280,   285,   289,   293,   295,   299,   301,
     303,   305,   307,   309,   311,   313,   315,   317,   322,   323,
     326,   328,   332,   334,   336,   338,   340,   342,   344,   345,
     347,   350,   352,   354,   356,   358,   360,   362,   364,   366,
     368,   370,   372,   374,   376,   378,   380,   382,   386,   387,
     389,   391,   395,   399,   401,   403,   407,   411,   413,   414,
     416
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      69,     0,    -1,    70,    -1,    71,    70,    -1,    -1,   125,
      -1,   129,    -1,    72,    -1,   120,    -1,    73,    76,    -1,
     144,     9,    74,   118,    75,    -1,    62,    -1,    10,    74,
      -1,    -1,    38,    77,    55,    60,    -1,    38,    55,    60,
      -1,    78,    -1,    77,    78,    -1,    87,    -1,    79,    -1,
      90,    -1,   144,    80,    81,    82,    -1,   141,    83,    -1,
      39,    84,    56,    -1,    60,    -1,    62,    -1,    85,    -1,
      84,    11,    85,    -1,    -1,   144,    86,    95,    97,    -1,
     141,    -1,    94,    -1,   144,    88,    89,    -1,   141,    96,
      97,    98,    -1,    60,    -1,   144,    91,    52,    92,    93,
      60,    -1,    62,    -1,    62,    -1,    31,   107,    -1,    -1,
      74,    52,    -1,    62,    -1,    62,    -1,    40,    47,    57,
      -1,    40,    57,    -1,    -1,    31,   100,    -1,    -1,   102,
      -1,   108,    -1,   109,    -1,   103,    -1,    42,    -1,   108,
      -1,   109,    -1,   102,    -1,   101,    11,   102,    -1,   103,
      -1,    42,    -1,   104,    -1,    51,    -1,     8,    -1,   106,
      -1,   105,    -1,    47,    -1,    61,    -1,    43,    -1,    34,
      -1,     7,    -1,    32,    -1,    66,    -1,   107,    -1,   106,
     107,    -1,    63,    -1,    38,   101,    55,    -1,    38,    55,
      -1,   110,    -1,   111,    -1,    13,   112,   114,    13,    -1,
     119,    -1,   113,    10,    -1,    -1,   107,    -1,    74,    46,
     115,    -1,   116,    -1,   115,    11,   116,    -1,   117,    31,
      99,    -1,    62,    -1,     5,   119,    -1,    -1,     3,    -1,
     121,   122,    -1,   144,    37,    44,    74,   118,    -1,    38,
     123,    55,    60,    -1,   124,    -1,   123,   124,    -1,   144,
      62,    31,   100,    60,    -1,   126,    -1,   128,    -1,    48,
      35,    39,   127,    56,    -1,   107,    -1,    48,   150,    39,
     151,    56,    -1,    50,   148,   130,   131,   135,    60,    -1,
      10,   141,    97,    98,    -1,   132,   133,    56,    -1,    11,
      59,    39,    -1,   134,    -1,   133,    11,   134,    -1,     9,
      -1,    58,    -1,     6,    -1,    36,    -1,    49,    -1,    53,
      -1,    41,    -1,    45,    -1,     4,    -1,    11,   136,   137,
      56,    -1,    -1,    33,    39,    -1,   138,    -1,   137,    11,
     138,    -1,    29,    -1,    12,    -1,    54,    -1,    64,    -1,
      65,    -1,   140,    -1,    -1,   138,    -1,   140,   138,    -1,
     142,    -1,   143,    -1,    24,    -1,    14,    -1,    17,    -1,
      28,    -1,    23,    -1,    25,    -1,    20,    -1,    26,    -1,
      21,    -1,    27,    -1,    22,    -1,    15,    -1,    18,    -1,
      19,    -1,   145,   146,    57,    -1,    -1,    40,    -1,   147,
      -1,   146,    11,   147,    -1,   148,   149,   139,    -1,    62,
      -1,   134,    -1,    39,   103,    56,    -1,    39,    42,    56,
      -1,   108,    -1,    -1,    62,    -1,    63,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   304,   304,   307,   308,   312,   313,   315,   317,   330,
     338,   358,   361,   362,   365,   366,   369,   370,   373,   376,
     379,   396,   407,   426,   429,   432,   438,   439,   440,   443,
     469,   470,   485,   499,   515,   530,   547,   550,   553,   554,
     557,   561,   564,   567,   572,   573,   576,   577,   584,   585,
     586,   592,   598,   604,   610,   620,   622,   635,   636,   639,
     640,   641,   642,   643,   646,   647,   648,   651,   654,   659,
     660,   663,   664,   670,   702,   704,   708,   709,   712,   728,
     736,   738,   741,   744,   750,   751,   754,   764,   767,   772,
     776,   790,   799,   814,   819,   820,   827,   884,   888,   894,
     901,   904,   922,   932,   940,   943,   947,   948,   953,   955,
     956,   957,   959,   960,   961,   962,   963,   967,   969,   975,
     978,   979,   991,   993,   994,   995,   996,   999,  1000,  1003,
    1004,  1008,  1009,  1010,  1011,  1012,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1026,  1027,  1039,  1040,  1046,
    1054,  1055,  1059,  1075,  1077,  1083,  1089,  1095,  1101,  1108,
    1111
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOK_ALIAS_IDENTIFIER", "TOK_ANY", 
  "TOK_AS", "TOK_ASSOCIATION", "TOK_BINARY_VALUE", "TOK_CHAR_VALUE", 
  "TOK_CLASS", "TOK_COLON", "TOK_COMMA", "TOK_DISABLEOVERRIDE", 
  "TOK_DQUOTE", "TOK_DT_BOOL", "TOK_DT_CHAR16", "TOK_DT_CHAR8", 
  "TOK_DT_DATETIME", "TOK_DT_REAL32", "TOK_DT_REAL64", "TOK_DT_SINT16", 
  "TOK_DT_SINT32", "TOK_DT_SINT64", "TOK_DT_SINT8", "TOK_DT_STR", 
  "TOK_DT_UINT16", "TOK_DT_UINT32", "TOK_DT_UINT64", "TOK_DT_UINT8", 
  "TOK_ENABLEOVERRIDE", "TOK_END_OF_FILE", "TOK_EQUAL", "TOK_FALSE", 
  "TOK_FLAVOR", "TOK_HEX_VALUE", "TOK_INCLUDE", "TOK_INDICATION", 
  "TOK_INSTANCE", "TOK_LEFTCURLYBRACE", "TOK_LEFTPAREN", 
  "TOK_LEFTSQUAREBRACKET", "TOK_METHOD", "TOK_NULL_VALUE", 
  "TOK_OCTAL_VALUE", "TOK_OF", "TOK_PARAMETER", "TOK_PERIOD", 
  "TOK_POSITIVE_DECIMAL_VALUE", "TOK_PRAGMA", "TOK_PROPERTY", 
  "TOK_QUALIFIER", "TOK_REAL_VALUE", "TOK_REF", "TOK_REFERENCE", 
  "TOK_RESTRICTED", "TOK_RIGHTCURLYBRACE", "TOK_RIGHTPAREN", 
  "TOK_RIGHTSQUAREBRACKET", "TOK_SCHEMA", "TOK_SCOPE", "TOK_SEMICOLON", 
  "TOK_SIGNED_DECIMAL_VALUE", "TOK_SIMPLE_IDENTIFIER", "TOK_STRING_VALUE", 
  "TOK_TOSUBCLASS", "TOK_TRANSLATABLE", "TOK_TRUE", "TOK_UNEXPECTED_CHAR", 
  "$accept", "mofSpec", "mofProductions", "mofProduction", 
  "classDeclaration", "classHead", "className", "superClass", "classBody", 
  "classFeatures", "classFeature", "methodDeclaration", "methodHead", 
  "methodBody", "methodEnd", "methodName", "parameters", "parameter", 
  "parameterType", "propertyDeclaration", "propertyBody", "propertyEnd", 
  "referenceDeclaration", "referencedObject", "referenceName", 
  "referencePath", "objectRef", "parameterName", "propertyName", "array", 
  "typedDefaultValue", "initializer", "typedInitializer", 
  "constantValues", "constantValue", "nonNullConstantValue", 
  "integerValue", "booleanValue", "stringValues", "stringValue", 
  "arrayInitializer", "referenceInitializer", "objectHandle", 
  "aliasInitializer", "namespaceHandleRef", "namespaceHandle", 
  "modelPath", "keyValuePairList", "keyValuePair", "keyValuePairName", 
  "alias", "aliasIdentifier", "instanceDeclaration", "instanceHead", 
  "instanceBody", "valueInitializers", "valueInitializer", 
  "compilerDirective", "compilerDirectiveInclude", "fileName", 
  "compilerDirectivePragma", "qualifierDeclaration", "qualifierValue", 
  "scope", "scope_begin", "metaElements", "metaElement", "defaultFlavor", 
  "flavorHead", "explicitFlavors", "explicitFlavor", "flavor", 
  "overrideFlavors", "dataType", "intDataType", "realDataType", 
  "qualifierList", "qualifierListBegin", "qualifiers", "qualifier", 
  "qualifierName", "typedQualifierParameter", "pragmaName", "pragmaVal", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    68,    69,    70,    70,    71,    71,    71,    71,    72,
      73,    74,    75,    75,    76,    76,    77,    77,    78,    78,
      78,    79,    80,    81,    82,    83,    84,    84,    84,    85,
      86,    86,    87,    88,    89,    90,    91,    92,    93,    93,
      94,    95,    96,    97,    97,    97,    98,    98,    99,    99,
      99,   100,   100,   100,   100,   101,   101,   102,   102,   103,
     103,   103,   103,   103,   104,   104,   104,   104,   104,   105,
     105,   106,   106,   107,   108,   108,   109,   109,   110,   111,
     112,   112,   113,   114,   115,   115,   116,   117,   118,   118,
     119,   120,   121,   122,   123,   123,   124,   125,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   133,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   135,   135,   136,
     137,   137,   138,   138,   138,   138,   138,   139,   139,   140,
     140,   141,   141,   141,   141,   141,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   143,   143,   144,   144,   145,
     146,   146,   147,   148,   148,   149,   149,   149,   149,   150,
     151
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     1,     1,     2,
       5,     1,     2,     0,     4,     3,     1,     2,     1,     1,
       1,     4,     2,     3,     1,     1,     1,     3,     0,     4,
       1,     1,     3,     4,     1,     6,     1,     1,     2,     0,
       2,     1,     1,     3,     2,     0,     2,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     3,     2,     1,     1,     4,     1,
       2,     0,     1,     3,     1,     3,     3,     1,     2,     0,
       1,     2,     5,     4,     1,     2,     5,     1,     1,     5,
       1,     5,     6,     4,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     0,     2,
       1,     3,     1,     1,     1,     1,     1,     1,     0,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     0,     1,
       1,     3,     3,     1,     1,     3,     3,     1,     0,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
     148,   149,     0,     0,     0,     2,   148,     7,     0,     8,
       0,     5,    97,    98,     6,     0,     0,     0,   159,     0,
     116,   110,   108,   111,   114,   115,   112,   113,   109,   153,
     154,     0,     1,     3,   148,     9,   148,    91,     0,     0,
       0,   150,   158,     0,     0,     0,     0,     0,   148,    16,
      19,    18,    20,     0,   148,    94,     0,    11,    89,     0,
       0,   147,     0,     0,   157,   128,    73,   100,     0,   160,
       0,   134,   144,   135,   145,   146,   139,   141,   143,   137,
     133,   138,   140,   142,   136,    45,   131,   132,     0,   118,
       0,    15,     0,    17,    36,     0,     0,     0,     0,     0,
      95,     0,     0,    13,    89,   151,    68,    61,    69,    67,
      58,    66,    64,    60,    75,    65,    70,     0,    55,    57,
      59,    63,    62,    71,     0,     0,   123,   122,   124,   125,
     126,   129,   152,   127,    99,   101,     0,    47,     0,     0,
       0,     0,   106,    14,   148,     0,    34,    32,     0,    42,
      22,    45,    93,     0,    90,    88,     0,    10,    92,     0,
      74,    72,   156,   155,   130,     0,    44,     0,   103,   105,
       0,     0,   102,     0,   104,     0,    26,     0,    24,    21,
      37,    39,    47,    81,    52,     0,    51,    53,    54,    76,
      77,    79,    12,    56,    43,    46,   119,     0,   120,   107,
     148,    23,     0,     0,    31,    30,     0,     0,    33,    82,
       0,     0,    96,     0,   117,    27,    40,    41,    45,    38,
      35,     0,     0,    80,   121,    29,     0,    78,    87,    83,
      84,     0,     0,     0,    85,    86,    48,    49,    50
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     4,     5,     6,     7,     8,    58,   157,    35,    48,
      49,    50,    95,   145,   179,   150,   175,   176,   203,    51,
      96,   147,    52,    97,   181,   207,   204,   218,   151,   137,
     168,   235,   185,   117,   118,   119,   120,   121,   122,   123,
     187,   188,   189,   190,   210,   211,   222,   229,   230,   231,
     103,   191,     9,    10,    37,    54,    55,    11,    12,    68,
      13,    14,    46,    89,    90,   141,    30,   140,   171,   197,
     131,   132,   133,    85,    86,    87,    15,    16,    40,    41,
      42,    65,    19,    70
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -151
static const short yypact[] =
{
      14,  -151,   -17,   169,    27,  -151,    14,  -151,   -10,  -151,
      -8,  -151,  -151,  -151,  -151,    11,   169,    -7,  -151,    -6,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,    26,  -151,  -151,    -5,  -151,    -3,  -151,   -13,    17,
       0,  -151,   -14,    -4,     3,   245,    42,     7,    45,  -151,
    -151,  -151,  -151,   215,    47,  -151,    24,  -151,    50,   -13,
     169,  -151,    72,   105,  -151,    95,  -151,  -151,    33,  -151,
      35,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,    53,  -151,  -151,    36,    87,
     170,  -151,    39,  -151,  -151,    64,    48,    57,    43,    56,
    -151,    86,   115,   110,    50,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,    29,  -151,  -151,
    -151,  -151,    -4,  -151,    65,    69,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,    95,  -151,  -151,   -28,    97,    91,    93,
      71,    12,  -151,  -151,     2,    74,  -151,  -151,    70,   101,
    -151,    53,  -151,     9,  -151,  -151,   -13,  -151,  -151,   138,
    -151,  -151,  -151,  -151,  -151,    79,  -151,     9,  -151,  -151,
     102,    95,  -151,   170,  -151,    15,  -151,   230,  -151,  -151,
    -151,   112,    97,    -4,  -151,    84,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,    20,  -151,  -151,
      -3,  -151,    98,    89,  -151,  -151,    -4,    94,  -151,  -151,
     -13,   143,  -151,    95,  -151,  -151,  -151,  -151,    53,  -151,
    -151,   111,   145,  -151,  -151,  -151,    99,  -151,  -151,   151,
    -151,   132,    99,    31,  -151,  -151,  -151,  -151,  -151
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -151,  -151,   158,  -151,  -151,  -151,   -55,  -151,  -151,  -151,
     121,  -151,  -151,  -151,  -151,  -151,  -151,   -23,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -141,
       1,  -151,    19,  -151,  -150,   -57,  -151,  -151,  -151,   -41,
     -42,   -51,  -151,  -151,  -151,  -151,  -151,  -151,   -45,  -151,
      80,    88,  -151,  -151,  -151,  -151,   134,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,   -83,  -151,  -151,  -151,
    -125,  -151,  -151,   -48,  -151,  -151,   -33,  -151,  -151,   133,
     189,  -151,  -151,  -151
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -29
static const short yytable[] =
{
      64,    53,    67,    56,   104,    98,   125,   142,   164,   193,
     182,    60,   154,   -28,    -4,    53,   106,   107,    17,   165,
      38,    56,   183,   173,    62,    63,   200,    32,    34,   166,
      36,   213,    43,    44,   154,     1,    45,     1,   106,   107,
     159,   108,     1,   109,   183,    18,   198,    62,    39,    57,
      47,   184,   111,    88,     1,   102,   112,    61,   -28,    66,
     113,    59,     2,   108,     3,   109,    69,    91,   174,    62,
     115,   201,    66,   110,   111,   116,   214,   225,   112,   106,
     107,   161,   113,   236,   160,     1,   101,     1,   224,   134,
     199,   135,   115,   136,    66,   138,   186,   116,   139,   143,
      92,   192,    99,   144,   108,   149,   109,   126,   146,   148,
     186,   177,   106,   107,   110,   111,   152,   153,   154,   112,
     156,   162,   202,   113,   127,   163,   170,   114,   167,   205,
     169,   172,   180,   115,   178,    66,   194,   108,   116,   109,
     -25,   196,   209,   206,   212,   106,   107,   124,   111,   128,
     216,   217,   112,   223,   220,   221,   113,   226,   227,   129,
     130,   228,   232,   233,    33,   219,   115,   177,    66,    93,
     108,   116,   109,    20,    20,    21,    21,   215,    22,    22,
     110,   111,   238,   208,   158,   112,   195,   234,   100,   113,
     155,   237,    31,   105,     0,     0,     0,     0,     0,   115,
       0,    66,     0,     0,   116,    23,    23,     0,     0,     0,
      24,    24,     0,     0,    25,    25,     0,     0,    26,    26,
       0,     0,    27,    27,     0,     0,     0,    28,    28,    71,
      72,    29,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    71,    72,     0,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    71,
      72,     0,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,     0,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    57
};

static const short yycheck[] =
{
      42,    34,    43,    36,    59,    53,    63,    90,   133,   159,
     151,    11,     3,    11,     0,    48,     7,     8,    35,    47,
       9,    54,    13,    11,    38,    39,    11,     0,    38,    57,
      38,    11,    39,    39,     3,    40,    10,    40,     7,     8,
      11,    32,    40,    34,    13,    62,   171,    38,    37,    62,
      55,    42,    43,    11,    40,     5,    47,    57,    56,    63,
      51,    44,    48,    32,    50,    34,    63,    60,    56,    38,
      61,    56,    63,    42,    43,    66,    56,   218,    47,     7,
       8,   122,    51,   233,    55,    40,    62,    40,   213,    56,
     173,    56,    61,    40,    63,    59,   153,    66,    11,    60,
      55,   156,    55,    39,    32,    62,    34,    12,    60,    52,
     167,   144,     7,     8,    42,    43,    60,    31,     3,    47,
      10,    56,   177,    51,    29,    56,    33,    55,    31,   177,
      39,    60,    62,    61,    60,    63,    57,    32,    66,    34,
      39,    39,   183,    31,    60,     7,     8,    42,    43,    54,
      52,    62,    47,    10,    60,   210,    51,    46,    13,    64,
      65,    62,    11,    31,     6,   206,    61,   200,    63,    48,
      32,    66,    34,     4,     4,     6,     6,   200,     9,     9,
      42,    43,   233,   182,   104,    47,   167,   232,    54,    51,
     102,   233,     3,    60,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    63,    -1,    -1,    66,    36,    36,    -1,    -1,    -1,
      41,    41,    -1,    -1,    45,    45,    -1,    -1,    49,    49,
      -1,    -1,    53,    53,    -1,    -1,    -1,    58,    58,    14,
      15,    62,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    14,    15,    -1,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    14,
      15,    -1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    40,    48,    50,    69,    70,    71,    72,    73,   120,
     121,   125,   126,   128,   129,   144,   145,    35,    62,   150,
       4,     6,     9,    36,    41,    45,    49,    53,    58,    62,
     134,   148,     0,    70,    38,    76,    38,   122,     9,    37,
     146,   147,   148,    39,    39,    10,   130,    55,    77,    78,
      79,    87,    90,   144,   123,   124,   144,    62,    74,    44,
      11,    57,    38,    39,   108,   149,    63,   107,   127,    63,
     151,    14,    15,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,   141,   142,   143,    11,   131,
     132,    60,    55,    78,    62,    80,    88,    91,   141,    55,
     124,    62,     5,   118,    74,   147,     7,     8,    32,    34,
      42,    43,    47,    51,    55,    61,    66,   101,   102,   103,
     104,   105,   106,   107,    42,   103,    12,    29,    54,    64,
      65,   138,   139,   140,    56,    56,    40,    97,    59,    11,
     135,   133,   134,    60,    39,    81,    60,    89,    52,    62,
      83,    96,    60,    31,     3,   119,    10,    75,   118,    11,
      55,   107,    56,    56,   138,    47,    57,    31,    98,    39,
      33,   136,    60,    11,    56,    84,    85,   144,    60,    82,
      62,    92,    97,    13,    42,   100,   103,   108,   109,   110,
     111,   119,    74,   102,    57,   100,    39,   137,   138,   134,
      11,    56,    74,    86,    94,   141,    31,    93,    98,   107,
     112,   113,    60,    11,    56,    85,    52,    62,    95,   107,
      60,    74,   114,    10,   138,    97,    46,    13,    62,   115,
     116,   117,    11,    31,   116,    99,   102,   108,   109
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 312 "cimmof.y"
    { /* FIXME: Where do we put directives? */ ;}
    break;

  case 6:
#line 314 "cimmof.y"
    { cimmofParser::Instance()->addQualifier(yyvsp[0].mofqualifier); delete yyvsp[0].mofqualifier; ;}
    break;

  case 7:
#line 316 "cimmof.y"
    { cimmofParser::Instance()->addClass(yyvsp[0].mofclass); ;}
    break;

  case 8:
#line 318 "cimmof.y"
    { cimmofParser::Instance()->addInstance(yyvsp[0].instance); ;}
    break;

  case 9:
#line 331 "cimmof.y"
    {
    YACCTRACE("classDeclaration");
    if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addClassAlias(g_currentAlias, yyval.mofclass, false);
;}
    break;

  case 10:
#line 339 "cimmof.y"
    {
    // create new instance of class with className and superclassName
    // put returned class object on stack
    YACCTRACE("classHead:");
    yyval.mofclass = cimmofParser::Instance()->newClassDecl(*yyvsp[-2].cimnameval, *yyvsp[0].cimnameval);
    
    // put list of qualifiers into class
    applyQualifierList(&g_qualifierList, yyval.mofclass);
    
    g_currentAlias = *yyvsp[-1].strval;
    if (g_currentClass)
        delete g_currentClass;
    g_currentClass = yyval.mofclass;
    delete yyvsp[-2].cimnameval;
    delete yyvsp[-1].strval;
    delete yyvsp[0].cimnameval;
;}
    break;

  case 11:
#line 358 "cimmof.y"
    {;}
    break;

  case 12:
#line 361 "cimmof.y"
    { yyval.cimnameval = new CIMName(*yyvsp[0].cimnameval); ;}
    break;

  case 13:
#line 362 "cimmof.y"
    { yyval.cimnameval = new CIMName(); ;}
    break;

  case 18:
#line 373 "cimmof.y"
    {
  YACCTRACE("classFeature:applyProperty");
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;}
    break;

  case 19:
#line 376 "cimmof.y"
    {
  YACCTRACE("classFeature:applyMethod");
  cimmofParser::Instance()->applyMethod(*g_currentClass, *yyvsp[0].method); ;}
    break;

  case 20:
#line 379 "cimmof.y"
    {
  YACCTRACE("classFeature:applyProperty");
  cimmofParser::Instance()->applyProperty(*g_currentClass, *yyvsp[0].property); delete yyvsp[0].property; ;}
    break;

  case 21:
#line 397 "cimmof.y"
    {
  YACCTRACE("methodDeclaration");
  yyval.method = yyvsp[-2].method;
;}
    break;

  case 22:
#line 408 "cimmof.y"
    {
    YACCTRACE("methodHead");
    if (g_currentMethod)
    delete g_currentMethod;

  // create new method instance with pointer to method name and datatype
  g_currentMethod = cimmofParser::Instance()->newMethod(*yyvsp[0].cimnameval, yyvsp[-1].datatype) ;
  
  // put new method on stack
  yyval.method = g_currentMethod;

  // apply the method qualifier list.
  applyQualifierList(&g_qualifierList, yyval.method);

  delete yyvsp[0].cimnameval;
;}
    break;

  case 25:
#line 432 "cimmof.y"
    { yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;}
    break;

  case 29:
#line 444 "cimmof.y"
    { 
  // ATTN: P2 2002 Question Need to create default value including type?
  
  YACCTRACE("parameter:");
  CIMParameter *p = 0;
  cimmofParser *cp = cimmofParser::Instance();

  // Create new parameter with name, type, isArray, array, referenceClassName
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
;}
    break;

  case 30:
#line 469 "cimmof.y"
    { yyval.datatype = yyvsp[0].datatype; ;}
    break;

  case 31:
#line 470 "cimmof.y"
    { yyval.datatype = CIMTYPE_REFERENCE; ;}
    break;

  case 32:
#line 486 "cimmof.y"
    {
    // set body to stack and apply qualifier list
    // ATTN: the apply qualifer only works here because
    // there are not lower level qualifiers.  We do productions
    // that might have lower level qualifiers differently by
    // setting up a xxxHead production where qualifiers are 
    // applied.
    YACCTRACE("propertyDeclaration:");
    yyval.property = yyvsp[-1].property;
    applyQualifierList(&g_qualifierList, yyval.property);
;}
    break;

  case 33:
#line 500 "cimmof.y"
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
;}
    break;

  case 35:
#line 532 "cimmof.y"
    {
  String s(*yyvsp[-4].strval);
  if (!String::equal(*yyvsp[-1].strval, String::EMPTY))
    s.append("." + *yyvsp[-1].strval);
  CIMValue *v = valueFactory::createValue(CIMTYPE_REFERENCE, -1, true, &s);
  //KS add the isArray and arraysize parameters. 8 mar 2002
  yyval.property = cimmofParser::Instance()->newProperty(*yyvsp[-2].strval, *v, false,0, *yyvsp[-4].strval);
  applyQualifierList(&g_qualifierList, yyval.property);
  delete yyvsp[-4].strval;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].strval; 
  delete v;
;}
    break;

  case 36:
#line 547 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 37:
#line 550 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 38:
#line 553 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 39:
#line 554 "cimmof.y"
    { yyval.strval = new String(String::EMPTY); ;}
    break;

  case 40:
#line 557 "cimmof.y"
    {  
                          g_referenceClassName = *yyvsp[-1].cimnameval; ;}
    break;

  case 41:
#line 561 "cimmof.y"
    { yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;}
    break;

  case 42:
#line 564 "cimmof.y"
    { yyval.cimnameval = new CIMName(*yyvsp[0].strval); ;}
    break;

  case 43:
#line 569 "cimmof.y"
    { yyval.ival = valueFactory::Stoi(*yyvsp[-1].strval);
		   delete yyvsp[-1].strval;
                 ;}
    break;

  case 44:
#line 572 "cimmof.y"
    { yyval.ival = 0; ;}
    break;

  case 45:
#line 573 "cimmof.y"
    { yyval.ival = -1; ;}
    break;

  case 46:
#line 576 "cimmof.y"
    { yyval.typedinitializer = yyvsp[0].typedinitializer; ;}
    break;

  case 47:
#line 577 "cimmof.y"
    {   /* empty */
                  g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                  g_typedInitializerValue.value = new String(String::EMPTY); 
                  yyval.typedinitializer = &g_typedInitializerValue;
              ;}
    break;

  case 48:
#line 584 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 49:
#line 585 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 50:
#line 586 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 51:
#line 593 "cimmof.y"
    { 
           g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;}
    break;

  case 52:
#line 599 "cimmof.y"
    {
           g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
           g_typedInitializerValue.value = new String(String::EMPTY); 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;}
    break;

  case 53:
#line 605 "cimmof.y"
    { 
           g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;}
    break;

  case 54:
#line 611 "cimmof.y"
    { 
           g_typedInitializerValue.type = CIMMOF_REFERENCE_VALUE;
           g_typedInitializerValue.value =  yyvsp[0].strval; 
           yyval.typedinitializer = &g_typedInitializerValue;
           ;}
    break;

  case 55:
#line 620 "cimmof.y"
    { 
            *yyval.strval = valueFactory::stringWComma(String(*yyvsp[0].strval)); ;}
    break;

  case 56:
#line 623 "cimmof.y"
    {
                YACCTRACE("constantValues:1, Value= " << *yyvsp[0].strval);
                (*yyval.strval).append(","); 
                //(*$$).append(*$3);
                (*yyval.strval).append(valueFactory::stringWComma(String(*yyvsp[0].strval)));
                delete yyvsp[0].strval;
              ;}
    break;

  case 57:
#line 635 "cimmof.y"
    {yyval.strval = yyvsp[0].strval;;}
    break;

  case 58:
#line 636 "cimmof.y"
    { yyval.strval = new String(String::EMPTY); ;}
    break;

  case 59:
#line 639 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 60:
#line 640 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 61:
#line 641 "cimmof.y"
    { yyval.strval =  yyvsp[0].strval; ;}
    break;

  case 62:
#line 642 "cimmof.y"
    { ;}
    break;

  case 63:
#line 643 "cimmof.y"
    { yyval.strval = new String(yyvsp[0].ival ? "T" : "F"); ;}
    break;

  case 66:
#line 648 "cimmof.y"
    {
                   yyval.strval = new String(cimmofParser::Instance()->oct_to_dec(*yyvsp[0].strval));
                   delete yyvsp[0].strval; ;}
    break;

  case 67:
#line 651 "cimmof.y"
    {
                   yyval.strval = new String(cimmofParser::Instance()->hex_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;}
    break;

  case 68:
#line 654 "cimmof.y"
    {
                 yyval.strval = new String(cimmofParser::Instance()->binary_to_dec(*yyvsp[0].strval));
	           delete yyvsp[0].strval; ;}
    break;

  case 69:
#line 659 "cimmof.y"
    { yyval.ival = 0; ;}
    break;

  case 70:
#line 660 "cimmof.y"
    { yyval.ival = 1; ;}
    break;

  case 71:
#line 663 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 72:
#line 665 "cimmof.y"
    { 
                (*yyval.strval).append(*yyvsp[0].strval);  delete yyvsp[0].strval;
              ;}
    break;

  case 73:
#line 671 "cimmof.y"
    { 
   //String oldrep = *$1;
   //String s(oldrep), s1(String::EMPTY);
   // Handle quoted quote
   //int len = s.size();
   //if (s[len] == '\n') {
       //error: new line inside a string constant unless it is quoted
       //if (s[len - 2] == '\\') {
           //if (len > 3)
	        //s1 = s.subString(1, len-3);
       //} else {
           //cimmof_error("New line in string constant");
           //}
       //cimmofParser::Instance()->increment_lineno();
   //} else { // Can only be a quotation mark
       //if (s[len - 2] == '\\') {  // if it is quoted
           //if (len > 3) s1 = s.subString(1, len-3);
           //s1.append('\"');
           //cimmof_yy_less(len-1);
       //} else { // This is the normal case:  real quotes on both end
           //s1 = s.subString(1, len - 2) ;
           //}
       //}
   //delete $1;
   yyval.strval = //new String(s1);
        new String(*yyvsp[0].strval);
   delete yyvsp[0].strval;
;}
    break;

  case 74:
#line 703 "cimmof.y"
    { yyval.strval = yyvsp[-1].strval; ;}
    break;

  case 75:
#line 705 "cimmof.y"
    { yyval.strval = new String(String::EMPTY); ;}
    break;

  case 76:
#line 708 "cimmof.y"
    {;}
    break;

  case 77:
#line 709 "cimmof.y"
    {  ;}
    break;

  case 78:
#line 713 "cimmof.y"
    { 
  // The objectName string is decomposed for syntactical purposes 
  // and reassembled here for later parsing in creation of an objname instance 
  String *s = new String(*yyvsp[-2].strval);
  if (!String::equal(*s, String::EMPTY) && yyvsp[-1].modelpath)
    (*s).append(":");
  if (yyvsp[-1].modelpath) {
    (*s).append(yyvsp[-1].modelpath->Stringrep());
  }
  yyval.strval = s;
  delete yyvsp[-2].strval;
  delete yyvsp[-1].modelpath;
;}
    break;

  case 79:
#line 728 "cimmof.y"
    {
        // convert somehow from alias to a CIM object name
        yyerror("'alias' is not yet supported (see bugzilla 14).");
        delete yyvsp[0].strval;
        YYABORT;
        ;}
    break;

  case 80:
#line 737 "cimmof.y"
    { ;}
    break;

  case 81:
#line 738 "cimmof.y"
    { yyval.strval = new String(String::EMPTY); ;}
    break;

  case 82:
#line 741 "cimmof.y"
    {;}
    break;

  case 83:
#line 744 "cimmof.y"
    {
             modelPath *m = new modelPath((*yyvsp[-2].cimnameval).getString(), g_KeyBindingArray);
             g_KeyBindingArray.clear(); 
             delete yyvsp[-2].cimnameval;;}
    break;

  case 84:
#line 750 "cimmof.y"
    { yyval.ival = 0; ;}
    break;

  case 85:
#line 751 "cimmof.y"
    { yyval.ival = 0; ;}
    break;

  case 86:
#line 755 "cimmof.y"
    {
		CIMKeyBinding *kb = new CIMKeyBinding(*yyvsp[-2].strval, *yyvsp[0].strval,
                               modelPath::KeyBindingTypeOf(*yyvsp[0].strval));
		g_KeyBindingArray.append(*kb);
		delete kb;
		delete yyvsp[-2].strval;
	        delete yyvsp[0].strval; ;}
    break;

  case 88:
#line 767 "cimmof.y"
    { 
              yyerror("'alias' is not yet supported (see bugzilla 14).");
              yyval.strval = yyvsp[0].strval;
              YYABORT;
              ;}
    break;

  case 89:
#line 772 "cimmof.y"
    { yyval.strval = new String(String::EMPTY); ;}
    break;

  case 91:
#line 791 "cimmof.y"
    { 
  yyval.instance = g_currentInstance; 
  if (g_currentAlias != String::EMPTY)
    cimmofParser::Instance()->addInstanceAlias(g_currentAlias, yyvsp[-1].instance, true);
;}
    break;

  case 92:
#line 800 "cimmof.y"
    {
  if (g_currentInstance)
    delete g_currentInstance;
  g_currentAlias = *yyvsp[0].strval;
  g_currentInstance = cimmofParser::Instance()->newInstance(*yyvsp[-1].cimnameval);
  // apply the qualifierlist to the current instance
  yyval.instance = g_currentInstance;
  applyQualifierList(&g_qualifierList, yyval.instance);
  delete yyvsp[-1].cimnameval;
  delete yyvsp[0].strval;
;}
    break;

  case 96:
#line 829 "cimmof.y"
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
  applyQualifierList(&g_qualifierList, newprop);

  //   6. and apply the CIMProperty to g_currentInstance.
  cp->applyProperty(*g_currentInstance, *newprop);
  delete yyvsp[-3].strval;
  delete yyvsp[-1].typedinitializer->value;
  delete oldprop;
  delete oldv;
  delete v;
  delete newprop;
;}
    break;

  case 97:
#line 885 "cimmof.y"
    {
    //printf("compilerDirectiveInclude "); 
;}
    break;

  case 98:
#line 889 "cimmof.y"
    {
    //printf("compilerDirectivePragma ");
;}
    break;

  case 99:
#line 896 "cimmof.y"
    {
  cimmofParser::Instance()->enterInlineInclude(*yyvsp[-1].strval); delete yyvsp[-1].strval;
;}
    break;

  case 100:
#line 901 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 101:
#line 906 "cimmof.y"
    { cimmofParser::Instance()->processPragma(*yyvsp[-3].strval, *yyvsp[-1].strval); 
		   delete yyvsp[-3].strval;
		   delete yyvsp[-1].strval;
		   ;}
    break;

  case 102:
#line 924 "cimmof.y"
    {
//    CIMQualifierDecl *qd = new CIMQualifierDecl($2, $3, $4, $5);
	yyval.mofqualifier = cimmofParser::Instance()->newQualifierDecl(*yyvsp[-4].strval, yyvsp[-3].value, *yyvsp[-2].scope, *yyvsp[-1].flavor);
        delete yyvsp[-4].strval;
	delete yyvsp[-3].value;  // CIMValue object created in qualifierValue production
;}
    break;

  case 103:
#line 933 "cimmof.y"
    {
    yyval.value = valueFactory::createValue(yyvsp[-2].datatype, yyvsp[-1].ival, 
                yyvsp[0].typedinitializer->type == CIMMOF_NULL_VALUE, yyvsp[0].typedinitializer->value);
    delete yyvsp[0].typedinitializer->value;
;}
    break;

  case 104:
#line 940 "cimmof.y"
    { yyval.scope = yyvsp[-1].scope; ;}
    break;

  case 105:
#line 943 "cimmof.y"
    { 
    g_scope = CIMScope (CIMScope::NONE); ;}
    break;

  case 106:
#line 947 "cimmof.y"
    { yyval.scope = yyvsp[0].scope; ;}
    break;

  case 107:
#line 949 "cimmof.y"
    { yyval.scope->addScope(*yyvsp[0].scope); ;}
    break;

  case 108:
#line 953 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::CLASS);        ;}
    break;

  case 109:
#line 955 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::CLASS); ;}
    break;

  case 110:
#line 956 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::ASSOCIATION);  ;}
    break;

  case 111:
#line 957 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::INDICATION);   ;}
    break;

  case 112:
#line 959 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::PROPERTY);     ;}
    break;

  case 113:
#line 960 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::REFERENCE);    ;}
    break;

  case 114:
#line 961 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::METHOD);       ;}
    break;

  case 115:
#line 962 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::PARAMETER);    ;}
    break;

  case 116:
#line 963 "cimmof.y"
    { yyval.scope = new CIMScope(CIMScope::ANY);          ;}
    break;

  case 117:
#line 968 "cimmof.y"
    { yyval.flavor = &g_flavor; ;}
    break;

  case 118:
#line 969 "cimmof.y"
    { yyval.flavor = new CIMFlavor (CIMFlavor::NONE); ;}
    break;

  case 119:
#line 975 "cimmof.y"
    {g_flavor = CIMFlavor (CIMFlavor::NONE);;}
    break;

  case 122:
#line 992 "cimmof.y"
    { g_flavor.addFlavor (CIMFlavor::ENABLEOVERRIDE); ;}
    break;

  case 123:
#line 993 "cimmof.y"
    { g_flavor.addFlavor (CIMFlavor::DISABLEOVERRIDE); ;}
    break;

  case 124:
#line 994 "cimmof.y"
    { g_flavor.addFlavor (CIMFlavor::RESTRICTED); ;}
    break;

  case 125:
#line 995 "cimmof.y"
    { g_flavor.addFlavor (CIMFlavor::TOSUBELEMENTS); ;}
    break;

  case 126:
#line 996 "cimmof.y"
    { g_flavor.addFlavor (CIMFlavor::TRANSLATABLE); ;}
    break;

  case 127:
#line 999 "cimmof.y"
    { yyval.flavor = &g_flavor; ;}
    break;

  case 128:
#line 1000 "cimmof.y"
    { yyval.flavor = new CIMFlavor (CIMFlavor::NONE); ;}
    break;

  case 131:
#line 1008 "cimmof.y"
    { yyval.datatype = yyvsp[0].datatype; ;}
    break;

  case 132:
#line 1009 "cimmof.y"
    { yyval.datatype = yyvsp[0].datatype; ;}
    break;

  case 133:
#line 1010 "cimmof.y"
    { yyval.datatype = CIMTYPE_STRING;   ;}
    break;

  case 134:
#line 1011 "cimmof.y"
    { yyval.datatype = CIMTYPE_BOOLEAN;  ;}
    break;

  case 135:
#line 1012 "cimmof.y"
    { yyval.datatype = CIMTYPE_DATETIME; ;}
    break;

  case 136:
#line 1015 "cimmof.y"
    { yyval.datatype = CIMTYPE_UINT8;  ;}
    break;

  case 137:
#line 1016 "cimmof.y"
    { yyval.datatype = CIMTYPE_SINT8;  ;}
    break;

  case 138:
#line 1017 "cimmof.y"
    { yyval.datatype = CIMTYPE_UINT16; ;}
    break;

  case 139:
#line 1018 "cimmof.y"
    { yyval.datatype = CIMTYPE_SINT16; ;}
    break;

  case 140:
#line 1019 "cimmof.y"
    { yyval.datatype = CIMTYPE_UINT32; ;}
    break;

  case 141:
#line 1020 "cimmof.y"
    { yyval.datatype = CIMTYPE_SINT32; ;}
    break;

  case 142:
#line 1021 "cimmof.y"
    { yyval.datatype = CIMTYPE_UINT64; ;}
    break;

  case 143:
#line 1022 "cimmof.y"
    { yyval.datatype = CIMTYPE_SINT64; ;}
    break;

  case 144:
#line 1023 "cimmof.y"
    { yyval.datatype = CIMTYPE_CHAR16; ;}
    break;

  case 145:
#line 1026 "cimmof.y"
    { yyval.datatype =CIMTYPE_REAL32; ;}
    break;

  case 146:
#line 1027 "cimmof.y"
    { yyval.datatype =CIMTYPE_REAL64; ;}
    break;

  case 148:
#line 1040 "cimmof.y"
    { 
                 //yydebug = 1; stderr = stdout;
                 ;}
    break;

  case 149:
#line 1046 "cimmof.y"
    { 

    //yydebug = 1; stderr = stdout;
    YACCTRACE("qualifierListbegin");
    g_qualifierList.init(); ;}
    break;

  case 150:
#line 1054 "cimmof.y"
    { ;}
    break;

  case 151:
#line 1055 "cimmof.y"
    { ;}
    break;

  case 152:
#line 1060 "cimmof.y"
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
 ;}
    break;

  case 153:
#line 1075 "cimmof.y"
    { 
    g_flavor = CIMFlavor (CIMFlavor::NONE); ;}
    break;

  case 154:
#line 1077 "cimmof.y"
    { 
                        yyval.strval = new String((*yyvsp[0].scope).toString ());
                        g_flavor = CIMFlavor (CIMFlavor::NONE); ;}
    break;

  case 155:
#line 1084 "cimmof.y"
    {
                    g_typedInitializerValue.type = CIMMOF_CONSTANT_VALUE;
                    g_typedInitializerValue.value =  yyvsp[-1].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;}
    break;

  case 156:
#line 1090 "cimmof.y"
    {
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;}
    break;

  case 157:
#line 1096 "cimmof.y"
    {
                    g_typedInitializerValue.type = CIMMOF_ARRAY_VALUE;
                    g_typedInitializerValue.value =  yyvsp[0].strval;
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;}
    break;

  case 158:
#line 1101 "cimmof.y"
    {   /* empty */
                    g_typedInitializerValue.type = CIMMOF_NULL_VALUE;
                    g_typedInitializerValue.value = new String(String::EMPTY);
                    yyval.typedinitializer = &g_typedInitializerValue;
                    ;}
    break;

  case 159:
#line 1108 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;

  case 160:
#line 1111 "cimmof.y"
    { yyval.strval = yyvsp[0].strval; ;}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2521 "cimmoftemp"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  Doesn't work in C++ */
#ifndef __cplusplus
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  __attribute__ ((__unused__))
#endif
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 1113 "cimmof.y"


